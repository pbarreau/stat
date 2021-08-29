#ifndef QT_NO_DEBUG
#include <QDebug>
#include "BTest.h"
#endif

#include <QSqlQuery>
#include <QSqlError>
#include <QStandardItemModel>

#include <QMessageBox>
#include <QHeaderView>
//#include <QButtonGroup>
#include <QToolBar>
#include <QAction>

#include "BTirages.h"
#include "Bc.h"
#include "BTirAna.h"
#include "BCustomPlot.h"

#include "BGraphicsView.h"
#include "db_tools.h"

int BTirages::cnt_tirSrc = 1; /// Compteur des sources de tirages
QString  BTirages::tbw_TbvTirages = "tbw_TirLst";
QString  BTirages::tbw_FltTirages = "tbw_TirFlt";

static QString  lab_ong = "R_%1";

//QList<QGridLayout *> *BTirages::gdl_list = nullptr;

QTabWidget * BTirages::tbw_calculs = nullptr;
QGridLayout * BTirages::gdl_all = nullptr;
QWidget * BTirages::wdg_reponses = nullptr;

BTirages::BTirages(const stGameConf *pGame, etTir gme_tir, QWidget *parent)
 : QWidget(parent),gme_cnf(pGame),eTir(gme_tir)
{
 QString cnx=pGame->db_ref->cnx;
 //gme_cnf->eTirType = gme_tir;

 // Etablir connexion a la base
 db_tir = QSqlDatabase::database(cnx);
 if(db_tir.isValid()==false){
  QString str_error = db_tir.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }

 game_lab = pGame->db_ref->src;
 lst_tirages = "";
 id_TirSrc = cnt_tirSrc;
 id_AnaSel = 0;
 usr_flt_counter = 0;
 og_AnaSel = nullptr; /// og: Onglet
 ana_TirLst = nullptr;

}

BView *BTirages::getTbvTirages()
{
 return tir_tbv;
}

void BTirages::showFdj(BTirAna *ana_tirages)
{
 QWidget *wdg_visual = new QWidget;
 QGridLayout *lay_visual = new QGridLayout;
 QTabWidget *tbw_visual = new QTabWidget;

 lay_visual->addWidget(this,0,0,2,1);

 QWidget *wdg_tmp = new QWidget;
 lay_fusion = new QGridLayout;
 lay_fusion->addWidget(ana_tirages,0,0);
 wdg_tmp->setLayout(lay_fusion);
 tbw_visual->addTab(wdg_tmp,"Nombres");

 QString ongNames[]={"Graphiques"};
 int nb_ong = sizeof(ongNames)/sizeof(QString);
 QWidget * (BTirages::*ptrFunc[])()=
 {
   &BTirages::Dessine
};

 wdg_tmp = nullptr;
 for (int ong_id=0;ong_id<nb_ong;ong_id++) {
  wdg_tmp = (this->*ptrFunc[ong_id])();
  if(wdg_tmp !=nullptr){
   tbw_visual->addTab(wdg_tmp,ongNames[ong_id]);
  }
 }



 lay_visual->addWidget(tbw_visual,0,1,1,2);
 lay_visual->setColumnStretch(0, 0); /// Exemple basic layouts
 lay_visual->setColumnStretch(1, 1);

 wdg_visual->setLayout(lay_visual);
 wdg_visual->setWindowTitle("Tirages FDJ : ");
 wdg_visual->show();

 connect(this,SIGNAL(BSig_AnaLgn(int,int)), ana_tirages,SLOT(BSlot_AnaLgnShow(int,int)));
 connect(this,SIGNAL(BSig_Show_Flt(const B2LstSel *)), ana_tirages,SLOT(BSlot_Show_Flt(const B2LstSel *)));
 connect(ana_tirages, SIGNAL(BSig_FilterRequest(BTirAna *, const Bp::E_Ico , const B2LstSel * )),
         this, SLOT(BSlot_Filter_Tir(BTirAna *, const Bp::E_Ico , const B2LstSel *)));
}

QWidget * BTirages::Dessine()
{
 QWidget *wdg_tmp = new QWidget;
 QGridLayout *lay_visual = new QGridLayout;

 graphAnaLgn = nullptr;

 QTabWidget *tbl = getTabedDetails();

 BGraphicsView * left = selGraphTargets();

 lay_visual->addWidget(tbl,0,0);
 lay_visual->addWidget(left,1,0);
 lay_visual->setRowStretch(0,1);
 lay_visual->setRowStretch(1,5);

 /*
 lay_visual->addWidget(tbw_dessins,0,1,1,2);
 lay_visual->setColumnStretch(0, 0); /// Exemple basic layouts
 lay_visual->setColumnStretch(1, 1);
 */

 wdg_tmp->setLayout(lay_visual);

 return wdg_tmp;
}

void BTirages::DrawCustomPlot()
{
 int tot_zn = gme_cnf->znCount;

 QWidget *wdg_toShow = new QWidget;
 QGridLayout *lay_visual = new QGridLayout;
 QTabWidget * try_01 = new QTabWidget;


 /// pour chacune des cles faire le graphe de chacune des zones
 for(int zn=0; zn<tot_zn;zn++){
  QStringList keys = gme_cnf->slFlt[zn][Bp::colDefTitres];
  QStringList tips = gme_cnf->slFlt[zn][Bp::colDefToolTips];

  QWidget *wdg_tmp = new QWidget;
  wdg_tmp->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

  QScrollArea *scrol_2 = new QScrollArea();
  scrol_2->setBackgroundRole(QPalette::Window);
  scrol_2->setFrameShadow(QFrame::Plain);
  scrol_2->setFrameShape(QFrame::NoFrame);
  scrol_2->setWidgetResizable(true);

  QVBoxLayout *vb_2 = new QVBoxLayout ();
  wdg_tmp->setLayout(vb_2);

  int max_keys = keys.size();
  for(int a_key=0;a_key<max_keys; a_key++){
   QString stKey = keys.at(a_key);
   QString stTip = tips.at(a_key);

   if(stTip.contains("special")){
    continue;
   }

   QHBoxLayout *layout = new QHBoxLayout();
   vb_2->addLayout(layout);

   BCustomPlot *monTest = new BCustomPlot(gme_cnf, tir_tbv, zn,stKey,stTip);
   monTest->setFixedHeight(200);
   layout->addWidget(monTest);
  }
  scrol_2->setWidget(wdg_tmp);
  try_01->addTab(scrol_2,gme_cnf->names[zn].std);
 }


 lay_visual->addWidget(try_01,0,0);
 wdg_toShow->setLayout(lay_visual);
 wdg_toShow->show();
}


