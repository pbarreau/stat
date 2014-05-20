#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsView>

#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlRecord>

#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QLineEdit>
#include <QFormLayout>
#include <QLabel>
#include <QStandardItemModel>
#include <QTableWidget>
#include <QModelIndex>
#include <QBrush>
#include <QTabWidget>
#include <math.h>

#include "MyGraphicsView.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "choixjeux.h"
#include "tirages.h"
#include "gererbase.h"

static stTiragesDef configJeu;

MainWindow::MainWindow(QWidget *parent,NE_FDJ::E_typeJeux leJeu, bool load, bool dest_bdd) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  DB_tirages = new GererBase;
  int i;
  tirages tmp(leJeu);
  QString ficSource;
  QTableView *qtv_MesChoix = new QTableView;

  // Recuperation des contantes du type de jeu
  tmp.getConfig(&configJeu);

  ui->setupUi(this);

  zoneCentrale = new QMdiArea;
  zoneCentrale->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  zoneCentrale->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

#if 0
  /////////////////////
  QStringList TST_Lst;
  TST_Lst << "A" << "B" << "C" << "D" << "E";
  //TST_Lst << "A" << "B" << "C" << "D" << "E";
  TST_Permute(&TST_Lst);
  ////////////////////
#endif

  // Creation sous fenetre pour mettre donnees de base
  fen_Tirages();

  fen_LstCouv();

  // Creation sous fenetre des voisins
  fen_Voisins();

  // Creation sous fenetre des ecarts
  fen_Ecarts();

  // Creation fenetre pour memoriser a selection
  fen_MaSelection(qtv_MesChoix);

  // Creation fenetre resultat
  fen_MesPossibles();

  // Creation fenetre pour parite
  fen_Parites();

  // Preparer la base de données
  DB_tirages->CreerBaseEnMemoire(dest_bdd,leJeu);


  // Creation des tables pour ce type jeu
  DB_tirages->CreerTableTirages(&tmp);

  // Recuperation des données fdj
  ficSource = tmp.SelectSource(load);
  DB_tirages->LireLesTirages(ficSource,&tmp);

  if(leJeu == NE_FDJ::fdj_euro){
    // Lecture des anciennes base des tirages
    ficSource = "euromillions_2.csv";
    DB_tirages->LireLesTirages(ficSource,&tmp);
    ficSource="euromillions.csv";
    DB_tirages->LireLesTirages(ficSource,&tmp);
  }
  else
  {
    //Rien
  }
  ///---------------------
  // Recherche de combinaison A deplacer ?
  //DB_tirages->RechercheCombinaison(&configJeu,tabWidget,zoneCentrale);
  TST_RechercheCombi(&configJeu,tabWidget);
  //QApplication::quit();

  //return;

  // Ordre arrivee des boules ?
  DB_tirages->CouvertureBase(qsim_Ecarts,&configJeu);

  //// GARDER L'ORDRE D'APPEL DES FONCTIONS PB VERROU SUR LA BASE
  for(i=1;i<=configJeu.limites->max;i++){
    // Remplir Sous Fen les ecarts
    DB_tirages->DistributionSortieDeBoule(i,qsim_Ecarts,&configJeu);
    // Montrer les valeurs probable
    DB_tirages->CouvMontrerProbable(i,3,1,qsim_Ecarts);


    // Calcul occurence de cette boule
    DB_tirages->TotalApparitionBoule(i,qsim_Voisins);
  }

  // Remplir la sousfenetre base de données
  DB_tirages->AfficherBase(qw_Tirages,qtv_Tirages);


  // Remplir la sous fenetre resultat couverture
  DB_tirages->AfficherResultatCouverture(qw_LstCouv,qtv_LstCouv);


  // Remplir la sous fenetre de parite
  DB_tirages->MLP_DansLaQtTabView(&configJeu, CL_PAIR, qsim_Parites);
  DB_tirages->MLP_DansLaQtTabView(&configJeu, CL_SGRP, qsim_Ensemble_1);
  DB_tirages->MLP_UniteDizaine(&configJeu, qsim_ud);

  setCentralWidget(zoneCentrale);
  TST_Graphe(zoneCentrale);


  // click dans fenetre ma selection
  connect( qtv_MesChoix, SIGNAL( doubleClicked(QModelIndex)) ,
           this, SLOT( slot_UneSelectionActivee( QModelIndex) ) );
  ///-----------------
#if 0
  // Selection a change
  connect(qtv_LstCouv->selectionModel(), SIGNAL(selectionChanged (const QItemSelection&, const QItemSelection&)),
          this, SLOT(slot_CouvertureSelChanged(QItemSelection,QItemSelection)));

#endif

}

void MainWindow::fen_Tirages(void)
{
  qw_Tirages = new QWidget;
  qtv_Tirages = new QTableView;
  QFormLayout *layout = new QFormLayout;


  // Gestion du QTableView
  qtv_Tirages->setSelectionMode(QAbstractItemView::SingleSelection);
  qtv_Tirages->setSelectionBehavior(QAbstractItemView::SelectItems);
  qtv_Tirages->setStyleSheet("QTableView {selection-background-color: red;}");
  qtv_Tirages->setEditTriggers(QAbstractItemView::NoEditTriggers);
  qtv_Tirages->setAlternatingRowColors(true);
  //qtv_Tirages->setMinimumHeight(800);
  qtv_Tirages->setMinimumWidth(460);

  layout->addWidget(qtv_Tirages);
  qw_Tirages->setLayout(layout);
  qw_Tirages->setWindowTitle("Base des tirages");
  qw_Tirages->setMinimumHeight(450);

  // Ratacher cette sous fenetre
  zoneCentrale->addSubWindow(qw_Tirages);
  //QMdiSubWindow *sousFenetre1 = zoneCentrale->addSubWindow(AfficherBase);

  // Double click dans sous fenetre base
  connect( qtv_Tirages, SIGNAL( doubleClicked(QModelIndex)) ,
           this, SLOT( slot_ChercheVoisins( QModelIndex) ) );


}

