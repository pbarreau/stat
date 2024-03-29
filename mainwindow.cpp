#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QtGui>

#include <QThread>

#include <QMdiSubWindow>
#include <QFormLayout>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTableView>
#include <QMessageBox>
#include <QSqlRecord>
#include <QSplitter>
#include <QTreeView>

#include <QStackedWidget>

#include <math.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "mygraphicsview.h"

#include "choixjeux.h"
#include "tirages.h"
#include "gererbase.h"
#include "labelclickable.h"
#include "pointtirage.h"
//#include "SyntheseDetails.h"
//#include "SyntheseGenerale.h"
#include "refetude.h"
#include "filtrecombinaisons.h"
//#include "lescomptages.h"

#include "BcUpl.h"
#include "BTbar1.h"
#include "BTirFdj.h"

#include "BCustomPlot.h"

#include "chartwidget.h"


#include "BcGrp.h"
#include "BcElm.h"

#include "BFdj.h"
#include "BTirAna.h"
#include "db_tools.h"
#include "BTest.h"
#include "BXmlFdj.h"

#include "BStepper.h"

static stTiragesDef configJeu;

void MainWindow::pslot_closeTabDetails(int index)
{
 gtab_Top->removeTab(index);
}

void MainWindow::slot_NOUVEAU_Ensemble(const B_RequeteFromTbv &calcul)
{
 qDebug()<<calcul.db_data;
 qDebug()<<calcul.tb_data;
}

void MainWindow::EtudierJeu(etFdj curGame, bool use_odb, bool fdj_new)
{

 //BXmlFdj my_xml = BXmlFdj(curGame);
 //return;

 stFdj *prm = new stFdj;
 prm->typeJeu = curGame;
 prm->use_odb = use_odb;
 prm->fdj_new = fdj_new;
 prm->db_type = eDbDsk;

 BFdj *charge = new BFdj(prm);

 stGameConf *curConf = charge->getConfig();

 //BStepper *t1 = new BStepper(curConf);
 //t1->show();


 bool b_retVal = true;

 //EtudierJeu_v1(curConf, use_odb);

 //return;
 if(use_odb==true){
  AfficherAnciensCalcul(curConf);
 }

 BTirFdj *lst_tirages = new BTirFdj(curConf);

 BTirAna *ana_tirages = new BTirAna(curConf);
 if(ana_tirages->self() == nullptr){
  QString msg = "Erreur de l'analyse des tirages :" + curConf->db_ref->src;
  QMessageBox::warning(nullptr, "Analyses", msg,QMessageBox::Yes);
  delete ana_tirages;
 }
 else{
  lst_tirages->showFdj(ana_tirages);
 }

 //b_retVal = BTest::montestRapideSql(curConf,0,4);
 //b_retVal = BTest::montestRapideSql(curConf,0,3);

 //BView *tabDesTirages = lst_tirages->getTbvTirages();


 return;

 EtudierJeu_v1(curConf, use_odb);
 EtudierJeu_v2(curConf);
 return;
}

void MainWindow::AfficherAnciensCalcul(stGameConf *pGame)
{
 // Etablir connexion a la base
 QString cnx=pGame->db_ref->cnx;
 QSqlDatabase db_1 = QSqlDatabase::database(cnx);
 if(db_1.isValid()==false){
  QString str_error = db_1.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }

 bool b_retVal = true;

 /// Verifier si la table de liste des jeux existe
 if(DB_Tools::isDbGotTbl("E_lst",db_1.connectionName())==false){
  return;
 }

 /// Pour les requetes
 QSqlQuery query(db_1);
 QString msg = "";

 /// Effacer anciens calculs des filtres
 b_retVal = DB_Tools::SupprimerResultatsPrecedent(cnx);
 DB_Tools::SupprimerResultatsPrecedent(cnx,"view","vt*");

#if 0
 select 'drop table ' || name || ';' from sqlite_master
   where(type='table' and name glob '*R[0-9]*');
#endif

 /// recuperer la liste des jeux deja effectue
 msg = "select * from E_lst "
       "where ( (type = '" + lstTirDef[eTirGen] + "')) ";

 /// si il y a des reponses les faire toutes
 if(((b_retVal=query.exec(msg))== true) && ((b_retVal=query.first())==true)){
  stGameConf * tmp = new stGameConf;

  tmp->znCount = 1;
  tmp->eTirType = eTirGen; /// A supprimer ?

  /// Partie commune
  tmp->limites = pGame->limites;
  tmp->names = pGame->names;
  tmp->eFdjType = pGame->eFdjType;

  /// sera reconstruit par la classe Analyse
  /// mappage des fonctions utilisateurs speciales
  /// d'analyses
  tmp->slFlt = nullptr;

  tmp->db_ref = new stParam_3;
  tmp->db_ref->fdj = pGame->db_ref->fdj;
  tmp->db_ref->cnx = pGame->db_ref->cnx;
  tmp->db_ref->ihm = pGame->db_ref->ihm;
  tmp->db_ref->jrs = pGame->db_ref->jrs;

  do{
   QString gameId = query.value(1).toString();
   tmp->db_ref->src = gameId;
   tmp->db_ref->flt = gameId+"_flt";

   AssemblerJeuxUsr(tmp);

  }while((b_retVal=query.next()) != false);

  delete tmp->db_ref;
  delete tmp;
 }



}

void MainWindow::AssemblerJeuxUsr(stGameConf *usrGame)
{
 BTirGen *lst_tirages = new BTirGen(usrGame);

 stGameConf * conf = lst_tirages->getGameConf();
 if( conf != nullptr){

  BTirAna *ana_tirages = new BTirAna(conf);
  if(ana_tirages->self() == nullptr){
   delete ana_tirages;
  }
  else {
   lst_tirages->showGen(ana_tirages);
  }
 }
 else {
  delete lst_tirages;
 }

}

void MainWindow::EtudierJeu_v1(stGameConf *curConf, bool dest_bdd)
{

 bool usePrevBdd = curConf->bUseMadeBdd;
 etFdj curGame = curConf->eFdjType;

 stParam input;
 input.destination = dest_bdd;
 input.bUseOneBdd = usePrevBdd;
 input.typeJeu = curGame;
 input.pgm_mdi = zoneCentrale;
 stErr NoErrors;
 NoErrors.status = true;
 NoErrors.msg = "None";

 DB_tirages = new GererBase(&input,&NoErrors,&configJeu);

 //EtudierJeu_MontrerUplet(curConf, DB_tirages);

 configJeu.db_cnx = DB_tirages->get_IdCnx(0);
 db_0 = QSqlDatabase::database(configJeu.db_cnx);


 if(NoErrors.status == false)
 {
  QMessageBox::critical(this,tr("Glob"),NoErrors.msg,QMessageBox::Ok,QMessageBox::NoButton);
  QApplication::quit();
 }

 /// ICI Commence le calcul
 RechercheProgressionBoules(&configJeu);


 /// Espaces Reponses pour details des calculs
 w_FenetreDetails = new QWidget;
 gtab_Top = new QTabWidget;
 gtab_Top->setTabsClosable(true);
 QFormLayout *mainLayout = new QFormLayout;
 mainLayout->addWidget(gtab_Top);
 w_FenetreDetails->setLayout(mainLayout);
 connect(gtab_Top,SIGNAL(tabCloseRequested(int)),this,SLOT(pslot_closeTabDetails(int)));

 TST_EtoileCombi(&configJeu);
 FEN_NewTirages(&configJeu);
}

void MainWindow::EtudierJeu_MontrerUplet(stGameConf *curConf, GererBase *use_db)
{
 bool usePrevBdd = curConf->bUseMadeBdd;

 if(usePrevBdd){
  QString cnx = use_db->get_IdCnx(1);
  /// BUG a resoudre en relecture code
  //BUplWidget *visu = new BUplWidget(cnx,1);
  //visu->show();
 }
}

void MainWindow::EtudierJeu_v2(stGameConf *curConf)
{
#if 0
 etFdj unJeu = curConf->eFdjType;
 bool usePrevBdd = curConf->bUseMadeBdd;

#if (SET_RUN_CHKP)
 QMessageBox::information(nullptr, "1C", "OK",QMessageBox::Yes);
#endif

 BPrevision::stPrmPrevision *prm = new BPrevision::stPrmPrevision;
 prm->bddStore = eDbDsk;
 prm->gameInfo.eTirType=eTirFdj;
 prm->gameInfo.eFdjType=unJeu;

 prm->gameInfo.bUseMadeBdd = usePrevBdd;
 prm->gameInfo.znCount = configJeu.nb_zone;
 prm->gameInfo.id = -1;
 prm->gameInfo.limites = nullptr;
 prm->gameInfo.names = nullptr;

 prm->tblFdj_dta="fdj";
 prm->tblFdj_ana="ana_z";
 prm->tblFdj_brc="";
 prm->tblUsr_dta="";
 prm->tblUsr_ana="";

 ///return;
 tous = new BPrevision(curConf, prm);



 connect(act_UGL_Create, SIGNAL(triggered()), tous, SLOT(slot_UGL_Create()));
 connect(act_UGL_SetFilters, SIGNAL(triggered()), tous, SLOT(slot_UGL_SetFilters()));
 connect(act_UGL_ClrFilters, SIGNAL(triggered()), tous, SLOT(slot_UGL_ClrFilters()));

 connect(tous,
         SIGNAL(sig_isClickedOnBall(QModelIndex)),
         syntheses,
         SLOT(slot_ShowBouleForNewDesign(QModelIndex)));

 connect(tous,
         SIGNAL(sig_isClickedOnBall(QModelIndex)),
         syntheses->GetTabEcarts(),
         SLOT(slot_ShowBoule_2(QModelIndex)));
#endif
}

QGridLayout *MainWindow::MonLayout_OldTbvTirage(int x, int y)
{
 QGridLayout *tmpGrid = new  QGridLayout;
 G_tbv_Tirages = new QTableView;

 G_tbv_Tirages->setSelectionMode(QAbstractItemView::SingleSelection);
 G_tbv_Tirages->setSelectionBehavior(QAbstractItemView::SelectItems);
 G_tbv_Tirages->setStyleSheet("QTableView {selection-background-color: red;}");
 G_tbv_Tirages->setEditTriggers(QAbstractItemView::NoEditTriggers);
 G_tbv_Tirages->setAlternatingRowColors(true);
 G_tbv_Tirages->setFixedSize(x,y);

 tmpGrid->addWidget(G_tbv_Tirages);
 return tmpGrid;
}

QGridLayout *MainWindow::MonLayout_OldTbvCouverture(int x, int y)
{
 QGridLayout *tmpGrid = new  QGridLayout;
 G_tbv_CouvTirages = new QTableView;

 G_tbv_CouvTirages->setSelectionMode(QAbstractItemView::MultiSelection);
 G_tbv_CouvTirages->setSelectionBehavior(QAbstractItemView::SelectItems);
 G_tbv_CouvTirages->setStyleSheet("QTableView {selection-background-color: red;}");
 G_tbv_CouvTirages->setEditTriggers(QAbstractItemView::NoEditTriggers);
 G_tbv_CouvTirages->setAlternatingRowColors(true);
 G_tbv_CouvTirages->setFixedSize(x,y);

 tmpGrid->addWidget(G_tbv_CouvTirages);
 return tmpGrid;
}
#if 1
void MainWindow::FEN_Old_Tirages(void)
{
#define C_TailleX   550
#define C_TailleY   300
#define C_Delta     50

 QWidget *w_DataFenetre = new QWidget;
 QTabWidget *tab_Top = new QTabWidget;

 QString ongNames[]={"Liste","Couverture"};
 int nbOng = sizeof(ongNames)/sizeof(QString);

 QWidget **wid_ForTop = new QWidget*[nbOng];
 QGridLayout **dsgOnglet = new QGridLayout * [nbOng];

 QGridLayout * (MainWindow::*ptrFunc[])(int,int)={
                                                 &MainWindow::MonLayout_OldTbvTirage,
                                                 &MainWindow::MonLayout_OldTbvCouverture};

 for(int i =0; i< nbOng;i++)
 {
  wid_ForTop[i]= new QWidget;

  tab_Top->addTab(wid_ForTop[i],ongNames[i]);

  dsgOnglet[i]= (this->*ptrFunc[i])(C_TailleX,C_TailleY);
  wid_ForTop[i]->setLayout(dsgOnglet[i]);
 }

 QFormLayout *mainLayout = new QFormLayout;
 mainLayout->addWidget(tab_Top);

 w_DataFenetre->setLayout(mainLayout);
 w_DataFenetre->setWindowTitle("Tirages");

 QMdiSubWindow *subWindow = zoneCentrale->addSubWindow(w_DataFenetre);
 subWindow->resize(C_TailleX+C_Delta,C_TailleY+C_Delta);
 subWindow->move(0,0);

 w_DataFenetre->setVisible(true);


}
//--------
#else
void MainWindow::FEN_Old_Tirages(void)
{
#define C_TailleX   550
#define C_TailleY   520

 // Onglet pere
 QTabWidget *tab_Top = new QTabWidget;
 QWidget **wid_ForTop = new QWidget*[2];
 QFormLayout **lay_ForTop = new QFormLayout*[2];

 QWidget *wTop_1 = new QWidget;
 QWidget *wTop_2 = new QWidget;
 QFormLayout *lTop_1 = new QFormLayout;
 QFormLayout *lTop_2 = new QFormLayout;

 wid_ForTop[0]= wTop_1;
 wid_ForTop[1]= wTop_2;
 lay_ForTop[0]= lTop_1;
 lay_ForTop[1]= lTop_2;
 tab_Top->addTab(wid_ForTop[0],tr("Liste"));
 tab_Top->addTab(wid_ForTop[1],tr("Couverture"));
 // ------------------

 // Info pour Tirages
 G_w_Tirages = new QWidget;
 G_tbv_Tirages = new QTableView;
 QFormLayout *lay_tirages = new QFormLayout;

 // Info pour couvertures Tirages
 G_w_CouvTirages = new QWidget;
 G_tbv_CouvTirages = new QTableView;
 //QFormLayout *lay_couvTirages = new QFormLayout;

 // Gestion du QTableView des Tirages
 G_tbv_Tirages->setSelectionMode(QAbstractItemView::SingleSelection);
 G_tbv_Tirages->setSelectionBehavior(QAbstractItemView::SelectItems);
 G_tbv_Tirages->setStyleSheet("QTableView {selection-background-color: red;}");
 G_tbv_Tirages->setEditTriggers(QAbstractItemView::NoEditTriggers);
 G_tbv_Tirages->setAlternatingRowColors(true);
 //qtv_Tirages->setMinimumHeight(800);
 //Gtv_Tirages->setMinimumWidth(460);
 G_tbv_Tirages->setFixedSize(C_TailleX,C_TailleY);

 lay_ForTop[0]->addWidget(G_tbv_Tirages);
 wid_ForTop[0]->setLayout(lay_ForTop[0]);


 // Gestion du QTableView de Couvertures Tirages
 G_tbv_CouvTirages->setSelectionMode(QAbstractItemView::MultiSelection);
 G_tbv_CouvTirages->setSelectionBehavior(QAbstractItemView::SelectItems);
 G_tbv_CouvTirages->setStyleSheet("QTableView {selection-background-color: red;}");
 G_tbv_CouvTirages->setEditTriggers(QAbstractItemView::NoEditTriggers);
 G_tbv_CouvTirages->setAlternatingRowColors(true);
 G_tbv_CouvTirages->setFixedSize(C_TailleX,C_TailleY);

 lay_ForTop[1]->addWidget(G_tbv_CouvTirages);
 wid_ForTop[1]->setLayout(lay_ForTop[1]);

 //lay_couvTirages->addWidget(Gtv_CouvTirages);
 //Gw_CouvTirages->setLayout(lay_couvTirages);

 lay_tirages->addWidget(tab_Top);
 G_w_Tirages->setLayout(lay_tirages);
 G_w_Tirages->setWindowTitle("Tirages");
 //Gw_Tirages->setMinimumHeight(420);
 //G_w_Tirages->setFixedSize(600,500);

 zoneCentrale->addSubWindow(G_w_Tirages);
 G_w_Tirages->setVisible(true);
 G_w_Tirages->showMinimized();
 //QMdiSubWindow *sousFenetre1 = zoneCentrale->addSubWindow(AfficherBase);

 // Simple click dans sous fenetre base
 connect( G_tbv_Tirages, SIGNAL( clicked(QModelIndex)) ,
          this, SLOT( slot_MontreTirageDansGraph( QModelIndex) ) );

 // Double click dans sous fenetre base
 connect( G_tbv_Tirages, SIGNAL( doubleClicked(QModelIndex)) ,
          this, SLOT( slot_ChercheVoisins( QModelIndex) ) );


}
#endif

void MainWindow::fen_LstCouv(void)
{
 G_w_CouvTirages = new QWidget;
 G_tbv_CouvTirages = new QTableView;
 QFormLayout *lay_couvTirages = new QFormLayout;
 //qtv_LstCouv->itemDelegate(new DelegationDeCouleur(qtv_LstCouv));

 lay_couvTirages->addWidget(G_tbv_CouvTirages);
 G_w_CouvTirages->setLayout(lay_couvTirages);
 G_w_CouvTirages->setWindowTitle("Couvertures des tirages");


 // Gestion du QTableView
 G_tbv_CouvTirages->setSelectionMode(QAbstractItemView::MultiSelection);
 G_tbv_CouvTirages->setSelectionBehavior(QAbstractItemView::SelectItems);
 G_tbv_CouvTirages->setStyleSheet("QTableView {selection-background-color: red;}");
 G_tbv_CouvTirages->setEditTriggers(QAbstractItemView::NoEditTriggers);
 G_tbv_CouvTirages->setAlternatingRowColors(true);

 // Ratacher cette sous fenetre
 zoneCentrale->addSubWindow(G_w_CouvTirages);
 G_w_CouvTirages->setVisible(true);
 G_w_CouvTirages->showMinimized();

#if 0
 // Selection a change
 connect(qtv_LstCouv->selectionModel(), SIGNAL(selectionChanged (const QItemSelection&, const QItemSelection&)),
         this, SLOT(slot_CouvertureSelChanged(QItemSelection,QItemSelection)));

#endif

}

///// --------test de delegate pour QtView
//MaQtvDelegation::MaQtvDelegation(QWidget *parent, int ligne, int col): QItemDelegate(parent)
MaQtvDelegation::MaQtvDelegation(QPersistentModelIndex &recherche)
{
 derTirage = recherche;
 //start = recherche.internalPointer();
 //coln =col;
 //lgn= ligne;
}

void MaQtvDelegation::paint(QPainter *painter, const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{

 //int col = index.column();
 //int ligne = index.row();

 //if ((col == coln) and (ligne == lgn)){
 if(index == derTirage){
  //if(index.internalId() == derTirage.internalId()){
  painter->fillRect(option.rect, (QBrush(QColor(200,170,100,140))));
 }

 QItemDelegate::paint(painter, option, index);

}

void MainWindow::MonLayout_Selectioncombi(QTabWidget *tabN1)
{
 bool status = false;
#if 0
 select t1.id as id, t1.tip as Repartition, count(t1.fk_idCombi_z1)as T,
   count (case when t1.J like 'lun%' then 1 end)as LUN,
   count (case when t1.J like 'mer%' then 1 end)as MER ,
   count (case when t1.J like 'sam%' then 1 end)as SAM
   from (
    SELECT lstcombi.id,lstcombi.tip,"+tb_ana_zn+".fk_idCombi_z1,"+tb_ana_zn+".id as id2, (case when "+tb_ana_zn+".id is null then null else
                                                                                          (
                                                                                           select tirages.jour_tirage from tirages where (tirages.id = "+tb_ana_zn+".id)
                                                                                           )
                                                                                          end) as J
    FROM lstcombi
    LEFT JOIN "+tb_ana_zn+"
    ON
    (
     (lstcombi.id = "+tb_ana_zn+".fk_idCombi_z1)
     )
    )as t1
   GROUP BY tip having (((t1.id=t1.fk_idCombi_z1) or (t1.fk_idCombi_z1 is null)))
   order by id asc;
#endif

 QString tb_ana_zn = "Ref_ana_z1";

 QGridLayout *lay_return = new QGridLayout;
 QWidget *wTop_1 = new QWidget;
 //QFormLayout * design_onglet_1 = MonLayout_VoisinsPresent();


 QSqlQueryModel *sqm_r1 = new QSqlQueryModel;
 QTableView *qtv_tmp = new QTableView;
 //QTableView *qtv_tmp = gtbv_SelectionBoulesDeZone[2];
 gtbv_SelectionBoulesDeZone[2]= qtv_tmp;
 QString st_msg ="";


 st_msg = "select t1.id as id, t1.tip as Repartition, count(t1.fk_idCombi_z1)as T, "
          "count (case when t1.J like 'lun%' then 1 end)as LUN, "
          "count (case when t1.J like 'mer%' then 1 end)as MER , "
          "count (case when t1.J like 'sam%' then 1 end)as SAM  "
          " from ( "
          "SELECT lstcombi.id,lstcombi.tip,"+tb_ana_zn+".fk_idCombi_z1,"+tb_ana_zn+".id as id2, "
                                                                                   "(case when "+tb_ana_zn+".id is null then null else  "
                                                                                                           "( "
                                                                                                           " select tirages.jour_tirage from tirages where (tirages.id = "+tb_ana_zn+".id) "
                                                                                                                                                                                     ") "
                                                                                                                                                                                     "end) as J "
                                                                                                                                                                                     "FROM lstcombi "
                                                                                                                                                                                     "LEFT JOIN "+tb_ana_zn+" "
                                                                                                                                                                                                            "ON  "
                                                                                                                                                                                                            "( "
                                                                                                                                                                                                            " (lstcombi.id = "+tb_ana_zn+".fk_idCombi_z1)  "
                                                                                                                                                                                                                                         ") "
                                                                                                                                                                                                                                         " )as t1 "
                                                                                                                                                                                                                                         " GROUP BY tip having (((t1.id=t1.fk_idCombi_z1) or (t1.fk_idCombi_z1 is null)))  "
                                                                                                                                                                                                                                         " order by id asc; ";


 sqm_r1->setQuery(st_msg,db_0);
 //sqm_r1->setHeaderData(1,Qt::Horizontal,"Combinaison");

#if 0
 QSortFilterProxyModel *m=new QSortFilterProxyModel();
 m->setDynamicSortFilter(true);
 m->setSourceModel(sqm_r1);
#endif
 // Filtre
 QFormLayout *FiltreLayout = new QFormLayout;
 int nbLines=0;
 FiltreCombinaisons *fltComb_tmp = new FiltreCombinaisons(nbLines);
 QList<qint32> colid;
 colid << 1;
 fltComb_tmp->setFiltreConfig(sqm_r1,qtv_tmp,colid);

 //fltComb_tmp->setFiltreConfig(sqm_r1,qtv_tmp,1);
 FiltreLayout->addRow("&Filtre Repartition", fltComb_tmp);

 qtv_tmp->setFixedSize(320,150);
 qtv_tmp->setColumnWidth(1,70);
 for(int j=2;j<=sqm_r1->columnCount();j++)
  qtv_tmp->setColumnWidth(j,35);
 // Ne pas modifier largeur des colonnes
 qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);


 qtv_tmp->hideColumn(0);
 qtv_tmp->setSortingEnabled(true);
 qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);

 //tabN1->addTab(tv_r1,"Combi");
 lay_return->addLayout(FiltreLayout,0,0,Qt::AlignLeft|Qt::AlignTop);
 lay_return->addWidget(qtv_tmp,1,0,Qt::AlignLeft|Qt::AlignTop);
 wTop_1->setLayout(lay_return);

 tabN1->addTab(wTop_1,"c");;

 // Mettre le dernier tirage en evidence
 QSqlQuery selection(db_0);

 st_msg = "select "+tb_ana_zn+".id, "+tb_ana_zn+".fk_idCombi_z1 from "+tb_ana_zn+" limit 1;";
 status = selection.exec(st_msg);
 status = selection.first();
 if(selection.isValid())
 {
  int value = selection.value(1).toInt();
  //tv_r1->setItemDelegate(new MaQtvDelegation(NULL,value-1,1));

  QAbstractItemModel *mon_model = qtv_tmp->model();
  //QStandardItemModel *dest= (QStandardItemModel*) mon_model;
  QModelIndex mdi_item1 = mon_model->index(0,0);

  if (mdi_item1.isValid()){
   //mdi_item1 = mdi_item1.model()->index(value-1,1);
   mdi_item1 = mon_model->index(value-1,1);
   QPersistentModelIndex depart(mdi_item1);

   qtv_tmp->selectionModel()->setCurrentIndex(mdi_item1, QItemSelectionModel::NoUpdate);
   qtv_tmp->scrollTo(mdi_item1);
   qtv_tmp->setItemDelegate(new MaQtvDelegation(depart));
  }
 }

 // click dans fenetre ma selection
 connect( gtbv_SelectionBoulesDeZone[2], SIGNAL( clicked(QModelIndex)) ,
   this, SLOT( slot_CriteresTiragesConstruire( QModelIndex) ) );

#if 0
 // click dans fenetre
 connect( qtv_tmp, SIGNAL( doubleClicked(QModelIndex)) ,
          this, SLOT( slot_UneCombiChoisie (QModelIndex) ) );
