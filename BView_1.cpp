#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QSortFilterProxyModel>
#include <QSqlQueryModel>

#include <QHeaderView>
#include <QVBoxLayout>

#include <QMessageBox>
#include <QToolTip>
#include <QList>
#include <QTime>
#include <QEvent>

#include <QApplication>
#include <QGuiApplication>

#include "BFdj.h"
#include "BTirGen.h"
#include "BView_1.h"
#include "BMenu.h"
#include "BTirAna.h"
#include "db_tools.h"

QWidget * BView_1::wdg_reponses = nullptr;
QGridLayout * BView_1::gdl_all = nullptr;
QList<QGridLayout *> *BView_1::gdl_list = nullptr;

QTabWidget * BView_1::tbw_calculs = nullptr;

BView_1::BView_1(const stGameConf *pGame, int in_zn, etCount in_typ)
 :BView(nullptr),BFlt(pGame, in_zn, in_typ), gme_current(pGame)
{
 db_tbv = db_flt;
 lbflt = cur_bflt;
 lview = this;

 setZid(in_zn);

 square = new BGpbMenu(cur_bflt, this);

 if(wdg_reponses == nullptr){
  /// Encapsulation sur cet objet
  connect(square,
          SIGNAL(sig_ShowMenu(const QGroupBox *, const QPoint)),
          square,
          SLOT(slot_ShowMenu(const QGroupBox *, const QPoint)));

  this->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(QPoint)),
          this, SLOT(slot_V2_ccmr_SetPriorityAndFilters(QPoint)));
 }

 this->setMouseTracking(true);
 connect(this,
         SIGNAL(entered(QModelIndex)),this,SLOT(BSlot_ShowToolTip(QModelIndex)));

 /// Suivit du click
 connect(this,SIGNAL(pressed(const QModelIndex)), this, SLOT(BSlot_MousePressed(const QModelIndex)));

 up = nullptr;
 btn_usrGame = nullptr;

#if 0
 if((tbw_calculs == nullptr) || (wdg_reponses == nullptr) || (gdl_all == nullptr)){
  wdg_reponses = new QWidget;
  tbw_calculs = new QTabWidget;
  gdl_all = new QGridLayout;
  gdl_list = new QList<QGridLayout *>;
 }
#endif
}

BView_1::~BView_1()
{
 ///appel du parent ligne suivante inutile
 ///delete square;
}

void BView_1::BSlot_MkUsrUplets_L1(const QItemSelectionModel *cur_sel)
{
 emit BSig_MkUsrUplets_L2(cur_sel);
}

void BView_1::slot_V2_ccmr_SetPriorityAndFilters(QPoint pos)
{
 /// http://www.qtcentre.org/threads/7388-Checkboxes-in-menu-items
 /// https://stackoverflow.com/questions/2050462/prevent-a-qmenu-from-closing-when-one-of-its-qaction-is-triggered

 BMenu *a = new BMenu (pos, cur_bflt, this);

 if(a->addr != nullptr){
  connect(a,SIGNAL(aboutToShow()), a, SLOT(BSlot_Menu_1()));
  connect(a,SIGNAL(BSig_MkUsrUplets_L1(const QItemSelectionModel*)),
          this,SLOT(BSlot_MkUsrUplets_L1(const QItemSelectionModel*)));
  a->exec(this->viewport()->mapToGlobal(pos));
 }
 else {
  delete a;
 }
}

stTbFiltres * BView_1::getFlt(void)
{
 return inf_flt;
}

Bp::E_Col BView_1::getSortCol(void)
{
 return colTotal;
}

void BView_1::BSlot_ShowToolTip(const QModelIndex & index)
{
 /// https://doc.qt.io/qt-5/qtooltip.html
 /// https://stackoverflow.com/questions/34197295/how-to-change-the-background-color-of-qtooltip-of-a-qtablewidget-item

 QString msg="";
 const QAbstractItemModel * pModel = index.model();
 int col = index.column();

 QVariant vCol = pModel->headerData(col,Qt::Horizontal);
 QString headTop= "";
 QString headRef = "";

 int start = Bp::colTxt;
 if(inf_flt->typ == eCountGrp){
  start = Bp::colId;
  vCol = pModel->headerData(col,Qt::Horizontal,Qt::ToolTipRole);
 }

 headRef = pModel->headerData(start,Qt::Horizontal).toString();
 headTop = vCol.toString();
 QString s_va = "";
 if ((col > start) &&
     (s_va = index.model()->index(index.row(),col).data().toString()) !="" )
 {
  QString s_nb = index.model()->index(index.row(),start).data().toString();
  QString s_hd = headTop;
  msg = msg + QString("Quand %1=%2,%3=%4 tirage(s)").arg(headRef).arg(s_nb).arg(s_hd).arg(s_va);
 }

 QToolTip::showText (QCursor::pos(), msg);
}

