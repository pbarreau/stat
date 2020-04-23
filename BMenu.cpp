#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QMetaType>

#include <QSortFilterProxyModel>

#include "BMenu.h"
#include "BFlags.h"
#include "db_tools.h"
#include "BTbView.h"

BMenu::BMenu(const QPoint pos, QString cnx,
						 const etCount eType, BTbView *view,
						 QWidget *parent):QMenu (parent)
{

 // Etablir connexion a la base
 db_menu = QSqlDatabase::database(cnx);
 if(db_menu.isValid()==false){
  QString str_error = db_menu.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }

 initialiser_v2(pos,eType,view);
}

void BMenu::initialiser_v1(const QPoint pos, const etCount eType, BTbView *view)
{

 eCalcul = eType;
 lview = view;
 index = view->indexAt(pos);

 val.tbName = "Filtres";
 val.id=-1;
 val.pri = -1;
 val.b_flt = Bp::F_Flt::noFlt;
 val.typ = eType;
 val.sta = Bp::E_Sta::noSta;
 val.zne = view->objectName().toInt();
 val.lgn=index.row();
 val.col =index.column();
 val.val = -1;

 /// Recuperer infos
 //DB_Tools::tbFltGet(&val,cnx);

 construireMenu();
}

void BMenu::initialiser_v2(const QPoint pos, const etCount eType, BTbView *view)
{
 val.tbName = "Filtres";
 val.b_flt = Bp::F_Flt::noFlt;
 val.pri = -1;
 val.id = -1;
 val.sta = Bp::E_Sta::noSta;
 val.typ = eType;
 val.zne = view->objectName().toInt();
 val.db_total = -1;

 lview = view;
 index = view->indexAt(pos);
 int cur_col = index.column();
 int cur_row = index.row();

 switch (val.typ) {
  case eCountElm:
  case eCountCmb:
  case eCountBrc:
   val.lgn = 10 * val.typ;
   val.col = cur_col;
   ///val.val = view->model()->index(cur_row,0).data().toInt();
   val.val = index.sibling(cur_row,0).data().toInt();
   break;

	case eCountGrp:
	 val.lgn = cur_row;
	 val.col = cur_col;
	 if(index.data().isValid()){
		if(index.data().isNull()){
		 val.val = -1;
		}
		else if(index.data().canConvert(QMetaType::Int)){
		 val.val=index.data().toInt();
		}
		else {
		 val.val=-2;
		}
	 }
	 else {
		val.val=-3;
	 }

	 break;

	default:
	 val.lgn = -1;
	 val.col = -1;
	 val.val = -4;
 }

 bool b_retVal = true;

 if(val.val > 0){
  /// regarder si connu
  b_retVal = DB_Tools::tbFltGet(&val,db_menu.connectionName());

	/// Verifier resultat
	if(b_retVal==false){
	 if(val.sta == Bp::E_Sta::Er_Result){
		b_retVal = DB_Tools::tbFltSet(&val,db_menu.connectionName());
	 }
	}
 }

 if(b_retVal==false){
  if(val.sta != Bp::E_Sta::Er_Result){
   DB_Tools::genStop("BMenu::initialiser_v2");
  }
 }
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

 if(chkShowMenu() == true){
  gererMenu_v2();
 }
 else {
  return;
 }
}