void MainWindow::fen_LstCouv(void)
{
  qw_LstCouv = new QWidget;
  qtv_LstCouv = new QTableView;
  QFormLayout *layout = new QFormLayout;
  //qtv_LstCouv->itemDelegate(new DelegationDeCouleur(qtv_LstCouv));

  layout->addWidget(qtv_LstCouv);
  qw_LstCouv->setLayout(layout);
  qw_LstCouv->setWindowTitle("Couvertures des tirages");


  // Gestion du QTableView
  qtv_LstCouv->setSelectionMode(QAbstractItemView::MultiSelection);
  qtv_LstCouv->setSelectionBehavior(QAbstractItemView::SelectItems);
  qtv_LstCouv->setStyleSheet("QTableView {selection-background-color: red;}");
  qtv_LstCouv->setEditTriggers(QAbstractItemView::NoEditTriggers);
  qtv_LstCouv->setAlternatingRowColors(true);

  // Ratacher cette sous fenetre
  zoneCentrale->addSubWindow(qw_LstCouv);

#if 0
  // Selection a change
  connect(qtv_LstCouv->selectionModel(), SIGNAL(selectionChanged (const QItemSelection&, const QItemSelection&)),
          this, SLOT(slot_CouvertureSelChanged(QItemSelection,QItemSelection)));

#endif

}
void MainWindow::fen_Voisins(void)
{
  int  i;
  QWidget *qw_Voisins = new QWidget;
  qtv_Voisins = new QTableView;
  int zn = 0;
  qsim_Voisins = new QStandardItemModel(configJeu.limites[zn].max,7);

  // entete du modele
  qsim_Voisins->setHeaderData(0,Qt::Horizontal,"B");
  qsim_Voisins->setHeaderData(1,Qt::Horizontal,"V:r0");
  qsim_Voisins->setHeaderData(2,Qt::Horizontal,"V:+1");
  qsim_Voisins->setHeaderData(3,Qt::Horizontal,"V:+2");
  qsim_Voisins->setHeaderData(4,Qt::Horizontal,"V:-1");
  qsim_Voisins->setHeaderData(5,Qt::Horizontal,"V:-2");
  qsim_Voisins->setHeaderData(6,Qt::Horizontal,"NbS");

  // Ecriture du numero de boule et reservation item position
  for(i=1;i<=configJeu.limites[zn].max;i++)
  {
    QStandardItem *item = new QStandardItem();
    item->setData(i,Qt::DisplayRole);
    qsim_Voisins->setItem(i-1,0,item);
    for (int j =1; j<7;j++)
    {
      QStandardItem *item_2 = new QStandardItem();
      qsim_Voisins->setItem(i-1,j,item_2);
    }
  }

  qtv_Voisins->setModel(qsim_Voisins);
  qtv_Voisins->setColumnWidth(0,60);
  qtv_Voisins->setColumnWidth(1,60);
  qtv_Voisins->setColumnWidth(2,60);
  qtv_Voisins->setColumnWidth(3,60);
  qtv_Voisins->setColumnWidth(4,60);
  qtv_Voisins->setColumnWidth(5,60);
  qtv_Voisins->setColumnWidth(6,60);

  //tblVoisin->setMaximumWidth(260);
  qtv_Voisins->setMinimumHeight(390);
  qtv_Voisins->setSortingEnabled(true);
  qtv_Voisins->sortByColumn(0,Qt::AscendingOrder);
  qtv_Voisins->setAlternatingRowColors(true);
  qtv_Voisins->setEditTriggers(QAbstractItemView::NoEditTriggers);

  nbSortie = new QLabel;
  nbSortie->setText("Nb total de sorties:");

  QFormLayout *layVoisin = new QFormLayout;
  layVoisin->addWidget(nbSortie);
  layVoisin->addWidget(qtv_Voisins);

  qw_Voisins->setMinimumHeight(435);
  qw_Voisins->setMinimumWidth(500);

  qw_Voisins->setLayout(layVoisin);
  qw_Voisins->setWindowTitle("Voisins de selection");
  //QMdiSubWindow *sousFenetre2 =
  zoneCentrale->addSubWindow(qw_Voisins);

  // click dans fenetre voisin pour afficher boule
  connect( qtv_Voisins, SIGNAL( clicked(QModelIndex)) ,
           this, SLOT( slot_MontrerBouleDansBase( QModelIndex) ) );


}