#if 1
QString BView_1::mkTitle(int zn, etCount E_Calcul, QTableView *view)
{
 QString title = "";
 return title;
}

#else
QString BTbView::mkTitle(int zn, etCount E_Calcul, QTableView *view)
{
 QSortFilterProxyModel *m = qobject_cast<QSortFilterProxyModel *>(view->model());
 QSqlQueryModel  * sqm_tmp = qobject_cast<QSqlQueryModel  *>(m->sourceModel());

 QString title = "";
 QString ref_info = "Initial : %1, R(%1), C(%1), F(%1), Disponible(s) : %1";
 QStringList tmp_lst = ref_info.split(",");

 /// Choix usr ou fdj ?
 if(inf_flt->tb_ref.compare("B_fdj")!=0){
  title = "Repartition : ";
  return title;
 }

 Bp::F_Flts msk = Bp::F_Flt::fltWanted|Bp::F_Flt::fltSelected|Bp::F_Flt::fltFiltred;
 Bp::F_Flts keys[]={Bp::F_Flt::fltWanted,Bp::F_Flt::fltSelected,Bp::F_Flt::fltFiltred};
 int nb_items = sizeof (keys)/sizeof(Bp::F_Flts);

 /// Necessaire pour compter toutes les lignes de reponses
 while (sqm_tmp->canFetchMore())
 {
  sqm_tmp->fetchMore();
 }
 /// Determination nb ligne par proxymodel
 int nb_lgn_ftr = m->rowCount();
 if(E_Calcul==eCountGrp){
  int count=view->horizontalHeader()->count();
  nb_lgn_ftr = nb_lgn_ftr * count;
 }

 title = tmp_lst[0].arg(nb_lgn_ftr)+", ";

 /// interrogation table Filtres
 /*
  * Select count(id) as T from Filtres
  * where (
  * (zne=0) and
  * (typ=1) and
  * ( ((flt & 0x1C) & 0x4) = 0x4))
  */

 bool b_retVal = true;
 QSqlQuery query(db_tbv);
 int memo_choisi = -1;
 QString tblFlt = inf_flt->tb_flt;
 QString msg = "";
 QString msg_1 ="Select count(id) as T from "+
                tblFlt+
                " where ( (zne="+
                QString::number(zn)+
                ") and (typ="+
                QString::number(E_Calcul)+
                ")";

 for(int i = 0; (i< nb_items) && b_retVal ; i++){
  QString msg_2 = " and ( ((flt & 0x"+
                  QString::number(msk,16).toUpper()+
                  ") & 0x"+
                  QString::number(keys[i],16).toUpper()+
                  ") = 0x"+
                  QString::number(keys[i],16).toUpper()+
                  "))";

  msg= msg_1+msg_2;

#ifndef QT_NO_DEBUG
	qDebug()<< "msg_1:"<<msg_1;
	qDebug()<< "msg_2:"<<msg_2;
	qDebug()<< "msg:"<<msg;
#endif

	b_retVal = query.exec(msg);

	int total = 0;
	if(query.first()){
	 total = query.value(0).toInt();

	 /// Verification nb choix suffisant
	 /// pour creer liste de jeux
	 if((inf_flt->typ == eCountElm) && (keys[i]== Bp::fltSelected))
	 {
		if(zn == 0){
		 bool activate = false;
		 if(total>= cur_game->limites[zn].win){
			/// Activer le bouton creer liste !!!
			activate = true;
		 }
		 else {
			activate = false;
		 }
		 if(btn_usrGame != nullptr){
			btn_usrGame->setEnabled(activate);
		 }
		}
	 }

	}
	title = title + tmp_lst[i+1].arg(total)+", ";

	if(i==1){
	 memo_choisi = total;
	}

 }
 title = title+ tmp_lst[tmp_lst.size()-1].arg(nb_lgn_ftr-memo_choisi);

 if(!b_retVal){
  DB_Tools::DisplayError("BCount::mkTitle",&query,msg);
  QMessageBox::warning(nullptr,"BCount","mkTitle",QMessageBox::Ok);
 }

 QString st_total = ref_info.arg(nb_lgn_ftr);

 return title;
}
#endif

void BView_1::updateTitle(QString a_tile)
{
 /// Mettre ensuite l'analyse du marquage
 QString st_total = "";

 if(a_tile.size() != 0 ){
  st_total = a_tile;
 }
 else{
  st_total = mkTitle(inf_flt->zne,inf_flt->typ,this);
 }

 this->setTitle(st_total);
 /*
 if(square != nullptr){
  square->setTitle(st_total);
 }
*/
}

