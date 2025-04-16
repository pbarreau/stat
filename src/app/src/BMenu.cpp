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
#include "BView_1.h"

BMenu::BMenu(const QPoint pos, const BFlt *conf, BView_1 *view):QMenu (nullptr),BFlt(*conf)
{
 db_menu = db_flt;
 addr = nullptr;
 if(displayTbvMenu_cell(pos,view)==true){
  addr = this;
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

 QAction *isUplets = main_menu->addAction("Uplets",this,SLOT(BSlot_isUplets(bool)));
 isUplets->setCheckable(false);
 isUplets->setEnabled(true);

}


void BMenu::BSlot_Menu_1()
{
 /// ce slot est appele apres le constructeur
 /// les verifications de base sont deja faites

 gererMenu_v2();
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

	if(inf_flt->typ == E_CountElm){
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

QMenu *BMenu::mnu_Priority(stTbFiltres *ret, const etCount eSrc, const BView_1 *view, const QModelIndex index)
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

void BMenu::BSlot_isUplets(bool chk)
{
 Q_UNUSED(chk)

 QItemSelectionModel *cur_sel = lview->selectionModel();
 QModelIndexList my_indexes = cur_sel->selectedIndexes();
 int len_data = my_indexes.size();
 BSig_MkUsrUplets_L1(cur_sel);
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