#endif

}
void MainWindow::MonLayout_SelectionBoules(QTabWidget *tabN1,stTiragesDef &pConf)
{
 int nb_zn = pConf.nb_zone;

 gsim_SelectionBoulesDeZone= new QStandardItemModel*[nb_zn];
 QWidget **tmpT_Widget = new QWidget*[nb_zn];
 QFormLayout **layT_MaSelection = new QFormLayout*[nb_zn];
 int *nbcol = new int [nb_zn];

 int  i=0,j=0, cell_val=0;

 for(int zn = 0;zn<nb_zn;zn++)
 {
  nbcol[zn] = (pConf.limites[zn].max)%pConf.nbElmZone[zn]?
               (pConf.limites[zn].max/pConf.nbElmZone[zn])+1:
               (pConf.limites[zn].max/pConf.nbElmZone[zn]);

  gsim_SelectionBoulesDeZone[zn]= new QStandardItemModel(pConf.nbElmZone[zn],nbcol[zn]);
  gtbv_SelectionBoulesDeZone[zn] = new QTableView;
  tmpT_Widget[zn] = new QWidget;
  gtbv_SelectionBoulesDeZone[zn]->setFixedSize(320,175);
  gtbv_SelectionBoulesDeZone[zn]->verticalHeader()->hide();
  gtbv_SelectionBoulesDeZone[zn]->horizontalHeader()->hide();

  for(i=1;i<=pConf.nbElmZone[zn];i++)/// Code a verifier en fonction bornes max
  { // Dans le cas max > 50
   for(j=1;j<=nbcol[zn];j++)
   {
    cell_val = j+(i-1)*nbcol[zn];
    if(cell_val<=pConf.limites[zn].max){
     QStandardItem *item = new QStandardItem( QString::number(i));
     item->setData(cell_val,Qt::DisplayRole);
     gsim_SelectionBoulesDeZone[zn]->setItem(i-1,j-1,item);
    }
   }
  }

  gtbv_SelectionBoulesDeZone[zn]->setModel(gsim_SelectionBoulesDeZone[zn]);
  gtbv_SelectionBoulesDeZone[zn]->setAlternatingRowColors(true);
  gtbv_SelectionBoulesDeZone[zn]->setEditTriggers(QAbstractItemView::NoEditTriggers);

  layT_MaSelection[zn] = new QFormLayout;
  layT_MaSelection[zn]->addWidget(gtbv_SelectionBoulesDeZone[zn]);

  for(j=0;j<10;j++)
  {
   gtbv_SelectionBoulesDeZone[zn]->setColumnWidth(j,30);
  }

  tmpT_Widget[zn]->setLayout(layT_MaSelection[zn]);
  tabN1->addTab(tmpT_Widget[zn],tr(configJeu.nomZone[zn].toLocal8Bit()));

  QString st_objName = "";
  st_objName = gtbv_SelectionBoulesDeZone[zn]->objectName();

  // click dans fenetre ma selection
  connect( gtbv_SelectionBoulesDeZone[zn], SIGNAL( clicked(QModelIndex)) ,
           this, SLOT( slot_CriteresTiragesConstruire( QModelIndex) ) );

#if 0
  // click dans fenetre ma selection
  connect( gtbv_SelectionBoulesDeZone[zn], SIGNAL( doubleClicked(QModelIndex)) ,
           this, SLOT( slot_UneSelectionActivee( QModelIndex) ) );
#endif
 }
}

QGridLayout *MainWindow::MonLayout_VoisinsPresent()
{
 QGridLayout *lay_return = new QGridLayout;

 int nb_zn = configJeu.nb_zone;
 int nb_colH = 9;
 QTabWidget *tabWidget = new QTabWidget;

 G_tbv_Voisins = new QTableView*[nb_zn];
 G_sim_Voisins= new QStandardItemModel*[nb_zn];
 G_lab_nbSorties = new LabelClickable*[nb_zn];
 QFormLayout **layT_Voisin = new QFormLayout*[nb_zn];
 QWidget **tmpT_Widget = new QWidget*[nb_zn];

 for(int zn = 0;zn<nb_zn;zn++)
 {
  QTableView *tmpTblView = new QTableView;
  QStandardItemModel * tmpStdItem =  new QStandardItemModel(configJeu.limites[zn].max,nb_colH);
  LabelClickable *tmpLabel = new LabelClickable;
  QFormLayout *tmpLayout = new QFormLayout;
  QWidget *tmpWidget = new QWidget;

  tmpLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
  G_sim_Voisins[zn]= tmpStdItem;
  G_tbv_Voisins[zn] = tmpTblView;
  G_lab_nbSorties[zn]= tmpLabel;
  layT_Voisin[zn] = tmpLayout;
  tmpT_Widget[zn] = tmpWidget;

  // entete du modele
  G_sim_Voisins[zn]->setHeaderData(0,Qt::Horizontal,"B");
  G_sim_Voisins[zn]->setHeaderData(1,Qt::Horizontal,"V:r0");
  G_sim_Voisins[zn]->setHeaderData(2,Qt::Horizontal,"V:+1");
  G_sim_Voisins[zn]->setHeaderData(3,Qt::Horizontal,"V:+2");
  G_sim_Voisins[zn]->setHeaderData(4,Qt::Horizontal,"V:-1");
  G_sim_Voisins[zn]->setHeaderData(5,Qt::Horizontal,"V:-2");
  G_sim_Voisins[zn]->setHeaderData(6,Qt::Horizontal,"T:+1");
  G_sim_Voisins[zn]->setHeaderData(7,Qt::Horizontal,"T:+2");
  G_sim_Voisins[zn]->setHeaderData(8,Qt::Horizontal,"NbS");

  // Ecriture du numero de boule et reservation item position
  for(int i=1;i<=configJeu.limites[zn].max;i++)
  {
   QStandardItem *item = new QStandardItem();
   item->setData(i,Qt::DisplayRole);
   G_sim_Voisins[zn]->setItem(i-1,0,item);
   for (int j =1; j<nb_colH;j++)
   {
    QStandardItem *item_2 = new QStandardItem();
    G_sim_Voisins[zn]->setItem(i-1,j,item_2);
   }
  }

  G_tbv_Voisins[zn]->setModel(G_sim_Voisins[zn]);
  for(int i=0;i<=nb_colH-1;i++)
  {
   G_tbv_Voisins[zn]->setColumnWidth(i,50);
  }

  //qtvT_Voisins[zn]->setMinimumHeight(390);
  //qtvT_Voisins[zn]->setMaximumWidth(400);

  G_tbv_Voisins[zn]->setSortingEnabled(true);
  G_tbv_Voisins[zn]->sortByColumn(0,Qt::AscendingOrder);
  G_tbv_Voisins[zn]->setAlternatingRowColors(true);
  G_tbv_Voisins[zn]->setEditTriggers(QAbstractItemView::NoEditTriggers);
  G_tbv_Voisins[zn]->setSelectionBehavior(QAbstractItemView::SelectItems);
  //qtvT_Voisins[zn]->setSelectionMode(QAbstractItemView::SingleSelection);
  //qtvT_Voisins[zn]->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::SelectedClicked);
  G_tbv_Voisins[zn]->setFixedSize(500,390);

  G_lab_nbSorties[zn]->setText("Nb total de sorties:");
  layT_Voisin[zn]->addWidget(G_lab_nbSorties[zn]);
  layT_Voisin[zn]->addWidget(G_tbv_Voisins[zn]);

  tmpT_Widget[zn]->setLayout(layT_Voisin[zn]);
  //tmpT_Widget[zn]->setMaximumWidth(420);
  tabWidget->addTab(tmpT_Widget[zn],tr(configJeu.nomZone[zn].toLocal8Bit()));

  // click dans fenetre voisin pour afficher boule
  connect( G_tbv_Voisins[zn], SIGNAL( clicked(QModelIndex)) ,
           this, SLOT( slot_MontrerBouleDansBase( QModelIndex) ) );

  // double click dans fenetre voisin pour afficher details boule
  connect( G_tbv_Voisins[zn], SIGNAL( doubleClicked(QModelIndex)) ,
           this, SLOT( slot_RechercherLesTirages( QModelIndex) ) );

  // Double click sur libellé recherche boule
  connect( G_lab_nbSorties[zn], SIGNAL( clicked(QString)) ,
           this, SLOT( slot_RepererLesTirages(QString) ) );


 }

 // Memo du design
 lay_return->addWidget(tabWidget);

 return(lay_return);
}


QGridLayout *MainWindow::MonLayout_VoisinsPresent_v2()
{
 QGridLayout *lay_return = new QGridLayout;
 bool ret = false;
 QSqlQuery query(db_0);
 QString msg1;
 int maxBoules = 49;

 // Creation de la table ou pas
 msg1 =  "CREATE table if not exists VoisinSql "
         "(id INTEGER PRIMARY KEY, r0 int, r1 int, rn1 int, tot int);";
 ret = query.exec(msg1);

 msg1="select count (id) from VoisinSql;";
 ret = query.exec(msg1);
 query.first();
 if(query.isValid())
 {
  int nblgn= query.value(0).toInt();

  if(!nblgn)
  {
   msg1 = "insert into VoisinSql (id) "
          "select id from oazb where id <="+QString::number(maxBoules)+";";
   ret = query.exec(msg1);
  }
 }

 // Mettre la table a Zero
 msg1 = "update VoisinSql "
        "set r0=-1,r1=-1,rn1=-1,tot=-1 "
        "where VoisinSql.id<="+QString::number(maxBoules)+";";
 ret = query.exec(msg1);

 // Ratacher la table a la Qview dans le bon onglet
 QSqlTableModel *tblModel = new QSqlTableModel;
 tblModel->setTable("VoisinSql");
 tblModel->select();
 // Associer toutes les valeurs a la vue
 while (tblModel->canFetchMore())
 {
  tblModel->fetchMore();
 }

 // Attach it to the view
 QTableView *qtv_tmp  = new QTableView;
 // Gestion du QTableView
 qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 //qtv_tmp->setStyleSheet("QTableView {selection-background-color: red;}");
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setFixedHeight(450);

 qtv_tmp->setModel(tblModel);
 qtv_tmp->setSortingEnabled(true);
 qtv_tmp->sortByColumn(0,Qt::AscendingOrder);

 for(int i=0;i<tblModel->columnCount();i++)
  qtv_tmp->setColumnWidth(i,30);



 // Memo du design
 lay_return->addWidget(qtv_tmp);
 G_tbv_TabPrevision_v2 = qtv_tmp;
 G_tblPrev_v2 = tblModel;

 return(lay_return);
}

//__________
void MainWindow::TST_CombiVoisin(int key)
{
 QSqlQuery query(db_0);
 QString msg = "";
 bool status = false;

 // Voir si il y a deja des indices
 msg = "select count (*) from DistriCombi";
 status = query.exec(msg);

 if(status){
  query.first();

  if(query.isValid()){
   int calcul = query.value(0).toInt();

   if(calcul == 0){
    msg = "insert into DistriCombi (id_com,tip) select id, tip  from lstcombi;";
    status = query.exec(msg);
    G_tab_1Model->select();
    TST_CombiVoisin(key);
   }
   else
   {
    // On deja des infos remplir les colonnes de total
    for(int i = 0; i<4;i++)
    {
     TST_NbRepartionCombi(i,key);
    }
    G_tab_1Model->select();
   }
  }
  else
  {
   ; //rien
  }
 }

}

void MainWindow::TST_NbRepartionCombi(int ecart,int key)
{
 QSqlQuery query(db_0);
 QSqlQuery sq_modif(db_0);
 QString msg = "";
 QString colNames[4]={"s1","s2","p1","p2"};
 int d[4]={1,2,-1,-2};
 bool status = false;

 QString tb_ana_zn = "Ref_ana_z1";

 msg =   "select id, count(fk_idCombi_z1) as T  from (SELECT lstcombi.id,t2.fk_idCombi_z1 "
         "FROM lstcombi "
         "LEFT JOIN (select * "
         "from (select "+tb_ana_zn+".id "
                                   "from "+tb_ana_zn+" where "+tb_ana_zn+".fk_idCombi_z1 = "
         +QString::number(key)+
         ") as t1 "
         "left join "+tb_ana_zn+" on t1.id = "+tb_ana_zn+".id + "
         +QString::number(d[ecart])+
         ") as t2 "
         "ON lstcombi.id = t2.fk_idCombi_z1)as t1 "
         "GROUP BY id having ((t1.id=t1.fk_idCombi_z1)or (t1.fk_idCombi_z1 is null)) "
         "order by t1.id asc;";

#ifndef QT_NO_DEBUG
 qDebug() << msg;
#endif

 status = query.exec(msg);
 if(status){
  query.first();
  if(query.isValid())
  {
   msg =   "update DistriCombi "
           "set "+colNames[ecart]+"=:LeTotal "
                                  "where (id_com=:LaCombi);";
   sq_modif.prepare(msg);
   int ligne = 0;
   int tot = 0;
   do
   {

    ligne =  query.value(0).toInt();
    tot =query.value(1).toInt();

    sq_modif.bindValue(":LaCombi", ligne);
    sq_modif.bindValue(":LeTotal", tot);
    status = sq_modif.exec();

   }while(query.next() && status);
  }
 }
}

//____________
//----------
QGridLayout * MainWindow::MonLayout_VoisinDistribution()
{
 QGridLayout *lay_return = new QGridLayout;

 //G_tbv_Absents = new QTableView*[2];
 G_sim_Absents= new QStandardItemModel*[2];
 G_lab_nbAbsents = new QLabel*[2];

 QString stColName[4]={"T+1","T+2","T-1","T-2"};

 G_lab_CritereCombi = new QLabel;
 QTableView *qtv_tmp  = new QTableView;

 G_lab_CritereCombi->setText("Critere:");
 G_tab_1Model = new QSqlTableModel;
 G_tab_1Model->setTable("DistriCombi");
 G_tab_1Model->select();

 // Filtre
 QFormLayout *FiltreLayout = new QFormLayout;
 int nbLines=0;
 FiltreCombinaisons *fltComb_tmp = new FiltreCombinaisons(nbLines);
 QList<qint32> colid;
 colid << 2;
 fltComb_tmp->setFiltreConfig(G_tab_1Model,qtv_tmp,colid);

 FiltreLayout->addRow("&Filtre Repartition", fltComb_tmp);

 qtv_tmp->setSortingEnabled(true);
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 qtv_tmp->hideColumn(0); // don't show the ID
 qtv_tmp->hideColumn(1);
 qtv_tmp->verticalHeader()->hide();

 // Taille/Nom des colonnes
 qtv_tmp->setColumnWidth(2,70);
 G_tab_1Model->setHeaderData(2, Qt::Horizontal, tr("Repartition"));
 for(int j=3;j<7;j++)
 {
  qtv_tmp->setColumnWidth(j,50);
  G_tab_1Model->setHeaderData(j, Qt::Horizontal, stColName[j-3]);
 }

 // Taille du tableau dans onglet
 qtv_tmp->setFixedSize(345,410);
 //G_tab_1->show();

 // Double click dans sous fenetre
 connect( qtv_tmp, SIGNAL( doubleClicked(QModelIndex)) ,
          this, SLOT( slot_F5_RechercherLesTirages( QModelIndex) ) );


 lay_return->addWidget(G_lab_CritereCombi,0,0,Qt::AlignLeft|Qt::AlignTop);
 lay_return->addLayout(FiltreLayout,1,0,Qt::AlignLeft|Qt::AlignTop);
 lay_return->addWidget(qtv_tmp,2,0,Qt::AlignLeft|Qt::AlignTop);

 qtv_tmp = G_tbv_CombiSourceSelection;
 return(lay_return);
}

#if 0
QFormLayout * MainWindow::MonLayout_VoisinsAbsent()
{
 QFormLayout *lay_return = new QFormLayout;

 int nb_zn = configJeu.nb_zone;
 QTabWidget *tab_conteneur = new QTabWidget;

 G_tbv_Absents = new QTableView*[nb_zn];
 G_sim_Absents= new QStandardItemModel*[nb_zn];
 G_lab_nbAbsents = new QLabel*[nb_zn];
 QFormLayout **layT_Absents = new QFormLayout*[nb_zn];
 QWidget **tmpT_Widget = new QWidget*[nb_zn];

 for(int zn = 0;zn<nb_zn;zn++)
 {
  QTableView *tmpTblView = new QTableView;
  QStandardItemModel * tmpStdItem =  new QStandardItemModel(configJeu.limites[zn].max,7);
  QLabel *tmpLabel = new QLabel;
  QFormLayout *tmpLayout = new QFormLayout;
  QWidget *tmpWidget = new QWidget;

  G_sim_Absents[zn]= tmpStdItem;
  G_tbv_Absents[zn] = tmpTblView;
  G_lab_nbAbsents[zn]= tmpLabel;
  layT_Absents[zn] = tmpLayout;
  tmpT_Widget[zn] = tmpWidget;

  // entete du modele
  G_sim_Absents[zn]->setHeaderData(0,Qt::Horizontal,"B");
  G_sim_Absents[zn]->setHeaderData(1,Qt::Horizontal,"V:r0");
  G_sim_Absents[zn]->setHeaderData(2,Qt::Horizontal,"V:+1");
  G_sim_Absents[zn]->setHeaderData(3,Qt::Horizontal,"V:+2");
  G_sim_Absents[zn]->setHeaderData(4,Qt::Horizontal,"V:-1");
  G_sim_Absents[zn]->setHeaderData(5,Qt::Horizontal,"V:-2");
  G_sim_Absents[zn]->setHeaderData(6,Qt::Horizontal,"NbS");

  // Ecriture du numero de boule et reservation item position
  for(int i=1;i<=configJeu.limites[zn].max;i++)
  {
   QStandardItem *item = new QStandardItem();
   item->setData(i,Qt::DisplayRole);
   G_sim_Absents[zn]->setItem(i-1,0,item);
   for (int j =1; j<7;j++)
   {
    QStandardItem *item_2 = new QStandardItem();
    G_sim_Absents[zn]->setItem(i-1,j,item_2);
   }
  }

  G_tbv_Absents[zn]->setModel(G_sim_Absents[zn]);
  for(int i=0;i<=6;i++)
  {
   G_tbv_Absents[zn]->setColumnWidth(i,50);
  }

  //qtvT_Voisins[zn]->setMinimumHeight(390);
  //qtvT_Voisins[zn]->setMaximumWidth(400);

  G_tbv_Absents[zn]->setSortingEnabled(true);
  G_tbv_Absents[zn]->sortByColumn(0,Qt::AscendingOrder);
  G_tbv_Absents[zn]->setAlternatingRowColors(true);
  G_tbv_Absents[zn]->setEditTriggers(QAbstractItemView::NoEditTriggers);
  G_tbv_Absents[zn]->setSelectionBehavior(QAbstractItemView::SelectItems);
  //qtvT_Voisins[zn]->setSelectionMode(QAbstractItemView::SingleSelection);
  //qtvT_Voisins[zn]->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::SelectedClicked);
  G_tbv_Absents[zn]->setFixedSize(395,390);

  G_lab_nbAbsents[zn]->setText("Nb total de sorties:");
  layT_Absents[zn]->addWidget(G_lab_nbAbsents[zn]);
  layT_Absents[zn]->addWidget(G_tbv_Absents[zn]);

  tmpT_Widget[zn]->setLayout(layT_Absents[zn]);
  //tmpT_Widget[zn]->setMaximumWidth(420);
  tab_conteneur->addTab(tmpT_Widget[zn],tr(configJeu.nomZone[zn].toLocal8Bit()));

  // click dans fenetre voisin pour afficher boule
  connect( G_tbv_Absents[zn], SIGNAL( clicked(QModelIndex)) ,
           this, SLOT( slot_MontrerBouleDansBase( QModelIndex) ) );

  // double click dans fenetre voisin pour afficher details boule
  connect( G_tbv_Absents[zn], SIGNAL( doubleClicked(QModelIndex)) ,
           this, SLOT( slot_RechercherLesTirages( QModelIndex) ) );

 }

 // Memo du design
 lay_return->addWidget(tab_conteneur);

 return(lay_return);
}
#endif
//--------

void MainWindow::FEN_Voisins(void)
{
 QWidget *qw_Voisins = new QWidget;

 // Onglet pere
 QTabWidget *tab_Top = new QTabWidget;
 QWidget **wid_ForTop = new QWidget*[3];

 QWidget *wTop_1 = new QWidget;
 QWidget *wTop_2 = new QWidget;
 QWidget *wTop_3 = new QWidget;

 wid_ForTop[0]= wTop_1;
 wid_ForTop[1]= wTop_2;
 wid_ForTop[2]= wTop_3;

 tab_Top->addTab(wid_ForTop[0],tr("Boules"));
 tab_Top->addTab(wid_ForTop[1],tr("Bv2"));
 tab_Top->addTab(wid_ForTop[2],tr("Distribution"));
 // ------------------

 QGridLayout * design_onglet_1 = MonLayout_VoisinsPresent();
 QGridLayout * design_onglet_2 = MonLayout_VoisinsPresent_v2();
 QGridLayout * design_onglet_3 = MonLayout_VoisinDistribution();

 wid_ForTop[0]->setLayout(design_onglet_1);
 wid_ForTop[1]->setLayout(design_onglet_2);
 wid_ForTop[2]->setLayout(design_onglet_3);

 QFormLayout *mainLayout = new QFormLayout;
 mainLayout->addWidget(tab_Top);

 qw_Voisins->setMinimumWidth(480);
 qw_Voisins->setLayout(mainLayout);
 qw_Voisins->setWindowTitle("Voisins de selection");

 QMdiSubWindow *subWindow = zoneCentrale->addSubWindow(qw_Voisins);
 subWindow->resize(580,570);
 subWindow->move(1180,0);
 //zoneCentrale->addSubWindow(qw_Voisins);
 qw_Voisins->setVisible(false);
}

#ifdef USE_OLD_CODE
void MainWindow::fen_Voisins(void)
{
 QWidget *qw_Voisins = new QWidget;
 QTabWidget *tabWidget = new QTabWidget;
 int nb_zn = configJeu.nb_zone;

 G_tbv_Voisins = new QTableView*[nb_zn];
 G_sim_Voisins= new QStandardItemModel*[nb_zn];
 G_lab_nbSorties = new QLabel*[nb_zn];
 QFormLayout **layT_Voisin = new QFormLayout*[nb_zn];
 QWidget **tmpT_Widget = new QWidget*[nb_zn];

 // Onglet pere
 QTabWidget *tab_Top = new QTabWidget;
 QWidget **wid_ForTop = new QWidget*[2];
 QFormLayout **lay_ForTop = new QFormLayout*[2];

 QWidget *wTop_1 = new QWidget;
 QWidget *wTop_2 = new QWidget;
 QFormLayout *lTop_1 = new QFormLayout;
 QFormLayout *lTop_2 = new QFormLayout;

 wid_ForTop[0]= wTop_1;
 wid_ForTop[1]= wTop_2;
 lay_ForTop[0]= lTop_1;
 lay_ForTop[1]= lTop_2;
 tab_Top->addTab(wid_ForTop[0],tr("Present"));
 tab_Top->addTab(wid_ForTop[1],tr("Absent"));
 // ------------------

 for(int zn = 0;zn<nb_zn;zn++)
 {
  QTableView *tmpTblView = new QTableView;
  QStandardItemModel * tmpStdItem =  new QStandardItemModel(configJeu.limites[zn].max,7);
  QLabel *tmpLabel = new QLabel;
  QFormLayout *tmpLayout = new QFormLayout;
  QWidget *tmpWidget = new QWidget;

  G_sim_Voisins[zn]= tmpStdItem;
  G_tbv_Voisins[zn] = tmpTblView;
  G_lab_nbSorties[zn]= tmpLabel;
  layT_Voisin[zn] = tmpLayout;
  tmpT_Widget[zn] = tmpWidget;

  // entete du modele
  G_sim_Voisins[zn]->setHeaderData(0,Qt::Horizontal,"B");
  G_sim_Voisins[zn]->setHeaderData(1,Qt::Horizontal,"V:r0");
  G_sim_Voisins[zn]->setHeaderData(2,Qt::Horizontal,"V:+1");
  G_sim_Voisins[zn]->setHeaderData(3,Qt::Horizontal,"V:+2");
  G_sim_Voisins[zn]->setHeaderData(4,Qt::Horizontal,"V:-1");
  G_sim_Voisins[zn]->setHeaderData(5,Qt::Horizontal,"V:-2");
  G_sim_Voisins[zn]->setHeaderData(6,Qt::Horizontal,"NbS");

  // Ecriture du numero de boule et reservation item position
  for(int i=1;i<=configJeu.limites[zn].max;i++)
  {
   QStandardItem *item = new QStandardItem();
   item->setData(i,Qt::DisplayRole);
   G_sim_Voisins[zn]->setItem(i-1,0,item);
   for (int j =1; j<7;j++)
   {
    QStandardItem *item_2 = new QStandardItem();
    G_sim_Voisins[zn]->setItem(i-1,j,item_2);
   }
  }

  G_tbv_Voisins[zn]->setModel(G_sim_Voisins[zn]);
  for(int i=0;i<=6;i++)
  {
   G_tbv_Voisins[zn]->setColumnWidth(i,50);
  }

  //qtvT_Voisins[zn]->setMinimumHeight(390);
  //qtvT_Voisins[zn]->setMaximumWidth(400);

  G_tbv_Voisins[zn]->setSortingEnabled(true);
  G_tbv_Voisins[zn]->sortByColumn(0,Qt::AscendingOrder);
  G_tbv_Voisins[zn]->setAlternatingRowColors(true);
  G_tbv_Voisins[zn]->setEditTriggers(QAbstractItemView::NoEditTriggers);
  G_tbv_Voisins[zn]->setSelectionBehavior(QAbstractItemView::SelectItems);
  //qtvT_Voisins[zn]->setSelectionMode(QAbstractItemView::SingleSelection);
  //qtvT_Voisins[zn]->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::SelectedClicked);
  G_tbv_Voisins[zn]->setFixedSize(395,390);

  G_lab_nbSorties[zn]->setText("Nb total de sorties:");
  layT_Voisin[zn]->addWidget(G_lab_nbSorties[zn]);
  layT_Voisin[zn]->addWidget(G_tbv_Voisins[zn]);

  tmpT_Widget[zn]->setLayout(layT_Voisin[zn]);
  //tmpT_Widget[zn]->setMaximumWidth(420);
  tabWidget->addTab(tmpT_Widget[zn],tr(configJeu.nomZone[zn].toLocal8Bit()));

  // click dans fenetre voisin pour afficher boule
  connect( G_tbv_Voisins[zn], SIGNAL( clicked(QModelIndex)) ,
           this, SLOT( slot_MontrerBouleDansBase( QModelIndex) ) );

  // double click dans fenetre voisin pour afficher details boule
  connect( G_tbv_Voisins[zn], SIGNAL( doubleClicked(QModelIndex)) ,
           this, SLOT( slot_RechercherLesTirages( QModelIndex) ) );

 }

 lay_ForTop[0]->addWidget(tabWidget);
 wid_ForTop[0]->setLayout(lay_ForTop[0]);

 //qw_Voisins->setMinimumHeight(420);
 qw_Voisins->setMinimumWidth(480);
 //qw_Voisins->setMaximumWidth(480);
 //qw_Voisins->setFixedSize(420,410);
 QFormLayout *mainLayout = new QFormLayout;

 mainLayout->addWidget(tab_Top);

 qw_Voisins->setLayout(mainLayout);
 qw_Voisins->setWindowTitle("Voisins de selection");

 //QMdiSubWindow *sousFenetre2 =
 zoneCentrale->addSubWindow(qw_Voisins);
 qw_Voisins->setVisible(true);
}
#endif