void MainWindow::fen_Ecarts(void)
{
  int  i;
  QWidget *qw_Ecarts = new QWidget;
  qtv_Ecarts = new QTableView;
  int zn = 0;
  qsim_Ecarts = new QStandardItemModel(configJeu.limites[zn].max,5);
  //modele2 = GererBase::modele2_0;

  qsim_Ecarts->setHeaderData(0,Qt::Horizontal,"B"); // Boules
  qsim_Ecarts->setHeaderData(1,Qt::Horizontal,"Ec"); // Ecart en cours
  qsim_Ecarts->setHeaderData(2,Qt::Horizontal,"Ep"); // ECart precedent
  qsim_Ecarts->setHeaderData(3,Qt::Horizontal,"Em"); // Ecart Moyen
  qsim_Ecarts->setHeaderData(4,Qt::Horizontal,"EM"); // Ecart Maxi

  for(i=1;i<=configJeu.limites[zn].max;i++)
  {
    QStandardItem *item = new QStandardItem( QString::number(i));
    item->setData(i,Qt::DisplayRole);
    qsim_Ecarts->setItem(i-1,0,item);
  }

  qtv_Ecarts->setModel(qsim_Ecarts);
  qtv_Ecarts->setColumnWidth(0,45);
  qtv_Ecarts->setColumnWidth(1,45);
  qtv_Ecarts->setColumnWidth(2,45);
  qtv_Ecarts->setColumnWidth(3,45);
  qtv_Ecarts->setColumnWidth(4,45);
  qtv_Ecarts->setSortingEnabled(true);
  qtv_Ecarts->sortByColumn(0,Qt::AscendingOrder);
  qtv_Ecarts->setAlternatingRowColors(true);
  qtv_Ecarts->setEditTriggers(QAbstractItemView::NoEditTriggers);
  //tblCouverture->setMaximumWidth(500);
  qtv_Ecarts->setMinimumHeight(420);
  qtv_Ecarts->setMinimumWidth(280);

  QFormLayout *layCouverture = new QFormLayout;
  layCouverture->addWidget(qtv_Ecarts);


  qw_Ecarts->setMinimumHeight(450);
  qw_Ecarts->setLayout(layCouverture);
  qw_Ecarts->setWindowTitle("Couverture boules");
  //QMdiSubWindow *sousFenetre3 =
  zoneCentrale->addSubWindow(qw_Ecarts);

  // Double click dans sous fenetre ecart
  connect( qtv_Ecarts, SIGNAL( doubleClicked(QModelIndex)) ,
           this, SLOT( slot_qtvEcart( QModelIndex) ) );


}

void MainWindow::fen_MaSelection(QTableView *qtv_MaSelection)
{
  int  i=0,j=0, cell_val=0;
  QWidget *qw_MaSelection = new QWidget;
  int zn = 0;
  int nbcol = (configJeu.limites[zn].max)%configJeu.nbElmZone[zn]?
                (configJeu.limites[zn].max/configJeu.nbElmZone[zn])+1:
                (configJeu.limites[zn].max/configJeu.nbElmZone[zn]);
  qsim_MaSelection = new QStandardItemModel(configJeu.nbElmZone[zn],nbcol);

  for(i=1;i<=configJeu.nbElmZone[zn];i++)/// Code a verifier en fonction bornes max
  { // Dans le cas max > 50
    for(j=1;j<=nbcol;j++)
    {
      cell_val = j+(i-1)*nbcol;
      if(cell_val<=configJeu.limites[zn].max){
        QStandardItem *item = new QStandardItem( QString::number(i));
        item->setData(cell_val,Qt::DisplayRole);
        qsim_MaSelection->setItem(i-1,j-1,item);
      }
    }
  }

  qtv_MaSelection->setModel(qsim_MaSelection);
  qtv_MaSelection->setAlternatingRowColors(true);
  qtv_MaSelection->setEditTriggers(QAbstractItemView::NoEditTriggers);
  qtv_MaSelection->setMinimumHeight(190);
  //qtv_MaSelection->setMinimumWidth(325);
  QFormLayout *layCouverture = new QFormLayout;
  layCouverture->addWidget(qtv_MaSelection);

  for(j=0;j<10;j++)
  {
    qtv_MaSelection->setColumnWidth(j,30);
  }
  //qw_MaSelection->setMinimumHeight(200);
  qw_MaSelection->setFixedSize(350,210);


  qw_MaSelection->setLayout(layCouverture);
  qw_MaSelection->setWindowTitle("Ma Selection");
  //QMdiSubWindow *sousFenetre3 =
  zoneCentrale->addSubWindow(qw_MaSelection);

}