BGraphicsView *BTirages::selGraphTargets()
{

 /// -----------------
 /// TEST BCustomPlot
 DrawCustomPlot();
 /// ----------------


 BGraphicsView *tmp_view = new BGraphicsView(gme_cnf, tir_tbv);

 int nb_zn = gme_cnf->znCount;
 for (int zn=0;zn<nb_zn;zn++) {
  QStringList *items = gme_cnf->slFlt[zn];
  QStringList lst_courbes = items[Bp::colDefTitres];
  int nb_courbes = lst_courbes.size();

  for (int id_courbe=0;id_courbe<nb_courbes;id_courbe++) {
   tmp_view->DessineCourbeSql(gme_cnf, zn, id_courbe);
  }
 }

 grp_screen = tmp_view;

 return tmp_view;
}

QTabWidget *BTirages::getTabedDetails()
{
 QTabWidget *tbw_zones = new QTabWidget;

#if 0
 int nb_zn = gme_cnf->znCount;

 if(graphAnaLgn == nullptr){
  graphAnaLgn = new BView * [nb_zn];
 }


 for (int zn=0;zn<nb_zn;zn++) {
  QString title = "Ong-"+gme_cnf->names[zn].abv;
  QWidget * tbl = usr_GrpTb2(zn);
  ret->addTab(tbl,title);
 }
#endif

 /// -------------
 int nb_zn = gme_cnf->znCount;
 for (int zn=0;zn<nb_zn;zn++) {
  QString title = gme_cnf->names[zn].abv;
  QToolBar *bar = new QToolBar("Courbes",tbw_zones);
  //bar->setMovable(true);
  //bar->setFloatable(true);
  bar->setOrientation(Qt::Orientation::Horizontal);
  bar->setObjectName(QString::number(zn));

  QStringList *items = gme_cnf->slFlt[zn];
  QStringList keys = items[Bp::colDefTitres];
  QStringList info = items[Bp::colDefToolTips];

  int nb_keys = keys.size();
  for (int a_key=0;a_key<nb_keys;a_key++) {
   QString label = keys[a_key];
   label = label.toUpper();
   if(label.contains(",")){
    QStringList items = label.split(",");
    label = items[0];
   }
   if(a_key == nb_keys-1){
    label = "C";
   }
   QAction *tmp = bar->addAction(label,this,SLOT(BSlot_Dessine(bool )));
   tmp->setParent(bar);
   tmp->setCheckable(true);
   if(info[a_key].contains("special")==true){
    QStringList tips = info[a_key].split(",");
    tmp->setToolTip(tips[1]);
   }
   else{
    tmp->setToolTip(info[a_key]);
   }
   tmp->setData(a_key);

   ///tmp_view->DessineCourbeSql(gme_cnf, zn, a_key);
  }
  tbw_zones->addTab(bar,title);
 }

 /// -------------

 return tbw_zones;
}

QWidget * BTirages::usr_GrpTb2(int zn)
{
 /// Creation d'un bandeau pour selection utilisateur
 /*
  *
  * select t1.tip as C, printf("%.2f",bc) as bc, p,g
  * from (B_cmb_z1) as t1, (B_ana_z1) as t2
  * where ((t2.id=1) and (t1.id=t2.idComb))
  *
  */

 graphAnaLgn[zn] = new BView;

 int l_id = -1;
 //QString sql_msg = getSqlForLine(l_id, zn);
 QSqlQueryModel  * sqm_tmp = new QSqlQueryModel;
 //sqm_tmp->setQuery(sql_msg, db_tir);
 graphAnaLgn[zn]->setModel(sqm_tmp);

 //showLineDetails(graphAnaLgn, zn, l_id, sql_msg);

 return graphAnaLgn[zn]->getScreen();
}

/*
QString BTirages::getSqlForLine(const int &l_id,int zn)
{
 QString sql_msg = "";

 QString lst_cols = BTirAna::getFilteringHeaders(gme_cnf,zn);


 QString tbLabAna = "";
 if(gme_cnf->db_ref->src.compare("B_fdj")==0){
  tbLabAna = "B";
 }
 else{
  tbLabAna = gme_cnf->db_ref->src;
 }
 tbLabAna = tbLabAna +"_ana_z"+QString::number(zn+1);

 sql_msg = "select t1.tip as C, printf(\"%.2f\",t2.bc) as Bc,"+
           lst_cols+
           " from (B_cmb_z"+
           QString::number(zn+1)+
           ") as t1, ("+
           tbLabAna+
           ") as t2 "
           "where((t2.id="+QString::number(l_id)+") and(t1.id=t2.idComb))";

#ifndef QT_NO_DEBUG
 qDebug() << sql_msg;
#endif

 return sql_msg;
}

void BTirages::showLineDetails(BView **tbl, int zn, int l_id, QString sql_msg)
{

 //BView *qtv_tmp= tbvAnaLgn[zn];
 BView *qtv_tmp= tbl[zn];

 QSqlQueryModel *cur_lgn = qobject_cast<QSqlQueryModel *> (qtv_tmp->model());
 cur_lgn->clear();
 cur_lgn->setQuery(sql_msg,db_tir);

 qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
 qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
 qtv_tmp->verticalHeader()->hide();

 /// Largeur du tableau
 int l = qtv_tmp->getMinWidth();
 qtv_tmp->setFixedWidth(l);


 /// Hauteur
 int h = qtv_tmp->getMinHeight();
 qtv_tmp->setFixedHeight(h);

 /// Titre
 QString title = "";
 if(l_id<=0){
  qtv_tmp->setVisible(false);
  title = "Selectionner une ligne tirage pour afficher details !!";
  qtv_tmp->setTitle(title, false);
 }
 else {
  qtv_tmp->setVisible(true);
  title = "Detail ligne : %1";
  title = title.arg(l_id);
  qtv_tmp->setTitle(title);
 }
}
*/

