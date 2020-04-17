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
#include "db_tools.h"

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
 val.flt = BFlags::Filtre::isNotSet;
 val.pri = -1;
 val.col =-1;
 val.lgn=-1;
 val.eTyp = eCountToSet;
 val.val = -1;
 val.zn = -1;

 construireMenu();
}

void BMenu::construireMenu(void)
{
 main_menu = this;

 QAction *isWanted = main_menu->addAction("Prendre",this,SLOT(slot_isWanted(bool)));
 isWanted->setCheckable(true);
 isWanted->setEnabled(true);

 QAction *isFiltred = main_menu->addAction("Filtrer",this,SLOT(slot_isFiltred(bool)));
 isFiltred->setCheckable(true);
 isFiltred->setDisabled(true);
}

void BMenu::slot_showMenu()
{
 QString msg = main_menu->title();
 QList<QAction *> lst = main_menu->actions();

 /// lecture info dans la base de la selection en cours
 bool isOk = getdbFlt(&val, eCalcul,lview,index);

 /// On a trouve une reponse
 if(isOk){

	///
	if((val.flt>BFlags::isNotSet) && (val.flt & BFlags::isWanted))
	{
	 lst.at(0)->setChecked(true);
	 lst.at(1)->setEnabled(true);
	}
	else {
	 lst.at(0)->setChecked(false);
	 lst.at(1)->setEnabled(false);
	}

	if((val.flt>BFlags::isNotSet) && (val.flt & BFlags::isFiltred))
	{
	 lst.at(1)->setChecked(true);
	}
	else {
	 lst.at(1)->setChecked(false);
	}

	if( (eCalcul == eCountElm)
			&& lst.at(0)->isChecked()
					 ){
	 QMenu *subMenu = mnu_Priority(&val, eCalcul,lview,index);
	 main_menu->addMenu(subMenu);
	}
 }
}

QMenu *BMenu::mnu_Priority(stTbFiltres *ret, const etCount eSrc, const QTableView *view, const QModelIndex index)
{
 bool isOk = false;
 int itm = 0;

 QSqlQuery query(db_1) ;
 QString msg = "";

 QString msg2 = "Priorites";
 QMenu *menu =new QMenu(msg2);

 QAction *setForAll = menu->addAction("Mettre tout :",this,SLOT(slot_ForWanted(bool)));
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

void BMenu::slot_isWanted(bool chk)
{
 QAction *chkFrom = qobject_cast<QAction *>(sender());

 /// Verifier bornes des filtres
 if((val.flt<BFlags::isNotSet) || (val.flt>=BFlags::isTerminated)){
  val.flt = BFlags::Filtre::isNotSet;
 }

 /// Analyser action utilisateur
 if(chk){
  val.flt |= BFlags::isWanted;
 }
 else {
  val.flt = val.flt &  (~BFlags::isWanted);
 }

 /// Mettre a jour action dans base
 if(setdbFlt(val)){
  chkFrom->setChecked(chk);
 }

}

void BMenu::slot_isFiltred(bool chk)
{
 QAction *chkFrom = qobject_cast<QAction *>(sender());

 /// Verifier bornes des filtres
 if((val.flt<BFlags::isNotSet) || (val.flt>=BFlags::isTerminated)){
  val.flt = 0;
 }

 /// Analyser action utilisateur
 if(chk){
  val.flt |= BFlags::isFiltred;
 }
 else {
  val.flt &= ~BFlags::isFiltred;
 }

 /// Mettre a jour action dans base
 if(setdbFlt(val)){
  chkFrom->setChecked(chk);
 }
}

void BMenu::slot_ForWanted(bool chk)
{
 QAction *chkFrom = qobject_cast<QAction *>(sender());
 chkFrom->setChecked(chk);
}

bool BMenu::setdbFlt(stTbFiltres in)
{
 bool isOk = false;
 QString msg = "";
 QSqlQuery query(db_1);

 if(in.isPresent==true){
  /// == 1 donc update
  QString op = "";
  if(in.flt>0){
   op="|";
  }
  else {
   op="&";
  }
  msg = "update "+in.tbName+
        " set pri="+QString::number(in.pri)+
        ", flt=(case when flt is (NULL or 0 or flt<0) then 0x"+
        QString::number(in.flt)+
        " else(flt"+op+"0x"+QString::number(in.flt)+
        ") end) where ("
        "zne="+QString::number(in.zn)+" and "+
        "typ="+QString::number(in.eTyp)+" and "+
        "lgn="+QString::number(in.lgn)+" and "+
        "col="+QString::number(in.col)+" and "+
        "val="+QString::number(in.val)+")";

 }
 else {
  /// Pas de resultat donc insert
  msg ="insert into "+in.tbName+
        " (id, zne, typ,lgn,col,val,pri,flt)"
        " values (NULL,"
        +QString::number(in.zn)+","
        +QString::number(in.eTyp)+","
        +QString::number(in.lgn)+","
        +QString::number(in.col)+","
        +QString::number(in.val)+","
        +QString::number(in.pri)+","
        +QString::number(in.flt)+")";
 }

 if(msg.size()){
#ifndef QT_NO_DEBUG
  qDebug() << "msg: "<<msg;
#endif
  isOk = query.exec(msg);
 }

 if(!isOk){
  DB_Tools::DisplayError("BMenu::setdbFlt",&query,msg);
  QMessageBox::warning(nullptr,"BMenu","setdbFlt",QMessageBox::Ok);
 }


 return isOk;
}

bool BMenu::getdbFlt(stTbFiltres *ret, const etCount in_typ, const QTableView *view, const QModelIndex index)
{
 bool isOk = false;
 etCount typ = in_typ;

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
 else {
  typ = eCountToSet;
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
  (*ret).flt = query_2.value("flt").toInt();
  //(*ret).flt = query_2.value("flt").value<BFlags::Filtre>();
  (*ret).pri = query_2.value("pri").toInt();
 }

 (*ret).isPresent = isOk;
 (*ret).zn = zn;
 (*ret).eTyp = typ;
 (*ret).lgn = lgn;
 (*ret).col = col;
 (*ret).val = val;

 return isOk;
}