void MainWindow::FEN_Splitter(void)
{
 QSplitter *splitter = new QSplitter;

 splitter->setWindowTitle("Controle");
 splitter->show();
}
void MainWindow::FEN_ChoisirBoules(void)
{
 QWidget *qw_tmpWindows = new QWidget;
 QTabWidget *tw_tmp = new QTabWidget;
 QString st_titre = "Criteres recherche Tirages";

 gtbv_SelectionBoulesDeZone = new QTableView*[3];

 MonLayout_Selectioncombi(tw_tmp);
 MonLayout_SelectionBoules(tw_tmp,configJeu);


 QGridLayout *mainLayout = new QGridLayout;
 QHBoxLayout *hb_tmp1 = new QHBoxLayout;
 QHBoxLayout *hb_tmp2 = new QHBoxLayout;
 QLabel * lab_tmp1 = new QLabel;
 lab_critere = new QLabel;
 QString st_cri_titre= "Selection c-b-e en cours :";
 QPushButton *b_efface = new QPushButton ;
 QPushButton *b_valide = new QPushButton ;

 lab_tmp1->setText(st_cri_titre);
 lab_critere->setText("aucun - aucun - aucun");

 hb_tmp1->addWidget(lab_tmp1,0,Qt::AlignLeft|Qt::AlignTop);
 hb_tmp1->addWidget(lab_critere,0,Qt::AlignLeft|Qt::AlignTop);

 b_efface->setText("Efface");
 b_valide->setText("Cherche");
 hb_tmp2->addWidget(b_efface,0,Qt::AlignLeft|Qt::AlignTop);
 hb_tmp2->addWidget(b_valide,0,Qt::AlignLeft|Qt::AlignTop);

 mainLayout->addLayout(hb_tmp1,0,0,1,1,Qt::AlignLeft|Qt::AlignTop);
 mainLayout->addWidget(tw_tmp,1,0,1,1,Qt::AlignLeft|Qt::AlignTop);
 mainLayout->addLayout(hb_tmp2,2,0,1,1,Qt::AlignLeft|Qt::AlignTop);

 //qw_MaSelection->setMinimumHeight(200);
 //qw_tmpWindows->setFixedSize(390,220);
 qw_tmpWindows->setLayout(mainLayout);
 qw_tmpWindows->setWindowTitle(st_titre);


 QMdiSubWindow *subWindow = zoneCentrale->addSubWindow(qw_tmpWindows);
 subWindow->resize(380,325);
 subWindow->move(1200,570);

 qw_tmpWindows->setVisible(true);
 qw_tmpWindows->show();
 qw_tmpWindows->showMinimized();

 // double click dans fenetre voisin pour afficher details boule
 connect( b_valide, SIGNAL( clicked()) ,
          this, SLOT( slot_CriteresTiragesAppliquer() ) );

 // double click dans fenetre voisin pour afficher details boule
 connect( b_efface, SIGNAL( clicked()) ,
          this, SLOT( slot_CriteresTiragesEffacer() ) );


}


void MainWindow::FEN_Ecarts(void)
{

 QWidget *w_DataFenetre = new QWidget;
 // Onglet pere
 QTabWidget *tab_Top = new QTabWidget;
 QWidget **wid_ForTop = new QWidget*[7];
 QGridLayout **dsgOnglet = new QGridLayout * [7];
 QString ongNames[]={"Psb_1","Psb_2","Absents","Ecarts","Details","Parite","<n/2"};

 QGridLayout * (MainWindow::*ptrFunc[])()={
                                          &MainWindow::MonLayout_PrevoirTirage,&MainWindow::MonLayout_ChoixPossible,
                                          &MainWindow::MonLayout_Absent,&MainWindow::MonLayout_Ecarts,
                                          &MainWindow::MonLayout_Details,
                                          &MainWindow::MonLayout_Parite,&MainWindow::MonLayout_Nsur2
};


 for(int i =0; i< 7;i++)
 {
  wid_ForTop[i]= new QWidget;

  if(i>2)
   tab_Top->addTab(wid_ForTop[i],ongNames[i]);

  dsgOnglet[i]= (this->*ptrFunc[i])();
  wid_ForTop[i]->setLayout(dsgOnglet[i]);
 }


 QFormLayout *mainLayout = new QFormLayout;
 mainLayout->addWidget(tab_Top);

 //w_DataFenetre->setMinimumHeight(450);
 //w_DataFenetre->setFixedSize(320,450);
 w_DataFenetre->setLayout(mainLayout);
 w_DataFenetre->setWindowTitle("Boules");

 QMdiSubWindow *subWindow = zoneCentrale->addSubWindow(w_DataFenetre);
 subWindow->resize(350,570);
 subWindow->move(981,0);

 //zoneCentrale->addSubWindow(w_DataFenetre);
 w_DataFenetre->setVisible(true);
}

#if 0
void MainWindow::FEN_Ecarts(void)
{
 QWidget *w_DataFenetre = new QWidget;
 // Onglet pere
 QTabWidget *tab_Top = new QTabWidget;
 QWidget **wid_ForTop = new QWidget*[6];
 QGridLayout **dsgOnglet = new QGridLayout * [6];
 QString ongNames[]={"Psb_1","Psb_2","Absents","Ecarts","Parite","<n/2"};

 QGridLayout * (MainWindow::*ptrFunc[])()={
                                          &MainWindow::MonLayout_PrevoirTirage,&MainWindow::MonLayout_ChoixPossible,
                                          &MainWindow::MonLayout_Absent,&MainWindow::MonLayout_Ecarts,
                                          &MainWindow::MonLayout_Parite,&MainWindow::MonLayout_Nsur2
};


 for(int i =0; i< 6;i++)
 {
  wid_ForTop[i]= new QWidget;

  if(i>2)
   tab_Top->addTab(wid_ForTop[i],ongNames[i]);

  dsgOnglet[i]= (this->*ptrFunc[i])();
  wid_ForTop[i]->setLayout(dsgOnglet[i]);
 }


 QFormLayout *mainLayout = new QFormLayout;
 mainLayout->addWidget(tab_Top);

 //w_DataFenetre->setMinimumHeight(450);
 //w_DataFenetre->setFixedSize(320,450);
 w_DataFenetre->setLayout(mainLayout);
 w_DataFenetre->setWindowTitle("Boules");

 QMdiSubWindow *subWindow = zoneCentrale->addSubWindow(w_DataFenetre);
 subWindow->resize(350,570);
 subWindow->move(830,0);

 //zoneCentrale->addSubWindow(w_DataFenetre);
 w_DataFenetre->setVisible(true);
}
#endif


QGridLayout * MainWindow:: MonLayout_PrevoirTirage(void)
{
 QGridLayout *returnLayout = new QGridLayout;

 QTableView *qtv_tmp  = new QTableView;

 // Gestion du QTableView
 qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
 qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
 //qtv_tmp->setStyleSheet("QTableView {selection-background-color: red;}");
 qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
 qtv_tmp->setAlternatingRowColors(true);
 qtv_tmp->setFixedHeight(450);

 returnLayout->addWidget(qtv_tmp);

 G_tbv_TabPrevision = qtv_tmp;

 return(returnLayout);
}

QGridLayout * MainWindow:: MonLayout_ChoixPossible(void)
{
 QGridLayout *returnLayout = new QGridLayout;

 //QWidget *w_MesPossibles = new QWidget;
 QHeaderView *hdv_horizontal;
 QString Lib[6]={"tot","r0","+1","+2","-1","-2"};
 G_tbv_MesPossibles = new QTableView;
 int zn = 0;

 G_tbv_MesPossibles->setContextMenuPolicy(Qt::CustomContextMenu);
 connect(G_tbv_MesPossibles, SIGNAL(customContextMenuRequested(QPoint)),
         SLOT(customMenuRequested(QPoint)));

 hdv_horizontal = G_tbv_MesPossibles->horizontalHeader();
 hdv_horizontal->setContextMenuPolicy(Qt::CustomContextMenu); //set contextmenu
 connect(hdv_horizontal, SIGNAL(customContextMenuRequested( const QPoint& )),
         this, SLOT(tablev_customContextMenu( const QPoint& )));

 Gmen_TrieMesPossibles=new QMenu(this);
 Gacg_MesTries = new QActionGroup (this);

 Gmen_TrieMesPossibles->setWindowFlags(Qt::Tool);
 Gmen_TrieMesPossibles->setWindowTitle("Possibles");

 for(int i = 0; i< 6; i++){
  Gaci_MesPossibles[i]= new QAction(Lib[i],this);
  Gaci_MesPossibles[i]->setCheckable(true);
  Gaci_MesPossibles[i]->setData(i);
  Gacg_MesTries->addAction(Gaci_MesPossibles[i]);
 }
 connect(Gaci_MesPossibles[0], SIGNAL(triggered()), this, SLOT(slot_ft1Possibles()));
 connect(Gaci_MesPossibles[1], SIGNAL(triggered()), this, SLOT(slot_ft2Possibles()));
 connect(Gaci_MesPossibles[2], SIGNAL(triggered()), this, SLOT(slot_ft3Possibles()));
 connect(Gaci_MesPossibles[3], SIGNAL(triggered()), this, SLOT(slot_ft4Possibles()));
 connect(Gaci_MesPossibles[4], SIGNAL(triggered()), this, SLOT(slot_ft5Possibles()));
 connect(Gaci_MesPossibles[5], SIGNAL(triggered()), this, SLOT(slot_ft6Possibles()));


 Gaci_MesPossibles[0]->setChecked(true);
 for(int i = 0; i< 6; i++){
  Gmen_TrieMesPossibles->addAction(Gaci_MesPossibles[i]);
 }


 //qsim_MesPossibles = new MonToolTips(configJeu.limites[zn].max,5);
 G_sim_MesPossibles = new QStandardItemModel(configJeu.limites[zn].max,5);

 G_sim_MesPossibles->setHeaderData(0,Qt::Horizontal,"C1"); // Boules
 G_sim_MesPossibles->setHeaderData(1,Qt::Horizontal,"C2"); // Ecart en cours
 G_sim_MesPossibles->setHeaderData(2,Qt::Horizontal,"C3"); // ECart precedent
 G_sim_MesPossibles->setHeaderData(3,Qt::Horizontal,"C4"); // Ecart Moyen
 G_sim_MesPossibles->setHeaderData(4,Qt::Horizontal,"C5"); // Ecart Maxi

 // Ecriture du numero de boule et reservation item position
 for(int i=1;i<=configJeu.limites[zn].max;i++)
 {
  for (int j =0; j<5;j++)
  {
   QStandardItem *item_2 = new QStandardItem();
   G_sim_MesPossibles->setItem(i-1,j,item_2);
  }
 }

 G_tbv_MesPossibles->setModel(G_sim_MesPossibles);
 G_tbv_MesPossibles->setColumnWidth(0,45);
 G_tbv_MesPossibles->setColumnWidth(1,45);
 G_tbv_MesPossibles->setColumnWidth(2,45);
 G_tbv_MesPossibles->setColumnWidth(3,45);
 G_tbv_MesPossibles->setColumnWidth(4,45);
 G_tbv_MesPossibles->setSortingEnabled(false);
 G_tbv_MesPossibles->sortByColumn(0,Qt::AscendingOrder);
 G_tbv_MesPossibles->setAlternatingRowColors(true);
 G_tbv_MesPossibles->setEditTriggers(QAbstractItemView::NoEditTriggers);
 //tblCouverture->setMaximumWidth(500);
 G_tbv_MesPossibles->setFixedSize(280,450);
 //Gtv_MesPossibles->setMinimumHeight(420);
 //Gtv_MesPossibles->setMinimumWidth(280);

 returnLayout->addWidget(G_tbv_MesPossibles);


 connect( G_tbv_MesPossibles, SIGNAL( clicked(QModelIndex)) ,
          this, SLOT( slot_MontrerBouleDansBase( QModelIndex) ) );


 return(returnLayout);
}

//-------
QGridLayout * MainWindow:: MonLayout_Absent(void)
{
 QGridLayout *returnLayout = new QGridLayout;

 //QWidget *w_MesPossibles = new QWidget;
 QHeaderView *hdv_horizontal;
 QString Lib[5]={"r0","+1","+2","-1","-2"};
 G_tbv_LesAbsents = new QTableView;
 int zn = 0;

 G_tbv_LesAbsents->setContextMenuPolicy(Qt::CustomContextMenu);
 connect(G_tbv_LesAbsents, SIGNAL(customContextMenuRequested(QPoint)),
         SLOT(customMenuRequested(QPoint)));

 hdv_horizontal = G_tbv_LesAbsents->horizontalHeader();
 hdv_horizontal->setContextMenuPolicy(Qt::CustomContextMenu); //set contextmenu
 connect(hdv_horizontal, SIGNAL(customContextMenuRequested( const QPoint& )),
         this, SLOT(pop_selAbsents( const QPoint& )));

 G_men_TrieMesAbsents=new QMenu(this);
 G_acg_MesAbsents = new QActionGroup (this);

 G_men_TrieMesAbsents->setWindowFlags(Qt::Tool);
 G_men_TrieMesAbsents->setWindowTitle("Absents");

 for(int i = 0; i< 5; i++){
  G_aci_MesAbsents[i]= new QAction(Lib[i],this);
  G_aci_MesAbsents[i]->setCheckable(true);
  G_aci_MesAbsents[i]->setData(i);
  G_acg_MesAbsents->addAction(G_aci_MesAbsents[i]);
 }
 connect(G_aci_MesAbsents[0], SIGNAL(triggered()), this, SLOT(slot_ftAbs1()));
 connect(G_aci_MesAbsents[1], SIGNAL(triggered()), this, SLOT(slot_ftAbs2()));
 connect(G_aci_MesAbsents[2], SIGNAL(triggered()), this, SLOT(slot_ftAbs3()));
 connect(G_aci_MesAbsents[3], SIGNAL(triggered()), this, SLOT(slot_ftAbs4()));
 connect(G_aci_MesAbsents[4], SIGNAL(triggered()), this, SLOT(slot_ftAbs5()));


 G_aci_MesAbsents[0]->setChecked(true);
 for(int i = 0; i< 5; i++){
  G_men_TrieMesAbsents->addAction(G_aci_MesAbsents[i]);
 }


 //qsim_MesPossibles = new MonToolTips(configJeu.limites[zn].max,5);
 G_sim_LesAbsents = new QStandardItemModel(configJeu.limites[zn].max,5);

 G_sim_LesAbsents->setHeaderData(0,Qt::Horizontal,"C1"); // Boules
 G_sim_LesAbsents->setHeaderData(1,Qt::Horizontal,"C2"); // Ecart en cours
 G_sim_LesAbsents->setHeaderData(2,Qt::Horizontal,"C3"); // ECart precedent
 G_sim_LesAbsents->setHeaderData(3,Qt::Horizontal,"C4"); // Ecart Moyen
 G_sim_LesAbsents->setHeaderData(4,Qt::Horizontal,"C5"); // Ecart Maxi

 // Ecriture du numero de boule et reservation item position
 for(int i=1;i<=configJeu.limites[zn].max;i++)
 {
  for (int j =0; j<5;j++)
  {
   QStandardItem *item_2 = new QStandardItem();
   G_sim_LesAbsents->setItem(i-1,j,item_2);
  }
 }

 G_tbv_LesAbsents->setModel(G_sim_LesAbsents);
 G_tbv_LesAbsents->setColumnWidth(0,45);
 G_tbv_LesAbsents->setColumnWidth(1,45);
 G_tbv_LesAbsents->setColumnWidth(2,45);
 G_tbv_LesAbsents->setColumnWidth(3,45);
 G_tbv_LesAbsents->setColumnWidth(4,45);
 G_tbv_LesAbsents->setSortingEnabled(false);
 G_tbv_LesAbsents->sortByColumn(0,Qt::AscendingOrder);
 G_tbv_LesAbsents->setAlternatingRowColors(true);
 G_tbv_LesAbsents->setEditTriggers(QAbstractItemView::NoEditTriggers);
 //tblCouverture->setMaximumWidth(500);
 G_tbv_LesAbsents->setFixedSize(280,450);
 //Gtv_MesPossibles->setMinimumHeight(420);
 //Gtv_MesPossibles->setMinimumWidth(280);

 returnLayout->addWidget(G_tbv_LesAbsents);


 connect( G_tbv_LesAbsents, SIGNAL( clicked(QModelIndex)) ,
          this, SLOT( slot_MontrerBouleDansBase( QModelIndex) ) );


 return(returnLayout);
}
//-------
QGridLayout * MainWindow:: MonLayout_Ecarts(void)
{
 QGridLayout *returnLayout = new QGridLayout;

 int  i;
 int zn = 0;
 G_sim_Ecarts = new QStandardItemModel(configJeu.limites[zn].max,5);
 G_tbv_Ecarts = new QTableView;

 G_sim_Ecarts->setHeaderData(0,Qt::Horizontal,"B"); // Boules
 G_sim_Ecarts->setHeaderData(1,Qt::Horizontal,"Ec"); // Ecart en cours
 G_sim_Ecarts->setHeaderData(2,Qt::Horizontal,"Ep"); // ECart precedent
 G_sim_Ecarts->setHeaderData(3,Qt::Horizontal,"Em"); // Ecart Moyen
 G_sim_Ecarts->setHeaderData(4,Qt::Horizontal,"EM"); // Ecart Maxi

 for(i=1;i<=configJeu.limites[zn].max;i++)
 {
  QStandardItem *item = new QStandardItem( QString::number(i));
  item->setData(i,Qt::DisplayRole);
  G_sim_Ecarts->setItem(i-1,0,item);
 }
 G_tbv_Ecarts->setModel(G_sim_Ecarts);

 G_tbv_Ecarts->setColumnWidth(0,45);
 G_tbv_Ecarts->setColumnWidth(1,45);
 G_tbv_Ecarts->setColumnWidth(2,45);
 G_tbv_Ecarts->setColumnWidth(3,45);
 G_tbv_Ecarts->setColumnWidth(4,45);
 G_tbv_Ecarts->setSortingEnabled(true);
 G_tbv_Ecarts->sortByColumn(0,Qt::AscendingOrder);
 G_tbv_Ecarts->setAlternatingRowColors(true);
 G_tbv_Ecarts->setEditTriggers(QAbstractItemView::NoEditTriggers);
 //tblCouverture->setMaximumWidth(500);
 //Gtv_Ecarts->setMinimumHeight(420);
 //Gtv_Ecarts->setMinimumWidth(280);
 G_tbv_Ecarts->setFixedSize(280,450);

 returnLayout->addWidget(G_tbv_Ecarts);

 return(returnLayout);
}
// ---------
QGridLayout * MainWindow::MonLayout_Details()
{
 QGridLayout *lay_return = new QGridLayout;
 QTableView *tmpTblView_1 = new QTableView;
 QTableView *tmpTblView_2 = new QTableView;

 // Compter le nombre de colonne a creer
 QString sqlReq = "";
 QSqlQuery sql_req(db_0);
 bool status = false;

 sqlReq = "select * from repartition_bh limit 1;";
 status = sql_req.exec(sqlReq);
 if(status)
 {
  QSqlRecord ligne = sql_req.record();
  int nb_col = ligne.count();
  QStandardItemModel * tmpStdItem =  new QStandardItemModel(1,nb_col-2);
  tmpTblView_1->setModel(tmpStdItem);
  gsim_AnalyseUnTirage = tmpStdItem;
  int val = 0;
  for(int i = 2; i<nb_col-1; i++)
  {
   tmpStdItem->setHeaderData(i-2,Qt::Horizontal,ligne.fieldName(i));
   QStandardItem *item = new QStandardItem();

   // Recherche de la config pour le dernier tirage
   val=RechercheInfoTirages(1,i-2,&configJeu);
   item->setData(val,Qt::DisplayRole);
   tmpStdItem->setItem(0,i-2,item);
   tmpTblView_1->setColumnWidth(i-2,30);
  }

  tmpTblView_1->setEditTriggers(QAbstractItemView::NoEditTriggers);
  tmpTblView_1->setSelectionBehavior(QAbstractItemView::SelectItems);
  tmpTblView_1->setSelectionMode(QAbstractItemView::SingleSelection);
  tmpTblView_1->verticalHeader()->hide();
  tmpTblView_1->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  tmpTblView_1->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);


 }
 lay_return->addWidget(tmpTblView_1,0,0);

 // Construction table 2
 QStandardItemModel * tmpStdItem_2 =  new QStandardItemModel(6,2);
 tmpTblView_2->setModel(tmpStdItem_2);
 // entete du modele
 QStringList lstheader;
 lstheader << "Q" << "V:+1";
 tmpStdItem_2->setHorizontalHeaderLabels(lstheader);
 for(int y=0;y<6;y++)
 {
  for(int x=0;x<2;x++)
  {
   QStandardItem *item = new QStandardItem();
   if(x==0)
   {
    item->setData(y,Qt::DisplayRole);
   }
   tmpStdItem_2->setItem(y,x,item);
   tmpTblView_2->setColumnWidth(x,50);
  }
 }
 tmpTblView_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
 tmpTblView_2->setSelectionBehavior(QAbstractItemView::SelectItems);
 tmpTblView_2->setSelectionMode(QAbstractItemView::SingleSelection);
 tmpTblView_2->verticalHeader()->hide();
 tmpTblView_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
 tmpTblView_2->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

 lay_return->addWidget(tmpTblView_2,1,0);

 return(lay_return);
}

int MainWindow::RechercheInfoTirages(int idTirage, int leCritere,stTiragesDef *ref)
{
 int retval = -1;
 QString sql_req;
 QSqlQuery req(db_0);
 bool status = false;

 QStringList cri_msg;
 QString tb_ana_zn = "Ref_ana_z1";

 int nbBoules = floor(ref->limites[0].max/10)+1;

 cri_msg <<"z1%2=0"<<"z1<"+QString::number((ref->limites[0].max)/2);
 for(int j=0;j<=9;j++)
 {
  cri_msg<< "z1 like '%" + QString::number(j) + "'";
 }
 for(int j=0;j<nbBoules;j++)
 {
  cri_msg<< "z1 >="+QString::number(10*j)+ " and z1<="+QString::number((10*j)+9);
 }

 QString st_cri1_1= cri_msg.at(leCritere);
 int dst = 0;


 sql_req =
   "select tb3.id as id, tb5.id as pid,"
   "tb3.jour_tirage as J,"
   "substr(tb3.date_tirage,-2,2)||'/'||substr(tb3.date_tirage,6,2)||'/'||substr(tb3.date_tirage,1,4) as D,"
   "tb5.tip as C,"
   "tb3.b1 as b1, tb3.b2 as b2,tb3.b3 as b3,tb3.b4 as b4,tb3.b5 as b5,"
   "tb3.e1 as e1,"
   "tb2.N as N from tirages as tb3,"
   ""+tb_ana_zn+" as tb4, lstcombi as tb5 "
                "inner join"
                "("
                "select *  from "
                "("
                "select tb1.*, count(tb2.B) as N from tirages as tb1 "
                "left join"
                "("
                "select id as B from Bnrz where (z1 not null and ("
   +st_cri1_1+
   ")) "
   ")as tb2 "
   "on"
   "("
   "tb1.b1 = tb2.B or "
   "tb1.b2 = tb2.B or "
   "tb1.b3 = tb2.B or "
   "tb1.b4 = tb2.B or "
   "tb1.b5 = tb2.B"
   ")"
   "group by tb1.id"
   ") as ensemble_1 "
   ") as tb2 "
   "on ("
   "(tb3.id = tb2.id + "
   +QString::number(dst)
   +") "
    "and"
    "(tb4.id = tb3.id)"
    "and"
    "(tb4.fk_idCombi_z1 = tb5.id)"
    "and"
    "(tb2.id="
   +QString::number(idTirage)+
   ")"
   ")";

#ifndef QT_NO_DEBUG
 // Effacer fenetre de debug
 //qDebug("\0x1B[2J\0x1b[;H");
 //qDebug() << sql_req;
#endif

 status = req.exec(sql_req);
 if(status)
 {
  QSqlRecord ligne;
  if(req.first())
  {
   ligne = req.record();
   retval = ligne.value("N").toInt();
  }

 }
 else
 {
  ; // Pb
 }

 return retval;
}