/*
void  BTbView::slot_ShowMenu(const QGroupBox *cible)
{
 menu->popup(cible->mapToGlobal(pos()));
}

void BTbView::mousePressEvent ( QMouseEvent * event )

{
 emit sig_ShowMenu(myGpb);
}
*/

void BView_1::construireMenu(void)
{

 menu = new QMenu();

 QAction *isWanted = menu->addAction("Reserver",this,SLOT(slot_isWanted(bool)));
 isWanted->setCheckable(true);
 isWanted->setEnabled(true);

 QAction *isChoosed = menu->addAction("Choisir",this,SLOT(slot_isChoosed(bool)));
 isChoosed->setCheckable(true);
 isChoosed->setDisabled(true);

 QAction *isFiltred = menu->addAction("Filtrer",this,SLOT(slot_isFiltred(bool)));
 isFiltred->setCheckable(true);
 isFiltred->setDisabled(true);
}

/*
BGpbMenu *BTbView::getGpb()
{
 return square;
}
*/

void BView_1::setUpLayout(QLayout *usr_up)
{
 up = usr_up;
}

void BView_1::setParentLayout(QLayout *parent_lay)
{
 parent_layout = parent_lay;
}

QLayout * BView_1::getParentLayout()
{
 return parent_layout;
}

/// Cette fonction lie le bouton qui permettra de creer
/// les jeux utilisateur
void BView_1::setUsrGameButton(QPushButton *usr_game)
{
 btn_usrGame = usr_game;
}

QPushButton * BView_1::getUsrGameButton(void)
{
 return btn_usrGame;
}
void BView_1::setRowModelCount(int nb)
{
 rowModelCount = nb;
}
void BView_1::setRowSourceModelCount(int nb)
{
 rowSourceModelCount = nb;
}

bool BView_1::isOnUsrGame(void)
{
 bool b_retVal;

 if(gme_current->db_ref->src.compare("B_fdj")==0){
  b_retVal = false;
 }
 else {
  b_retVal = true;
 }

 return b_retVal;
}

void  BView_1::BSlot_MakeCustomGame()
{
 QCursor myCursor = QCursor(Qt::BusyCursor);
 QApplication::setOverrideCursor(myCursor);
 QGuiApplication::changeOverrideCursor(myCursor);

 /// https://stackoverflow.com/questions/244646/get-elapsed-time-in-qt
 /// Temps de calcul
 QTime r;
 QTime t;
 QString t_human = "";
 QString gme_tbl = "";
 QItemSelectionModel *mysel = this->selectionModel();
 QModelIndexList lst_cells = mysel->selectedIndexes();
 int size = lst_cells.size();

 /// On cree une config adapte a notre custom
 stGameConf *tmp_usr = new stGameConf;
 *tmp_usr = *gme_current;
 tmp_usr->znCount = 1;
 tmp_usr->eTirType= eTirGen;

 r.setHMS(0,0,0,0);
 t.start();
 BTirGen *lst_tirages = new BTirGen(tmp_usr);
 r = r.addMSecs(t.elapsed());
 t_human = r.toString("hh:mm:ss:zzz");

 stGameConf * tmp = lst_tirages->getGameConf();
 if(tmp != nullptr){
  gme_tbl = lst_tirages->getGameLabel();

  saveTimeInTable(Bp::clkStart,gme_tbl,t_human);

  r.setHMS(0,0,0,0);
  t.restart();
  BTirAna *ana_tirages = new BTirAna(tmp);
  r = r.addMSecs(t.elapsed());
  t_human = r.toString("hh:mm:ss:zzz");
  saveTimeInTable(Bp::clkStop,gme_tbl,t_human);

  if(ana_tirages->self() == nullptr){
   QString msg = "Erreur de l'analyse des tirages :" + tmp->db_ref->src;
   QMessageBox::warning(nullptr, "Analyses", msg,QMessageBox::Yes);
   delete ana_tirages;
  }
  else {
   lst_tirages->showGen(ana_tirages);
  }
 }
 else {
  delete lst_tirages;
 }
 QApplication::restoreOverrideCursor();
}

void BView_1::saveTimeInTable(Bp::E_Clk ref, QString tb_name, QString humanTime)
{
 QSqlQuery query(db_tbv);
 bool b_retVal = true;
 QString time_col = "";

 if(ref==Bp::clkStart){
  time_col = "t1";
 }
 else {
  time_col = "t2";
 }

 QString msg = "update E_lst set " +
               time_col+"='"+humanTime
               +"' where( (name='"+tb_name+"') and (type='"+
               TXT_TirDef[eTirGen]+"'))";

 b_retVal = query.exec(msg);
}