void BTirages::BSlot_Dessine(bool chk)
{
 QAction *action = qobject_cast<QAction *>(sender());
 int zn = action->parent()->objectName().toInt();
 int item = action->data().toInt();
 QGraphicsScene *cur_screen = grp_screen->getScene();
 QGraphicsItemGroup * lgn = grp_screen->getLine(zn,item);

 if(chk == true){
  cur_screen->addItem(lgn);
 }
 else {
  cur_screen->removeItem(lgn);
 }

}

void BTirages::showGen(BTirAna *ana_tirages)
{

 if(tbw_calculs == nullptr){
  tbw_calculs = new QTabWidget;
  gdl_all = new QGridLayout;
  wdg_reponses = new QWidget;
  //gdl_list = new QList<QGridLayout *>;
 }

 QString st_obj = "Ensemble_"+ QString::number(id_TirSrc).rightJustified(2,'0');
 tbw_calculs->setObjectName(st_obj);
 connect(tbw_calculs,SIGNAL(tabBarClicked(int)),this,SLOT(BSlot_Ensemble_Tir(int)));


 QGridLayout *lay_fusion = this->addAna(ana_tirages);
 QWidget *wdg_fusion = new QWidget;
 wdg_fusion->setLayout(lay_fusion);

 QString name = this->getGameLabel();
 int that_index = tbw_calculs->addTab(wdg_fusion, name);
 tbw_calculs->setCurrentIndex(that_index);
 gdl_all->addWidget(tbw_calculs);
 wdg_reponses->setLayout(gdl_all);
 wdg_reponses->setWindowTitle("Tirages AUTO : ");
 wdg_reponses->show();

 connect(this,SIGNAL(BSig_AnaLgn(int,int)), ana_tirages,SLOT(BSlot_AnaLgnShow(int,int)));
 connect(this,SIGNAL(BSig_Show_Flt(const B2LstSel *)), ana_tirages,SLOT(BSlot_Show_Flt(const B2LstSel *)));
 connect(ana_tirages, SIGNAL(BSig_FilterRequest(BTirAna *, const Bp::E_Ico , const B2LstSel * )),
         this, SLOT(BSlot_Filter_Tir(BTirAna *, const Bp::E_Ico , const B2LstSel *)));
}

QGridLayout * BTirages::addAna(BTirAna* ana)
{
 lay_fusion = new QGridLayout;
 //QVBoxLayout * vly = new QVBoxLayout;
 //vly->addWidget(this);

 lay_fusion->addWidget(this,0,0,2,1);
 lay_fusion->addWidget(ana,0,1,1,2);///,Qt::AlignTop|Qt::AlignLeft
 lay_fusion->setColumnStretch(0, 0); /// Exemple basic layouts
 lay_fusion->setColumnStretch(1, 1);

 return lay_fusion;
}

QString BTirages::getGameLabel(void)
{
 return game_lab;
}

QString BTirages::getTiragesList(const stGameConf *pGame, QString tbl_src)
{
 /// Element des zones
 int nb_zn = pGame->znCount;
 QString str_cols = "";
 for (int zn=0;zn<nb_zn;zn++) {
  str_cols = str_cols + BCount::FN1_getFieldsFromZone(pGame,zn,"t1");
  if(zn<nb_zn-1){
   str_cols = str_cols + ",";
  }
 }

 QString st_lst_1 = "";
 QString st_lst_2 = "";
 if(eTir == eTirGen){
  st_lst_1 = "t1.id,";
  st_lst_2 = ",t1.chk";
 }
 else if (eTir == eTirFdj) {
  st_lst_1 = "t1.id, t1.D, t1.J,";
  st_lst_2 = "";
 }

 QString msg = " with tb1 as (select "+
               st_lst_1+
               str_cols+
               st_lst_2+
               " from ("+
               tbl_src+
               ") as t1)";
 return msg;
}