QGridLayout * MainWindow::MonLayout_Parite()
{
 QGridLayout *lay_return = new QGridLayout;

 int nb_zn = configJeu.nb_zone;
 QTabWidget *tab_conteneur = new QTabWidget;

 gtbv_DernierTirageDetail = new QTableView*[nb_zn];
 gsim_DernierTirageDetail = new QStandardItemModel*[nb_zn];
 G_lab_PariteVoisin = new QLabel*[nb_zn];
 QFormLayout **layT_Tmp_1 = new QFormLayout*[nb_zn];
 QWidget **tmpT_Widget = new QWidget*[nb_zn];

 QTableView ** fn_refTbv = gtbv_DernierTirageDetail;
 QStandardItemModel ** fn_refSim = gsim_DernierTirageDetail;
 QLabel ** fn_refLab = G_lab_PariteVoisin;

 for(int zn = 0;zn<nb_zn;zn++)
 {
  QTableView *tmpTblView = new QTableView;
  QStandardItemModel * tmpStdItem =  new QStandardItemModel(6,3);
  QLabel *tmpLabel = new QLabel;
  QFormLayout *tmpLayout = new QFormLayout;
  QWidget *tmpWidget = new QWidget;

  fn_refSim[zn]= tmpStdItem;
  fn_refTbv[zn] = tmpTblView;
  fn_refLab[zn]= tmpLabel;
  layT_Tmp_1[zn] = tmpLayout;
  tmpT_Widget[zn] = tmpWidget;

  // entete du modele
  fn_refSim[zn]->setHeaderData(0,Qt::Horizontal,"BPair");
  fn_refSim[zn]->setHeaderData(1,Qt::Horizontal,"V:+1");
  fn_refSim[zn]->setHeaderData(2,Qt::Horizontal,"V:+2");

  // Ecriture du numero de boule et reservation item position
  for(int i=1;i<=configJeu.nbElmZone[zn]+1;i++)
  {
   QStandardItem *item = new QStandardItem();
   item->setData(i-1,Qt::DisplayRole);
   fn_refSim[zn]->setItem(i-1,0,item);

   for (int j = 1; j<=2;j++)
   {
    QStandardItem *item_2 = new QStandardItem();
    fn_refSim[zn]->setItem(i-1,j,item_2);
   }
  }

  fn_refTbv[zn]->setModel(fn_refSim[zn]);
  for(int i=0;i<=2;i++)
  {
   fn_refTbv[zn]->setColumnWidth(i,50);
  }

  fn_refTbv[zn]->setSortingEnabled(true);
  fn_refTbv[zn]->sortByColumn(0,Qt::AscendingOrder);
  fn_refTbv[zn]->setAlternatingRowColors(true);
  fn_refTbv[zn]->setEditTriggers(QAbstractItemView::NoEditTriggers);
  fn_refTbv[zn]->setSelectionBehavior(QAbstractItemView::SelectItems);
  //qtvT_Voisins[zn]->setSelectionMode(QAbstractItemView::SingleSelection);
  //qtvT_Voisins[zn]->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::SelectedClicked);
  fn_refTbv[zn]->setFixedSize(270,390);

  fn_refLab[zn]->setText("Nb total de sorties:");
  layT_Tmp_1[zn]->addWidget(fn_refLab[zn]);
  layT_Tmp_1[zn]->addWidget(fn_refTbv[zn]);

  tmpT_Widget[zn]->setLayout(layT_Tmp_1[zn]);
  tab_conteneur->addTab(tmpT_Widget[zn],tr(configJeu.nomZone[zn].toLocal8Bit()));

  // click dans fenetre voisin pour afficher boule
  //connect( fn_refTbv[zn], SIGNAL( clicked(QModelIndex)) ,
  //         this, SLOT( slot_MontrerBouleDansBase( QModelIndex) ) );

  // double click dans fenetre voisin pour afficher details boule
  connect( fn_refTbv[zn], SIGNAL( doubleClicked(QModelIndex)) ,
           this, SLOT( slot_F3_RechercherLesTirages( QModelIndex) ) );

 }

 // Memo du design
 lay_return->addWidget(tab_conteneur);

 return(lay_return);
}
//--------
QGridLayout *MainWindow::MonLayout_Nsur2()
{
 QGridLayout *lay_return = new QGridLayout;

 int nb_zn = configJeu.nb_zone;
 QTabWidget *tab_conteneur = new QTabWidget;

 G_tbv_Nsur2 = new QTableView*[nb_zn];
 G_sim_Nsur2= new QStandardItemModel*[nb_zn];
 G_lab_Nsur2 = new QLabel*[nb_zn];
 QFormLayout **layT_Tmp_1 = new QFormLayout*[nb_zn];
 QWidget **tmpT_Widget = new QWidget*[nb_zn];

 QTableView ** fn_refTbv = G_tbv_Nsur2;
 QStandardItemModel ** fn_refSim = G_sim_Nsur2;
 QLabel ** fn_refLab = G_lab_Nsur2;

 for(int zn = 0;zn<nb_zn;zn++)
 {
  QTableView *tmpTblView = new QTableView;
  QStandardItemModel * tmpStdItem =  new QStandardItemModel(6,3);
  QLabel *tmpLabel = new QLabel;
  QFormLayout *tmpLayout = new QFormLayout;
  QWidget *tmpWidget = new QWidget;

  fn_refSim[zn]= tmpStdItem;
  fn_refTbv[zn] = tmpTblView;
  fn_refLab[zn]= tmpLabel;
  layT_Tmp_1[zn] = tmpLayout;
  tmpT_Widget[zn] = tmpWidget;

  // entete du modele
  fn_refSim[zn]->setHeaderData(0,Qt::Horizontal,"BPair");
  fn_refSim[zn]->setHeaderData(1,Qt::Horizontal,"V:+1");
  fn_refSim[zn]->setHeaderData(2,Qt::Horizontal,"V:+2");

  // Ecriture du numero de boule et reservation item position
  for(int i=1;i<=configJeu.nbElmZone[zn]+1;i++)
  {
   QStandardItem *item = new QStandardItem();
   item->setData(i-1,Qt::DisplayRole);
   fn_refSim[zn]->setItem(i-1,0,item);

   for (int j = 1; j<=2;j++)
   {
    QStandardItem *item_2 = new QStandardItem();
    fn_refSim[zn]->setItem(i-1,j,item_2);
   }
  }

  fn_refTbv[zn]->setModel(fn_refSim[zn]);
  for(int i=0;i<=2;i++)
  {
   fn_refTbv[zn]->setColumnWidth(i,50);
  }

  fn_refTbv[zn]->setSortingEnabled(true);
  fn_refTbv[zn]->sortByColumn(0,Qt::AscendingOrder);
  fn_refTbv[zn]->setAlternatingRowColors(true);
  fn_refTbv[zn]->setEditTriggers(QAbstractItemView::NoEditTriggers);
  fn_refTbv[zn]->setSelectionBehavior(QAbstractItemView::SelectItems);
  //qtvT_Voisins[zn]->setSelectionMode(QAbstractItemView::SingleSelection);
  //qtvT_Voisins[zn]->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::SelectedClicked);
  fn_refTbv[zn]->setFixedSize(270,390);

  fn_refLab[zn]->setText("Nb total de sorties:");
  layT_Tmp_1[zn]->addWidget(fn_refLab[zn]);
  layT_Tmp_1[zn]->addWidget(fn_refTbv[zn]);

  tmpT_Widget[zn]->setLayout(layT_Tmp_1[zn]);
  tab_conteneur->addTab(tmpT_Widget[zn],tr(configJeu.nomZone[zn].toLocal8Bit()));

  // click dans fenetre voisin pour afficher boule
  //connect( fn_refTbv[zn], SIGNAL( clicked(QModelIndex)) ,
  //         this, SLOT( slot_MontrerBouleDansBase( QModelIndex) ) );

  // double click dans fenetre voisin pour afficher details boule
  connect( fn_refTbv[zn], SIGNAL( doubleClicked(QModelIndex)) ,
           this, SLOT( slot_F4_RechercherLesTirages( QModelIndex) ) );

 }

 // Memo du design
 lay_return->addWidget(tab_conteneur);

 return(lay_return);
}
//--------

#if 0
void MainWindow::fen_Ecarts(void)
{
 int  i;
 QWidget *qw_Ecarts = new QWidget;
 Gtv_Ecarts = new QTableView;
 int zn = 0;
 Gsim_Ecarts = new QStandardItemModel(configJeu.limites[zn].max,5);
 //modele2 = GererBase::modele2_0;

 Gsim_Ecarts->setHeaderData(0,Qt::Horizontal,"B"); // Boules
 Gsim_Ecarts->setHeaderData(1,Qt::Horizontal,"Ec"); // Ecart en cours
 Gsim_Ecarts->setHeaderData(2,Qt::Horizontal,"Ep"); // ECart precedent
 Gsim_Ecarts->setHeaderData(3,Qt::Horizontal,"Em"); // Ecart Moyen
 Gsim_Ecarts->setHeaderData(4,Qt::Horizontal,"EM"); // Ecart Maxi

 for(i=1;i<=configJeu.limites[zn].max;i++)
 {
  QStandardItem *item = new QStandardItem( QString::number(i));
  item->setData(i,Qt::DisplayRole);
  Gsim_Ecarts->setItem(i-1,0,item);
 }

 Gtv_Ecarts->setModel(Gsim_Ecarts);
 Gtv_Ecarts->setColumnWidth(0,45);
 Gtv_Ecarts->setColumnWidth(1,45);
 Gtv_Ecarts->setColumnWidth(2,45);
 Gtv_Ecarts->setColumnWidth(3,45);
 Gtv_Ecarts->setColumnWidth(4,45);
 Gtv_Ecarts->setSortingEnabled(true);
 Gtv_Ecarts->sortByColumn(0,Qt::AscendingOrder);
 Gtv_Ecarts->setAlternatingRowColors(true);
 Gtv_Ecarts->setEditTriggers(QAbstractItemView::NoEditTriggers);
 //tblCouverture->setMaximumWidth(500);
 Gtv_Ecarts->setMinimumHeight(420);
 Gtv_Ecarts->setMinimumWidth(280);

 QFormLayout *lay_Ecart = new QFormLayout;
 lay_Ecart->addWidget(Gtv_Ecarts);


 qw_Ecarts->setMinimumHeight(450);
 qw_Ecarts->setLayout(lay_Ecart);
 qw_Ecarts->setWindowTitle("Ecart boules");
 //QMdiSubWindow *sousFenetre3 =
 zoneCentrale->addSubWindow(qw_Ecarts);
 qw_Ecarts->setVisible(true);
 // Double click dans sous fenetre ecart
 connect( Gtv_Ecarts, SIGNAL( doubleClicked(QModelIndex)) ,
          this, SLOT( slot_qtvEcart( QModelIndex) ) );


}
#endif

#if 0
void MainWindow::fen_MesPossibles(void)
{
 QWidget *w_MesPossibles = new QWidget;
 QHeaderView *hdv_horizontal;
 QString Lib[6]={"tot","r0","+1","+2","-1","-2"};
 //QString Methode[6]={"ft1", "ft2", "ft3", "ft4", "ft5", "ft6"};
 Gtv_MesPossibles = new QTableView;
 int zn = 0;

 Gtv_MesPossibles->setContextMenuPolicy(Qt::CustomContextMenu);
 connect(Gtv_MesPossibles, SIGNAL(customContextMenuRequested(QPoint)),
         SLOT(customMenuRequested(QPoint)));

 hdv_horizontal = Gtv_MesPossibles->horizontalHeader();
 hdv_horizontal->setContextMenuPolicy(Qt::CustomContextMenu); //set contextmenu
 connect(hdv_horizontal, SIGNAL(customContextMenuRequested( const QPoint& )),
         this, SLOT(tablev_customContextMenu( const QPoint& )));

 Gmen_TrieMesPossibles=new QMenu(this);
 Gacg_MesTries = new QActionGroup (this);

 Gmen_TrieMesPossibles->setWindowFlags(Qt::Tool);
 Gmen_TrieMesPossibles->setWindowTitle("Trie");
 //menuTrieMesPossibles->add
 for(int i = 0; i< 6; i++){
  Gaci_MesPossibles[i]= new QAction(Lib[i],this);
  Gaci_MesPossibles[i]->setCheckable(true);
  Gaci_MesPossibles[i]->setData(i);
  Gacg_MesTries->addAction(Gaci_MesPossibles[i]);
 }
 connect(Gaci_MesPossibles[0], SIGNAL(triggered()), this, SLOT(ft1()));
 connect(Gaci_MesPossibles[1], SIGNAL(triggered()), this, SLOT(ft2()));
 connect(Gaci_MesPossibles[2], SIGNAL(triggered()), this, SLOT(ft3()));
 connect(Gaci_MesPossibles[3], SIGNAL(triggered()), this, SLOT(ft4()));
 connect(Gaci_MesPossibles[4], SIGNAL(triggered()), this, SLOT(ft5()));
 connect(Gaci_MesPossibles[5], SIGNAL(triggered()), this, SLOT(ft6()));


 Gaci_MesPossibles[0]->setChecked(true);
 for(int i = 0; i< 6; i++){
  Gmen_TrieMesPossibles->addAction(Gaci_MesPossibles[i]);
 }


 //qsim_MesPossibles = new MonToolTips(configJeu.limites[zn].max,5);
 Gsim_MesPossibles = new QStandardItemModel(configJeu.limites[zn].max,5);

 Gsim_MesPossibles->setHeaderData(0,Qt::Horizontal,"C1"); // Boules
 Gsim_MesPossibles->setHeaderData(1,Qt::Horizontal,"C2"); // Ecart en cours
 Gsim_MesPossibles->setHeaderData(2,Qt::Horizontal,"C3"); // ECart precedent
 Gsim_MesPossibles->setHeaderData(3,Qt::Horizontal,"C4"); // Ecart Moyen
 Gsim_MesPossibles->setHeaderData(4,Qt::Horizontal,"C5"); // Ecart Maxi

 Gtv_MesPossibles->setModel(Gsim_MesPossibles);
 Gtv_MesPossibles->setColumnWidth(0,45);
 Gtv_MesPossibles->setColumnWidth(1,45);
 Gtv_MesPossibles->setColumnWidth(2,45);
 Gtv_MesPossibles->setColumnWidth(3,45);
 Gtv_MesPossibles->setColumnWidth(4,45);
 Gtv_MesPossibles->setSortingEnabled(false);
 Gtv_MesPossibles->sortByColumn(0,Qt::AscendingOrder);
 Gtv_MesPossibles->setAlternatingRowColors(true);
 Gtv_MesPossibles->setEditTriggers(QAbstractItemView::NoEditTriggers);
 //tblCouverture->setMaximumWidth(500);
 Gtv_MesPossibles->setMinimumHeight(420);
 Gtv_MesPossibles->setMinimumWidth(280);

 QFormLayout *layCouverture = new QFormLayout;
 layCouverture->addWidget(Gtv_MesPossibles);


 w_MesPossibles->setMinimumHeight(450);
 w_MesPossibles->setLayout(layCouverture);
 w_MesPossibles->setWindowTitle("Mes possibles");

 connect( Gtv_MesPossibles, SIGNAL( clicked(QModelIndex)) ,
          this, SLOT( slot_MontrerBouleDansBase( QModelIndex) ) );

 //QMdiSubWindow *sousFenetre3 =
 zoneCentrale->addSubWindow(w_MesPossibles);
 w_MesPossibles->setVisible(true);
}
#endif

QGridLayout * MainWindow::MonLayout_pFnNsr1(stTiragesDef *pConf)
{
 QGridLayout *lay_return = new QGridLayout;
 int zone = 0;
 SyntheseGenerale::stPSynG a;
 a.pLaBase = DB_tirages;
 a.ptabSynt = gtab_Top;
 a.zn = zone;
 a.pConf = pConf;
 a.visuel = zoneCentrale;
 a.ptabVue = gtab_vue;

 syntheses = new SyntheseGenerale(a);
 lay_return = syntheses->GetDisposition();
 //w_FenetreDetails->setVisible(true);
 //w_FenetreDetails->lower();


 return(lay_return);
}

QGridLayout * MainWindow::MonLayout_Graphes(stTiragesDef *pConf)
{
 QGridLayout *lay_return = new QGridLayout;
 //etFdj test;

 // Essai de mise en onglet des  graphiques
 QTabWidget *tabWidget = new QTabWidget;
 UnConteneurDessin *dessin;

#ifdef CHARTWIDGET_H
 ChartWidget *test = new ChartWidget;
 //test->show();

 tabWidget->addTab(test,"Spirale");
#endif
 dessin = TST_Graphe_1(pConf);
 tabWidget->addTab(dessin,"Tirages");

 dessin = TST_Graphe_2(pConf);
 tabWidget->addTab(dessin,"Parites");

 dessin = TST_Graphe_3(pConf);
 tabWidget->addTab(dessin,"b<N/2");

 lay_return->addWidget(tabWidget);
#if (SET_DBG_LIVE&&SET_DBG_LEV1)
 QMessageBox::information(NULL, "Pgm", "Old 11 in b!",QMessageBox::Yes);
#endif


 return(lay_return);
}

void MainWindow::FEN_NewTirages(stTiragesDef *pConf)
{
 QWidget *qw_nsr = new QWidget;
 QTabWidget *tab_Top = new QTabWidget;
 gtab_vue = tab_Top;
 //QWidget **wid_ForTop = new QWidget*[2];
 QString stNames[2]={"Analyses","Recherches"};
 //QWidget *design_onglet[2];

 // Tableau de pointeur de fonction
 QWidget *(MainWindow::*ptrFunc[2])(stTiragesDef *pConf)=
 {&MainWindow::FEN_Analyses,&MainWindow::FEN_Recherches};

 for(int i =0; i<2;i++)
 {
  QWidget *myDesign = (this->*ptrFunc[i])(pConf);
  tab_Top->addTab(myDesign,tr(stNames[i].toUtf8()));
 }

 QFormLayout *mainLayout = new QFormLayout;
 mainLayout->addWidget(tab_Top);
 qw_nsr->setWindowTitle("Loto");
 qw_nsr->setLayout(mainLayout);


 QMdiSubWindow *subWindow = zoneCentrale->addSubWindow(qw_nsr);
 subWindow->resize(1300,800);
 subWindow->move(0,0);
 qw_nsr->setVisible(true);
}

QWidget * MainWindow::FEN_Recherches(stTiragesDef *pConf)
{
 QWidget *qw_nsr = w_FenetreDetails;

 return qw_nsr;
}

QWidget * MainWindow::FEN_Analyses(stTiragesDef *pConf)
{
 QWidget *qw_nsr = new QWidget;
 QTabWidget *tab_Top = new QTabWidget;
 QWidget **wid_ForTop = new QWidget*[2];
 QString stNames[2]={"Glob","Graf"};
 QGridLayout *design_onglet[2];


 // pour reecriture
 QString *st_tmp1 = new QString;
 QString *st_tmp2 = new QString;
 QString *st_tmp3 = new QString;
 QString *st_tmp4 = new QString;


 //*st_tmp2 = OrganiseChampsDesTirages("tirages", pConf);
 *st_tmp1 = C_TousLesTirages;
 *st_tmp2 = *st_tmp1 ;
 *st_tmp3 = *st_tmp1 ;
 *st_tmp4 =CompteJourTirage(db_0.connectionName());

 //---------------------------------------------------------------
 //
 // test de nouvelle fenetre
 //RefEtude *unTest = new RefEtude(DB_tirages,*st_tmp2,0,pConf);
 //return;
 //
 //---------------------------------------------------------------

 critereTirages.st_LDT_Depart = st_tmp1;
 critereTirages.st_Ensemble_1 = st_tmp2;
 critereTirages.st_LDT_Filtre = st_tmp3;
 critereTirages.st_jourDef = st_tmp4;

 // Tableau de pointeur de fonction
 QGridLayout *(MainWindow::*ptrFunc[2])(stTiragesDef *pConf)=
 {&MainWindow::MonLayout_pFnNsr1,&MainWindow::MonLayout_Graphes};


#if (SET_DBG_LIVE&&SET_DBG_LEV1)
 QMessageBox::information(NULL, "Pgm", "Old 11 in!",QMessageBox::Yes);
#endif

 for(int i =0; i<2;i++)
 {
  wid_ForTop[i]=new QWidget;
  tab_Top->addTab(wid_ForTop[i],tr(stNames[i].toUtf8()));

  //
  design_onglet[i] = (this->*ptrFunc[i])(pConf);
  wid_ForTop[i]->setLayout(design_onglet[i]);
 }

 // Les deux onglets sont crees faire une nouvelle spirale
 //QStandardItemModel *pSimEcart = new QStandardItemModel;
 //QStandardItemModel **pSimEcart = new QStandardItemModel *;
 //RefEtude *tmpEtude = new RefEtude();
 //QStandardItemModel * pSimEcart = tmpEtude->GetPtrToModel();


 //ChartWidget *test = new ChartWidget(pSimEcart);
 //test->show();

#if (SET_DBG_LIVE&&SET_DBG_LEV1)
 QMessageBox::information(NULL, "Pgm", "Old 11 out!",QMessageBox::Yes);
#endif

 QFormLayout *mainLayout = new QFormLayout;
 mainLayout->addWidget(tab_Top);
 qw_nsr->setLayout(mainLayout);
 return qw_nsr;

}

void MainWindow::fen_Parites(void)
{
 QWidget *qw_Parites = new QWidget;
 //QTabWidget *tabWidget = new QTabWidget;
 G_tbw_MontabWidget = new QTabWidget;



 int zn = 0;

 G_sim_Parites = new QStandardItemModel(configJeu.nbElmZone[zn],configJeu.nb_zone+1);

 G_sim_Parites->setHeaderData(0,Qt::Horizontal,"Nb");
 G_sim_Parites->setHeaderData(1,Qt::Horizontal,"B");
 G_sim_Parites->setHeaderData(2,Qt::Horizontal,"E");

 G_tbv_Parites = new QTableView;
 G_tbv_Parites->setModel(G_sim_Parites);
 G_tbv_Parites->setColumnWidth(0,45);
 G_tbv_Parites->setColumnWidth(1,55);
 G_tbv_Parites->setColumnWidth(2,55);
 G_tbv_Parites->setSortingEnabled(false);
 G_tbv_Parites->sortByColumn(0,Qt::AscendingOrder);
 G_tbv_Parites->setAlternatingRowColors(true);
 G_tbv_Parites->setEditTriggers(QAbstractItemView::NoEditTriggers);
 G_tbv_Parites->setMinimumHeight(220);

 G_sim_Ensemble_1 = new QStandardItemModel(configJeu.nbElmZone[zn],configJeu.nb_zone+1);

 G_sim_Ensemble_1->setHeaderData(0,Qt::Horizontal,"N(E/2)");
 G_sim_Ensemble_1->setHeaderData(1,Qt::Horizontal,"B");
 G_sim_Ensemble_1->setHeaderData(2,Qt::Horizontal,"E");

 QTableView *qtv_E1 = new QTableView;
 qtv_E1->setModel(G_sim_Ensemble_1);
 qtv_E1->setColumnWidth(0,60);
 qtv_E1->setColumnWidth(1,55);
 qtv_E1->setColumnWidth(2,55);
 qtv_E1->setSortingEnabled(false);
 qtv_E1->sortByColumn(0,Qt::AscendingOrder);
 qtv_E1->setAlternatingRowColors(true);
 qtv_E1->setEditTriggers(QAbstractItemView::NoEditTriggers);
 qtv_E1->setMinimumHeight(220);

 int v = (configJeu.limites[zn].max /10) +1;
 G_sim_ud = new QStandardItemModel(v+1,v+1);
 G_sim_ud->setHeaderData(0,Qt::Horizontal,"Nb");
 for(int j=0; j< v;j++)
 {
  QString name = configJeu.nomZone[zn] + "d" + QString::number(j);
  G_sim_ud->setHeaderData(j+1,Qt::Horizontal,name);
 }

 // Ecriture du numero de boule et reservation item position
 for(int j=0; j<= v;j++)
 {
  QStandardItem *item = new QStandardItem();
  item->setData(j,Qt::DisplayRole);
  G_sim_ud->setItem(j,0,item);
  for (int k =1; k<=v;k++)
  {
   QStandardItem *item_2 = new QStandardItem();
   G_sim_ud->setItem(j,k,item_2);
  }
 }

 QTableView *qtv_E2 = new QTableView;
 qtv_E2->setModel(G_sim_ud);
 for(int j=0; j< v+1;j++)
 {
  qtv_E2->setColumnWidth(j,50);
 }

 qtv_E2->setAlternatingRowColors(true);;
 qtv_E2->setSortingEnabled(false);
 qtv_E2->setEditTriggers(QAbstractItemView::NoEditTriggers);

 G_tbw_MontabWidget->addTab(G_tbv_Parites,tr("Parites"));
 G_tbw_MontabWidget->addTab(qtv_E1,tr("< N/2"));
 G_tbw_MontabWidget->addTab(qtv_E2,tr("Unites"));


 QFormLayout *mainLayout = new QFormLayout;
 mainLayout->addWidget(G_tbw_MontabWidget);
 qw_Parites->setWindowTitle("Etude des boules");
 qw_Parites->setLayout(mainLayout);

#if 0
 connect( qtv_Parites, SIGNAL( clicked(QModelIndex)) ,
          this, SLOT( slot_MontrerTirageDansBase( QModelIndex) ) );
#endif

 // double click dans fenetre voisin pour afficher details boule
 connect( G_tbv_Parites, SIGNAL( doubleClicked(QModelIndex)) ,
          this, SLOT( slot_F2_RechercherLesTirages( QModelIndex) ) );
 connect( qtv_E1, SIGNAL( doubleClicked(QModelIndex)) ,
          this, SLOT( slot_F2_RechercherLesTirages( QModelIndex) ) );


 QMdiSubWindow *subWindow = zoneCentrale->addSubWindow(qw_Parites);
 subWindow->resize(493,280);
 subWindow->move(350,580);
 subWindow->showMinimized();
 //zoneCentrale->addSubWindow(qw_Parites);
 qw_Parites->setVisible(true);
}

MainWindow::~MainWindow()
{
 //delete ui;
}



void MainWindow::ouvrir_mainwindows(void)
{
 closewindows = false;

 if(closewindows){
  this->~MainWindow();
 }
 else
 {
  this->show();
 }
}



void MainWindow::slot_ChercheVoisins(const QModelIndex & index)
{
 int val;
 int nb_zone = configJeu.nb_zone;
 int zn=0;
 QStringList select;

 // Click sur la date seulement
 // faire toute les boules et creer une mise a jour pour mes possibles
 if (index.column()==0)
 {
  // Recherche de toute les boules
  int col_bpos = 0;
  VUE_MontreLeTirage(index.row()+1);
  for(zn=0;zn<nb_zone;zn++)
  {
   for(int b_pos=0; b_pos<configJeu.nbElmZone[zn];b_pos++)
   {
    col_bpos += 1;
    val=index.model()->index(index.row(),col_bpos).data().toInt();

    DB_tirages->RechercheVoisin(val,zn,&configJeu,G_lab_nbSorties[zn],G_sim_Voisins[zn]);
    DB_tirages->RechercheAbsent(val,zn,&configJeu,G_lab_nbAbsents[zn],G_sim_Absents[zn]);
   }
   // Pour rendre la fenetre voisin vide
   select.clear();
   DB_tirages->TST_RechercheVoisin(select,zn,&configJeu,G_lab_nbSorties[zn],G_sim_Voisins[zn]);
   //TBD:DB_tirages->TST_RechercheAbsent(select,zn,&configJeu,G_lab_nbAbsents[zn],G_sim_Absents[zn]);
  }

#if 0
  for (i=1;i<6;i++)
  {
   val = index.model()->index(index.row(),i).data().toInt();
   DB_tirages->RechercheVoisin(val,0,&configJeu,qlT_nbSorties[0],qsimT_Voisins[0]);
  }

  // Recherche voisin des etoiles
  for (i=6;i<6+configJeu.nbElmZone[1];i++)
  {
   val = index.model()->index(index.row(),i).data().toInt();
   DB_tirages->RechercheVoisin(val,1,&configJeu,qlT_nbSorties[1],qsimT_Voisins[1]);
  }
#endif

  // Affichage des resultats
  DB_tirages->MontreMesPossibles(index,&configJeu,G_tbv_MesPossibles);
  DB_tirages->MontreMesAbsents(index,&configJeu,G_tbv_LesAbsents);

  // Mise en evidence numero commun
  DB_tirages->MLB_MontreLesCommuns(&configJeu,G_tbv_MesPossibles);
 }

 if(index.column()>0 && index.column()<6){
  zn=0;
  G_tbv_CouvTirages->clearSelection();
  val = index.data().toInt();

  G_tbv_Voisins[0]->sortByColumn(0,Qt::AscendingOrder);

  select << index.data().toString();
  DB_tirages->TST_RechercheVoisin(select,zn,&configJeu,G_lab_nbSorties[zn],G_sim_Voisins[zn]);
  //DB_tirages->RechercheVoisin(val,0,&configJeu,qlT_nbSorties[0],qsimT_Voisins[0]);

  DB_tirages->MLB_DansCouverture(val,&configJeu,G_tbv_CouvTirages);
  //DB_tirages->MLB_DansMesPossibles(val,QBrush(Qt::yellow),qtv_MesPossibles);
 }

}


