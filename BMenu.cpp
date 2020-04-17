#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>

#include <QSortFilterProxyModel>

#include "BMenu.h"
#include "BFlags.h"

BMenu::BMenu(const QPoint pos, QString cnx, const etCount eType, const QTableView *view):QMenu ("Actions")
{
 // Etablir connexion a la base
 db_1 = QSqlDatabase::database(cnx);
 if(db_1.isValid()==false){
  QString str_error = db_1.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }

 eCalcul = eType;
 lview = view;
 index = view->indexAt(pos);

 val.tbName = "Filtres";
 val.flt = -1;
 val.pri = -1;
 val.col =-1;
 val.lgn=-1;
 val.eTyp = eCountToSet;
 val.val = -1;
 val.zn = -1;

 construireMenu();
}

void BMenu::slot_showMenu()
{
 QString msg = main_menu->title();
 QList<QAction *> lst = main_menu->actions();

 //stTbFiltres val;
 bool isOk = getFiltre(&val, eCalcul,lview,index);

 if(val.flt && (val.flt & BFlags::isWanted))
 {
  lst.at(0)->setChecked(true);
 }

 if(eCalcul == eCountElm){
  QMenu *subMenu = mnu_Priority(&val, eCalcul,lview,index);
  main_menu->addMenu(subMenu);
 }

}

void BMenu::slot_Filtre(bool chk)
{
 QAction *chkFrom = qobject_cast<QAction *>(sender());
 chkFrom->setChecked(chk);
}

void BMenu::slot_SetAll(bool chk)
{
 QAction *chkFrom = qobject_cast<QAction *>(sender());
 chkFrom->setChecked(chk);
}

void BMenu::construireMenu(void)
{
 main_menu = this;

 QAction *filtrer = main_menu->addAction("Filtrer",this,SLOT(slot_Filtre(bool)));
 filtrer->setCheckable(true);
}

bool BMenu::getFiltre(stTbFiltres *ret, const etCount typ, const QTableView *view, const QModelIndex index)
{
 stTbFiltres a;
 bool isOk = true;

 QSortFilterProxyModel *m = qobject_cast<QSortFilterProxyModel *>(view->model());
 int zn = view->objectName().toInt();
 int lgn = -1;
 int col = -1;
 int val = -1;


 if(typ >= eCountToSet && typ <= eCountEnd){
  switch (typ) {
   case eCountElm:
   case eCountCmb:
   case eCountBrc:
    lgn = typ *10;
    col = index.model()->index(index.row(),0).data().toInt();
    val = col;
    break;
   case eCountGrp:
    lgn = index.row();
    col = index.column();
    if(index.model()->index(lgn,col).data().canConvert(QMetaType::Int)){
     val = index.model()->index(lgn,col).data().toInt();
    }
    break;
   case eCountToSet:
   case eCountEnd:
    break;
  }
 }

 QSqlQuery query_2(db_1);
 QString tbFiltre = (*ret).tbName;

 /// Verifier si info presente dans table
 QString msg = "Select *  from "+tbFiltre
               +" where ("
                 "zne="+QString::number(zn)+" and "+
               "typ="+QString::number(typ)+" and "+
               "lgn="+QString::number(lgn)+" and "+
               "col="+QString::number(col)+" and "+
               "val="+QString::number(val)+")";

#ifndef QT_NO_DEBUG
 qDebug() << "mgs_2: "<<msg;
#endif
 isOk = query_2.exec(msg);

 if((isOk = query_2.first()))
 {
  //(*ret).tbName = tbFiltre;
  (*ret).flt = query_2.value("flt").toInt();
  (*ret).pri = query_2.value("pri").toInt();

	(*ret).lgn = lgn;
	(*ret).col = col;
	(*ret).val = val;
	(*ret).zn = zn;
	(*ret).eTyp = typ;
 }

 return isOk;
}

QMenu *BMenu::mnu_Priority(stTbFiltres *ret, const etCount eSrc, const QTableView *view, const QModelIndex index)
{
 bool isOk = false;
 int itm = 0;

 QSqlQuery query(db_1) ;
 QString msg = "";

 QString msg2 = "Priorite";
 QMenu *menu =new QMenu(msg2);

 QAction *setForAll = menu->addAction("SetAll",this,SLOT(slot_SetAll(bool)));
 setForAll->setCheckable(true);

 QActionGroup *grpPri = new  QActionGroup(menu);

 /// Total de priorite a afficher
 for(int i =1; i<=5;i++)
 {
  QAction *radio = new QAction(QString::number(i),grpPri);

	radio->setCheckable(true);
	menu->addAction(radio);
 }
 ///connect(grpPri,SIGNAL(triggered(QAction*)),this,SLOT(slot_ChoosePriority(QAction*)));

 if((*ret).pri>0)
 {
  QAction *uneAction;
  uneAction = qobject_cast<QAction *>(grpPri->children().at((*ret).pri-1));
  uneAction->setChecked(true);
 }

/*
 int row = index.row();
 stTbFiltres a;
 a.tbName = "Filtres";
 a.zn = view->objectName().toInt();;
 a.eTyp = eSrc;
 a.lgn = -1;
 a.col = index.column();
 a.pri = -1;
 a.val = -1;
 a.flt = 0;

 if(eSrc == eCountGrp)
 {
  a.lgn = index.row() ;

	if(index.model()->index(index.row(),a.col).data().canConvert(QMetaType::Int))
	{
	 a.val =  index.model()->index(index.row(),a.col).data().toInt();
	}
 }
 else {
  a.lgn = a.eTyp * 10;

  a.val =  index.model()->index(index.row(),0).data().toInt();
 }

 */

#if 0
 isOk = setFiltre(a,db_1);


 //if((typeFiltre.at(0)->currentIndex()==0) && (typeFiltre.at(1)->currentIndex()==0))
 {
  setForAll->setCheckable(true);
  setForAll->setObjectName("k_all");
  menu->addAction(setForAll);

	/// Total de priorite a afficher
	for(int i =1; i<=5;i++)
	{
	 QAction *radio = new QAction(QString::number(i),grpPri);

	 //radio->setObjectName(name);
	 radio->setCheckable(true);
	 menu->addAction(radio);
	}

	/*
	connect(setForAll,SIGNAL(triggered(bool)),this,SLOT(slot_wdaFilter(bool)));
	///connect(setForAll,SIGNAL(triggered(QAction*)),this,SLOT(slot_ChoosePriority(QAction*)));
	connect(grpPri,SIGNAL(triggered(QAction*)),this,SLOT(slot_ChoosePriority(QAction*)));
	if(pri>0)
	{
	 QAction *uneAction;
	 uneAction = qobject_cast<QAction *>(grpPri->children().at(pri-1));
	 uneAction->setChecked(true);
	}
 */

 }

 if(setPriorityToAll){
  setForAll->setChecked(true);
 }
#endif

 return menu;
}
