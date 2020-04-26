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

BMenu::BMenu(const QPoint pos, const BFlt *conf, BTbView *view):QMenu (nullptr),BFlt(*conf)
{
 db_menu = db_flt;
 addr = nullptr;
 if(displayTbvMenu_cell(pos,view)==true){
  addr = this;
 }
}

#if 0
void BMenu::initialiser_v2(const QPoint pos, const etCount eType, BTbView *view)
{
 inf_flt->tb_flt = "Filtres";
 inf_flt->b_flt = Bp::F_Flt::noFlt;
 inf_flt->pri = -1;
 inf_flt->id = -1;
 inf_flt->sta = Bp::E_Sta::noSta;
 inf_flt->typ = eType;
 inf_flt->zne = view->objectName().toInt();
 inf_flt->dbt = -1;

 lview = view;
 index = view->indexAt(pos);
 int cur_col = index.column();
 int cur_row = index.row();

 switch (inf_flt->typ) {
  case eCountElm:
  case eCountCmb:
  case eCountBrc:
   if(cur_col){
    inf_flt->lgn = 10 * inf_flt->typ;
    inf_flt->col = index.sibling(cur_row,0).data().toInt();
    inf_flt->val = inf_flt->col;
    ///inf_flt->val = view->model()->index(cur_row,0).data().toInt();
    ///inf_flt->val = index.sibling(cur_row,0).data().toInt();
   }
   break;

	case eCountGrp:
	 inf_flt->lgn = cur_row;
	 inf_flt->col = cur_col;
	 if(index.data().isValid()){
		if(index.data().isNull()){
		 inf_flt->val = -1;
		}
		else if(index.data().canConvert(QMetaType::Int)){
		 inf_flt->val=index.data().toInt();
		}
		else {
		 inf_flt->val=-2;
		}
	 }
	 else {
		inf_flt->val=-3;
	 }

	 break;

	default:
	 inf_flt->lgn = -1;
	 inf_flt->col = -1;
	 inf_flt->val = -4;
 }

 bool b_retVal = true;

 if(inf_flt->val > 0){
  /// regarder si connu
  b_retVal = DB_Tools::tbFltGet(inf_flt,db_menu.connectionName());

	/// Verifier resultat
	if(b_retVal==false){
	 if(inf_flt->sta == Bp::E_Sta::Er_Result){
		b_retVal = DB_Tools::tbFltSet(inf_flt,db_menu.connectionName());
	 }
	}
 }

 if(b_retVal==false){
  if(inf_flt->sta != Bp::E_Sta::Er_Result){
   DB_Tools::genStop("BMenu::initialiser_v2");
  }
 }
}
#endif

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

void BMenu::gererMenu_v2()
{
 bool    b_retVal = DB_Tools::tbFltGet(inf_flt,db_menu.connectionName());

 if(b_retVal==false){
  if(inf_flt->sta == Bp::E_Sta::Er_Result){
   b_retVal = DB_Tools::tbFltSet(inf_flt,db_menu.connectionName());
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

 if((inf_flt->b_flt & Bp::F_Flt::fltWanted)== Bp::F_Flt::fltWanted){
  lst.at(0)->setChecked(true);
  lst.at(1)->setEnabled(true);
  lst.at(2)->setEnabled(true);

	if(inf_flt->typ == eCountElm){
	 QMenu *subMenu = mnu_Priority(inf_flt, inf_flt->typ,lview,index);
	 main_menu->addMenu(subMenu);
	}
 }
 else {
  lst.at(2)->setChecked(false);
  inf_flt->b_flt = inf_flt->b_flt & ~(Bp::F_Flt::fltFiltred);

	lst.at(1)->setChecked(false);
	inf_flt->b_flt = inf_flt->b_flt & ~(Bp::F_Flt::fltSelected);

  lst.at(0)->setChecked(false);
 }

 /// --------- selected
 if((inf_flt->b_flt & Bp::F_Flt::fltSelected)== Bp::F_Flt::fltSelected){
  lst.at(1)->setChecked(true);
 }
 else {
  lst.at(1)->setChecked(false);
 }

 /// --------- filtred
 if((inf_flt->b_flt & Bp::F_Flt::fltFiltred)== Bp::F_Flt::fltFiltred){
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

 //QAction *setForAll = menu->addAction("Tous",this,SLOT(slot_priorityForAll(bool)));
 //setForAll->setCheckable(true);

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
  inf_flt->b_flt = inf_flt->b_flt | Bp::F_Flt::fltWanted;
 }
 else {
  inf_flt->b_flt = inf_flt->b_flt & ~Bp::F_Flt::fltWanted;
  inf_flt->b_flt = inf_flt->b_flt & ~Bp::F_Flt::fltSelected;
  inf_flt->b_flt = inf_flt->b_flt & ~Bp::F_Flt::fltFiltred;
 }

 /// Mettre a jour action dans base
 if(DB_Tools::tbFltSet(inf_flt,db_menu.connectionName()) == true){
  chkFrom->setChecked(chk);
 }

 lview->updateTitle();
}


void BMenu::slot_isChoosed(bool chk)
{
 QAction *chkFrom = qobject_cast<QAction *>(sender());

 if(chk){
  inf_flt->b_flt = inf_flt->b_flt | Bp::F_Flt::fltSelected;
 }
 else {
  inf_flt->b_flt = inf_flt->b_flt & ~Bp::F_Flt::fltSelected;
  inf_flt->b_flt = inf_flt->b_flt & ~Bp::F_Flt::fltFiltred;
 }

 /// Mettre a jour action dans base
 if(DB_Tools::tbFltSet(inf_flt,db_menu.connectionName()) == true){
  chkFrom->setChecked(chk);
 }

 lview->updateTitle();
}

void BMenu::slot_isFiltred(bool chk)
{
 QAction *chkFrom = qobject_cast<QAction *>(sender());

 if(chk){
  inf_flt->b_flt = inf_flt->b_flt | Bp::F_Flt::fltSelected;
  inf_flt->b_flt = inf_flt->b_flt | Bp::F_Flt::fltFiltred;
 }
 else {
  inf_flt->b_flt = inf_flt->b_flt & ~Bp::F_Flt::fltFiltred;
 }

 /// Mettre a jour action dans base
 if(DB_Tools::tbFltSet(inf_flt,db_menu.connectionName()) == true){
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
 if(value==inf_flt->pri){
  inf_flt->pri= 0;
 }
 else {
  inf_flt->pri=value;
 }

 b_retVal = DB_Tools::tbFltSet(inf_flt,db_menu.connectionName());
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
 val->dbt = -1;

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

 switch (inf_flt->typ) {
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
   if((inf_flt->val >=0) && (index.column())>0){
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