void MainWindow::slot_UneCombiChoisie(const QModelIndex & index)
{
 int ligne = index.row();
 int colon = index.column();

 int clef = index.model()->index(ligne,0).data().toInt();

 QString msg = index.model()->index(ligne,1).data().toString();

 if(colon == 1)
 {
  msg = "Critere : " + msg;
  G_lab_CritereCombi->setText(msg);
  G_CombiKey = clef;
  TST_CombiVoisin(clef);
 }

 if(colon == -1)
 {
  VUE_ListeTiragesFromDistribution(clef,5,0);
 }

 // Pour bloquer
 if(colon >=2)
 {
  int val = index.data().toInt();
  const QAbstractItemModel * pModel = index.model();
  QVariant vCol = pModel->headerData(colon,Qt::Horizontal);
  QString headName = vCol.toString();

  stCurDemande *etude = new stCurDemande;

  QStringList stl_tmp;
  //stl_tmp << QString::number(ligne+1);

  etude->origine = Tableau3;
  etude->db_cnx = DB_tirages->get_IdCnx(0);

  etude->lgn[3] = index.model()->index(index.row(),0).data().toInt();;
  etude->col[3] = colon;
  etude->val[3] = val;
  etude->stc[3] = headName;
  etude->st_titre = msg;
  etude->lst_boules[3] = stl_tmp;
  etude->ref = &configJeu;

  // Nouvelle de fenetre de detail de cette boule
  SyntheseDetails::SynD_param a;
  a.pEtude =etude;
  a.visuel=zoneCentrale;
  a.tab_Top = gtab_Top;
  a.tab_vue = gtab_vue;

  SyntheseDetails *unDetail = new SyntheseDetails(a);
  //w_FenetreDetails->setVisible(true);

 }


}
void MainWindow::slot_CriteresTiragesAppliquer()
{
 stCurDemande *etude = new stCurDemande;

 critereTirages.origine = Tableau4;




 // recopie de la config courante
 critereTirages.cur_dst = 0;
 *etude = critereTirages;
 etude->db_cnx = DB_tirages->get_IdCnx(0);


 // Nouvelle de fenetre de detail de cette boule
 SyntheseDetails::SynD_param a;
 a.pEtude =etude;
 a.visuel=zoneCentrale;
 a.tab_Top = gtab_Top;
 a.tab_vue = gtab_vue;

 SyntheseDetails *unDetail = new SyntheseDetails(a);
 //w_FenetreDetails->setVisible(true);

}

void MainWindow::slot_CriteresTiragesConstruire(const QModelIndex & index)
{
 void *pSource = index.internalPointer();

 // Boules
 if(pSource == gtbv_SelectionBoulesDeZone[0]->model()->index(0,0).internalPointer())
 {
  MemoriserCriteresTirages(0, gtbv_SelectionBoulesDeZone[0], index);

 }

 // Etoiles
 if(pSource == gtbv_SelectionBoulesDeZone[1]->model()->index(0,0).internalPointer())
 {
  MemoriserCriteresTirages(1, gtbv_SelectionBoulesDeZone[1], index);
 }

 // Combinaison
 if(pSource == gtbv_SelectionBoulesDeZone[2]->model()->index(0,0).internalPointer())
 {
  int col = index.column();
  int val = index.data().toInt();

  if(col>=2 && val)
   MemoriserCriteresTirages(2, gtbv_SelectionBoulesDeZone[2], index);
 }
}


void MainWindow::MemoriserCriteresTirages(int zn, QTableView *ptbv, const QModelIndex & index)
{

 QItemSelectionModel *selection;
 QItemSelection une_cellule_choisie(index,index);
 QModelIndexList indexes ;
 QModelIndex un_index;

 QStringList lst_tmp ;
 QString stNomZone = "";
 QString st_z1 = "";
 QString newCritere[3];

 int col = index.column();
 int ligne = index.row();
 int val = index.data().toInt();

 const QAbstractItemModel * pModel = index.model();
 QVariant vCol = pModel->headerData(col,Qt::Horizontal);
 QString headName = vCol.toString();


 int nbZone = configJeu.nb_zone;
 int nb_element_max_zone = 0;
 int nb_items = 0;
 int Zone = 0;




 selection = ptbv->selectionModel();
 indexes = selection->selectedIndexes();
 nb_items = indexes.size();

 if(zn==2)
 {
  Zone = 0;
  nb_element_max_zone=1;
  stNomZone="Combi";
 }
 else
 {
  nb_element_max_zone = configJeu.nbElmZone[zn];
  Zone=zn+1;
  stNomZone = configJeu.nomZone[zn];
 }



 if(nb_items <= nb_element_max_zone)
 {
  critereTirages.col[zn]=col;
  critereTirages.stc[zn]="";
  critereTirages.val[zn]=val;
  critereTirages.lgn[zn]=ligne;

  int i =0;
  lst_tmp = lab_critere->text().split(" - ");

  for(i =0; i< lst_tmp.size();i++)
  {
   newCritere[i] = lst_tmp.at(i);
  }

  // Nouvelle selection utilisateur
  critereTirages.lst_boules[zn].clear();

  foreach(un_index, indexes)
  {
   if(zn<2)
   {
    critereTirages.lst_boules[zn]<< un_index.data().toString();
   }
   else
   {
    critereTirages.lst_boules[zn]<<index.model()->index(index.row(),0).data().toString();
   }
  }

  st_z1="";
  for(i =0; i< nb_items;i++)
  {
   st_z1 = st_z1 + critereTirages.lst_boules[zn].at(i) + ",";
  }
  st_z1.remove(st_z1.length()-1,1);

  if(zn==2 && nb_items){
   st_z1 = index.model()->index(index.row(),1).data().toString();
   critereTirages.stc[zn]=headName;
  }

  if(st_z1 == "")
   st_z1="aucun";


  newCritere[Zone]=st_z1;

  st_z1 = "";
  for(i =0; i< lst_tmp.size();i++)
  {
   st_z1 = st_z1 + newCritere[i]+" - ";
  }
  st_z1.remove(st_z1.length()-3,3);
  lab_critere->setText(st_z1);
 }
 else
 {
  //un message d'information
  QMessageBox::warning(0, stNomZone, tr("Attention, maximum deja selectionne !"),QMessageBox::Yes);

  // deselectionner l'element
  selection->select(une_cellule_choisie, QItemSelectionModel::Deselect);
 }
}

void MainWindow::slot_UneSelectionActivee(const QModelIndex & index)
{
 //static QStringList *select = new QStringList *[configJeu.nb_zone];
 static QStringList select[2];
 int zn = -1;

 // determination de la table dans l'onglet ayant recu le click
 if (index.internalPointer() == gsim_SelectionBoulesDeZone[0]->index(index.row(),index.column()).internalPointer()){
  zn = 0;
 }
 else
 {
  zn=1;
 }



 QStandardItem *item1 = gsim_SelectionBoulesDeZone[zn]->itemFromIndex(index);
#ifndef QT_NO_DEBUG
 qDebug()<< item1->background();
#endif

 if(zn != -1)
 {
  // Regarder si fenetre voisin a deja des infos ?


  // Regarder si fenetre selection deja utilisee
  if(item1->background() == Qt::red)
  {
   // un element deja selectionne le supprimer
   int pos = select[zn].indexOf(item1->data(Qt::DisplayRole).toString());
   select[zn].removeAt(pos);

   // Remettre la bonne couleur de fond
   if(index.row()%2==0){
    //QBrush macouleur(Qt::white);
    item1->setBackground(QBrush(QColor::fromRgb(1, 1, 0, 0) , Qt::SolidPattern ));
   }
   else
   {
    //QBrush macouleur (Qt::gray);
    item1->setBackground(QBrush(QColor::fromRgb(1, 0, 0, 0) , Qt::NoBrush ));
   }
  }
  else
  {
   // Memoriser la selection
   //QBrush macouleur(Qt::red);
   select[zn] << item1->data(Qt::DisplayRole).toString();
   item1->setBackground(QBrush(Qt::red));
  }

  G_tbv_Voisins[zn]->sortByColumn(0,Qt::AscendingOrder);
  DB_tirages->TST_RechercheVoisin(select[zn],zn,&configJeu,G_lab_nbSorties[zn],G_sim_Voisins[zn]);

  // Nouvelle table
  NEW_ChercherTotalBoules(select[0],&configJeu);
 }

}
#if 0
void MainWindow::slot_MontrerTirageDansBase(const QModelIndex & index)
{
 //int val = 0;
 //int cellule = 0;

 // determination de la fenetre ayant recu le click
 if (index.internalPointer() == qsim_Parites->index(index.row(),index.column()).internalPointer()){
  int val = qsim_Parites->index(index.row(),0).data().toInt();
  int cellule = qsim_Parites->index(index.row(),index.column()).data().toInt();

  // Recherche des tirages de la base ayant la contrainte
  //DB_tirages->RechercheBaseTiragesPariteNbBoule(val, &configJeu, qtv_Tirages);
 }
}
#endif
void MainWindow::slot_RepererLesTirages(const QString & myData)
{
 // qDebug()<< "link :" <<  myData;
 QStringList list;
 QRegExp reg_number ("(\\d+)");
 //QString  str_br = G_lab_nbSorties[zn]->text();
 int pos = 0;
 int zn=0;

 while ((pos = reg_number.indexIn(myData, pos)) != -1) {
  list << reg_number.cap(1);
  pos += reg_number.matchedLength();
 }

 // on retire le dernier chiffre de str_br
 if(!list.isEmpty())
 {
  list.removeLast();
 }

 if(!list.isEmpty())
 {
  TST_MontreTirageAyantCritere(NE_FDJ::critere_boule,zn,&configJeu,list);
 }
}

void MainWindow::slot_CriteresTiragesEffacer(void)
{
 QItemSelectionModel *selection;
 int nbZone = configJeu.nb_zone;

 for(int i=0;i<nbZone;i++)
 {
  selection = gtbv_SelectionBoulesDeZone[i]->selectionModel();
  selection->clearSelection();
 }

 for(int i=0;i<3;i++)
 {
  critereTirages.lst_boules[i].clear();
 }
 lab_critere->setText("aucun - aucun - aucun");
}

void MainWindow::slot_F2_RechercherLesTirages(const QModelIndex & index)
{
 //int col=0;
 int val = 0;
 int zn=0;
 QStringList list;

 // determination de la table dans l'onglet ayant recu le click
 if (index.internalPointer() == G_sim_Parites->index(index.row(),index.column()).internalPointer())
 {
  //col = index.column();
  val = G_sim_Parites->index(index.row(),0).data().toInt();
  list << QString::number(val);
  TST_MontreTirageAyantCritere(NE_FDJ::critere_parite,zn,&configJeu,list);
 }

 if (index.internalPointer() == G_sim_Ensemble_1->index(index.row(),index.column()).internalPointer())
 {
  val = G_sim_Ensemble_1->index(index.row(),0).data().toInt();
  list << QString::number(val);
  TST_MontreTirageAyantCritere(NE_FDJ::critere_enemble,zn,&configJeu,list);
 }

}

void MainWindow::slot_RechercherLesTirages(const QModelIndex & index)
{
 int val = 0;
 int cellule = 0;
 int col=0;
 int zn = -1;
 const int d[5]={0,1,2,-1,-2};
 const int t[2]={1,2};

 // determination de la table dans l'onglet ayant recu le click
 if (index.internalPointer() == G_sim_Voisins[0]->index(index.row(),index.column()).internalPointer())
 {
  zn = 0;
 }

 if (index.internalPointer() == G_sim_Voisins[1]->index(index.row(),index.column()).internalPointer())
 {
  zn = 1;
 }

 if(zn != -1)
 {
  cellule = G_sim_Voisins[zn]->index(index.row(),index.column()).data().toInt();

  // Valeur != 0
  if(cellule)
  {
   QStringList list;
   // Boule a  rechercher
   val = G_sim_Voisins[zn]->index(index.row(),0).data().toInt();

   col = index.column();

   if(col==CL_IHM_TOT_0) // Colonne Nb total sortie
   {
    list << QString::number(val);
    TST_MontreTirageAyantCritere(NE_FDJ::critere_boule,zn,&configJeu,list);
   }

   if(col >0 && col < CL_IHM_TOT_1) // colonne V:r0, V:+1, V+2, V:-1,V:-2
   {
    QRegExp reg_number ("(\\d+)");
    QString  str_br = G_lab_nbSorties[zn]->text();
    int pos = 0;

    // boule(s) de references dans tirages a trouver
    while ((pos = reg_number.indexIn(str_br, pos)) != -1) {
     list << reg_number.cap(1);
     pos += reg_number.matchedLength();
    }

    // on retire le dernier chiffre de str_br
    if(!list.isEmpty())
    {
     list.removeLast();
    }

    if(!list.isEmpty())
    {
     //DB_tirages->TST_LBcDistBr(zn,&configJeu,d[col-1],br,val);
     this->TST_LBcDistBr(zn,&configJeu,d[col-1],list,val);

    }

   }

   if(col >= CL_IHM_TOT_1 && col <= CL_IHM_TOT_2) // Total ????
   {
    list << QString::number(val);
    this->TST_LBcDistBr(zn,&configJeu,t[col-CL_IHM_TOT_1],list,val);
   }
  }
 }
}

void MainWindow::slot_F3_RechercherLesTirages(const QModelIndex & index)
{
 int rch = 0;
 int col=0;
 int zn = -1;
 const int d[2]={-1,-2};

 // determination de la table dans l'onglet ayant recu le click
 if (index.internalPointer() == gsim_DernierTirageDetail[0]->index(index.row(),index.column()).internalPointer())
 {
  zn = 0;
 }

 if (index.internalPointer() == gsim_DernierTirageDetail[1]->index(index.row(),index.column()).internalPointer())
 {
  zn = 1;
 }

 if(zn != -1)
 {
  // determination de colonne
  col = index.column();
  rch = gsim_DernierTirageDetail[zn]->index(index.row(),0).data().toInt();

  if(col >0 && col < 3)
  {
   int dist = d[col-1];
   // Recup clef
   QRegExp reg_number ("(\\d+)");
   QString  str_br = G_lab_PariteVoisin[zn]->text();
   QStringList list;
   int pos = 0;

   while ((pos = reg_number.indexIn(str_br, pos)) != -1) {
    list << reg_number.cap(1);
    pos += reg_number.matchedLength();
   }

   QString msg = "select *  from "   TB_BASE
                 " inner join  ( select *  from "   TB_BASE
                 " where ( "+ configJeu.nomZone[zn] +
                 CL_PAIR + "=" + list.at(0) ;

   msg = msg + ")) as r1 on tirages.id = r1.id + %1 ) as r2";
   msg = (msg).arg(dist);

   QString msg_2 = configJeu.nomZone[zn] +CL_PAIR
                   + "=" + QString::number(rch) ;
   msg_2= "select  *  from (" +msg+ " where (" +msg_2+ " );" ;

   QString titre = "Tirages d:" + QString::number(dist) +
                   "Ref P:"+list.at(0) +
                   "Cible :"+QString::number(rch);

   TST_FenetreReponses(titre,zn,msg_2,list,&configJeu);
  }

 }
}

void MainWindow::slot_F4_RechercherLesTirages(const QModelIndex & index)
{
 int rch = 0;
 int col=0;
 int zn = -1;
 const int d[2]={-1,-2};

 // determination de la table dans l'onglet ayant recu le click
 if (index.internalPointer() == G_sim_Nsur2[0]->index(index.row(),index.column()).internalPointer())
 {
  zn = 0;
 }

 if (index.internalPointer() == G_sim_Nsur2[1]->index(index.row(),index.column()).internalPointer())
 {
  zn = 1;
 }

 if(zn != -1)
 {
  // determination de colonne
  col = index.column();
  rch = G_sim_Nsur2[zn]->index(index.row(),0).data().toInt();

  if(col >0 && col < 3)
  {
   int dist = d[col-1];
   // Recup clef
   QRegExp reg_number ("(\\d+)");
   QString  str_br = G_lab_Nsur2[zn]->text();
   QStringList list;
   int pos = 0;

   while ((pos = reg_number.indexIn(str_br, pos)) != -1) {
    list << reg_number.cap(1);
    pos += reg_number.matchedLength();
   }

   QString msg = "select *  from "   TB_BASE
                 " inner join  ( select *  from "   TB_BASE
                 " where ( "+ configJeu.nomZone[zn] +
                 CL_SGRP + "=" + list.at(0) ;

   msg = msg + ")) as r1 on tirages.id = r1.id + %1 ) as r2";
   msg = (msg).arg(dist);

   QString msg_2 = configJeu.nomZone[zn] +CL_SGRP
                   + "=" + QString::number(rch) ;
   msg_2= "select  *  from (" +msg+ " where (" +msg_2+ " );" ;

   QString titre = "Tirages d:" + QString::number(dist) +
                   "Ref Sg:"+list.at(0) +
                   "Cible :"+QString::number(rch);

   TST_FenetreReponses(titre,zn,msg_2,list,&configJeu);
  }

 }
}
void MainWindow::slot_F5_RechercherLesTirages(const QModelIndex & index)
{
 // determination de colonne
 int col = index.column();
 int lgn = index.model()->index(index.row(),0).data().toInt();

 if (col > 2 && col < 7)
 {
  if (index.data().toInt())
  {
   VUE_ListeTiragesFromDistribution(G_CombiKey,col-3,lgn);
  }
 }

}

void MainWindow::slot_MontrerBouleDansBase(const QModelIndex & index)
{
 int val = 0;
 int cellule = 0;
 int b_id =0;
 int col_id = 0;
 QStringList lst_boule;
 boolean bNouvelleRecherche = false;

 // determination de la fenetre ayant recu le click
 if(index.internalPointer() == G_sim_Voisins[0]->index(index.row(),index.column()).internalPointer())
 {

  val = G_sim_Voisins[0]->index(index.row(),0).data().toInt();
  cellule = val;//G_sim_Voisins[0]->index(index.row(),index.column()).data().toInt();;
  bNouvelleRecherche = false;
 }

 if(index.internalPointer() == G_sim_MesPossibles->index(index.row(),index.column()).internalPointer())
 {
  val = DB_tirages->CouleurVersBid(G_tbv_MesPossibles);
  cellule = G_sim_MesPossibles->index(index.row(),index.column()).data().toInt();
  if(cellule != val)
  {
   // Effacer les recherches de boules precedentes
   DB_tirages->MLB_DansMesPossibles(0,QBrush(Qt::NoBrush),G_tbv_MesPossibles);
  }
  b_id = BidFCId_MesPossibles(index.column(),G_tbv_MesPossibles);
  col_id = colonne_tri;
  bNouvelleRecherche = true;
 }

 if(index.internalPointer() == G_sim_LesAbsents->index(index.row(),index.column()).internalPointer())

 {
  cellule = G_sim_LesAbsents->index(index.row(),index.column()).data().toInt();
  val = cellule;
  b_id = BidFCId_MesPossibles(index.column(),G_tbv_LesAbsents);
  col_id = G_colTrieAbsent;
  bNouvelleRecherche = true;
 }



 // Affichage de la table des voisins
 if(bNouvelleRecherche){
  lst_boule << QString::number(b_id);
  G_tbv_Voisins[0]->sortByColumn(0,Qt::AscendingOrder);
  DB_tirages->TST_RechercheVoisin(lst_boule,0,&configJeu,G_lab_nbSorties[0],G_sim_Voisins[0]);

  G_sim_Voisins[0]->sort(col_id+1,Qt::AscendingOrder);
  G_sim_Voisins[0]->sort(col_id+1,Qt::DescendingOrder);
 }

#if 0
 if (index.internalPointer() == G_sim_Voisins[0]->index(index.row(),index.column()).internalPointer()){
  val = G_sim_Voisins[0]->index(index.row(),0).data().toInt();
  cellule = val;
 }
 else
 {
  // regarder si l'on deja selectionne une boule
  val = DB_tirages->CouleurVersBid(G_tbv_MesPossibles);

  // recuperer la boule de la cellule
  cellule = G_sim_MesPossibles->index(index.row(),index.column()).data().toInt();

  // si different effacer l'ancienne selection

  if(cellule != val)
  {
   // Effacer les recherches de boules precedentes
   DB_tirages->MLB_DansMesPossibles(0,QBrush(Qt::NoBrush),G_tbv_MesPossibles);
  }

  // determination du numero de boule grace a la colonne
  int b_id = BidFCId_MesPossibles(index.column(),G_tbv_MesPossibles);
  QStringList lst_boule;
  lst_boule << QString::number(b_id);

  // Affichage de la table des voisins
  //DB_tirages->RechercheVoisin(b_id,0,&configJeu,qlT_nbSorties[0],qsimT_Voisins[0]);
  DB_tirages->TST_RechercheVoisin(lst_boule,0,&configJeu,G_lab_nbSorties[0],G_sim_Voisins[0]);

  G_sim_Voisins[0]->sort(colonne_tri+1,Qt::AscendingOrder);
  G_sim_Voisins[0]->sort(colonne_tri+1,Qt::DescendingOrder);
 }
#endif

#ifndef QT_NO_DEBUG
 qDebug() << QString::number(index.row());
#endif


 if(cellule){
  // Une Boule Trouvee la montrer dans les autres fenetres
  G_tbv_Tirages->clearSelection();
  G_tbv_Tirages->clearFocus();

  /// Montrer dans les courbes
  ptir = new PointTirage(db_0.connectionName() ,configJeu.choixJeu,eTirage);
  //QGraphicsItem *unPoint = new QGraphicsItem;
  //unPoint->setPos(10);
  //ptir->setPos(10,69.8);
  //QGraphicsScene *pScene;
  //pScene = myview[0]->GetScene();
  //pScene->setFocusItem(ptir);

  DB_tirages->MontrerLaBoule(cellule,G_tbv_Tirages);
  DB_tirages->MLB_DansLaQtTabView(cellule,G_tbv_Voisins[0]);
  DB_tirages->MLB_DansLaQtTabView(cellule,G_tbv_Ecarts);
  G_tbv_CouvTirages->clearSelection();
  DB_tirages->MLB_DansCouverture(cellule,&configJeu,G_tbv_CouvTirages);

  // Effacer les recherches de boules precedentes
  DB_tirages->MLB_DansMesPossibles(0,QBrush(Qt::NoBrush),G_tbv_MesPossibles);
  G_tbv_MesPossibles->clearSelection();
  G_tbv_MesPossibles->clearFocus();

  // Mise en evidence numero commun
  DB_tirages->MLB_MontreLesCommuns(&configJeu,G_tbv_MesPossibles);
  DB_tirages->MLB_DansMesPossibles(cellule,QBrush(Qt::yellow),G_tbv_MesPossibles);

  // Onglet les Absents: Efface selection, montre nouvelle
  DB_tirages->MLB_DansMesPossibles(0,QBrush(Qt::NoBrush),G_tbv_LesAbsents);
  G_tbv_MesPossibles->clearSelection();
  G_tbv_MesPossibles->clearFocus();
  DB_tirages->MLB_DansMesPossibles(cellule,QBrush(Qt::yellow),G_tbv_LesAbsents);

 }
}


#if 0
void MainWindow::slot_CouvertureSelChanged(const QItemSelection & now,const QItemSelection & prev)
{
 QModelIndexList items;
 QModelIndex index;

 //qtv_LstCouv->clearSelection();
 items = prev.indexes();

 foreach (index, items){
  ;
 }

}
#endif

#if 0
MonToolTips::MonToolTips(int rows, int columns, QObject *parent)
 : QStandardItemModel(rows,columns,parent)
{
}


void MonToolTips::setData ( const QVariant & value, int role )
{
}


QVariant MonToolTips::data(const QModelIndex &index, int role) const
{
 QVariant data;

 if (!index.isValid())
  return QVariant();

 if (role == Qt::ToolTipRole) data = "test";


 return data;

}
#endif

void MainWindow::customMenuRequested(QPoint pos)
{
 QModelIndex index = G_tbv_MesPossibles->indexAt(pos);
 int v_id = G_sim_MesPossibles->index(index.row(),index.column()).data().toInt();
 QVariant  hdata =  index.model()->headerData(index.column(),Qt::Horizontal);
 QString msg = hdata.toString();
 msg = msg.split("b").at(1);
 int b_id = msg.toInt();
 QMenu *menu=new QMenu(this);

 DB_tirages->PopulateCellMenu(b_id, v_id, 0, &configJeu, menu, this);
 menu->popup(G_tbv_MesPossibles->viewport()->mapToGlobal(pos));

}

#if 0
void MainWindow::customMenuRequested(QPoint pos){
 QModelIndex index = qtv_MesPossibles->indexAt(pos);
 //int val = qsim_MesPossibles->index(index.row(),index.column()).data().toInt();

 QMenu *menu=new QMenu(this);

 // On recupere les valeurs des voisins
 QString Lib[6]={"tot:","r0:","+1:","+2:","-1:","-2:"};
 QString vVoisin[6];
 int somme=0;

 // Hypothese la selection sur la table des voisins deja faite
 for(int i = 1; i<6;i++)
 {
  int val = qsim_Voisins->index(qtv_Voisins->currentIndex().row(),i).data().toInt();
  somme+=val;
  vVoisin[i]= Lib[i]+QString::number(val);
 }
 vVoisin[0]= Lib[0]+QString::number(somme);

 for(int i = 0; i<6;i++)
 {
  menu->addAction(new QAction(vVoisin[i], this));
 }

 menu->popup(qtv_MesPossibles->viewport()->mapToGlobal(pos));
}
#endif