void BMenu::gererMenu_v1()
{
 QString msg = main_menu->title();
 QList<QAction *> lst = main_menu->actions();

 /// lecture info dans la base de la selection en cours
 bool b_retVal = getdbFlt(&val, eCalcul,lview,index);

 if(b_retVal==false){
  if(val.sta == Bp::E_Sta::Er_Result){
   b_retVal = DB_Tools::tbFltSet(&val,db_menu.connectionName());
  }
  else {
   DB_Tools::genStop("BMenu::slot_showMenu");
  }
 }


 /// On a trouve/cree une reponse
 if(b_retVal){

	/// wanted ?
	if((val.b_flt & Bp::F_Flt::fltWanted)== Bp::F_Flt::fltWanted){
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
	 //val.b_flt = val.b_flt & ~(Bp::F_Flt::fltFiltred);
	 val.b_flt = val.b_flt ^ Bp::F_Flt::fltFiltred;

	 lst.at(1)->setChecked(false);
	 //val.b_flt = val.b_flt & ~(Bp::F_Flt::fltSelected);
	 val.b_flt = val.b_flt ^ Bp::F_Flt::fltSelected;

	 lst.at(0)->setChecked(false);
	}

	/// --------- selected
	if((val.b_flt & Bp::F_Flt::fltSelected)== Bp::F_Flt::fltSelected){
	 lst.at(1)->setChecked(true);
	}
	else {
	 lst.at(1)->setChecked(false);
	}

	/// --------- filtred
	if((val.b_flt & Bp::F_Flt::fltFiltred)== Bp::F_Flt::fltFiltred){
	 lst.at(2)->setChecked(true);
	}
	else {
	 lst.at(2)->setChecked(false);
	}
 }
}

void BMenu::gererMenu_v2()
{
 bool    b_retVal = DB_Tools::tbFltGet(&val,db_menu.connectionName());

 if(b_retVal==false){
  if(val.sta == Bp::E_Sta::Er_Result){
   b_retVal = DB_Tools::tbFltSet(&val,db_menu.connectionName());
   if(b_retVal == true){
    presenterMenu();
   }
   else {
    DB_Tools::genStop("BMenu::gererMenu_v2");
   }
  }
  else {
   DB_Tools::genStop("BMenu::gererMenu_v2");
  }
 }
 else {
  presenterMenu();
 }

}

void BMenu::presenterMenu()
{
 construireMenu();
 QList<QAction *> lst = main_menu->actions();

 if((val.b_flt & Bp::F_Flt::fltWanted)== Bp::F_Flt::fltWanted){
  lst.at(0)->setChecked(true);
  lst.at(1)->setEnabled(true);
  lst.at(2)->setEnabled(true);

	if(val.typ == eCountElm){
	 QMenu *subMenu = mnu_Priority(&val, eCalcul,lview,index);
	 main_menu->addMenu(subMenu);
	}
 }
 else {
  lst.at(2)->setChecked(false);
  val.b_flt = val.b_flt & ~(Bp::F_Flt::fltFiltred);

	lst.at(1)->setChecked(false);
	val.b_flt = val.b_flt & ~(Bp::F_Flt::fltSelected);

  lst.at(0)->setChecked(false);
 }

 /// --------- selected
 if((val.b_flt & Bp::F_Flt::fltSelected)== Bp::F_Flt::fltSelected){
  lst.at(1)->setChecked(true);
 }
 else {
  lst.at(1)->setChecked(false);
 }

 /// --------- filtred
 if((val.b_flt & Bp::F_Flt::fltFiltred)== Bp::F_Flt::fltFiltred){
  lst.at(2)->setChecked(true);
 }
 else {
  lst.at(2)->setChecked(false);
 }

}

QMenu *BMenu::mnu_Priority(stTbFiltres *ret, const etCount eSrc, const BTbView *view, const QModelIndex index)
{
 Q_UNUSED(eSrc)
 Q_UNUSED(view)
 Q_UNUSED(index)

 QSqlQuery query(db_menu) ;
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
  val.b_flt = val.b_flt | Bp::F_Flt::fltWanted;
 }
 else {
  val.b_flt = val.b_flt & ~Bp::F_Flt::fltWanted;
  val.b_flt = val.b_flt & ~Bp::F_Flt::fltSelected;
  val.b_flt = val.b_flt & ~Bp::F_Flt::fltFiltred;
 }

 /// Mettre a jour action dans base
 if(DB_Tools::tbFltSet(&val,db_menu.connectionName()) == true){
  chkFrom->setChecked(chk);
 }

 lview->updateTitle();
}