QTabWidget * BTirages::memoriserSelectionUtilisateur(const B2LstSel * sel)
{
 QTabWidget *tbw_visual = nullptr;
 QTabWidget *tt5 = nullptr;
 bool chk_db =true;
 QString msg = "";
 QSqlQuery query(db_tir);

 /// Verifier si la table de liste des reponses existe
 if(DB_Tools::isDbGotTbl("B_lst",db_tir.connectionName())==false){
  msg = "CREATE TABLE if not EXISTS B_lst (id integer PRIMARY key, name text, id_onglet int, id_zn int, lst TEXT)";
  if(!query.exec(msg)){
   DB_Tools::DisplayError("BTirages::memoriserSelectionUtilisateur (1)", &query, msg);
   chk_db = false;
  }
 }

 int nb_items = sel->size();
 if(nb_items != 0){
  tbw_visual = new QTabWidget;
  QTableView *qtv_tmp = nullptr;
  QStandardItemModel *visu = nullptr;
  QStandardItem * std_tmp = nullptr;

  // nb_items en lien avec BCount::onglet[eCountEnd]
  // type de calcul
  for (int i=0; i< nb_items;i++) {

   QList<BLstSelect *> *tmp = sel->at(i);
   etCount type = tmp->at(0)->type;
   QString name = BCount::onglet[type];

   int nb_Subitems = tmp->size();

   int val =0;
   tt5 = new QTabWidget;
   for (int j=0; j< nb_Subitems;j++) {
    /// nb_Subitems en relation avec la zone
    BLstSelect * item = tmp->at(j);
    QString title = gme_cnf->names[item->zn].abv;

    /// ---------------------------
    qtv_tmp = new QTableView;

    qtv_tmp->setAlternatingRowColors(true);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtv_tmp->setSelectionMode(QAbstractItemView::NoSelection);

    switch (item->type) {
     case eCountElm:
     {
      QList <int> lst_value;

      /// analyse de chaque element selectionné dans la zone concernée
      /// pour le calcul en cours
      QModelIndex un_index;

      foreach (un_index, item->indexes) {
       val = un_index.data().toInt();
       //val = un_index.sibling(un_index.row(),0).data().toInt();
       lst_value << val;
      }
      std::sort(lst_value.begin(), lst_value.end());

      int nb_row = 10;
      int nb_col = (gme_cnf->limites[j].max/nb_row)+1;
      visu = new QStandardItemModel(nb_row, nb_col);
      qtv_tmp->setModel(visu);

      for (int i=0;i<lst_value.size();i++) {


       int val = lst_value.at(i);
       int col_id = val/10;
       int row_id = val%10;

       QString cel_val = QString::number(val).rightJustified(2,'0');
       std_tmp = new QStandardItem(cel_val);
       std_tmp->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
       visu->setItem(row_id,col_id,std_tmp);
      }


      /// Fixer largeur colonne
      for (int i = 0; i< nb_col; i++) {
       qtv_tmp->setColumnWidth(i,30);
      }

      /// faire disparaite barres
      qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
      qtv_tmp->horizontalHeader()->hide();

      qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
      qtv_tmp->verticalHeader()->hide();

      qtv_tmp->setFixedWidth((nb_col+0.5) * 30);;
      qtv_tmp->setFixedHeight((nb_row+0.5) * 30);
     }
      break;

     case eCountCmb:
     {
      QStringList lst_value;

      QString test_b = "";
      QModelIndex un_index;
      foreach (un_index, item->indexes) {
       //val = un_index.sibling(un_index.row(),0).data().toInt();
       lst_value << un_index.data().toString();
      }
      std::sort(lst_value.begin(), lst_value.end());
      test_b = lst_value.join(",");

      //qtv_tmp = new QTableView;
      int nb_row = lst_value.size();
      int nb_col = 1;
      visu = new QStandardItemModel(nb_row, nb_col);
      qtv_tmp->setModel(visu);
      visu->setHeaderData(0,Qt::Horizontal,"R");
      visu->setHeaderData(0,Qt::Horizontal,QBrush(Qt::red),Qt::ForegroundRole);

      for (int i=0;i<lst_value.size();i++) {
       QString cel_val = lst_value.at(i);
       std_tmp = new QStandardItem(cel_val);
       std_tmp->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
       visu->setItem(i,0,std_tmp);
      }
     }
      break;

     case eCountBrc:
     {
      QList <double> lst_value;

      QModelIndex un_index;
      foreach (un_index, item->indexes) {
       //val = un_index.sibling(un_index.row(),0).data().toInt();
       lst_value << un_index.data().toDouble();
      }
      std::sort(lst_value.begin(), lst_value.end());

      QString test_a = "";
      int tot_items = lst_value.size();

      for(int one_item=0; one_item<tot_items;one_item++){
       test_a = test_a + QString::number(lst_value.at(one_item));
       if(one_item<tot_items-1){
        test_a=test_a+",";
       }
      }

      //qtv_tmp = new QTableView;
      int nb_row = lst_value.size();
      int nb_col = 1;
      visu = new QStandardItemModel(nb_row, nb_col);
      qtv_tmp->setModel(visu);
      visu->setHeaderData(0,Qt::Horizontal,"R");
      visu->setHeaderData(0,Qt::Horizontal,QBrush(Qt::red),Qt::ForegroundRole);

      for (int i=0;i<lst_value.size();i++) {
       QString cel_val = QString::number(lst_value.at(i));
       std_tmp = new QStandardItem(cel_val);
       std_tmp->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
       visu->setItem(i,0,std_tmp);
      }
     }
      break;
     case eCountGrp:
     {
      typedef struct _grp_item
      {
        int x;
        int y;
        int v;
      }grp_itemp;
      QList <grp_itemp *> lst_value;

      QModelIndex un_index;
      int col = -1;
      int lgn = -1;
      foreach (un_index, item->indexes) {
       lgn = un_index.sibling(un_index.row(),0).data().toInt();
       col = un_index.column();
       val = un_index.data().toInt();

       grp_itemp *tmp_data = new grp_itemp;
       tmp_data->x = col;
       tmp_data->y = lgn;
       tmp_data->v = val;
       lst_value << tmp_data;
      }

      QStringList cols = gme_cnf->slFlt[item->zn][Bp::colDefTitres] ;
      //qtv_tmp = new QTableView;
      //qtv_tmp->alternatingRowColors();

      int nb_row = gme_cnf->limites[item->zn].win + 1;
      int nb_col = cols.indexOf("X1");

      visu = new QStandardItemModel(nb_row, nb_col+2);
      qtv_tmp->setModel(visu);

      /// Titre des colonnes
      visu->setHeaderData(0,Qt::Horizontal,"Nb");
      visu->setHeaderData(0,Qt::Horizontal,QBrush(Qt::red),Qt::ForegroundRole);
      for(int i = 1; i<= nb_col+1;i++){
       visu->setHeaderData(i,Qt::Horizontal,cols.at(i-1));
       qtv_tmp->setColumnWidth(i,30);
      }

      //Titre des lignes
      qtv_tmp->verticalHeader()->hide();
      for(int i = 0; i< nb_row;i++){
       QString cel_val = QString::number(i).rightJustified(2,'0');
       std_tmp = new QStandardItem(cel_val);
       std_tmp->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
       visu->setItem(i,0,std_tmp);
      }

      /// Valeurs
      for(int i = 0; i< lst_value.size();i++){
       QString cel_val = QString::number((lst_value.at(i))->v);
       std_tmp = new QStandardItem(cel_val);
       std_tmp->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
       visu->setItem((lst_value.at(i))->y,(lst_value.at(i))->x,std_tmp);
      }
      qtv_tmp->resizeColumnsToContents();
     }
      break;

     default:
      break;
    }
    /// --------------

    QWidget *calcul = new QWidget;
    tt5->addTab(qtv_tmp,title);
   }

   tbw_visual->addTab(tt5,name);
  }
 }

 return tbw_visual;
}