void MainWindow::pop_selAbsents(QPoint pos)
{

 G_men_TrieMesAbsents->popup(G_tbv_LesAbsents->viewport()->mapToGlobal(pos));
}

void MainWindow::tablev_customContextMenu(QPoint pos)
{

 Gmen_TrieMesPossibles->popup(G_tbv_MesPossibles->viewport()->mapToGlobal(pos));
}

void MainWindow::slot_ft1Possibles(void)
{
 ft_LancerTri(-1);
}

void MainWindow::slot_ftAbs1(void)
{
 ft_TriDesAbsents(0);
}

void MainWindow::slot_ftAbs2(void)
{
 ft_TriDesAbsents(1);
}
void MainWindow::slot_ftAbs3(void)
{
 ft_TriDesAbsents(2);
}
void MainWindow::slot_ftAbs4(void)
{
 ft_TriDesAbsents(3);
}
void MainWindow::slot_ftAbs5(void)
{
 ft_TriDesAbsents(4);
}

// BoulesId from Col ID
int MainWindow::BidFCId_MesPossibles(int col_id,QTableView * tbv_ptr)
{
 int b_id = 0;

 QVariant  hdata =  tbv_ptr->model()->headerData(col_id,Qt::Horizontal);
 QString msg = hdata.toString();

 if (!msg.contains("C")){
  msg = msg.split("b").at(1);
  b_id = msg.toInt();
 }

 return b_id;
}
void MainWindow::ft_TriDesAbsents(int tri_id)
{
 int col_id = 0;
 G_colTrieAbsent = tri_id;
 static QStringList select[2];
 int zn=0;

 // retirer affichage de selection de boule
 G_tbv_LesAbsents->clearSelection();
 G_tbv_LesAbsents->clearFocus();

 // Memoriser la recherche precedente eventuelle
 int boule = DB_tirages->CouleurVersBid(G_tbv_LesAbsents);

 if(boule)
 {
  // Effacer les recherches de boules precedentes
  DB_tirages->MLB_DansMesPossibles(0,QBrush(Qt::NoBrush),G_tbv_LesAbsents);
 }


 // Effectuer l'affichage du trie concerne pour toute les boules du tirage
 for(col_id = 0; col_id < 5;col_id++)
 {
  int b_id = BidFCId_MesPossibles(col_id,G_tbv_LesAbsents);

  if(b_id)
  {
   // Montrer les nouveaux resultats
   DB_tirages->EffectuerTrieMesAbsents(tri_id,col_id,b_id,&configJeu,G_sim_LesAbsents);
  }
 }

 if(boule)
 {
  // Remontrer la recherche eventuelle
  DB_tirages->MLB_DansMesPossibles(boule, QBrush(Qt::yellow),G_tbv_LesAbsents);
 }

 G_sim_Voisins[zn]->sort(tri_id+1,Qt::AscendingOrder);
 G_sim_Voisins[zn]->sort(tri_id+1,Qt::DescendingOrder);
}

void MainWindow::ft_LancerTri(int tri_id)
{
 int col_id = 0;
 colonne_tri = tri_id;

 // retirer affichage de selection de boule
 G_tbv_MesPossibles->clearSelection();
 G_tbv_MesPossibles->clearFocus();

 // Memoriser la recherche precedente eventuelle
 int boule = DB_tirages->CouleurVersBid(G_tbv_MesPossibles);

 if(boule)
 {
  // Effacer les recherches de boules precedentes
  DB_tirages->MLB_DansMesPossibles(0,QBrush(Qt::NoBrush),G_tbv_MesPossibles);
 }

 // Effectuer l'affichage du trie concerne pour toute les boules du tirage
 for(col_id = 0; col_id < 5;col_id++)
 {
  int b_id = BidFCId_MesPossibles(col_id,G_tbv_MesPossibles);

  if(b_id)
  {
   // Montrer les nouveaux resultats
   DB_tirages->EffectuerTrieMesPossibles(tri_id,col_id,b_id,&configJeu,G_sim_MesPossibles);
  }
 }

 // Mise en evidence numero commun
 DB_tirages->MLB_MontreLesCommuns(&configJeu,G_tbv_MesPossibles);

 if(boule)
 {
  // Remontrer la recherche eventuelle
  DB_tirages->MLB_DansMesPossibles(boule, QBrush(Qt::yellow),G_tbv_MesPossibles);
 }

 // Tableau voisin actualise
 if(tri_id >= 0){
  col_id =  tri_id;
 }
 else
 {
  col_id = 5;
 }

 G_sim_Voisins[0]->sort(col_id+1,Qt::AscendingOrder);
 G_sim_Voisins[0]->sort(col_id+1,Qt::DescendingOrder);

}

void MainWindow::slot_ft2Possibles(void)
{
 ft_LancerTri(0);
 //qsim_Voisins->sort(1);
}

void MainWindow::slot_ft3Possibles(void)
{
 ft_LancerTri(1);
 //qsim_Voisins->sort(2);
}

void MainWindow::slot_ft4Possibles(void)
{
 ft_LancerTri(2);
 //qsim_Voisins->sort(3);
}

void MainWindow::slot_ft5Possibles(void)
{
 ft_LancerTri(3);
 //qsim_Voisins->sort(4);
}

void MainWindow::slot_ft6Possibles(void)
{
 ft_LancerTri(4);
 //qsim_Voisins->sort(5);
}

// Recherche des combinaison possible
void MainWindow::TST_CombiRec(int k, QStringList &l, const QString &s, QStringList &ret)
{
 QStringList tmp = l;

 tmp.removeAt(0);

 if (k==0) {
  ret << s ;
  return;
 }
 if (l.isEmpty()) return;

 if (s.isEmpty())
 {
  TST_CombiRec(k-1, tmp, l.at(0),ret);
 }
 else
 {
  TST_CombiRec(k-1, tmp, s+","+l.at(0),ret);
 }

 TST_CombiRec(k, tmp, s,ret);
}

// https://fr.wikipedia.org/wiki/Partition_d'un_entier
// http://www.geeksforgeeks.org/generate-unique-partitions-of-an-integer/
// https://monsiterdex.wordpress.com/2013/04/12/integer-partition-in-cc/
// http://stackoverflow.com/questions/14162798/generating-all-distinct-partitions-of-a-number
// http://caml.inria.fr/pub/old_caml_site/lettre_de_caml/lettre6.pdf
// http://homeomath2.imingo.net/algorithme1.htm

QStringList *MainWindow::TST_PartitionEntier(int n)
{

 QStringList *tmp = new(QStringList);
 QString lgn ;

 int p[n]; // An array to store a partition
 int k = 0;  // Index of last element in a partition
 p[k] = n;  // Initialize first partition as number itself

 // This loop first prints current partition, then generates next
 // partition. The loop stops when the current partition has all 1s
 while (true)
 {
  // print current partition
  lgn = TST_PartitionEntierAdd(p, k+1);
  *tmp << lgn;

  // Generate next partition

  // Find the rightmost non-one value in p[]. Also, update the
  // rem_val so that we know how much value can be accommodated
  int rem_val = 0;
  while (k >= 0 && p[k] == 1)
  {
   rem_val += p[k];
   k--;
  }

  // if k < 0, all the values are 1 so there are no more partitions
  if (k < 0)  return(tmp);

  // Decrease the p[k] found above and adjust the rem_val
  p[k]--;
  rem_val++;


  // If rem_val is more, then the sorted order is violeted.  Divide
  // rem_val in differnt values of size p[k] and copy these values at
  // different positions after p[k]
  while (rem_val > p[k])
  {
   p[k+1] = p[k];
   rem_val = rem_val - p[k];
   k++;
  }

  // Copy rem_val to next position and increment position
  p[k+1] = rem_val;
  k++;
 }
 return tmp;
}

QString MainWindow::TST_PartitionEntierAdd(int p[], int n)
{
 QStringList tmp;
 QString retval;

 for (int i = 0; i < n; i++)
  tmp << QString::number(p[i]) ;

 retval = tmp.join(",");

 return retval;
}

//void MainWindow::TST_EtoileCombi(stTiragesDef *ref, QTabWidget *onglets)
void MainWindow::TST_EtoileCombi(stTiragesDef *ref)
{
 QStringList sl_etoiles;
 QString msg = "";
 //bool status = false;
 QStringList sl_Lev0;

 //QTabWidget *onglets = ;

 for(int i = 1; i<= ref->limites[1].max;i++)
  sl_Lev0 << QString::number(i);

 // Recuperation des combinaisons C(2,11)
 TST_CombiRec(ref->nbElmZone[1], sl_Lev0, "" , sl_etoiles);
 TST_EtoilesVersTable(sl_etoiles,ref,150);

}

void MainWindow::TST_EtoilesVersTable (QStringList &combi,stTiragesDef *ref, double ponder)
{
 bool status = false;
 int zn = 1;


 QSqlQuery sql_1(db_0);
 QString st_cols = "";
 QString st_vals = "";
 QString msg_1 = "";
 int pon_step = ponder;

 QString msg_2 = ref->pTir->qs_zColBaseName(zn);

 msg_2.replace(",", " int,");

 msg_1 = "CREATE table if not exists "
         TB_COMBI "_"
         + ref->nomZone[zn]
         + "(id INTEGER PRIMARY KEY,";

 msg_1 = msg_1 + msg_2 + " int,poids real);";

 status = sql_1.exec(msg_1);
 if(status)
 {
  // Parcourir chaque element du tableau
  for (int i = 0; i< combi.size();i++)
  {
   QStringList item = combi.at(i).split(",");
   int nbitems = item.size();
   for(int j=0;(j<nbitems)&& (status == true);j++)
   {
    st_cols ="";
    st_vals ="";
    for(int k =0; k< nbitems;k++)
    {
     st_cols = st_cols
               +ref->nomZone[zn]+QString::number(k+1)
               +",";
     st_vals = st_vals+item.at((j+k)%nbitems)
               +",";
    }
    st_cols.remove(st_cols.length()-1,1);
    st_vals.remove(st_vals.length()-1,1);
    msg_1 = "insert into "
            TB_COMBI "_"
            + ref->nomZone[zn]
            + " (id,"+ st_cols + ",poids)"
                                 "Values (NULL," + st_vals + "," + QString::number(pon_step)+");";
    status = sql_1.exec(msg_1);
    pon_step += 0.2;

   }
   // Prendre la valeur entiere immediatement superieur
   pon_step -=0.2;
   pon_step = ceil(pon_step)+3;
  }
 }
}

// http://forum.hardware.fr/hfr/Programmation/C-2/resolu-renvoyer-combinaison-sujet_23393_1.htm
// http://www.dcode.fr/generer-calculer-combinaisons
void MainWindow::TST_RechercheCombi(stTiragesDef *ref, QTabWidget *onglets)
{
 QStringList sl_Lev1[5];
 QString msg = "";
 //bool status = false;
 QStringList sl_Lev0;

 int nbBoules = floor(ref->limites[0].max/10)+1;

 for(int i = 1; i<=nbBoules;i++)
 {
  sl_Lev0 << QString::number(i);
 }

#if 0
 if(ref->limites[0].max == 49)
 {
  sl_Lev0 << "1" << "2" << "3" << "4" << "5";
  //ShowCol = 5;
 }
 else
 {
  sl_Lev0 << "1" << "2" << "3" << "4" << "5" << "6";
  //ShowCol = 6;
 }
#endif

 // Recuperation des combinaison C(1,5), C(2,5), C(3,5), C(4,5), C(5,5)
 for (int i = 0; i< 5; i++)
 {
  TST_CombiRec(i+1, sl_Lev0, "" , sl_Lev1[i]);

  // Inscription de cette liste dans une table
  TST_CombiVersTable(sl_Lev1[i],ref);
 }

 // Insertion de la ponderation de la liste des combis
 TST_PonderationCombi(50);

 // Affectation d'un poids a un tirage
 TST_AffectePoidsATirage(ref);

 // Ecriture du fichier resultat
 TST_MettrePonderationSurTirages();

 // Faire un onglet (Pere) par type de possibilite de gagner
#define NEW_ONGLET
#ifdef NEW_ONGLET
 for(int i =5; i>2; i--)
 {
  TST_OngletN1(onglets,i,&sl_Lev1,ref);
 }
 //QTabWidget *tw_rep = TST_OngletN1(onglets,5,&sl_Lev1,ref);
#else
 for(int NbBg=5; NbBg >2; NbBg --) // Nb boule permettant de gagner
 {
  // Rajouter un onglet
  QTabWidget *tw_rep = new QTabWidget;
  QString st_OngName = "Comb" + QString::number(NbBg);
  onglets->addTab(tw_rep,tr(st_OngName.toLocal8Bit()));
  QStandardItemModel * qsim_t = new QStandardItemModel(50,2);
  QTableView *qtv_t = new QTableView;
  int NbTotLgn = 0;

  for(int loop=0;loop < 50; loop++)
  {
   QStandardItem *item_1 = new QStandardItem();
   QStandardItem *item_2 = new QStandardItem();
   item_1->setData(loop+1,Qt::DisplayRole);
   qsim_t->setItem(loop,0,item_1);
   qsim_t->setItem(loop,1,item_2);
  }

  // Faire un onglet (Fils)
  int decomposition = 0;
  for(int sousOnglet = 0; sousOnglet < NbBg;sousOnglet++)
  {
   int nbItems = 0;
   int nbI2tems = 0;
   QStandardItemModel * qsim_r = NULL;
   QStandardItemModel * qsim_synthese = new QStandardItemModel(50,2);
   QTableView *qtv_r = new QTableView;
   QTableView *qtv_synthese = new QTableView;
   QTabWidget *tw_lgnCombi = new QTabWidget;


   nbItems = sl_Lev1[sousOnglet].size();
   nbI2tems = sl_Lev1[sousOnglet].at(0).split(",").size();

   if((sousOnglet == 1) || (sousOnglet ==3))
   {
    decomposition = 1;
   }


   if(sousOnglet == NbBg-1)
   {
    qsim_r = new QStandardItemModel(nbItems,3);
   }
   else
   {
    qsim_r = new QStandardItemModel(nbItems*nbI2tems,3);
   }


   // Preparer le tableau des valeurs de synthese
   for(int loop=0;loop < 50; loop++)
   {
    QStandardItem *item_1 = new QStandardItem();
    QStandardItem *item_2 = new QStandardItem();
    qsim_synthese->setItem(loop,0,item_1);
    qsim_synthese->setItem(loop,1,item_2);
   }

   qsim_r->setHeaderData(0,Qt::Horizontal,"ReqHide");
   qsim_r->setHeaderData(1,Qt::Horizontal,"Combinaison");
   qsim_r->setHeaderData(2,Qt::Horizontal,"Total");
   qtv_r->setModel(qsim_r);

   qtv_r->setSortingEnabled(true);
   qtv_r->setAlternatingRowColors(true);
   qtv_r->setEditTriggers(QAbstractItemView::NoEditTriggers);
   qtv_r->setColumnWidth(0,180);
   //qtv_r->setColumnWidth(1,60);
   qtv_r->hideColumn(0);
   qtv_r->setColumnWidth(2,50);


   qsim_synthese->setHeaderData(0,Qt::Horizontal,"B");
   qsim_synthese->setHeaderData(1,Qt::Horizontal,"T");
   qtv_synthese->setModel(qsim_synthese);

   qtv_synthese->setSortingEnabled(true);
   qtv_synthese->setAlternatingRowColors(true);
   qtv_synthese->setEditTriggers(QAbstractItemView::NoEditTriggers);
   qtv_synthese->setColumnWidth(0,50);
   qtv_synthese->setColumnWidth(1,50);

   QString st_SubOngName = "R"
                           + QString::number(sousOnglet)
                           +":"
                           +QString::number(nbItems);

   tw_rep->addTab(tw_lgnCombi,tr(st_SubOngName.toLocal8Bit()));

   tw_lgnCombi->addTab(qtv_r,tr("Req"));
   tw_lgnCombi->addTab(qtv_synthese,tr("Val"));

   // Si zoom sur total demande
   connect( qtv_r, SIGNAL( doubleClicked (QModelIndex)) ,
            this, SLOT( slot_TST_DetailsCombinaison( QModelIndex) ) );



   for (int i = 0; i < nbItems; ++i)
   {
    QString st_CombLine = sl_Lev1[sousOnglet].at(i);
    QStringList sl_Lev2 = st_CombLine.split(",");
    //int taille = nb.size();
    int d[5] = {0};

    switch(nbI2tems)
    {
     case 1:
      d[0]=NbBg;
      break;
     case 2:
      if(decomposition ==1){
       d[0]=NbBg-1;
       d[1]=1;
      }
      else{
       d[0]=NbBg-2;
       d[1]=2;
      }
      break;
     case 3:
      if(decomposition ==1){
       d[0]=NbBg-2;
       d[1]=1;
       d[2]=1;
      }
      else
      {
       d[0]=NbBg-3;
       d[1]=2;
       d[2]=1;
      }
      break;
     case 4:
      d[0]=NbBg-3;
      d[1]=1;
      d[2]=1;
      d[3]=1;
      break;
     case 5:
      d[0]=NbBg-4;
      d[1]=1;
      d[2]=1;
      d[3]=1;
      d[4]=1;
      break;
     default:
      ;// Erreur
      break;
    }

    QString colsel = "";
    if((nbI2tems >1) && (nbI2tems <5))
    {
     if(sousOnglet != NbBg-1)
     {
      for (int j = 0; j < nbI2tems; ++j)
      {
       colsel="";
       msg="";
       for (int k = 0; k < nbI2tems; ++k)
       {
        int ival = sl_Lev2.at(k).toInt()-1;
        colsel = colsel +
                 "bd" + QString::number(ival)
                 + "="+QString::number(d[(j+k)%nbI2tems])+" and ";
       }

       colsel.remove(colsel.length()-5,5);
       msg = "select count (*) from "+tb_ana_zn+" where ("
             + colsel + ");";

       for(int loop=0;loop<3;loop++)
       {
        QStandardItem *item_2 = new QStandardItem();
        qsim_r->setItem((i*nbI2tems)+j,loop,item_2);
       }

       status = DB_tirages->TST_Requete(ShowCol,msg,(i*nbI2tems)+j,colsel,qsim_r);
      }
     }
     else
     {
      // Dernier onglet !!!
      for (int j = 0; j < nbI2tems; ++j)
      {
       int ival = sl_Lev2.at(j).toInt()-1;
       colsel = colsel +
                "bd" + QString::number(ival)
                + "=%"+QString::number(j+1)+" and ";
       colsel = colsel.arg(d[j]);
      }

      colsel.remove(colsel.length()-5,5);
      msg = "select count (*) from "+tb_ana_zn+" where ("
            + colsel + ");";
      for(int loop=0;loop<3;loop++)
      {
       QStandardItem *item_2 = new QStandardItem();
       qsim_r->setItem(i,loop,item_2);
      }

      status = DB_tirages->TST_Requete(ShowCol,msg,i,colsel,qsim_r);

     }
    }
    else
    {
     for (int j = 0; j < nbI2tems; ++j)
     {
      int ival = sl_Lev2.at(j).toInt()-1;
      colsel = colsel +
               "bd" + QString::number(ival)
               + "=%"+QString::number(j+1)+" and ";
      colsel = colsel.arg(d[j]);
     }

     colsel.remove(colsel.length()-5,5);
     msg = "select count (*) from "+tb_ana_zn+" where ("
           + colsel + ");";
     for(int loop=0;loop<3;loop++)
     {
      QStandardItem *item_2 = new QStandardItem();
      qsim_r->setItem(i,loop,item_2);
     }

     status = DB_tirages->TST_Requete(ShowCol,msg,i,colsel,qsim_r);
    }
   }

   // Un onglet est construit
   decomposition++;

   // Faire la synthese des boules trouvees
   qtv_r->sortByColumn(2,Qt::DescendingOrder);
   //qsim_t->sort(0,Qt::AscendingOrder);
   qsim_t->sort(1,Qt::DescendingOrder);
   TST_SyntheseDesCombinaisons(qtv_r,qsim_synthese, qsim_t, &NbTotLgn);
   qtv_synthese->sortByColumn(1,Qt::DescendingOrder);
  }

  // Onglet "Total" pour la combinaison en cours
  qsim_t->setHeaderData(0,Qt::Horizontal,"B");
  qsim_t->setHeaderData(1,Qt::Horizontal,"T");
  qtv_t->setModel(qsim_t);

  qtv_t->setSortingEnabled(true);
  qtv_t->setAlternatingRowColors(true);
  qtv_t->setEditTriggers(QAbstractItemView::NoEditTriggers);
  qtv_t->setColumnWidth(0,50);
  qtv_t->setColumnWidth(1,50);
  qtv_t->sortByColumn(1,Qt::DescendingOrder);
  QString st_SubOngTotal = "Total:"
                           +QString::number(NbTotLgn);

  tw_rep->addTab(qtv_t,tr(st_SubOngTotal.toLocal8Bit()));
 }
#endif
 // Fin onglet pere
}

QTabWidget *MainWindow::TST_OngletN1(QTabWidget *pere,int pos, QStringList (*lst_comb)[5],stTiragesDef *ref)
{
 QTabWidget *tmp = new QTabWidget;
 QStringList *coef = NULL;

 QTableView *qtv_r3 = new QTableView;
 QStandardItemModel * qsim_r3 = TST_SetTblViewVal(50, qtv_r3);;

 QString tb_ana_zn = "Ref_ana_z1";

 //bool status = false;
 int ShowCol = ((ref->limites[0].max)/10)+1;
 int NbTotLgn = 0; // Total de reponses trouvee

 // Nom de l'onglet
 QString st_OngName = "Comb" + QString::number(pos);
 pere->addTab(tmp,tr(st_OngName.toLocal8Bit()));

 // Decomposition possible du nombre
 coef = TST_PartitionEntier(pos);
 int taille = coef->size();

 // Mise en place des coefficients pour les combinaisons
 int Decompose[taille][5];
 for(int i = 0; i< taille;i++)
 {
  int nbItems = ((coef->at(i)).split(",")).size();
  int val = 0;
  for(int j=0;j<5;j++)
  {
   if(j<nbItems)
   {
    val = ((coef->at(i)).split(",")).at(j).toInt();
   }
   else
   {
    val = 0;
   }
   Decompose[i][j]=val;
  }
 }

 // Creation des onglets nommes
 for(int i = 0; i< taille;i++)
 {
  QTabWidget *tw_n2 = new QTabWidget; // Onglet pour une combinaison

  QTableView *qtv_r1 = new QTableView;
  QStandardItemModel * qsim_r1 = NULL;

  QTableView *qtv_r2 = new QTableView;
  QStandardItemModel * qsim_r2 = NULL;


  int nbItems = ((coef->at(i)).split(",")).size();
  int tot_comb = ((*lst_comb)[nbItems-1]).size();
  int nb_reponses = 0;

  QString st_tmp = "R"
                   + QString::number(i)
                   +":"
                   +QString::number(tot_comb);

  tmp->addTab(tw_n2,tr(st_tmp.toLocal8Bit()));
  tw_n2->addTab(qtv_r1,tr("Req"));
  tw_n2->addTab(qtv_r2,tr("Val"));

  // Si zoom sur total demande
  connect( qtv_r1, SIGNAL( doubleClicked (QModelIndex)) ,
           this, SLOT( slot_TST_DetailsCombinaison( QModelIndex) ) );

  // Preparation des emplacements de reponses
  if(nbItems == ShowCol )
  {
   nb_reponses = 1;
  }
  else
  {
   nb_reponses = nbItems * tot_comb;
  }
  qsim_r1 = TST_SetTblViewCombi(nb_reponses, qtv_r1);

  // Onglet val de cette combinaison
  //nb_reponses = ref->limites[0].max;
  nb_reponses = 50;
  qsim_r2 = TST_SetTblViewVal(nb_reponses, qtv_r2);
  //qsim_r3 = TST_SetTblViewVal(nb_reponses, qtv_r3);

  // Creation des requetes de recherche
  for(int j = 0; j< tot_comb;j++)
  {
   QStringList tmp_lst = (((*lst_comb)[nbItems-1]).at(j)).split(",");

   QString s_msg= "";
   QString s_col = "";
   int i_lgn = 0;

   // Cas 1 elment comme combinaison possible

   for(int k = 0; k< nbItems; k++)
   {

    int i_v1 = tmp_lst.at(k).toInt() -1 ;
    //int i_v1 = (((*lst_comb)[nbItems-1]).at(j)).split(",").at(k).toInt() - 1;
    //int i_v2 = ((coef->at(i)).split(",")).at(k).toInt();

    s_col = s_col +
            "bd" + QString::number(i_v1)
            + "=%"+QString::number(k)+" and ";
    s_col = s_col.arg(Decompose[i][k]);
   }
   s_col.remove(s_col.length()-5,5); // retire dernier and
   s_msg = "select count (*) from "+tb_ana_zn+" where ("
           + s_col + ");";

   // La requete est cree. L'executer !!
   i_lgn = (j*nbItems);
   //status =
   DB_tirages->TST_Requete(ShowCol,s_msg,i_lgn,s_col,qsim_r1);

   if((nbItems >1) && (nbItems <pos))
   {
    //rotation circulaire
    for(int loop=1;loop < nbItems; loop ++)
    {

     //QString s_msg= "";
     //QString s_col = "";
     s_msg= "";
     s_col= "";
     for(int k = 0; k< nbItems; k++)
     {
      //int i_v1 = ((*lst_comb)[nbItems-1]).at(j).toInt() - 1;
      //int i_v1 = (((*lst_comb)[nbItems-1]).at(j)).split(",").at(k).toInt() - 1;
      int i_v1 = tmp_lst.at(k).toInt() -1 ;
      //int i_v2 = ((coef->at(i)).split(",")).at((k+1)%nbItems).toInt();

      s_col = s_col +
              "bd" + QString::number(i_v1)
              + "=%"+QString::number(k)+" and ";
      s_col = s_col.arg(Decompose[i][(k+loop)%nbItems]);
     }
     s_col.remove(s_col.length()-5,5); // retire dernier and
     s_msg = "select count (*) from "+tb_ana_zn+" where ("
             + s_col + ");";


     // La requete est cree. L'executer !!
     i_lgn = loop+(j*nbItems);
     //status =
     DB_tirages->TST_Requete(ShowCol,s_msg,i_lgn,s_col,qsim_r1);
    }
   }

   //tip
  } // Fin requetes de recherche

  // Recherche du nb de sorties des boules pour cette combinaison
  qsim_r1->sort(0,Qt::AscendingOrder);
  qsim_r2->sort(0,Qt::AscendingOrder);
  qsim_r3->sort(0,Qt::AscendingOrder);
  TST_SyntheseDesCombinaisons(qtv_r1,qsim_r2, qsim_r3, &NbTotLgn);


  // Rajout du dernier Onglet
  st_tmp = "Total:"
           +QString::number(NbTotLgn);
  tmp->addTab(qtv_r3,tr(st_tmp.toLocal8Bit()));

  // mettre les max en premier
  qtv_r1->sortByColumn(2,Qt::DescendingOrder);
  qtv_r2->sortByColumn(1,Qt::DescendingOrder);
  qtv_r3->sortByColumn(1,Qt::DescendingOrder);
 }

 return tmp;

}