void BMenu::slot_isChoosed(bool chk)
{
 QAction *chkFrom = qobject_cast<QAction *>(sender());

 if(chk){
  val.b_flt = val.b_flt | Bp::F_Flt::fltSelected;
 }
 else {
  val.b_flt = val.b_flt & ~Bp::F_Flt::fltSelected;
  val.b_flt = val.b_flt & ~Bp::F_Flt::fltFiltred;
 }

 /// Mettre a jour action dans base
 if(DB_Tools::tbFltSet(&val,db_menu.connectionName()) == true){
  chkFrom->setChecked(chk);
 }

 lview->updateTitle();
}

void BMenu::slot_isFiltred(bool chk)
{
 QAction *chkFrom = qobject_cast<QAction *>(sender());

 if(chk){
  val.b_flt = val.b_flt | Bp::F_Flt::fltSelected;
  val.b_flt = val.b_flt | Bp::F_Flt::fltFiltred;
 }
 else {
  val.b_flt = val.b_flt & ~Bp::F_Flt::fltFiltred;
 }

 /// Mettre a jour action dans base
 if(DB_Tools::tbFltSet(&val,db_menu.connectionName()) == true){
  chkFrom->setChecked(chk);
 }

 lview->updateTitle();
}

void BMenu::slot_priorityForAll(bool chk)
{
 QAction *chkFrom = qobject_cast<QAction *>(sender());
 chkFrom->setChecked(chk);
}

void BMenu::slot_ChoosePriority(QAction *cmd)
{
 /// https://stackoverflow.com/questions/9187538/how-to-add-a-list-of-qactions-to-a-qmenu-and-handle-them-with-a-single-slot
 bool b_retVal = true;
 QSqlQuery query(db_menu);

 int value = cmd->data().toInt();

 /// Supprimer la priorite ?
 if(value==val.pri){
  val.pri= 0;
 }
 else {
  val.pri=value;
 }

 b_retVal = DB_Tools::tbFltSet(&val,db_menu.connectionName());
}


bool BMenu::getdbFlt(stTbFiltres *val, const etCount in_typ, const BTbView *view, const QModelIndex index)
{
 bool b_retVal = false;
 //QSortFilterProxyModel *m = qobject_cast<QSortFilterProxyModel *>(view->model());

 val->typ = in_typ;

 val->zne = view->objectName().toInt();
 val->lgn = -1;
 val->col = -1;
 val->val = -1;
 val->db_total = -1;

 if(val->typ >= eCountToSet && val->typ <= eCountEnd){
  switch (val->typ) {
   case eCountElm:
   case eCountCmb:
   case eCountBrc:
    val->lgn = val->typ *10;
    val->col = index.model()->index(index.row(),0).data().toInt();
    val->val = val->col;
    break;
   case eCountGrp:
    val->lgn = index.row();
    val->col = index.column();
    if(index.model()->index(val->lgn,val->col).data().canConvert(QMetaType::Int)){
     val->val = index.model()->index(val->lgn,val->col).data().toInt();
    }
    break;
   case eCountToSet:
   case eCountEnd:
    break;
  }
 }
 else {
  val->typ = eCountToSet;
 }

 b_retVal = DB_Tools::tbFltGet(val,db_menu.connectionName());

 return b_retVal;

}

bool BMenu::chkShowMenu(void)
{
 bool b_retVal = false;

 int cur_col = index.column();
 int cur_row = index.row();

 switch (val.typ) {
  case eCountElm:
  case eCountCmb:
  case eCountBrc:
   if(index.column()==1){
    b_retVal = true;
   }
   else {
    b_retVal = false;
   }
   break;
  case eCountGrp:
   if((val.val >=0) && (index.column())>0){
    b_retVal = true;
   }
   else {
    b_retVal = false;
   }
   break;
  default:
   b_retVal = false;
 }
 return b_retVal;
}