QTableView * BTirages::FillUsrSelectionTbv(etCount typ_usr, QList <QVariant> lst_usr)
{
 QTableView *qtv_tmp = new QTableView;
 QStandardItemModel *visu = nullptr;
 QStandardItem *tmp = nullptr;

 int nb_row = lst_usr.size();
 int nb_col = -1;

 switch (typ_usr) {
  case eCountElm:
  {
   QList <int> lst_value ;//= lst_usr.value().value<QList<int>>;

   nb_col = (gme_cnf->limites[0].max/nb_row)+1;
   visu = new QStandardItemModel(nb_row,nb_col);

   for (int i=0;i<nb_row;i++) {
    //lst_value.at(i)=lst_usr.value(i).value<QList<int>>;
    //int val = ();//lst_value.at(i).toInt();
    int val = 0;
    int col_id = val/10;
    int row_id = val%10;

    QString cel_val = QString::number(val).rightJustified(2,'0');
    tmp = new QStandardItem(cel_val);
    tmp->setData(Qt::AlignCenter, Qt::TextAlignmentRole);
    visu->setItem(row_id,col_id,tmp);
   }

   /// alterner couleur ligne
   qtv_tmp->setAlternatingRowColors(true);

   /// Fixer largeur colonne
   for (int i = 0; i< nb_col; i++) {
    qtv_tmp->setColumnWidth(i,30);
   }

   /// faire disparaite barres
   qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
   qtv_tmp->horizontalHeader()->hide();

   qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
   qtv_tmp->verticalHeader()->hide();

   qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
   qtv_tmp->setSelectionMode(QAbstractItemView::NoSelection);

   qtv_tmp->setFixedWidth((nb_col+0.5) * 30);;
   qtv_tmp->setFixedHeight((nb_row+0.5) * 30);

  }
   break;

  default:
   break;

 }

 return qtv_tmp;
}



QString BTirages::makeSqlFromSelection(const B2LstSel * sel, QString *tbl_lst)
{
 QString ret_all = "";
 QString ret_elm = "";
 QString ret_cmb = "";
 QString ret_add = "";

 QString use_tirages = game_lab;
 if(use_tirages.compare("B_fdj")==0){
  use_tirages="B";
 }

 int cur_tbl_id = 3;
 QString local_list = "";

 int nb_items = sel->size();
 for (int i=0;i<nb_items;i++)
 {
  QList<BLstSelect *> *tmp = sel->at(i);

  ret_add = "";
  ret_elm = "";
  local_list = "";


  int nb_zone = tmp->size();
  for (int j=0;j<nb_zone;j++)
  {
   BLstSelect *item = tmp->at(j);


   QString tbl_ana = "("+use_tirages+"_ana_z"+QString::number((item->zn)+1)+") as t";

   if((item->type) == eCountElm){
    ret_elm = select_elm(item->indexes, item->zn);
   }
   else {
    cur_tbl_id = cur_tbl_id + 1;
    local_list = local_list + tbl_ana+QString::number(cur_tbl_id);///"("+game_lab+"_ana_z"+QString::number((item->zn)+1)+") as t"+QString::number(cur_tbl_id);
    if(j<nb_zone-1){
     local_list = local_list + ",";
    }

    switch (item->type) {
     case eCountCmb:
      ret_elm = select_cmb(item->indexes, item->zn, cur_tbl_id);
      break;
     case eCountBrc:
      ret_elm = select_brc(item->indexes, item->zn, cur_tbl_id);
      break;
     case eCountGrp:
      ret_elm = select_grp(item->indexes, item->zn, cur_tbl_id);
      break;

     case eCountUpl:
      int a = 12; /// Pas encore traite
      return "";
      break;
      /*
     default:
      QString cal_err = QString::number(item->type)+" : label ?\n Fn: BTirages::makeSqlFromSelection";
      QMessageBox::warning(nullptr, "Type calclul inconnu !!",cal_err)	;
*/
		}
	 }
	 ret_add = ret_add + ret_elm;
	 if(j <nb_zone -1){
		ret_add = ret_add  + " and ";
	 }

	}

	ret_all = ret_all + ret_add;

	if(local_list.size()){
	 *tbl_lst = *tbl_lst+ "," + local_list;
	}

	if(i<nb_items -1){
	 ret_all = ret_all + " and ";
	}
 }

#ifndef QT_NO_DEBUG
 qDebug() << "\n\nret :\n" <<ret_all;
#endif

 return ret_all;
}

QString BTirages::select_elm(const QModelIndexList &indexes, int zn)
{
 int taille = indexes.size();
 int loop = gme_cnf->limites[zn].win;
 QString msg = "";

 if(taille <= loop){
  msg = elmSel_1(indexes, zn);
 }
 else {
  msg = elmSel_2(indexes, zn);
 }



 msg = "("+msg+")";

#ifndef QT_NO_DEBUG
 qDebug() << "\n\nselect_elm :\n" <<msg;
#endif

 return msg;
}

QString BTirages::elmSel_1(const QModelIndexList &indexes, int zn)
{
 QString msg = "";

 int loop = indexes.size();

 QString st_cols = BCount::FN1_getFieldsFromZone(gme_cnf, zn, "t1");
 QString key = "%1 in("+st_cols+")";

 for(int i = 0; i< loop; i++){
  QString val = indexes.at(i).data().toString();
  msg = msg + key.arg(val);
  if(i<loop-1){
   msg=msg+" and ";
  }
 }

#ifndef QT_NO_DEBUG
 qDebug() <<"\n"<< "BTirages::elmSel_1 :\n" <<msg;
#endif

 return msg;
}