QStandardItemModel * MainWindow::TST_SetTblViewCombi(int nbLigne, QTableView *qtv_r)
{
 QStandardItemModel *qsim_r = new QStandardItemModel(nbLigne,3);

 qsim_r->setHeaderData(0,Qt::Horizontal,"ReqHide");
 qsim_r->setHeaderData(1,Qt::Horizontal,"Combinaison");
 qsim_r->setHeaderData(2,Qt::Horizontal,"Total");
 qtv_r->setModel(qsim_r);

 qtv_r->setSortingEnabled(true);
 qtv_r->setAlternatingRowColors(true);
 qtv_r->setEditTriggers(QAbstractItemView::NoEditTriggers);
 qtv_r->setColumnWidth(0,180);
 qtv_r->hideColumn(0);
 qtv_r->setColumnWidth(2,50);

 for(int i = 0; i<3; i++)
 {
  for(int j=0;j<nbLigne;j++)
  {
   QStandardItem *item_1 = new QStandardItem();
   qsim_r->setItem(j,i,item_1);
  }
 }



 return qsim_r;
}

QStandardItemModel * MainWindow::TST_SetTblViewVal(int nbLigne, QTableView *qtv_r)
{
 QStandardItemModel *qsim_r = new QStandardItemModel(nbLigne,2);

 qsim_r->setHeaderData(0,Qt::Horizontal,"B");
 qsim_r->setHeaderData(1,Qt::Horizontal,"T");
 qtv_r->setModel(qsim_r);

 qtv_r->setSortingEnabled(true);
 qtv_r->setAlternatingRowColors(true);
 qtv_r->setEditTriggers(QAbstractItemView::NoEditTriggers);
 qtv_r->setColumnWidth(0,50);
 qtv_r->setColumnWidth(1,50);

 for(int i = 0; i<2; i++)
 {
  for(int j=0;j<nbLigne;j++)
  {
   QStandardItem *item_1 = new QStandardItem();
   if(i==0){
    item_1->setData(j+1,Qt::DisplayRole);
   }
   qsim_r->setItem(j,i,item_1);
  }
 }
 return qsim_r;
}

void MainWindow::TST_SyntheseDesCombinaisons(QTableView *p_in, QStandardItemModel * qsim_rep,QStandardItemModel * qsim_total, int *TotalLigne)
{
 int ligne = 0;
 int val = 0;
 QModelIndex modelIndex;
 QString tb_ana_zn = "Ref_ana_z1";
 //QAbstractItemModel *theModel = p_in->model();
 //QStandardItemModel *dest= (QStandardItemModel*) theModel;
 QString SqlReq = "";

 do
 {
  modelIndex = p_in->model()->index(ligne,2, QModelIndex());

  if(modelIndex.isValid()){
   val = modelIndex.data().toInt();
   if(val)
   {
    //QStandardItem *item1 = dest->item(ligne,0) ;
    modelIndex = p_in->model()->index(ligne,0, QModelIndex());
    //QString msg = item1->data().toString();
    QString msg = modelIndex.data().toString();

    msg.replace("c","bd");
    msg.replace(",","and");

    msg = "Select * from tirages inner join (select * from "+tb_ana_zn+" where("
          + msg +
          "))as s on tirages.id = s.id union ";

    SqlReq = SqlReq + msg;
   }
  }
  ligne++;
 }while(modelIndex.isValid());

 if(!SqlReq.isEmpty())
 {
  SqlReq.remove(SqlReq.length()-6,6);

  //#ifndef QT_NO_DEBUG
  //qDebug()<< SqlReq;
  //#endif

  int static union_id = 0;
  QString st_ut = "create table ru_" +QString::number(union_id)
                  +" as SELECT * FROM ("
                  +SqlReq + ");";

  bool status = false;
  QSqlQuery sql_1(db_0);
  status = sql_1.exec(st_ut);

  // Compter le nombre de ligne du tableau union
  st_ut = "Select count (*) from  ru_" +QString::number(union_id) + ";";
  status = false;
  status = sql_1.exec(st_ut);
  if(status)
  {
   sql_1.first();
   if (sql_1.isValid())
   {
    (*TotalLigne) += sql_1.value(0).toInt();
   }
  }


  // Compter les occurences de chaque boule
  for(int i =1; (i< 51) ;i++)
  {
   QStandardItem * item_1 = qsim_rep->item(i-1,0);
   QStandardItem * item_2 = qsim_rep->item(i-1,1);
   QStandardItem * item_total = qsim_total->item(i-1,1);
   int total_glob = item_total->data(Qt::DisplayRole).toInt();

   item_1->setData(i,Qt::DisplayRole);
   qsim_rep->setItem(i-1,0,item_1);
   // -------------------
   QString msg_2 = "select count (*) from ru_" +QString::number(union_id)+ " where (" +
                   "b1 = " +QString::number(i) + " or " +
                   "b2 = " +QString::number(i) + " or " +
                   "b3 = " +QString::number(i) + " or " +
                   "b4 = " +QString::number(i) + " or " +
                   "b5 = " +QString::number(i) + " );" ;

   status = sql_1.exec(msg_2);
   int tot=0;
   if(status)
   {
    sql_1.first();
    if(sql_1.isValid())
    {
     tot = sql_1.value(0).toInt();
    }
   }

   // ------------------
   //int tot = TST_TotBidDansGroupememnt(i,SqlReq);
   item_2->setData(tot,Qt::DisplayRole);
   qsim_rep->setItem(i-1,1,item_2);

   // Total pour cette combi
   total_glob +=tot;
   item_total->setData(total_glob,Qt::DisplayRole);
   qsim_total->setItem(i-1,1,item_total);
  }
  union_id++;
 }
}

void MainWindow::slot_TST_DetailsCombinaison( const QModelIndex & index)
{
 QString msg = index.model()->index(index.row(),0).data().toString();
 //int val = index.model()->index(index.row(),1).data().toInt();

 TST_MontrerDetailCombinaison(msg, &configJeu);
}

void MainWindow:: VUE_ListeTiragesFromDistribution(int critere, int distance, int choix)
{
#if 0
 select * from (select id, fk_idCombi_z1 from "+tb_ana_zn+" where "+tb_ana_zn+".fk_idCombi_z1 = 107);

 select  "+tb_ana_zn+".id, "+tb_ana_zn+".fk_idCombi_z1 from
   (select id, fk_idCombi_z1 from "+tb_ana_zn+" where "+tb_ana_zn+".fk_idCombi_z1 = 107) as t1
   left join "+tb_ana_zn+" on t1.id = "+tb_ana_zn+".id+1;

 select t2.fk_idCombi_z1,tirages.* from
   (select  "+tb_ana_zn+".id, "+tb_ana_zn+".fk_idCombi_z1 from
    (select id, fk_idCombi_z1 from "+tb_ana_zn+" where "+tb_ana_zn+".fk_idCombi_z1 = 107) as t1
    left join "+tb_ana_zn+" on t1.id = "+tb_ana_zn+".id+1) as t2
   left join tirages on t2.id=tirages.id where(t2.fk_idCombi_z1 = 57);
#endif
 QWidget *qw_fenResu = new QWidget;
 QTabWidget *tw_resu = new QTabWidget;
 QFormLayout *mainLayout = new QFormLayout;
 bool status = false;
 int d[5]={1,2,-1,-2,0};

 // ---
 QTableView *tv_r1 = new QTableView;
 QSqlQueryModel *sqm_r1 = new QSqlQueryModel;
 QSqlQuery query(db_0);
 QString st_msg ="";
 QString st_titre="";
 QString st_where = "";

 QString tb_ana_zn = "Ref_ana_z1";

 if(choix != 0)
 {
  st_where =   "where(t2.fk_idCombi_z1 = "+QString::number(choix)+")" ;
 }

 //t2.fk_idCombi_z1,
 // "+QString::number(d[distance])+"
 st_msg = "select tirages.* from "
          "(select  "+tb_ana_zn+".id, "+tb_ana_zn+".fk_idCombi_z1 from "
                                                  "(select id, fk_idCombi_z1 from "+tb_ana_zn+" where "+tb_ana_zn+".fk_idCombi_z1 = "
          +QString::number(critere)
          +") as t1 "
           "left join "+tb_ana_zn+" on t1.id = "+tb_ana_zn+".id + %1 ) as t2 "
                                                           "left join tirages on t2.id=tirages.id "+ st_where +";";

 st_msg=st_msg.arg(d[distance]);
 //qDebug()<< st_msg;

 sqm_r1->setQuery(st_msg,db_0);
 tv_r1->setModel(sqm_r1);
 tw_resu->addTab(tv_r1,tr("Details"));

 tv_r1->hideColumn(0);
 //tv_r1->hideColumn(1);
 for(int j=2;j<12;j++)
  tv_r1->setColumnWidth(j,30);

 for(int j =12; j<=sqm_r1->columnCount();j++)
  tv_r1->hideColumn(j);

 // Onglet Synthese
 VUE_Ltfd_Synthese(tw_resu,st_msg);

 // ---
 mainLayout->addWidget(tw_resu);
 qw_fenResu->setLayout(mainLayout);

 st_msg= "select tip from lstcombi where (id=:valeur);";
 query.prepare(st_msg);
 query.bindValue(":valeur",critere);
 status = query.exec();
 if(status){
  query.first();
  if(query.isValid())
  {
   st_titre = query.value(0).toString();
  }
 }
 st_titre = st_titre + " ,D" + QString::number(d[distance]) + " ,";

 query.bindValue(":valeur",choix);
 status = query.exec();
 if(status){
  query.first();
  if(query.isValid())
  {
   st_titre = st_titre + query.value(0).toString();
  }
 }

 qw_fenResu->setWindowTitle(st_titre);

 // Double click dans sous fenetre ecart
 connect( tv_r1, SIGNAL( doubleClicked(QModelIndex)) ,
          this, SLOT( slot_MontreLeTirage( QModelIndex) ) );

 zoneCentrale->addSubWindow(qw_fenResu);
 qw_fenResu->show();
}
void MainWindow::VUE_Ltfd_Synthese(QTabWidget *pere, QString &st_msg)
{
 int zn = 0;
 int nb_boules = configJeu.limites[zn].max;

 QTableView *qtv_rep = new QTableView;
 QStandardItemModel *qsim_rep = new QStandardItemModel(nb_boules,2);

 // suppression dernier ';' dans SQL
 st_msg.remove(st_msg.length()-1,1);


 qsim_rep->setHeaderData(0,Qt::Horizontal,"B");
 qsim_rep->setHeaderData(1,Qt::Horizontal,"T");

 qtv_rep->setModel(qsim_rep);;
 qtv_rep->setSortingEnabled(true);
 qtv_rep->setEditTriggers(QAbstractItemView::NoEditTriggers);

 qtv_rep->setColumnWidth(0,40);
 qtv_rep->setColumnWidth(1,40);

 // Compter les occurences de chaque boule
 for(int i =1; (i< nb_boules+1) ;i++)
 {
  QStandardItem * item_1 = new QStandardItem;
  QStandardItem * item_2 = new QStandardItem;

  item_1->setData(i,Qt::DisplayRole);
  qsim_rep->setItem(i-1,0,item_1);

  int tot = TST_TotBidDansGroupememnt(i,st_msg);
  item_2->setData(tot,Qt::DisplayRole);
  qsim_rep->setItem(i-1,1,item_2);
 }

 pere->addTab(qtv_rep,tr("Synthese"));
 qtv_rep->sortByColumn(1,Qt::DescendingOrder);
}

void MainWindow::TST_MontrerDetailCombinaison(QString msg, stTiragesDef *pTDef)
{
 QWidget *qw_fenResu = new QWidget;
 QTabWidget *tw_resu = new QTabWidget;
 QSqlQueryModel *sqm_r1 = new QSqlQueryModel;
 QTableView *tv_r1 = new QTableView;
 QString st_msg ="";
 QFormLayout *mainLayout = new QFormLayout;
 QString tb_ana_zn = "Ref_ana_z1";


 msg.replace("c","bd");
 msg.replace(",","and");

 st_msg = "Select * from tirages inner join (select * from "+tb_ana_zn+" where("
          + msg +
          "))as s on tirages.id = s.id;";





 sqm_r1->setQuery(st_msg,db_0);
 tv_r1->setModel(sqm_r1);
 tw_resu->addTab(tv_r1,tr("Details"));

 //view->setSortingEnabled(true);

 tv_r1->hideColumn(0);
 for(int j=2;j<12;j++)
  tv_r1->setColumnWidth(j,30);

 for(int j =12; j<=sqm_r1->columnCount();j++)
  tv_r1->hideColumn(j);


 ////////////////////////////////////////////////////////////
 QTableView *qtv_rep = new QTableView;
 int zn = 0;
 int nb_boules = pTDef->limites[zn].max;
 QStandardItemModel *qsim_rep = new QStandardItemModel(nb_boules,2);
 //QTabWidget *tw_resu_2 = new QTabWidget;

 qsim_rep->setHeaderData(0,Qt::Horizontal,"B");
 qsim_rep->setHeaderData(1,Qt::Horizontal,"T");

 qtv_rep->setModel(qsim_rep);;
 qtv_rep->setSortingEnabled(true);
 qtv_rep->setEditTriggers(QAbstractItemView::NoEditTriggers);

 qtv_rep->setColumnWidth(0,40);
 qtv_rep->setColumnWidth(1,40);

 st_msg.remove(st_msg.length()-1,1);
 // Compter les occurences de chaque boule
 for(int i =1; (i< nb_boules+1) ;i++)
 {
  QStandardItem * item_1 = new QStandardItem;
  QStandardItem * item_2 = new QStandardItem;

  item_1->setData(i,Qt::DisplayRole);
  qsim_rep->setItem(i-1,0,item_1);

  // -----------------

  // -----------------
  int tot = TST_TotBidDansGroupememnt(i,st_msg);
  item_2->setData(tot,Qt::DisplayRole);
  qsim_rep->setItem(i-1,1,item_2);

 }


 //qtv_rep->show();

 // ??
 tw_resu->addTab(qtv_rep,tr("Synthese"));
 qtv_rep->sortByColumn(1,Qt::DescendingOrder);
 mainLayout->addWidget(tw_resu);
 qw_fenResu->setLayout(mainLayout);
 qw_fenResu->setWindowTitle(msg);

 // Double click dans sous fenetre ecart
 connect( tv_r1, SIGNAL( doubleClicked(QModelIndex)) ,
          this, SLOT( slot_MontreLeTirage( QModelIndex) ) );

 zoneCentrale->addSubWindow(qw_fenResu);
 qw_fenResu->show();

}

//////
void MainWindow::TST_LBcDistBr(int zn,stTiragesDef *pConf,int dist, QStringList boules,int bc)
{
#if 0
 select * from
   (
    select * from
    (
     select t1.*, t2.*  from tirages as t1
     inner join
     (
      select *  from tirages
      where
      (
       (b1=25 or b2=25 or b3=25 or b4=25 or b5=25 )
       )
      ) as t2 on t2.id = t1.id + 2
                         ) as r1
    where (b1=38 or b2=38 or b3=38 or b4=38 or b5=38  )
    )as r2;
#endif

 QWidget *qw_fenResu = new QWidget;
 QTabWidget *tw_resu = new QTabWidget;
 QSqlQueryModel *sqm_r1 = new QSqlQueryModel;
 QTableView *tv_r1 = new QTableView;
 QString st_msg ="";
 QFormLayout *mainLayout = new QFormLayout;
 QString msg = "select * from (select * from (select t1.*, t2.*  from tirages as t1"
               " inner join (select *  from tirages where (";
 //QStringList boules;
 //boules << QString::number(br);
 QString w_msg = DB_tirages->TST_ConstruireWhereData(zn,pConf,boules);
 msg = msg + w_msg;
 msg = msg + ")) as t2 on t2.id = t1.id + %1 ) as r1 "
             "where (";
 msg = (msg).arg(dist);

 QString msg_2 = DB_tirages->TST_ZoneRequete(pConf, zn,"or",bc,"=");
 msg_2= msg+ msg_2+ " ))as r2;" ;





 sqm_r1->setQuery(msg_2,db_0);
 tv_r1->setModel(sqm_r1);
 //view->setSortingEnabled(true);

 tv_r1->hideColumn(0);
 for(int j=2;j<12;j++)
  tv_r1->setColumnWidth(j,30);

 for(int j =12; j<=sqm_r1->columnCount();j++)
  tv_r1->hideColumn(j);


 ////////////////////////////////////////////////////////////
 QTableView *qtv_rep = new QTableView;
 //int zn = 0;
 int nb_boules = pConf->limites[zn].max;
 QStandardItemModel *qsim_rep = new QStandardItemModel(nb_boules,2);
 //QTabWidget *tw_resu_2 = new QTabWidget;

 qsim_rep->setHeaderData(0,Qt::Horizontal,"B");
 qsim_rep->setHeaderData(1,Qt::Horizontal,"T");

 qtv_rep->setModel(qsim_rep);;
 qtv_rep->setSortingEnabled(true);
 qtv_rep->setEditTriggers(QAbstractItemView::NoEditTriggers);

 qtv_rep->setColumnWidth(0,40);
 qtv_rep->setColumnWidth(1,40);

 msg_2.remove(msg_2.length()-1,1);
 // Compter les occurences de chaque boule
 for(int i =1; (i< nb_boules+1) ;i++)
 {
  QStandardItem * item_1 = new QStandardItem;
  QStandardItem * item_2 = new QStandardItem;

  item_1->setData(i,Qt::DisplayRole);
  qsim_rep->setItem(i-1,0,item_1);

  // -----------------

  // -----------------
  int tot = TST_TotBidDansGroupememnt(i,msg_2);
  item_2->setData(tot,Qt::DisplayRole);
  qsim_rep->setItem(i-1,1,item_2);

 }


 // ??
 tw_resu->addTab(tv_r1,tr("Details"));
 tw_resu->addTab(qtv_rep,tr("Synthese"));
 mainLayout->addWidget(tw_resu);
 qw_fenResu->setLayout(mainLayout);
 msg = "Br:"+boules.join(",")+", Bc:" + QString::number(bc) + ", D:" + QString::number(dist);
 qw_fenResu->setWindowTitle(msg);

 // Double click dans sous fenetre ecart
 connect( tv_r1, SIGNAL( doubleClicked(QModelIndex)) ,
          this, SLOT( slot_MontreLeTirage( QModelIndex) ) );

 zoneCentrale->addSubWindow(qw_fenResu);
 qw_fenResu->show();

}
////////

void MainWindow::TST_MontreTirageAyantCritere(NE_FDJ::E_typeCritere lecritere,int zn,stTiragesDef *pConf, QStringList boules)
{
 QString msg = "select *  from tirages where (";
 //QStringList boules;
 //boules << QString::number(br);

 QString w_msg = "";
 QString titre = "";
 switch(lecritere)
 {
  case NE_FDJ::critere_boule:
   w_msg = DB_tirages->TST_ConstruireWhereData(zn,pConf,boules);
   break;
  case NE_FDJ::critere_parite:
   w_msg = pConf->nomZone[zn]+CL_PAIR+"="+boules.at(0);
   break;
  case NE_FDJ::critere_enemble:
   w_msg =pConf->nomZone[zn]+CL_SGRP+"="+boules.at(0);
   //QString::number(j)
   break;

  default:
   break;
 }

 msg = msg + w_msg;
 msg = msg + ");";

 //titre = "B:"+boules.join(",");

 TST_FenetreReponses(titre,zn,msg,boules,pConf);

}

void MainWindow::TST_FenetreReponses(QString fen_titre, int zn, QString req_msg,QStringList st_list,stTiragesDef *pConf)
{

 QWidget *qw_fenResu = new QWidget;
 QTabWidget *tw_resu = new QTabWidget;
 QSqlQueryModel *sqm_r1 = new QSqlQueryModel;
 QTableView *tv_r1 = new QTableView;
 QString st_msg ="";
 QFormLayout *mainLayout = new QFormLayout;
 QString fenetre_titre = "";
 QString msg = req_msg;
 //bool status = false;

 if (fen_titre == ""){
  fenetre_titre = "B:"+st_list.join(",");
 }
 else
 {
  fenetre_titre =  fen_titre;
 }

 sqm_r1->setQuery(msg,db_0);


 tv_r1->setModel(sqm_r1);
 //view->setSortingEnabled(true);

 tv_r1->hideColumn(0);
 for(int j=2;j<12;j++)
  tv_r1->setColumnWidth(j,30);

 for(int j =12; j<=sqm_r1->columnCount();j++)
  tv_r1->hideColumn(j);


 ////////////////////////////////////////////////////////////
 QTableView *qtv_rep = new QTableView;
 //int zn = 0;
 int nb_boules = pConf->limites[zn].max;
 QStandardItemModel *qsim_rep = new QStandardItemModel(nb_boules,2);
 //QTabWidget *tw_resu_2 = new QTabWidget;

 qsim_rep->setHeaderData(0,Qt::Horizontal,"B");
 qsim_rep->setHeaderData(1,Qt::Horizontal,"T");

 qtv_rep->setModel(qsim_rep);;
 qtv_rep->setSortingEnabled(true);
 qtv_rep->setEditTriggers(QAbstractItemView::NoEditTriggers);

 qtv_rep->setColumnWidth(0,40);
 qtv_rep->setColumnWidth(1,40);

 msg.remove(msg.length()-1,1);
 // Compter les occurences de chaque boule
 for(int i =1; (i< nb_boules+1) ;i++)
 {
  QStandardItem * item_1 = new QStandardItem;
  QStandardItem * item_2 = new QStandardItem;

  item_1->setData(i,Qt::DisplayRole);
  qsim_rep->setItem(i-1,0,item_1);

  // -----------------

  // -----------------
  int tot = TST_TotBidDansGroupememnt(i,msg);
  item_2->setData(tot,Qt::DisplayRole);
  qsim_rep->setItem(i-1,1,item_2);

 }


 // ??
 tw_resu->addTab(tv_r1,tr("Details"));
 tw_resu->addTab(qtv_rep,tr("Synthese"));
 mainLayout->addWidget(tw_resu);
 qw_fenResu->setLayout(mainLayout);
 qw_fenResu->setWindowTitle(fenetre_titre);

 // Double click dans sous fenetre ecart
 connect( tv_r1, SIGNAL( doubleClicked(QModelIndex)) ,
          this, SLOT( slot_MontreLeTirage( QModelIndex) ) );

 zoneCentrale->addSubWindow(qw_fenResu);
 qw_fenResu->show();

}

void MainWindow::slot_MontreTirageDansGraph(const QModelIndex & index)
{
 if (index.column()==0 || index.column()==2)
 {
  VUE_MontreLeTirage(index.row()+1);
 }
}


void MainWindow::slot_MontreTirageAnalyse(const QModelIndex & index)
{

#ifndef QT_NO_DEBUG
 qDebug() << "A debuger slot_MontreTirageAnalyse";
#endif
 return;

 int id = index.row()+1;
 int nb_col = gsim_AnalyseUnTirage->columnCount();

 for(int i = 2; i<=nb_col; i++)
 {
  QStandardItem *item = gsim_AnalyseUnTirage->item(0,i-2);
  int val = 0;

  // Recherche de la config pour le  tirage
  val=RechercheInfoTirages(id,i-2,&configJeu);

  // affectation
  item->setData(val,Qt::DisplayRole);

  // visualisation
  gsim_AnalyseUnTirage->setItem(0,i-2,item);
 }


}
void MainWindow::slot_PresenteLaBoule(const QModelIndex & index)
{
 /// se mettre sur le bon onglet
 gtab_vue->setCurrentIndex(0);

 // recuperer la valeur de la boule
 int val = index.model()->index(index.row(),0).data().toInt();

 int path[]={0,0,0};
 syntheses->Surligne(path,val);
 return;

 QTableView *pTbv1 = NULL;
 QSqlQueryModel *pSqm1 =NULL;
 QStandardItemModel *pSim1 =NULL;
 QSortFilterProxyModel *pSfpm1 =NULL;



 // Recuperer le tableau des ecarts
 ecarts = syntheses->GetTabEcarts();
 ecarts->GetInfoTableau(0, &pTbv1 , &pSim1 , &pSfpm1);
 pSim1->sort(0);
 pTbv1->scrollTo(pSim1->index(val-1,1));

 // Recuperer le tableau des comptages
 syntheses->GetInfoTableau(0, &pTbv1 , &pSqm1 , &pSfpm1);
 pSfpm1->sort(0);
 pTbv1->scrollTo(pSfpm1->index(val-1,0));
}

