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

 val.id=-1;
 val.tbName = "Filtres";
 val.flt = BFlags::Filtre::isNotSet;
 val.pri = -1;
 val.col =-1;
 val.lgn=-1;
 val.typ = eCountToSet;
 val.val = -1;
 val.zne = -1;
 val.b_flt = Bp::Filtering::isNotSet;

 construireMenu();
}

void BMenu::construireMenu(void)
{
 main_menu = this;

 QAction *isWanted = main_menu->addAction("Reserver",this,SLOT(slot_isWanted(bool)));
 isWanted->setCheckable(true);
 isWanted->setEnabled(true);

 QAction *isChoosed = main_menu->addAction("Choisir",this,SLOT(slot_isChoosed(bool)));
 isChoosed->setCheckable(true);
 isChoosed->setDisabled(true);

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

	/// wanted ?
	if((val.b_flt & Bp::Filtering::isWanted)== Bp::Filtering::isWanted){
	 lst.at(0)->setChecked(true);
	 lst.at(1)->setEnabled(true);
	 lst.at(2)->setEnabled(true);

	 if(eCalcul == eCountElm){
		QMenu *subMenu = mnu_Priority(&val, eCalcul,lview,index);
		main_menu->addMenu(subMenu);
	 }
	}
	else {
	 lst.at(2)->setChecked(false);
	 val.b_flt = val.b_flt & ~(Bp::Filtering::isFiltred);

	 lst.at(1)->setChecked(false);
	 val.b_flt = val.b_flt & ~(Bp::Filtering::isChoosed);

	 lst.at(0)->setChecked(false);
	}

	/// --------- selected
	if((val.b_flt & Bp::Filtering::isChoosed)== Bp::Filtering::isChoosed){
	 lst.at(1)->setChecked(true);
	}
	else {
	 lst.at(1)->setChecked(false);
	}

	/// --------- filtred
	if((val.b_flt & Bp::Filtering::isFiltred)== Bp::Filtering::isFiltred){
	 lst.at(2)->setChecked(true);
	}
	else {
	 lst.at(2)->setChecked(false);
	}
 }
}

QMenu *BMenu::mnu_Priority(stTbFiltres *ret, const etCount eSrc, const QTableView *view, const QModelIndex index)
{
Q_UNUSED(eSrc)
Q_UNUSED(view)
Q_UNUSED(index)

 QSqlQuery query(db_1) ;
 QString msg = "";

 QString msg2 = "Priorites";
 QMenu *menu =new QMenu(msg2);

 QAction *setForAll = menu->addAction("Tous",this,SLOT(slot_priorityForAll(bool)));
 setForAll->setCheckable(true);

 QActionGroup *grpPri = new  QActionGroup(menu);

 /// Total de priorite a afficher
 for(int i =1; i<=5;i++)
 {
  QAction *radio = new QAction(QString::number(i),grpPri);
	radio->setCheckable(true);
	menu->addAction(radio);
	grpPri->addAction(radio)->setData(i);
 }
 connect(grpPri,SIGNAL(triggered(QAction*)),this,SLOT(slot_ChoosePriority(QAction*)));

 if((*ret).pri>0)
 {
  QAction *uneAction;
  uneAction = qobject_cast<QAction *>(grpPri->children().at((*ret).pri-1));
  uneAction->setChecked(true);
 }

 return menu;
}

void BMenu::slot_isWanted(bool chk)
{
 QAction *chkFrom = qobject_cast<QAction *>(sender());

 if(chk){
  val.b_flt = val.b_flt | Bp::Filtering::isWanted;
 }
 else {
  val.b_flt = val.b_flt & ~Bp::Filtering::isWanted;
  val.b_flt = val.b_flt & ~Bp::Filtering::isChoosed;
  val.b_flt = val.b_flt & ~Bp::Filtering::isFiltred;
 }

 /// Mettre a jour action dans base
 if(setdbFlt(val)){
  chkFrom->setChecked(chk);
 }

}


void BMenu::slot_isChoosed(bool chk)
{
 QAction *chkFrom = qobject_cast<QAction *>(sender());

 if(chk){
  val.b_flt = val.b_flt | Bp::Filtering::isChoosed;
 }
 else {
  val.b_flt = val.b_flt & ~Bp::Filtering::isChoosed;
 }

 /// Mettre a jour action dans base
 if(setdbFlt(val)){
  chkFrom->setChecked(chk);
 }

}

void BMenu::slot_isFiltred(bool chk)
{
 QAction *chkFrom = qobject_cast<QAction *>(sender());

 if(chk){
  val.b_flt = val.b_flt | Bp::Filtering::isChoosed;
  val.b_flt = val.b_flt | Bp::Filtering::isFiltred;
 }
 else {
  val.b_flt = val.b_flt & ~Bp::Filtering::isFiltred;
 }

 /// Mettre a jour action dans base
 if(setdbFlt(val)){
  chkFrom->setChecked(chk);
 }

}

void BMenu::slot_priorityForAll(bool chk)
{
 QAction *chkFrom = qobject_cast<QAction *>(sender());
 chkFrom->setChecked(chk);
}

void BMenu::slot_ChoosePriority(QAction *cmd)
{
 /// https://stackoverflow.com/questions/9187538/how-to-add-a-list-of-qactions-to-a-qmenu-and-handle-them-with-a-single-slot
 bool isOk = true;
 QSqlQuery query(db_1);

 int value = cmd->data().toInt();

 /// Supprimer la priorite ?
 if(value==val.pri){
  value=0;
 }

 QString msg = "update  Filtres set pri="+QString::number(value)+
               " where("
               "id="+QString::number(val.id)+
               ");";

#ifndef QT_NO_DEBUG
 qDebug() << "msg: "<<msg;
#endif
 isOk = query.exec(msg);

 if(!isOk){
  DB_Tools::DisplayError("BMenu::slot_ChoosePriority",&query,msg);
  QMessageBox::warning(nullptr,"BMenu","slot_ChoosePriority",QMessageBox::Ok);
 }

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
				", flt="+QString::number(in.b_flt)+
				" where ("
				"zne="+QString::number(in.zne)+" and "+
				"typ="+QString::number(in.typ)+" and "+
				"lgn="+QString::number(in.lgn)+" and "+
				"col="+QString::number(in.col)+" and "+
				"val="+QString::number(in.val)+")";

 }
 else {
  /// Pas de resultat donc insert
  msg ="insert into "+in.tbName+
        " (id, zne, typ,lgn,col,val,pri,flt)"
        " values (NULL,"
        +QString::number(in.zne)+","
        +QString::number(in.typ)+","
        +QString::number(in.lgn)+","
        +QString::number(in.col)+","
        +QString::number(in.val)+","
        +QString::number(in.pri)+","
        +QString::number(in.b_flt)+")";
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
 Bp::Filterings my_flt = Bp::Filtering::isNotSet;


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
  (*ret).id = query_2.value("id").toInt();
  int valeur = query_2.value("flt").toInt();
  my_flt = static_cast<Bp::Filterings>(valeur);
  (*ret).b_flt = my_flt;
  (*ret).flt = query_2.value("flt").toInt();
  //(*ret).flt = query_2.value("flt").value<BFlags::Filtre>();
  (*ret).pri = query_2.value("pri").toInt();
 }

 (*ret).isPresent = isOk;
 (*ret).zne = zn;
 (*ret).typ = typ;
 (*ret).lgn = lgn;
 (*ret).col = col;
 (*ret).val = val;

 return isOk;
}