QString BTirages::elmSel_2(const QModelIndexList &indexes, int zn)
{
 /// cette fn calcul le Cnp pour le cas nb selection > nb win de la zone

 QString msg = "";

 QString key = "t1."+gme_cnf->names[zn].abv+"%1 in(%2)";

 QString ret = "";
 int taille = indexes.size();

 for(int i = 0; i< taille; i++){
  QString val = indexes.at(i).data().toString();
  if(i<taille-1){
   val=val+",";
  }
  ret = ret+val;
 }

 int loop = gme_cnf->limites[zn].win;
 for(int i = 0; i< loop; i++){
  msg = msg + key.arg(i+1).arg(ret);
  if(i<loop-1){
   msg=msg+" and ";
  }
 }

#ifndef QT_NO_DEBUG
 qDebug() <<"\n"<< "BTirages::elmSel_2 :\n" <<msg;
#endif

 return msg;
}

QString BTirages::select_cmb(const QModelIndexList &indexes, int zn, int tbl_id)
{
 QString msg = "";

 QString key = "t"+QString::number(tbl_id)+".idComb in(%1)";

 QString ret = "";
 int taille = indexes.size();

 for(int i = 0; i< taille; i++){
  QModelIndex cur_index = indexes.at(i);
  QString val = cur_index.sibling(cur_index.row(),Bp::colId).data().toString();
  if(i<taille-1){
   val=val+",";
  }
  ret = ret+val;
 }

 msg = "(t"+QString::number(tbl_id)+".id = t1.id) and ("+key.arg(ret)+")";

#ifndef QT_NO_DEBUG
 qDebug() << "\n\nselect_cmb :\n" <<msg;
#endif

 return msg;
}

QString BTirages::select_brc(const QModelIndexList &indexes, int zn, int tbl_id)
{
 QString msg = "";

 QString key = "t"+QString::number(tbl_id)+".bc in(%1)";

 QString ret = "";
 int taille = indexes.size();

 for(int i = 0; i< taille; i++){
  QModelIndex cur_index = indexes.at(i);
  QString val = cur_index.sibling(cur_index.row(),Bp::colTxt).data().toString();
  if(i<taille-1){
   val=val+",";
  }
  ret = ret+val;
 }

 msg = "(t"+QString::number(tbl_id)+".id = t1.id) and ("+key.arg(ret)+")";

#ifndef QT_NO_DEBUG
 qDebug() << "\n\nselect_brc :\n" <<msg;
#endif

 return msg;
}

QString BTirages::select_grp(const QModelIndexList &indexes, int zn, int tbl_id)
{
 QString msg = "";
 QString ret = "";


 /// Tableau de memorisation choix usr
 const QAbstractItemModel * p_aim = indexes.at(0).model();
 int nb_col = 	p_aim->columnCount();
 QString *tab_sel = new QString[nb_col];
 for(int i=0; i< nb_col;i++){
  tab_sel[i]="";
 }

 /// Recuperation de la valeur de nb pour chaque index
 int col_usr = 0;
 int taille = indexes.size();
 for(int i = 0; i< taille; i++){
  QModelIndex cur_index = indexes.at(i);

  int col = cur_index.column();
  if(col>0){
   QString s_nb = cur_index.model()->index(cur_index.row(),Bp::colId).data().toString();
   if(tab_sel[col-1].size()){
    tab_sel[col-1]=tab_sel[col-1]+","+s_nb;
   }
   else {
    tab_sel[col-1] = s_nb;
    col_usr++;
   }
  }
 }

 /// Construction de la requete de chaque colonne
 QString ref = "(t"+QString::number(tbl_id)+".%1 in(%2))";
 for (int i = 0; i<nb_col;i++) {
  if(!tab_sel[i].size()){
   continue;
  }
  QVariant vCol = p_aim->headerData(i+1,Qt::Horizontal);
  QString colName = vCol.toString();
  ret = ret + ref.arg(colName).arg(tab_sel[i]);
  if(col_usr>1){
   ret = ret + " and ";
   col_usr--;
  }

 }

 delete[] tab_sel;
 msg = "(t"+QString::number(tbl_id)+".id = t1.id) and ("+ret+")";

#ifndef QT_NO_DEBUG
 qDebug() << "\n\nselect_grp :\n" <<msg;
#endif

 return msg;
}

QWidget *BTirages::ana_fltSelection(QTabWidget *tbw_flt, QString st_obj, BTirages *parent, BTirAna **J)
{
 QWidget *ret = new QWidget;
 QTabWidget *tab_Top = new QTabWidget;
 tab_Top->setObjectName(st_obj);

 QString ongNames[]={"J","J+1"};
 BTirAna * (BTirages::*ptrFunc[])(const stGameConf *pGame, QString msg)=
 {
   &BTirages::doLittleAna
};
 int nb_func_0 = sizeof(ptrFunc)/sizeof(BTirAna); //mauvais BUG
 int nb_func = sizeof(ongNames)/sizeof(QString);

 for (int i=0;i<nb_func;i++) {
  QWidget *tmp_wdg = J[i];
  if(tmp_wdg != nullptr){
   tab_Top->addTab(tmp_wdg,ongNames[i]);
  }
 }

 QVBoxLayout *tmp_l_0 = new QVBoxLayout;
 QVBoxLayout *tmp_lay = new QVBoxLayout;
 QGridLayout *tmp_lay_2 = new QGridLayout;
 if(tab_Top->count() !=0){
  tmp_lay->addWidget(tab_Top);
  connect(tab_Top,SIGNAL(tabBarClicked(int)),parent,SLOT(BSlot_Tir_flt(int)));
 }
 else {
  delete 	tab_Top;
  QLabel *tmp = new QLabel("Erreur pas de resultats a montrer !!");
  tmp_lay->addWidget(tmp);
 }

 QGroupBox *info = new QGroupBox;
 info->setTitle("Analyse apres selection utilisateur");

 QGroupBox *info_a = new QGroupBox;
 info_a->setTitle("Selection");
 tmp_l_0->addWidget(tbw_flt);
 info_a->setLayout(tmp_l_0);


 QGroupBox *info_b = new QGroupBox;
 info_b->setTitle("Résultats");
 info_b->setLayout(tmp_lay);

 tmp_lay_2 ->addWidget(info_a,0,0);
 tmp_lay_2 ->addWidget(info_b,0,1);

 info->setLayout(tmp_lay_2);

 ret = info;

 return ret;
}