void MainWindow::slot_MontreLeTirage(const QModelIndex & index)
{
 // recuperer la ligne de la table
 int val = index.model()->index(index.row(),0).data().toInt();

 /// se mettre sur le bon onglet
 gtab_vue->setCurrentIndex(0);
 //int path[]={0,0,0};
 //syntheses->Surligne(path,val);

 // Montrer la selection dans les graphiques
 VUE_MontreLeTirage(val);

 // Montre dans la fenetre table tirages
 //MontreDansLaQtView(G_tbv_Tirages,val,0);


 QTableView * pTableauTirages = NULL;
 pTableauTirages = syntheses->GetListeTirages();

 // se mettre sur l'onglet qui contient ce table view
 QObject *unParent = pTableauTirages;
 QStackedWidget *target = NULL;

 do{
  unParent = unParent->parent();
  target=unParent->findChild<QStackedWidget *>();
 }while(target==NULL);

 QTabWidget * tmp = qobject_cast<QTabWidget *>(unParent->parent());
 tmp->setCurrentIndex(0);

 MontreDansLaQtView(pTableauTirages,val,2);
}

void MainWindow::MontreDansLaQtView(QTableView *ptr_qtv, int val,int col_id)
{

 QAbstractItemModel *mon_model = ptr_qtv->model();
 QModelIndex item1 = mon_model->index(0,0, QModelIndex());

 ptr_qtv->setAutoScroll(true);
 ptr_qtv->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);

 if (item1.isValid()){
  item1 = item1.model()->index(val-1,col_id,QModelIndex());

  // Associer toutes les valeurs a la vue
  while (mon_model->canFetchMore(item1))
  {
   mon_model->fetchMore(item1);
  }


  // Montrer la selection dans le tableau des tirages
  ptr_qtv->setCurrentIndex(item1);
  ptr_qtv->scrollTo(item1);
  //ptr_qtv->selectRow(val-1);

  QItemSelectionModel *selectionModel (ptr_qtv->selectionModel());
  QItemSelection macellule(item1, item1);
  selectionModel->select(macellule, QItemSelectionModel::Select);
 }

}

int MainWindow::TST_TotBidDansGroupememnt(int bId, QString &st_grp)
{
 int ret_val = 0;
 bool status = false;
 QSqlQuery sql_1(db_0);

 QString msg_2 = "select count (*) from (" +st_grp+ ") where (" +
                 "b1 = " +QString::number(bId) + " or " +
                 "b2 = " +QString::number(bId) + " or " +
                 "b3 = " +QString::number(bId) + " or " +
                 "b4 = " +QString::number(bId) + " or " +
                 "b5 = " +QString::number(bId) + " );" ;

 status = sql_1.exec(msg_2);
 if(status)
 {
  sql_1.first();
  if(sql_1.isValid())
  {
   ret_val = sql_1.value(0).toInt();
  }
 }

 return ret_val;
}

// http://www.geeksforgeeks.org/write-a-c-program-to-print-all-permutations-of-a-given-string/
// http://www.cut-the-knot.org/do_you_know/AllPerm.shtml
// http://msdn.microsoft.com/fr-fr/magazine/cc163513.aspx
// http://www.dcode.fr/generer-permutations
void MainWindow::TST_Permute(QStringList  *lst)
{
 QStringList ret_val;

 ret_val.clear();

 //TST_PrivPermute(lst,0,lst->size()-1, &ret_val);
 TST_PrivPermute_2(lst,lst->size(),&ret_val);

 QString tmp = ret_val.at(1);

}

#if 0
void MainWindow::TST_PrivPermute(QStringList *a, int i, int n, QStringList  *ret)
{
 QString lgn;

 if (i == n){
  *ret<< a->join(",");//printf("%s\n", a);
 }
 else
 {
  for (int j = i; j <= n; j++)
  {
   a->swap(i,j);//swap((a+i), (a+j));
   TST_PrivPermute(a, i+1, n, ret);
   a->swap(i,j);//swap((a+i), (a+j)); //backtrack
  }

 }
}
#endif


// http://www.cut-the-knot.org/do_you_know/AllPerm.shtml
void MainWindow::TST_PrivPermute_2(QStringList  *item, int n, QStringList  *ret)
{
 if (n == 1)
  *ret<< item->join(",");
 else {
  for (int i = 0; i < n; i++)
  {
   TST_PrivPermute_2(item, n-1, ret);
   if (n % 2 == 1)  // if n is odd
    item->swap(0, n-1);
   else            // if n is even
    item->swap(i, n-1);
  }
 }
}


void MainWindow::TST_CombiVersTable (QStringList &combi,stTiragesDef *ref)
{
 bool status = false;
 static int loop = 0;
 int zn = 0;
 int nbBoules = floor(ref->limites[zn].max/10);

 QSqlQuery sql_1(db_0);
 QString st_cols = "";
 QString st_vals = "";
 QString st_valtips = "";
 QString msg_1 = " CREATE table if not exists lstcombi (id INTEGER PRIMARY KEY, pos int, comb int,rot int, b1 int, b2 int ,b3 int ,b4 int, b5 int, b6 int, poids real, tip text);";


 int coef[5][2][5] = {
  {{5,0,0,0,0}},
  {{4,1,0,0,0},{3,2,0,0,0}},
  {{3,1,1,0,0},{2,2,1,0,0}},
  {{2,1,1,1,0}},
  {{1,1,1,1,1}}
 };


 status = sql_1.exec(msg_1);
 if(status)
 {
  // Parcourir chaque element du tableau
  for (int i = 0; i< combi.size();i++)
  {
   QStringList item = combi.at(i).split(",");
   int nbitems = item.size();

   st_cols = "";
   st_vals = "";
   status = true;
   int valtip[6]={0,0,0,0,0,0};
   st_valtips = "";

   // Rotation circulaire ?
   if(nbitems> 1 && nbitems <= nbBoules)
   {
    for(int sub=0;sub<2;sub++)
    {
     if(coef[loop][sub][0])
     {
      for(int j=0;(j<nbitems)&& (status == true);j++)
      {
       st_cols ="";
       st_vals ="";
       for(int k =0; k< nbitems;k++)
       {
        int indice = item.at(k).toInt();
        int value = coef[loop][sub][(j+k)%nbitems];

        st_cols = st_cols
                  +"b"+QString::number(indice)
                  +",";
        st_vals = st_vals
                  + QString::number(value)
                  +",";
        valtip[indice-1]= value;

       }
       st_cols.remove(st_cols.length()-1,1);
       st_vals.remove(st_vals.length()-1,1);

       // Creation du texte unite/dizaine/v/t/c
       st_valtips="";
       for(int k=0; k<= nbBoules; k++)
       {
        st_valtips = st_valtips + QString::number(valtip[k])
                     + "/";
       }
       st_valtips.remove(st_valtips.length()-1,1);

       msg_1 = "insert into lstcombi (id,pos,comb,rot,"
               + st_cols + ",tip) Values (NULL,"
               + QString::number(loop)+","
               + QString::number(i)+","
               + QString::number(j) +","
               + st_vals + ",\""
               + st_valtips + "\");";
       status = sql_1.exec(msg_1);
      }
     }
    }
   }
   else
   {
    //static bool OneShot = true;

    for(int j=0;j<nbitems;j++)
    {
     st_cols = st_cols + "b"+item.at(j)+",";
     st_vals = st_vals +QString::number(coef[loop][0][j])+",";
     valtip[item.at(j).toInt()-1]= coef[loop][0][j];
    }
    st_cols.remove(st_cols.length()-1,1);
    st_vals.remove(st_vals.length()-1,1);

    // Creation du texte unite/dizaine/v/t/c
    st_valtips="";
    for(int k=0; k<= nbBoules; k++)
    {
     st_valtips = st_valtips + QString::number(valtip[k])
                  + "/";
    }
    st_valtips.remove(st_valtips.length()-1,1);

    msg_1 = "insert into lstcombi (id,pos,comb,rot,"
            + st_cols + ",tip) Values (NULL,"
            + QString::number(loop)+","
            + QString::number(i)+",0,"
            + st_vals + ",\""
            + st_valtips + "\");";

    status = sql_1.exec(msg_1);

   }
  }

  // Localisation de la boucle du dessus
  loop++;
 }

}

void MainWindow::TST_PonderationCombi(int delta)
{
 bool status = false;
 QSqlQuery sql_1(db_0);
 QSqlQuery sql_2(db_0);
 QString msg_1 = "select pos, count(pos)as T from lstcombi group by pos;";


 status = sql_1.exec(msg_1);
 if(status)
 {
  sql_1.first();
  if(sql_1.isValid())
  {
   int depart = 1;
   double palier = delta;
   int loop = 0;
   do
   {
    // Recuperer le nb de ligne
    int nblgn = sql_1.value(1).toInt();
    double val = palier;

    for(int i = depart; (i<(depart+nblgn)) && (status == true);i++)
    {
     msg_1 = "update lstcombi set poids="
             +QString::number(val)+" where (id="
             +QString::number(i)
             +");";
     status = sql_2.exec(msg_1);
     val += 0.2;
    }

    // Prendre la valeur entiere immediatement superieur
    val -=0.2;
    val = ceil(val);

    // Faire un saut
    palier = val +1;
    depart = depart+nblgn;

    loop++;
   }while(sql_1.next());
  }

 }
}

void MainWindow::TST_AffectePoidsATirage(stTiragesDef *ref)
{
 bool status = false;
 QSqlQuery sql_1(db_0);
 QSqlQuery sql_2(db_0);
 QString tb_ana_zn = "Ref_ana_z1";

 QString msg_1 = "select * from lstcombi;";

 int zn = 0;
 int nbBoules = floor(ref->limites[zn].max/10);

 status = sql_1.exec(msg_1);
 if(status)
 {
  sql_1.first();
  if(sql_1.isValid())
  {
   //int lastcol = sql_1.record().count();


   do{
    int coef[6]={0};
    QString msg_2 = "";
    int fk_idCombi_z1 = sql_1.value(0).toInt();

    for(int i = 0; i<= nbBoules;i++)
    {
     coef[i] = sql_1.value(4+i).toInt();
     msg_2 = msg_2 + "bd"+QString::number(i)
             +"="+QString::number(coef[i])+ " and ";
    }

    // creation d'une requete mise a jour des poids
    //double poids = sql_1.value(lastcol-1).toDouble();
#if 0
    update "+tb_ana_zn+" set fk_idCombi_z1=14 where(id in
                                                    (select id from "+tb_ana_zn+" where (bd0=1 and bd1=1 and bd2=2 and bd3=1 and bd4=0 and bd5=0)
                                                     ));
#endif
    msg_2.remove(msg_2.length()-5,5);
    msg_2 = "Update "+tb_ana_zn+" set fk_idCombi_z1="
            +QString::number(fk_idCombi_z1)
            +" where(id in (select id from "+tb_ana_zn+" where("
            +msg_2+")"
            +"));";

    status = sql_2.exec(msg_2);

   }while(sql_1.next()&& status);
  }
 }

}

void MainWindow::TST_MettrePonderationSurTirages(void)
{
#if 0
 select "+tb_ana_zn+".id, lstcombi.poids from "+tb_ana_zn+" inner join lstcombi on "+tb_ana_zn+".fk_idCombi_z1 = lstcombi.id;
#endif
 bool status = false;
 QSqlQuery sql_1(db_0);
 QString tb_ana_zn = "Ref_ana_z1";
 QString msg_1 = "select "+tb_ana_zn+".id, lstcombi.poids from "+tb_ana_zn+" inner join lstcombi on "+tb_ana_zn+".fk_idCombi_z1 = lstcombi.id;";

 QFile fichier("ponder.txt");
 fichier.open(QIODevice::WriteOnly | QIODevice::Text);
 QTextStream flux(&fichier);

 status = sql_1.exec(msg_1);
 if(status)
 {
  sql_1.last();
  if(sql_1.isValid())
  {
   do{
    //int tirage = sql_1.value(0).toInt();
    QString ponder = sql_1.value(1).toString();
    // Ecriture dans fichier texte
    flux << sql_1.value(0).toString() << ":" << ponder.replace(".",",")<< endl;

   }while(sql_1.previous());
   fichier.close();
  }
 }

}

void MainWindow::FEN_Graphe(stTiragesDef *pConf)
{
 // Essai de mise en onglet des  graphiques
 QTabWidget *tabWidget = new QTabWidget;
 UnConteneurDessin *dessin;


 //tabWidget->set
 dessin = TST_Graphe_1(pConf);
 tabWidget->addTab(dessin,"Tirages");

 dessin = TST_Graphe_2(pConf);
 tabWidget->addTab(dessin,"Parites");

 dessin = TST_Graphe_3(pConf);
 tabWidget->addTab(dessin,"b<N/2");

 tabWidget->setVisible(true);

 QMdiSubWindow *subWindow = zoneCentrale->addSubWindow(tabWidget);
 subWindow->resize(350,280);
 subWindow->move(981,571);
 tabWidget->show();
}

UnConteneurDessin * MainWindow::TST_Graphe_1(stTiragesDef *pConf)
{
 une_vue[0] = new UnConteneurDessin;
 QString msg_2="";
 QString msg_3 = "";
 QString tb_ana_zn = "Ref_ana_z1";

 myview[0] = new MyGraphicsView(pConf->db_cnx, eRepartition,une_vue[0], "Tirages",Qt::white);

 //msg_2="select "+tb_ana_zn+".id, lstcombi.poids, lstcombi.pos from "+tb_ana_zn+" inner join lstcombi on "+tb_ana_zn+".fk_idCombi_z1 = lstcombi.id;";
 msg_2="select "+tb_ana_zn+".id, lstcombi.poids from "+tb_ana_zn+" inner join lstcombi on "+tb_ana_zn+".fk_idCombi_z1 = lstcombi.id;";
 myview[0]->DessineCourbeSql(msg_2,pConf->choixJeu,Qt::red,2,20);

 // select tirages.id, comb_e.poids from tirages inner join comb_e on comb_e.e1=tirages.e1 and comb_e.e2 = tirages.e2
 msg_2 = "select tirages.id, comb_e.poids from tirages inner join comb_e on ";

 // Courbes des etoiles
 int zn=1;
 for(int i =0; i<pConf->nbElmZone[zn];i++)
 {
  msg_3 = msg_3 + TB_COMBI "_" + pConf->nomZone[zn] + "." + pConf->nomZone[zn] + QString::number(i+1)
          +"=" + TB_BASE + "." + pConf->nomZone[zn] + QString::number(i+1) + " and ";
 }
 msg_3.remove(msg_3.length()-5,5);
 msg_2 = msg_2 + msg_3 + ";";
 myview[0]->DessineCourbeSql(msg_2,pConf->choixJeu,Qt::blue);

 //zoneCentrale->addSubWindow(une_vue[0]);
 //une_vue[0]->setVisible(true);
 //une_vue[0]->show();
 return (une_vue[0]);
}

UnConteneurDessin * MainWindow::TST_Graphe_2(stTiragesDef *pConf)
{
 //UnConteneurDessin *
 une_vue[1] = new UnConteneurDessin;
 QString msg_2="";
 QString msg_3 = "";

 myview[1] = new MyGraphicsView(pConf->db_cnx,eParite,une_vue[1], "Parites",Qt::gray );//QColor(200,170,100,140));
 msg_2 = "select tirages.id, tirages.bp from tirages";
 myview[1]->DessineCourbeSql(msg_2,pConf->choixJeu,Qt::red,1,20);


 msg_2 = "select tirages.id, tirages.ep from tirages";
 myview[1]->DessineCourbeSql(msg_2,pConf->choixJeu,Qt::blue,1,20,150);

 //une_vue[1]->show();
 //zoneCentrale->addSubWindow(une_vue[1]);
 //une_vue[1]->setVisible(true);
 return (une_vue[1]);


}

UnConteneurDessin * MainWindow::TST_Graphe_3(stTiragesDef *pConf)
{
 //UnConteneurDessin *
 une_vue[2] = new UnConteneurDessin;
 QString msg_2="";
 QString msg_3 = "";

 myview[2] = new MyGraphicsView(pConf->db_cnx,eGroupe,une_vue[2], "b<N/2", Qt::lightGray );// QColor(230,200,130,170));
 msg_2 = "select tirages.id, tirages.bg from tirages";
 myview[2]->DessineCourbeSql(msg_2,pConf->choixJeu,Qt::red,1,20);


 msg_2 = "select tirages.id, tirages.eg from tirages";
 myview[2]->DessineCourbeSql(msg_2,pConf->choixJeu,Qt::blue,1,20,150);

 //une_vue[2]->show();
 //zoneCentrale->addSubWindow(une_vue[2]);
 //une_vue[2]->setVisible(true);
 return (une_vue[2]);
}

void MainWindow::TST_PrevisionNew(stTiragesDef *pConf)
{
 QSqlQuery query(db_0);
 QString msg1 = "";
 bool ret = false;

 int zone = 0;
 int maxBoules = pConf->limites[zone].max;

 msg1 =  "create table MaPrevision "
         "(id INTEGER PRIMARY KEY, b1 int, b2 int, b3 int, b4 int, b5 int, e1 int, e2 int);";
 ret = query.exec(msg1);

 msg1 = "insert into MaPrevision (id) "
        "select id from oaze where id <="+QString::number(maxBoules)+";";
 ret = query.exec(msg1);

 // Mettre la table a Zero
 msg1 = "update MaPrevision "
        "set b1=0,b2=0,b3=0,b4=0,b5=0,e1=0,e2=0 "
        "where MaPrevision.id<="+QString::number(maxBoules)+";";
 ret = query.exec(msg1);

 // Ratacher la table a la Qview dans le bon onglet
 QSqlTableModel *tblModel = new QSqlTableModel;
 tblModel->setTable("MaPrevision");
 tblModel->select();
 // Associer toutes les valeurs a la vue
 while (tblModel->canFetchMore())
 {
  tblModel->fetchMore();
 }
 //tblModel->removeColumn(tblModel->columnCount()-1); // e2
 //tblModel->removeColumn(tblModel->columnCount()-2); // e1
 tblModel->removeColumns(6,2);

 // Attach it to the view
 G_tbv_TabPrevision->setModel(tblModel);
 G_tbv_TabPrevision->setSortingEnabled(true);
 G_tbv_TabPrevision->sortByColumn(0,Qt::AscendingOrder);

 for(int i=0;i<tblModel->columnCount();i++)
  G_tbv_TabPrevision->setColumnWidth(i,30);

 // selectionner les boules du dernier tirage
 msg1 = "select t1.id from "
        "(select id from oaze where id <= "
        +QString::number(maxBoules)+
        ") as t1 "
        "inner join "
        "(select tirages.* from tirages limit 1) as t2 "
        "on "
        "("
        "t1.id = t2.b1 or "
        "t1.id = t2.b2 or "
        "t1.id = t2.b3 or "
        "t1.id = t2.b4 or "
        "t1.id = t2.b5 "
        ");";
 ret = query.exec(msg1);

 if(ret)
 {
  query.first();
  if(query.isValid())
  {
   int posBoule=0;
   do{

    posBoule++;
    // Prendre chacune des boules
    int uneBoule = query.value(0).toInt();
    tblModel->setHeaderData(posBoule,Qt::Horizontal,"b"+QString::number(uneBoule));


    // Chercher les 10 premiers tirages ayant cette boule
    // Cela permettra de calculer des distances de references
    QSqlQuery req_2(db_0);
    bool ret2 = false;

    msg1 = "select tirages.id from tirages "
           "where "
           "("
           "tirages.b1 = "
           +QString::number(uneBoule)+
           " or "
           "tirages.b2 = "
           +QString::number(uneBoule)+
           " or "
           "tirages.b3 = "
           +QString::number(uneBoule)+
           " or "
           "tirages.b4 = "
           +QString::number(uneBoule)+
           " or "
           "tirages.b5 = "
           +QString::number(uneBoule)
           +" "
            ") order by tirages.id limit 2;";

    ret2 = req_2.exec(msg1);
    if(ret2)
    {
     req_2.first();
     if(req_2.isValid())
     {
      do{
       int val_id = req_2.value(0).toInt();

       // Pour chaque distance calculer les occurances
       // des boules manquantes
       ret2 = TST_MettreLesTotaux(posBoule,uneBoule,val_id);
      }while(req_2.next() && ret2);
     }
     req_2.finish();
    }
   }while(query.next() && ret);
   // Mise a jour de la recherche
   tblModel->select();
  }
 }
}

bool MainWindow::TST_MettreLesTotaux(int idBoule, int vBoule, int dBoule)
{
 bool status = false;
 QSqlQuery query(db_0);
 QString msg1 = "";

 msg1 = "select MaPrevision.id,MaPrevision.b1, t2.T1, (MaPrevision.b1+ t2.T1) as Somme "
        "From MaPrevision "
        "inner join "
        "("
        "select t3.boule as B1, table_2.T as T1 "
        "from (select boule from oazb)as t3 "
        "left join "
        "("
        "select boule as B, count (boule) as T "
        "from (select boule from oazb )as t1 "
        "left join "
        "("
        "select * from "
        "("
        "select id as id1 from tirages "
        "where ( "
        "tirages.b1= "
        +QString::number(vBoule)+
        " or "
        "tirages.b2= "
        +QString::number(vBoule)+
        " or "
        "tirages.b3= "
        +QString::number(vBoule)
        +" or "
         "tirages.b4= "
        +QString::number(vBoule)
        +" or "
         "tirages.b5= "
        +QString::number(vBoule)
        +" "
         ")) as tabl4 "
         "left join tirages "
         "on tabl4.id1=tirages.id + "
        +QString::number(dBoule)
        +") as t2 "
         "on (( "
         "t1.boule = t2.b1 or "
         "t1.boule = t2.b2 or "
         "t1.boule = t2.b3 or "
         "t1.boule = t2.b4 or "
         "t1.boule = t2.b5 "
         ")) group by boule) as table_2 "
         "on (B1=table_2.B) group by B1 "
         ")as t2  on(t2.B1 = MaPrevision.id) order by Maprevision.id;";

#ifndef QT_NO_DEBUG
 qDebug() << msg1;
#endif

 status = query.exec(msg1);

 if(status)
 {
  query.first();
  if(query.isValid())
  {
   do{
    QSqlQuery req_2(db_0);
    int id=query.value(0).toInt();
    //int bt=query.value(1).toInt(); // val dans table de total
    //int rt=query.value(2).toInt(); // Val de nouvelle recherche
    int nt=query.value(3).toInt(); // val a mettre dans a table

    msg1 = "update MaPrevision set b"+QString::number(idBoule)+"="
           +QString::number(nt)
           + " "
             "where MaPrevision.id="+QString::number(id)+";";
    status = req_2.exec(msg1);
   }while (query.next() && status);
  }
 }
 return status;
}

//----------
void MainWindow::TST_PrevisionType(NE_FDJ::E_typeCritere cri_type, stTiragesDef *pConf)
{
 QStandardItemModel **modele;
 QLabel **label;
 QSqlQuery query(db_0);
 bool status;

 int cri_val = 0;

 //QStringList lst_boule;
 const int d[2]={-1,-2};
 const int col[2]={1,2};
 //const QColor fond[3]={QColor(255,156,86,167),QColor(140,255,124,167),QColor(10,255,250,167)};

 // Recuperer le dernier tirage
 QString msg = "";
 QString cri_col = "";

 for(int zone=0;zone<pConf->nb_zone;zone++)
 {
  status = false;
  msg = "select ";
  cri_col = "";

  switch (cri_type)
  {
   case NE_FDJ::critere_parite:
   {
    modele = gsim_DernierTirageDetail;
    label = G_lab_PariteVoisin;
    cri_col = pConf->nomZone[zone]+ CL_PAIR;
   }
    break;

   case NE_FDJ::critere_enemble:
   default:
   {
    cri_col = pConf->nomZone[zone]+ CL_SGRP;
    modele = G_sim_Nsur2;
    label = G_lab_Nsur2;
    cri_col = pConf->nomZone[zone]+ CL_SGRP;
   }
    break;
  }

  msg = msg + cri_col +" from " + TB_BASE + " limit 1;";
  status = query.exec(msg);
  if(status)
  {
   query.first();
   if(query.isValid())
   {
    cri_val = query.value(0).toInt();
    QString Repere = "Tirages ref " +cri_col+ "=" + QString::number(cri_val);
    label[zone]->setText(Repere);
    // recherche des voisins pour ce critere
    int v = sizeof(d)/sizeof(int);
    for(int j=0;j<v;j++)
    {
     for(int cible= 0; cible < pConf->nbElmZone[zone]+1;cible ++)
     {
      // Recuperer pointeur de cellule
      QStandardItem *item1 = modele[zone]->item(cible,col[j]);

      int total= 0;
      total = DB_tirages->TST_TotalRechercheADistance_F2(d[j],cri_col,cri_val,cible);

      // Mettre la valeur trouvee dans le tableau
      item1->setData(total,Qt::DisplayRole);
      modele[zone]->setItem(cible,col[j],item1);

     }
    }
   }
  }
 }

}
//----------

void MainWindow::VUE_MontreLeTirage(double x)
{
 QSqlQuery query(db_0);
 QString st_msg[3] = {""};
 bool status = false;
 int scale[3]={20,20,20};
 int delta[3]={0,0,0};
 QString tb_ana_zn = "Ref_ana_z1";

 st_msg[0] = "select pos from lstcombi inner  join "
             "(select fk_idCombi_z1 as clef from "+tb_ana_zn+" where id = "
             +QString::number(x)+
             ") as t2 on t2.clef = lstcombi.id ;";

 st_msg[1] = "select tirages.bp from tirages where tirages.id = "
             +QString::number(x)+ ";";

 st_msg[2] = "select tirages.bg from tirages where tirages.id = "
             +QString::number(x)+ ";";

 // Recuperer itemGraphique par rapport a x et y
 for(int graph=0; graph< 3 ; graph ++)
 {
#ifndef QT_NO_DEBUG
  qDebug() << st_msg[graph];
#endif

  status = query.exec(st_msg[graph]);
  if(status)
  {
   query.first();
   if(query.isValid())
   {
    double y = query.value(0).toInt();
    int pos_x = x * C_COEF_X;
    QGraphicsScene *lavue = une_vue[graph]->scene();
    PointTirage *un_test = new PointTirage(db_0.connectionName() ,eFdjLoto,eTirage);


    double pos_y = myview[graph]->height();
    pos_y = pos_y - (y * C_COEF_Y * scale[graph]) - delta[graph];

    un_test->setPos(pos_x,pos_y);

    une_vue[graph]->centerOn(un_test);
    une_vue[graph]->ensureVisible(un_test);
    une_vue[graph]->fitInView(un_test,Qt::KeepAspectRatio);
    lavue->setFocusItem(un_test);

   }
  }
 }
}