void MainWindow::fen_MesPossibles(void)
{
  QWidget *qw_MesPossibles = new QWidget;
  QHeaderView *horizontalHeader;
  QString Lib[6]={"tot","r0","+1","+2","-1","-2"};
  //QString Methode[6]={"ft1", "ft2", "ft3", "ft4", "ft5", "ft6"};
  qtv_MesPossibles = new QTableView;
  int zn = 0;

  qtv_MesPossibles->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(qtv_MesPossibles, SIGNAL(customContextMenuRequested(QPoint)),
          SLOT(customMenuRequested(QPoint)));

  horizontalHeader = qtv_MesPossibles->horizontalHeader();
  horizontalHeader->setContextMenuPolicy(Qt::CustomContextMenu); //set contextmenu
  connect(horizontalHeader, SIGNAL(customContextMenuRequested( const QPoint& )),
          this, SLOT(tablev_customContextMenu( const QPoint& )));

  menuTrieMesPossibles=new QMenu(this);
  MesTries = new QActionGroup (this);

  menuTrieMesPossibles->setWindowFlags(Qt::Tool);
  menuTrieMesPossibles->setWindowTitle("Trie");
  //menuTrieMesPossibles->add
  for(int i = 0; i< 6; i++){
    tabAction[i]= new QAction(Lib[i],this);
    tabAction[i]->setCheckable(true);
    tabAction[i]->setData(i);
    MesTries->addAction(tabAction[i]);
  }
  connect(tabAction[0], SIGNAL(triggered()), this, SLOT(ft1()));
  connect(tabAction[1], SIGNAL(triggered()), this, SLOT(ft2()));
  connect(tabAction[2], SIGNAL(triggered()), this, SLOT(ft3()));
  connect(tabAction[3], SIGNAL(triggered()), this, SLOT(ft4()));
  connect(tabAction[4], SIGNAL(triggered()), this, SLOT(ft5()));
  connect(tabAction[5], SIGNAL(triggered()), this, SLOT(ft6()));


  tabAction[0]->setChecked(true);
  for(int i = 0; i< 6; i++){
    menuTrieMesPossibles->addAction(tabAction[i]);
  }


  //qsim_MesPossibles = new MonToolTips(configJeu.limites[zn].max,5);
  qsim_MesPossibles = new QStandardItemModel(configJeu.limites[zn].max,5);

  qsim_MesPossibles->setHeaderData(0,Qt::Horizontal,"C1"); // Boules
  qsim_MesPossibles->setHeaderData(1,Qt::Horizontal,"C2"); // Ecart en cours
  qsim_MesPossibles->setHeaderData(2,Qt::Horizontal,"C3"); // ECart precedent
  qsim_MesPossibles->setHeaderData(3,Qt::Horizontal,"C4"); // Ecart Moyen
  qsim_MesPossibles->setHeaderData(4,Qt::Horizontal,"C5"); // Ecart Maxi

  qtv_MesPossibles->setModel(qsim_MesPossibles);
  qtv_MesPossibles->setColumnWidth(0,45);
  qtv_MesPossibles->setColumnWidth(1,45);
  qtv_MesPossibles->setColumnWidth(2,45);
  qtv_MesPossibles->setColumnWidth(3,45);
  qtv_MesPossibles->setColumnWidth(4,45);
  qtv_MesPossibles->setSortingEnabled(false);
  qtv_MesPossibles->sortByColumn(0,Qt::AscendingOrder);
  qtv_MesPossibles->setAlternatingRowColors(true);
  qtv_MesPossibles->setEditTriggers(QAbstractItemView::NoEditTriggers);
  //tblCouverture->setMaximumWidth(500);
  qtv_MesPossibles->setMinimumHeight(420);
  qtv_MesPossibles->setMinimumWidth(280);

  QFormLayout *layCouverture = new QFormLayout;
  layCouverture->addWidget(qtv_MesPossibles);


  qw_MesPossibles->setMinimumHeight(450);
  qw_MesPossibles->setLayout(layCouverture);
  qw_MesPossibles->setWindowTitle("Mes possibles");

  connect( qtv_MesPossibles, SIGNAL( clicked(QModelIndex)) ,
           this, SLOT( slot_MontrerBouleDansBase( QModelIndex) ) );

  //QMdiSubWindow *sousFenetre3 =
  zoneCentrale->addSubWindow(qw_MesPossibles);

}

void MainWindow::fen_Parites(void)
{
  QWidget *qw_Parites = new QWidget;
  //QTabWidget *tabWidget = new QTabWidget;
  tabWidget = new QTabWidget;



  int zn = 0;

  qsim_Parites = new QStandardItemModel(configJeu.nbElmZone[zn],configJeu.nb_zone+1);

  qsim_Parites->setHeaderData(0,Qt::Horizontal,"Nb");
  qsim_Parites->setHeaderData(1,Qt::Horizontal,"B");
  qsim_Parites->setHeaderData(2,Qt::Horizontal,"E");

  qtv_Parites = new QTableView;
  qtv_Parites->setModel(qsim_Parites);
  qtv_Parites->setColumnWidth(0,45);
  qtv_Parites->setColumnWidth(1,55);
  qtv_Parites->setColumnWidth(2,55);
  qtv_Parites->setSortingEnabled(false);
  qtv_Parites->sortByColumn(0,Qt::AscendingOrder);
  qtv_Parites->setAlternatingRowColors(true);
  qtv_Parites->setEditTriggers(QAbstractItemView::NoEditTriggers);
  qtv_Parites->setMinimumHeight(220);

  qsim_Ensemble_1 = new QStandardItemModel(configJeu.nbElmZone[zn],configJeu.nb_zone+1);

  qsim_Ensemble_1->setHeaderData(0,Qt::Horizontal,"N(E/2)");
  qsim_Ensemble_1->setHeaderData(1,Qt::Horizontal,"B");
  qsim_Ensemble_1->setHeaderData(2,Qt::Horizontal,"E");

  QTableView *qtv_E1 = new QTableView;
  qtv_E1->setModel(qsim_Ensemble_1);
  qtv_E1->setColumnWidth(0,60);
  qtv_E1->setColumnWidth(1,55);
  qtv_E1->setColumnWidth(2,55);
  qtv_E1->setSortingEnabled(false);
  qtv_E1->sortByColumn(0,Qt::AscendingOrder);
  qtv_E1->setAlternatingRowColors(true);
  qtv_E1->setEditTriggers(QAbstractItemView::NoEditTriggers);
  qtv_E1->setMinimumHeight(220);

  int v = (configJeu.limites[zn].max /10) +1;
  qsim_ud = new QStandardItemModel(v+1,v+1);
  qsim_ud->setHeaderData(0,Qt::Horizontal,"Nb");
  for(int j=0; j< v;j++)
  {
    QString name = configJeu.nomZone[zn] + "d" + QString::number(j);
    qsim_ud->setHeaderData(j+1,Qt::Horizontal,name);
  }

  // Ecriture du numero de boule et reservation item position
  for(int j=0; j<= v;j++)
  {
    QStandardItem *item = new QStandardItem();
    item->setData(j,Qt::DisplayRole);
    qsim_ud->setItem(j,0,item);
    for (int k =1; k<=v;k++)
    {
      QStandardItem *item_2 = new QStandardItem();
      qsim_ud->setItem(j,k,item_2);
    }
  }

  QTableView *qtv_E2 = new QTableView;
  qtv_E2->setModel(qsim_ud);
  for(int j=0; j< v+1;j++)
  {
    qtv_E2->setColumnWidth(j,50);
  }

  qtv_E2->setAlternatingRowColors(true);;
  qtv_E2->setSortingEnabled(false);
  qtv_E2->setEditTriggers(QAbstractItemView::NoEditTriggers);

  tabWidget->addTab(qtv_Parites,tr("Parites"));
  tabWidget->addTab(qtv_E1,tr("< N/2"));
  tabWidget->addTab(qtv_E2,tr("Unites"));


  QFormLayout *mainLayout = new QFormLayout;
  mainLayout->addWidget(tabWidget);
  qw_Parites->setWindowTitle("Etude des boules");
  qw_Parites->setLayout(mainLayout);

#if 0
  connect( qtv_Parites, SIGNAL( clicked(QModelIndex)) ,
           this, SLOT( slot_MontrerTirageDansBase( QModelIndex) ) );
#endif

  zoneCentrale->addSubWindow(qw_Parites);

}