BTirAna * BTirages::doLittleAna(const stGameConf *pGame, QString msg)
{
 BTirAna *uneAnalyse = nullptr;

 stGameConf *flt_game = new stGameConf;

 /// Partie commune
 flt_game->limites = pGame->limites;
 flt_game->names = pGame->names;
 flt_game->znCount = pGame->znCount;
 flt_game->eTirType = pGame->eTirType;
 flt_game->eFdjType = pGame->eFdjType;

 /// sera reconstruit par la classe Analyse
 /// mappage des fonctions utilisateurs speciales
 /// d'analyses
 flt_game->slFlt = nullptr;

 flt_game->db_ref = new stParam_3;
 flt_game->db_ref->fdj = pGame->db_ref->fdj;
 flt_game->db_ref->cnx = pGame->db_ref->cnx;
 flt_game->db_ref->dad = game_lab;
 flt_game->db_ref->jrs = pGame->db_ref->jrs;

 flt_game->db_ref->ihm = pGame->db_ref->ihm;

 QString sub_tirages  =  game_lab + "R" +QString::number(id_AnaSel).rightJustified(2,'0');
 flt_game->db_ref->src= sub_tirages;
 flt_game->db_ref->sql = msg;
 flt_game->db_ref->flt= sub_tirages+"_flt";

 QSqlQuery query(db_tir);
 bool b_retVal = true;

 msg = "create table if not exists " +
       sub_tirages +
       " as " + msg;

#ifndef QT_NO_DEBUG
 qDebug()<< "\nView :\n" <<msg;
#endif

 if((b_retVal = query.exec(msg)) == true){
  uneAnalyse = new BTirAna(flt_game);
  if(uneAnalyse->self() != nullptr){

   id_AnaSel++;
   connect(uneAnalyse, SIGNAL(BSig_FilterRequest(BTirAna *, const Bp::E_Ico , const B2LstSel * )),
           this, SLOT(BSlot_Filter_Tir(BTirAna *, const Bp::E_Ico , const B2LstSel *)));
  }
  else {
   delete uneAnalyse;
   uneAnalyse = nullptr;
   id_AnaSel--;
  }
 }

 return uneAnalyse;
}

void BTirages::updateTbv(QString box_title, QString msg)
{
#ifndef QT_NO_DEBUG
 qDebug()<< "\n\nMsg :\n" <<msg;
#endif

 QSqlQueryModel * cible = sqm_resu;

 /// Effacer le champ recherche (rapide)
 ble_rch->clear();

 if(gme_cnf->eTirType == eTirGen){
  cible = qobject_cast<BSqlQmTirages_3 *>(sqm_resu);
 }
 else {
  cible = sqm_resu;
 }


 cible->setQuery(msg,db_tir);
 while (cible->canFetchMore())
 {
  cible->fetchMore();
 }
 int nb_rows = cible->rowCount();

 BView *qtv_tmp = tir_tbv;
 int rows_proxy = qtv_tmp->model()->rowCount();
 QString st_title = box_title+
                    "Nb tirages : "+QString::number(nb_rows)+
                    " sur " + QString::number(rows_proxy);
 qtv_tmp->setTitle(st_title);
}

void BTirages::BSlot_closeTab(int index)
{
 og_AnaSel->removeTab(index);
 if(og_AnaSel->count() == 0){
  QWidget *tmp = og_AnaSel->widget(index);
  delete tmp;
  delete  og_AnaSel;
  og_AnaSel = nullptr;
  QString box_title = "";
  updateTbv(box_title, lst_tirages);
 }
}

void BTirages::BSlot_Filter_Tir(BTirAna *from, const Bp::E_Ico ana, const B2LstSel * sel)
{
 if(from == nullptr) return;

 QString msg  = "select t1.* from ";
 QString tbl_lst = "(tb1) as t1";
 QString clause = "";
 //QString msg_1  = "";
 QString msg_2  = "";
 QString flt_tirages = "";
 QString box_title ="";
 QString lst_tirages = "";

 lst_tirages = from->getSql().simplified();
 if(lst_tirages.size() == 0){
  lst_tirages = getTiragesList(gme_cnf, game_lab);
 }
 else {
  return; /// Juste un marqueur
 }

 if((ana != Bp::icoRaz) && (sel !=nullptr)){
  int nb_sel = sel->size();

  //checkMemory();

  QTabWidget *tab_SelUsr = memoriserSelectionUtilisateur(sel);

  /// Creer la requete de filtrage
  clause = makeSqlFromSelection(sel, &tbl_lst);
  msg = msg + tbl_lst;
  if(clause.size()){
   msg = msg + " where("+clause+")";
  }

  //msg_1 = ", tb2 as ("+ msg +")";

  /// mettre la liste des tirages a jour
  flt_tirages = lst_tirages + msg;

  /// verifier si simplement montrer tirages
  if(ana == Bp::icoShow){
   updateTbv(box_title,flt_tirages);
   return;
  }

  effectueAnalyses(tab_SelUsr,msg,1);
 }
 else {
  msg =  lst_tirages + msg + tbl_lst; /// supprimer les reponses precedentes si elles existent
  updateTbv(box_title,msg);
 }
}

void BTirages::checkMemory()
{
 if(og_AnaSel==nullptr){
  ana_TirFlt = new QList<BTirAna **>;
  og_AnaSel = new QTabWidget;
  og_AnaSel->setObjectName(tbw_FltTirages);
  id_AnaOnglet = 0;
  og_AnaSel->setTabsClosable(true);
  connect(og_AnaSel,SIGNAL(tabCloseRequested(int)),this,SLOT(BSlot_closeTab(int)));
  connect(og_AnaSel,SIGNAL(tabBarClicked(int)),this,SLOT(BSlot_Result_Tir(int)));
 }
}