void BView_1::agencerResultats(BTirages *lst, BTirAna* ana)
{
 /// Verifier premier passage
 if((tbw_calculs == nullptr) || (wdg_reponses == nullptr) || (gdl_all == nullptr)){
  wdg_reponses = new QWidget;
  tbw_calculs = new QTabWidget;
  gdl_all = new QGridLayout;
  gdl_list = new QList<QGridLayout *>;
 }

 QGridLayout *tmp_layout = new QGridLayout;
 gdl_list->append(tmp_layout);

 QSpacerItem *ecart = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);

 tmp_layout->addWidget(lst,0,0,2,1);///,Qt::AlignTop|Qt::AlignLeft
 tmp_layout->addWidget(ana,0,1,1,1);

 tmp_layout->addItem(ecart,1,1);
 tmp_layout->setRowStretch(0,10);
 tmp_layout->setRowStretch(1,10);
 tmp_layout->setColumnStretch(1, 10); /// Exemple basic layouts
 tmp_layout->setColumnStretch(2, 20);

 QWidget * tmp = new QWidget;
 tmp->setLayout(tmp_layout);

 QString name = lst->getGameLabel();
 tbw_calculs->addTab(tmp, name);
 gdl_all->addWidget(tbw_calculs);
 wdg_reponses->setLayout(gdl_all);


 name = "Resultats : " + BFdj::getCurDbFile();
 wdg_reponses->setWindowTitle(name);
 wdg_reponses->show();
}

#if 0
void BView_1::activateTargetTab(QString id)
{
 QTabWidget * tmp = f;

 int target = -1;
 for(int i = 0; i<tbw_BtirCalculs->count();i++){
  QString label = tbw_BtirCalculs->tabText(i);
  if(label == id ){
   target = i;
  }
 }
 if(target >= 0){
  tbw_calculs->setCurrentIndex(target);
  wdg_reponses->show();
 }
}
#endif
void BView_1::addSubFlt(int id, QWidget *tab)
{
 gdl_list->at(id-1)->addWidget(tab,1,1);
}

void BView_1::addSpacer(int id, QSpacerItem *space)
{
 gdl_list->at(id-1)->addItem(space,1,1);
}

void BView_1::showUsrGame(QWidget * une_selection, QString name)
{
 /*
 if(tab_usrGame != nullptr){
  int item = BTirAna::getCounter() -1;
  QString titre = QString::number(item).rightJustified(2,'0') + " : " + name;

  tab_usrGame->addTab(une_selection, titre);
  tab_usrGame->show();

 }
*/
}

/*
QGroupBox * BTbView::getScreen()
{
 /// Mettre ensuite l'analyse du marquage
 updateTitle();

 QVBoxLayout *layout = new QVBoxLayout;
 if(up != nullptr){
  ///layout->addLayout(up, Qt::AlignCenter|Qt::AlignTop);
  this->addUpLayout(up);
 }
 layout->addWidget(this, Qt::AlignCenter|Qt::AlignTop);
 square->setLayout(layout);

 return square;
}
*/

void BView_1::BSlot_TrackSelection(const QItemSelection &cur, const QItemSelection &last)
{
 QList<QModelIndex> indexes_1 = cur.indexes();
 QList<QModelIndex> indexes_2 = last.indexes();

 QItemSelectionModel *sel = qobject_cast<QItemSelectionModel *>(sender());
 int total = sel->selectedIndexes().size();

 QString title = "Selection : " + QString::number(total).rightJustified(2,'0')+"/"+QString::number(rowModelCount);
 this->setTitle(title);

 if((inf_flt->typ == eCountElm) && (inf_flt->zne == 0))
 {
  bool activate = false;
  if(total >= gme_current->limites[0].win){
   /// Activer le bouton creer liste !!!
   activate = true;
  }
  else {
   activate = false;
  }
  if(btn_usrGame != nullptr){
   btn_usrGame->setEnabled(activate);
  }
 }
}

void BView_1::BSlot_MousePressed(const QModelIndex &index)
{
 BView_1 *view = qobject_cast<BView_1 *>(sender());
 QItemSelectionModel *selectionModel = view->selectionModel();

 int col = index.column();
 bool do_return = false;

 /// verifier la selection demandee
 if(index == QModelIndex()){
  return;
 }

 if((do_return == false) && index.data().isNull()){
  do_return = true;
 }

 if((do_return == false) && (inf_flt->typ !=eCountGrp) && (col != Bp::colTxt)){
  do_return = true;
 }

 if ((do_return == false) && (col == Bp::colId)) {
  do_return = true;
 }

 int total = selectionModel->selectedIndexes().size();
 if((do_return == false) && (!total)){
  do_return = true;
 }

 if(do_return)
 {
  selectionModel->select(index,QItemSelectionModel::SelectionFlag::Deselect);

  //selectionModel->clearSelection();
  return;
 }

 //emit bsg_clicked(index, inf_flt->zne, inf_flt->typ);
}