MainWindow::~MainWindow()
{
  delete ui;
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

#if 0
void MainWindow::closeEvent(QCloseEvent *event)
{
  zoneCentrale->closeAllSubWindows();
  if (zoneCentrale->currentSubWindow()) {
    event->ignore();
  } else {
    //writeSettings();
    event->accept();
  }
}
#endif

void MainWindow::slot_ChercheVoisins(const QModelIndex & index)
{
  int val;

  if (index.column()==0)
  {
    // Recherche de toute les boules
    int i;

    for (i=1;i<6;i++)
    {
      val = index.model()->index(index.row(),i).data().toInt();
      DB_tirages->RechercheVoisin(val,&configJeu,nbSortie,qsim_Voisins);
    }

    // Affichage des resultats
    DB_tirages->MontreMesPossibles(index,&configJeu,qtv_MesPossibles);

    // Mise en evidence numero commun
    DB_tirages->MLB_MontreLesCommuns(&configJeu,qtv_MesPossibles);
  }

  if(index.column()>0 && index.column()<6){
    qtv_LstCouv->clearSelection();
    val = index.data().toInt();
    qtv_Voisins->sortByColumn(0,Qt::AscendingOrder);
    DB_tirages->RechercheVoisin(val,&configJeu,nbSortie,qsim_Voisins);
    DB_tirages->MontrerBouleCouverture(val,qtv_LstCouv);
  }

}

void MainWindow::slot_qtvEcart(const QModelIndex & index)
{
  int val = 0;

  if(index.column()==0){
    // Effacer precedente selection
    qtv_LstCouv->clearSelection();
    qtv_Tirages->clearSelection();

    val = index.data().toInt();
    qtv_Voisins->sortByColumn(0,Qt::AscendingOrder);
    DB_tirages->RechercheVoisin(val,&configJeu,nbSortie,qsim_Voisins);
  }
}

void MainWindow::slot_UneSelectionActivee(const QModelIndex & index)
{
  static QStringList select;

  QStandardItem *item1 = qsim_MaSelection->itemFromIndex(index);
#ifndef QT_NO_DEBUG
  qDebug()<< item1->background();
#endif

  if(item1->background() == Qt::red)
  {
    // un element existe le supprimer
    int pos = select.indexOf(item1->data(Qt::DisplayRole).toString());
    select.removeAt(pos);

    // Il y a deja une selection
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
    select << item1->data(Qt::DisplayRole).toString();
    item1->setBackground(QBrush(Qt::red));
  }

  qtv_Voisins->sortByColumn(0,Qt::AscendingOrder);
  DB_tirages->TST_RechercheVoisin(select,&configJeu,nbSortie,qsim_Voisins);

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

void MainWindow::slot_MontrerBouleDansBase(const QModelIndex & index)
{
  int val = 0;
  int cellule = 0;

  // determination de la fenetre ayant recu le click
  if (index.internalPointer() == qsim_Voisins->index(index.row(),index.column()).internalPointer()){
    val = qsim_Voisins->index(index.row(),0).data().toInt();
    cellule = qsim_Voisins->index(index.row(),index.column()).data().toInt();
  }
  else
  {
    val = qsim_MesPossibles->index(index.row(),index.column()).data().toInt();
    cellule = val;
    //qsim_MesPossibles->index(index.row(),index.column()).data(Qt::ToolTipRole);

    // determination du numero de boule grace a la colonne
    int b_id = BouleIdFromColId(index.column());

    // Affichage de la table des voisins
    DB_tirages->RechercheVoisin(b_id,&configJeu,nbSortie,qsim_Voisins);
    qsim_Voisins->sort(colonne_tri+1,Qt::AscendingOrder);
    qsim_Voisins->sort(colonne_tri+1,Qt::DescendingOrder);
  }
#ifndef QT_NO_DEBUG
  qDebug() << QString::number(index.row());
#endif


  if(cellule){
    // Une Boule Trouvee la montrer dans les autres fenetres
    qtv_Tirages->clearSelection();
    DB_tirages->MontrerLaBoule(val,qtv_Tirages);
    DB_tirages->MLB_DansLaQtTabView(val,qtv_Voisins);
    DB_tirages->MLB_DansLaQtTabView(val,qtv_Ecarts);
    qtv_LstCouv->clearSelection();
    DB_tirages->MontrerBouleCouverture(val,qtv_LstCouv);

    DB_tirages->MLB_DansMesPossibles(val,QBrush(Qt::yellow),qtv_MesPossibles);
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
  QModelIndex index = qtv_MesPossibles->indexAt(pos);
  int v_id = qsim_MesPossibles->index(index.row(),index.column()).data().toInt();
  QVariant  hdata =  index.model()->headerData(index.column(),Qt::Horizontal);
  QString msg = hdata.toString();
  msg = msg.split("b").at(1);
  int b_id = msg.toInt();
  QMenu *menu=new QMenu(this);

  DB_tirages->PopulateCellMenu(b_id, v_id, menu, this);
  menu->popup(qtv_MesPossibles->viewport()->mapToGlobal(pos));

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

void MainWindow::tablev_customContextMenu(QPoint pos)
{

  menuTrieMesPossibles->popup(qtv_MesPossibles->viewport()->mapToGlobal(pos));
}

void MainWindow::ft1(void)
{
  ft_LancerTri(-1);
}

int MainWindow::BouleIdFromColId(int col_id)
{
  int b_id = 0;

  QVariant  hdata =  qtv_MesPossibles->model()->headerData(col_id,Qt::Horizontal);
  QString msg = hdata.toString();

  if (!msg.contains("C")){
    msg = msg.split("b").at(1);
    b_id = msg.toInt();
  }

  return b_id;
}

void MainWindow::ft_LancerTri(int tri_id)
{
  int col_id = 0;
  colonne_tri = tri_id;

  for(col_id = 0; col_id < 5;col_id++)
  {
    int b_id = BouleIdFromColId(col_id);

    if(b_id)
    {
      // Memoriser la recherche precedente eventuelle
      int boule = DB_tirages->CouleurVersBid(qtv_MesPossibles);

      // Effacer les recherches de boules precedentes
      DB_tirages->MLB_DansMesPossibles(0,QBrush(Qt::yellow),qtv_MesPossibles);

      // Montrer les nouveaux resultats
      DB_tirages->EffectuerTrieMesPossibles(tri_id,col_id,b_id,qsim_MesPossibles);

      // Montrer les boules communes
      // Remontrer la recherche eventuelle
      DB_tirages->MLB_DansMesPossibles(boule, QBrush(Qt::yellow),qtv_MesPossibles);
    }
  }

  // Mise en evidence numero commun
  DB_tirages->MLB_MontreLesCommuns(&configJeu,qtv_MesPossibles);



}

void MainWindow::ft2(void)
{
  ft_LancerTri(0);
  //qsim_Voisins->sort(1);
}

void MainWindow::ft3(void)
{
  ft_LancerTri(1);
  //qsim_Voisins->sort(2);
}

void MainWindow::ft4(void)
{
  ft_LancerTri(2);
  //qsim_Voisins->sort(3);
}

void MainWindow::ft5(void)
{
  ft_LancerTri(3);
  //qsim_Voisins->sort(4);
}

void MainWindow::ft6(void)
{
  ft_LancerTri(4);
  //qsim_Voisins->sort(5);
}

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
// http://forum.hardware.fr/hfr/Programmation/C-2/resolu-renvoyer-combinaison-sujet_23393_1.htm
// http://www.dcode.fr/generer-calculer-combinaisons
void MainWindow::TST_RechercheCombi(stTiragesDef *ref, QTabWidget *onglets)
{
  QStringList sl_Lev1[5];
  QString msg = "";
  bool status = false;
  QStringList sl_Lev0;

  //QTabWidget *onglets = ;

  if(ref->limites[0].max == 49)
  {
    sl_Lev0 << "1" << "2" << "3" << "4" << "5";
  }
  else
  {
    sl_Lev0 << "1" << "2" << "3" << "4" << "5" << "6";
  }

  // Recuperation des combinaison C(1,5), C(2,5), C(3,5), C(4,5), C(5,5)
  for (int i = 0; i< 5; i++)
  {
    TST_CombiRec(i+1, sl_Lev0, "" , sl_Lev1[i]);

    // Inscription de cette liste dans une table
    TST_CombiVersTable(sl_Lev1[i],ref);
  }

  // Insertion de la ponderation de la liste des combis
  TST_PonderationCombi(5);

  // Affectation d'un poids a un tirage
  TST_AffectePoidsATirage(ref);

  // Ecriture du fichier resultat
  TST_MettrePonderationSurTirages();

  // Faire un onglet (Pere) par type de possibilite de gagner
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



      if(sousOnglet == NbBg-1)
      {
        qsim_r = new QStandardItemModel(nbItems,2);;
      }
      else
      {
        qsim_r = new QStandardItemModel(nbItems*nbI2tems,2);
      }


      // Preparer le tableau des valeurs de synthese
      for(int loop=0;loop < 50; loop++)
      {
        QStandardItem *item_1 = new QStandardItem();
        QStandardItem *item_2 = new QStandardItem();
        qsim_synthese->setItem(loop,0,item_1);
        qsim_synthese->setItem(loop,1,item_2);
      }

      qsim_r->setHeaderData(0,Qt::Horizontal,"Combinaison");
      qsim_r->setHeaderData(1,Qt::Horizontal,"Total");
      qtv_r->setModel(qsim_r);

      qtv_r->setSortingEnabled(true);
      qtv_r->setAlternatingRowColors(true);
      qtv_r->setEditTriggers(QAbstractItemView::NoEditTriggers);
      qtv_r->setColumnWidth(0,260);
      qtv_r->setColumnWidth(1,50);


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
            d[0]=NbBg-1;
            d[1]=1;
            break;
          case 3:
            d[0]=NbBg-2;
            d[1]=1;
            d[2]=1;
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
              msg = "select count (*) from analyses where ("
                    + colsel + ");";

              for(int loop=0;loop<2;loop++)
              {
                QStandardItem *item_2 = new QStandardItem();
                qsim_r->setItem((i*nbI2tems)+j,loop,item_2);
              }

              status = DB_tirages->TST_Requete(msg,(i*nbI2tems)+j,colsel,qsim_r);
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
            msg = "select count (*) from analyses where ("
                  + colsel + ");";
            for(int loop=0;loop<2;loop++)
            {
              QStandardItem *item_2 = new QStandardItem();
              qsim_r->setItem(i,loop,item_2);
            }

            status = DB_tirages->TST_Requete(msg,i,colsel,qsim_r);

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
          msg = "select count (*) from analyses where ("
                + colsel + ");";
          for(int loop=0;loop<2;loop++)
          {
            QStandardItem *item_2 = new QStandardItem();
            qsim_r->setItem(i,loop,item_2);
          }

          status = DB_tirages->TST_Requete(msg,i,colsel,qsim_r);
        }
      }
      // Un onglet est construit
      // Faire la synthese des boules trouvees
      qtv_r->sortByColumn(1,Qt::DescendingOrder);
      qsim_t->sort(0,Qt::AscendingOrder);
      TST_SyntheseDesCombinaisons(qtv_r,qsim_synthese, qsim_t, &NbTotLgn);
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

    QString st_SubOngTotal = "Total:"
                             +QString::number(NbTotLgn);

    tw_rep->addTab(qtv_t,tr(st_SubOngTotal.toLocal8Bit()));
  }
}

void MainWindow::TST_SyntheseDesCombinaisons(QTableView *p_in, QStandardItemModel * qsim_rep,QStandardItemModel * qsim_total, int *TotalLigne)
{
  int ligne = 0;
  int val = 0;
  QModelIndex modelIndex;
  //QAbstractItemModel *theModel = p_in->model();
  //QStandardItemModel *dest= (QStandardItemModel*) theModel;
  QString SqlReq = "";

  do
  {
    modelIndex = p_in->model()->index(ligne,1, QModelIndex());

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

        msg = "Select * from tirages inner join (select * from analyses where("
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
    QSqlQuery sql_1;
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

  TST_MontrerDetailCombinaison(msg);
}

void MainWindow::TST_MontrerDetailCombinaison(QString msg)
{
  QWidget *qw_fenResu = new QWidget;
  QTabWidget *tw_resu = new QTabWidget;
  QSqlQueryModel *sqm_r1 = new QSqlQueryModel;
  QTableView *tv_r1 = new QTableView;
  QString st_msg ="";
  QFormLayout *mainLayout = new QFormLayout;
  //bool status = true;


  msg.replace("c","bd");
  msg.replace(",","and");

  st_msg = "Select * from tirages inner join (select * from analyses where("
           + msg +
           "))as s on tirages.id = s.id;";





  sqm_r1->setQuery(st_msg);
  tv_r1->setModel(sqm_r1);
  //view->setSortingEnabled(true);

  tv_r1->hideColumn(0);
  for(int j=2;j<12;j++)
    tv_r1->setColumnWidth(j,30);

  for(int j =12; j<=sqm_r1->columnCount();j++)
    tv_r1->hideColumn(j);


  ////////////////////////////////////////////////////////////
  QTableView *qtv_rep = new QTableView;
  QStandardItemModel *qsim_rep = new QStandardItemModel(50,2);
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
  for(int i =1; (i< 51) ;i++)
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
#if 0
  for(int i =1; (i< 51) && status ;i++)
  {
    QStandardItem * item_1 = new QStandardItem;
    QStandardItem * item_2 = new QStandardItem;

    item_1->setData(i,Qt::DisplayRole);
    qsim_rep->setItem(i-1,0,item_1);

    QSqlQuery sql_1;
    QString msg_2 = "select count (*) from (" +st_msg+ " where (" +
                    "b1 = " +QString::number(i) + " or " +
                    "b2 = " +QString::number(i) + " or " +
                    "b3 = " +QString::number(i) + " or " +
                    "b4 = " +QString::number(i) + " or " +
                    "b5 = " +QString::number(i) + " ));" ;

    status = sql_1.exec(msg_2);
    if(status)
    {
      sql_1.first();
      if(sql_1.isValid())
      {
        int tot = sql_1.value(0).toInt();
        item_2->setData(tot,Qt::DisplayRole);

        qsim_rep->setItem(i-1,1,item_2);
      }
    }
  }
#endif
  //qtv_rep->show();

  // ??
  tw_resu->addTab(tv_r1,tr("Details"));
  tw_resu->addTab(qtv_rep,tr("Synthese"));
  mainLayout->addWidget(tw_resu);
  qw_fenResu->setLayout(mainLayout);
  qw_fenResu->setWindowTitle(msg);

  zoneCentrale->addSubWindow(qw_fenResu);
  qw_fenResu->show();

}

int MainWindow::TST_TotBidDansGroupememnt(int bId, QString &st_grp)
{
  int ret_val = 0;
  bool status = false;
  QSqlQuery sql_1;
  QString msg_2 = "select count (*) from (" +st_grp+ " where (" +
                  "b1 = " +QString::number(bId) + " or " +
                  "b2 = " +QString::number(bId) + " or " +
                  "b3 = " +QString::number(bId) + " or " +
                  "b4 = " +QString::number(bId) + " or " +
                  "b5 = " +QString::number(bId) + " ));" ;

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

  QSqlQuery sql_1;
  QString st_cols = "";
  QString st_vals = "";
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
              st_cols = st_cols
                        +"b"+item.at(k)
                        +",";
              st_vals = st_vals
                        +QString::number(coef[loop][sub][(j+k)%nbitems])
                        +",";

            }
            st_cols.remove(st_cols.length()-1,1);
            st_vals.remove(st_vals.length()-1,1);
            msg_1 = "insert into lstcombi (id,pos,comb,rot,"
                    + st_cols + ",tip) Values (NULL,"
                    + QString::number(loop)+","
                    + QString::number(i)+","
                    + QString::number(j) +","
                    + st_vals + ",\""
                    + st_vals + "\");";
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
      }
      st_cols.remove(st_cols.length()-1,1);
      st_vals.remove(st_vals.length()-1,1);
      msg_1 = "insert into lstcombi (id,pos,comb,rot,"
              + st_cols + ") Values (NULL,"
              + QString::number(loop)+","
              + QString::number(i)+",0,"
              + st_vals + ");";

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
  QSqlQuery sql_1;
  QSqlQuery sql_2;
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
  QSqlQuery sql_1;
  QSqlQuery sql_2;
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
        int id_poids = sql_1.value(0).toInt();

        for(int i = 0; i<= nbBoules;i++)
        {
          coef[i] = sql_1.value(4+i).toInt();
          msg_2 = msg_2 + "bd"+QString::number(i)
                  +"="+QString::number(coef[i])+ " and ";
        }

        // creation d'une requete mise a jour des poids
        //double poids = sql_1.value(lastcol-1).toDouble();
#if 0
        update analyses set id_poids=14 where(id in
                                              (select id from analyses where (bd0=1 and bd1=1 and bd2=2 and bd3=1 and bd4=0 and bd5=0)
                                               ));
#endif
        msg_2.remove(msg_2.length()-5,5);
        msg_2 = "Update analyses set id_poids="
                +QString::number(id_poids)
                +" where(id in (select id from analyses where("
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
  select analyses.id, lstcombi.poids from analyses inner join lstcombi on analyses.id_poids = lstcombi.id;
#endif
  bool status = false;
  QSqlQuery sql_1;
  QString msg_1 = "select analyses.id, lstcombi.poids from analyses inner join lstcombi on analyses.id_poids = lstcombi.id;";

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

void MainWindow::TST_Graphe(QMdiArea *obj)
{
  QWidget *qw_view = new QWidget;
  myview = new MyGraphicsView(qw_view);

#ifdef USE_WORKING
  QSqlQuery sql_1;
  bool status = false;
  QString msg_1="select max(analyses.id), max(lstcombi.poids) from analyses , lstcombi;";

  qw_view->setWindowTitle("Graphique");
  qw_view->setMinimumWidth(390);

  status = sql_1.exec(msg_1);
  if(status)
  {
    sql_1.first();
    if(sql_1.isValid())
    {
      int mx = sql_1.value(0).toInt() + 1;
      int my = ceil(sql_1.value(1).toDouble()) + 1;

      myview = new MyGraphicsView(qw_view);
      myview->setRenderHints( QPainter::Antialiasing );
      myview->setDragMode(QGraphicsView::ScrollHandDrag);
      //myview.show();

#if 0
      qw_view->setMinimumHeight(my+10);

      qgr_scene = new QGraphicsScene(QRectF(0, 0, mx, my+10),qw_view);
      qgr_scene->setBackgroundBrush(Qt::yellow);


      qgr_view = new QGraphicsView( qgr_scene,qw_view);
      qgr_view->setRenderHints( QPainter::Antialiasing );
      qgr_view->setDragMode(QGraphicsView::ScrollHandDrag);
      qgr_view->show();
#endif
    }
  }
#endif

  obj->addSubWindow(qw_view);
}

#if 0
void MainWindow::TST_Graphe(QMdiArea *obj)
{

  QWidget *qw_view = new QWidget;

  qw_view->setWindowTitle("Graphique");
  qw_view->setMinimumHeight(390);
  qw_view->setMinimumWidth(390);

  qgr_scene = new QGraphicsScene(QRectF(0, 0, 100, 100),qw_view);
  qgr_scene->setBackgroundBrush(Qt::yellow);


  qgr_view = new QGraphicsView( qgr_scene,qw_view);
  qgr_view->setRenderHints( QPainter::Antialiasing );
  qgr_view->setDragMode(QGraphicsView::ScrollHandDrag);
  qgr_view->show();

  obj->addSubWindow(qw_view);

}
#endif