void BTirages::effectueAnalyses(QTabWidget *tbw_flt, QString ref_sql, int distance,QString sep)
{
 BTirAna **J = new BTirAna *[2];
 QWidget * resu = nullptr;

 QString lst_tirages = getTiragesList(gme_cnf, game_lab);

 QString st_with = "";
 if(sep.simplified().compare(",")==0){
  st_with = "with ";
 }

 QString flt_tirages = lst_tirages + sep+ ref_sql;
 QString 	msg_1 = ", tb2 as ("+ st_with + ref_sql +")";
 QString  msg_2 = lst_tirages + msg_1 +
                  "select tb1.* from tb1,tb2 where(tb1.id=tb2.id-"+
                  QString::number(distance)+")";

#ifndef QT_NO_DEBUG
 //BTest::writetoFile("A1_req.txt", lst_tirages);
 BTest::writetoFile("A2_req.txt", flt_tirages);
 //BTest::writetoFile("A3_req.txt", msg_1);
 BTest::writetoFile("A4_req.txt", msg_2);
#endif

 /// faire une analyse pour J
 J[0] = doLittleAna(gme_cnf,flt_tirages);

 /// recherche J+1
 J[1] = doLittleAna(gme_cnf,msg_2);

 /// Nommage de l'onglet
 QString st_id = lab_ong;
 st_id = st_id.arg(QString::number(usr_flt_counter).rightJustified(2,'0'));

 resu = ana_fltSelection(tbw_flt, st_id, this, J);
 if(resu!=nullptr){
  checkMemory();
  usr_flt_counter++;
  ana_TirFlt->append(J);
  int tab_index = og_AnaSel->addTab(resu,st_id);
  if(gme_cnf->eTirType == eTirFdj){
   lay_fusion->addWidget(og_AnaSel,1,0);
   //lay_fusion->addWidget(og_AnaSel,1,1);
  }
  else {
   lay_fusion->addWidget(og_AnaSel,1,1);
  }
  og_AnaSel->setCurrentIndex(tab_index);
  og_AnaSel->tabBarClicked(tab_index);
 }
 else {
  usr_flt_counter--;
 }

}

B2LstSel *BTirages::SauverSelection(const B2LstSel * sel)
{
 B2LstSel * ret = new B2LstSel;

 int nb_items = sel->size();
 for (int i=0;i<nb_items;i++)
 {
  QList<BLstSelect *> *src = sel->at(i);
  QList<BLstSelect *> *dst = new QList<BLstSelect *>;
  ret->append(dst);

  int nb_zone = src->size();
  for (int j=0;j<nb_zone;j++)
  {
   BLstSelect *item_src = src->at(j);
   BLstSelect *item_dst = new BLstSelect;
   item_dst->type = item_src->type;
   item_dst->zn = item_src->zn;

   QModelIndex un_index;
   foreach (un_index, item_src->indexes) {
    //QPersistentModelIndex ici(un_index);
    item_dst->indexes.append(un_index);
   }
   dst->append(item_dst);
  }
 }

 return ret;
}

void BTirages::BSlot_Tir_flt(int index)
{
 if((index<0)){
  return;
 }
 QTabWidget * from = qobject_cast<QTabWidget *>(sender());
 BTirAna ** tmp = ana_TirFlt->at(id_AnaOnglet);
 BTirAna * tmp_ana = qobject_cast<BTirAna *>(tmp[index]);
 QString msg = tmp_ana->getSql();

 /// Pour rappeller la selection choisie
 //emit BSig_Show_Flt(save_sel);

 QString box_title = og_AnaSel->tabText(id_AnaOnglet)+" ("+from->tabText(index)+"). ";
 updateTbv(box_title, msg);
}

void BTirages::BSlot_Result_Tir(const int index)
{
 QTabWidget * from = qobject_cast<QTabWidget *>(sender());

 id_AnaOnglet = index;

 /// se Mettre sur l'onglet J adequat
 QString ref = lab_ong;
 ref = ref.arg(QString::number(index).rightJustified(2,'0'));
 QList<QTabWidget *> child_1 = from->findChildren<QTabWidget*>(ref);
 /// idem ligne precedente : QTabWidget * child_3 = from->findChild<QTabWidget *>(ref);

 if(child_1.size()){
  int cur_index = child_1.at(0)->currentIndex();
  child_1.at(0)->tabBarClicked(cur_index);
 }
}

void BTirages::BSlot_Ensemble_Tir(const int index)
{
 Q_UNUSED(index)

 QTabWidget * from = qobject_cast<QTabWidget *>(sender());

 /// Rechercher le tabwidget qui contient la liste des tirages
 QTabWidget * child_lstTir = from->findChild<QTabWidget *>(tbw_TbvTirages);
 child_lstTir->setCurrentIndex(1);

 /// Rechercher le tabwidget qui contient la liste des analyses des filtrages
 QTabWidget * child_lstFlt = from->findChild<QTabWidget *>(tbw_FltTirages);
 if(child_lstFlt == nullptr){
  return;
 }
 int cur_index = child_lstFlt->currentIndex();

#ifndef QT_NO_DEBUG
 QList<QTabWidget *> child_0 = from->findChildren<QTabWidget*>();
#endif

 /// se Mettre sur l'onglet J adequat
 QString ref = lab_ong;
 ref = ref.arg(QString::number(cur_index).rightJustified(2,'0'));
 QList<QTabWidget *> child_1 = child_lstFlt->findChildren<QTabWidget*>(ref);
 /// idem ligne precedente : QTabWidget * child_3 = child_lstFlt->findChild<QTabWidget *>(ref);

 if(child_1.size()){
  int cur_index = child_1.at(0)->currentIndex();
  //child_1.at(0)->setCurrentIndex(0);
  child_1.at(0)->tabBarClicked(cur_index);
 }
}
