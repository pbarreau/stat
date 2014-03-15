#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QMessageBox>
#include <QSqlQuery>
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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "choixjeux.h"
#include "tirages.h"
#include "gererbase.h"

static stTiragesDef configJeu;

MainWindow::MainWindow(QWidget *parent,NE_FDJ::E_typeJeux leJeu, bool load) :
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
  DB_tirages->CreerBaseEnMemoire(true);


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

  // Ordre arrivee des boules ?
  DB_tirages->CouvertureBase(qsim_Ecarts,&configJeu);

  //// GARDER L'ORDRE D'APPEL DES FONCTIONS PB VERROU SUR LA BASE
  // Remplir Sous Fen les ecarts
  for(i=1;i<=configJeu.limites->max;i++){
	DB_tirages->DistributionSortieDeBoule(i,qsim_Ecarts,&configJeu);
  }

  // Remplir la sousfenetre base de données
  DB_tirages->AfficherBase(qw_Tirages,qtv_Tirages);


  // Remplir la sous fenetre resultat couverture
  DB_tirages->AfficherResultatCouverture(qw_LstCouv,qtv_LstCouv);


  // Remplir la sous fenetre de parite
  DB_tirages->MLP_DansLaQtTabView(&configJeu, CL_PAIR, qsim_Parites);
  DB_tirages->MLP_DansLaQtTabView(&configJeu, CL_SGRP, qsim_Ensemble_1);

  setCentralWidget(zoneCentrale);

  // Double click dans sous fenetre base
  connect( qtv_Tirages, SIGNAL( doubleClicked(QModelIndex)) ,
		   this, SLOT( slot_ChercheVoisins( QModelIndex) ) );

  // Double click dans sous fenetre ecart
  connect( qtv_Ecarts, SIGNAL( doubleClicked(QModelIndex)) ,
		   this, SLOT( slot_qtvEcart( QModelIndex) ) );

  // click dans fenetre ma selection
  connect( qtv_MesChoix, SIGNAL( doubleClicked(QModelIndex)) ,
		   this, SLOT( slot_UneSelectionActivee( QModelIndex) ) );

  // click dans fenetre voisin pour afficher boule
  connect( qtv_Voisins, SIGNAL( clicked(QModelIndex)) ,
		   this, SLOT( slot_MontrerBouleDansBase( QModelIndex) ) );
  connect( qtv_MesPossibles, SIGNAL( clicked(QModelIndex)) ,
		   this, SLOT( slot_MontrerBouleDansBase( QModelIndex) ) );

  // Selection a change
  connect(qtv_LstCouv->selectionModel(), SIGNAL(selectionChanged (const QItemSelection&, const QItemSelection&)),
		  this, SLOT(slot_CouvertureSelChanged(QItemSelection,QItemSelection)));

}

void MainWindow::fen_Tirages(void)
{
  qw_Tirages = new QWidget;
  qtv_Tirages = new QTableView;
  QFormLayout *layout = new QFormLayout;

  layout->addWidget(qtv_Tirages);
  qw_Tirages->setLayout(layout);
  qw_Tirages->setWindowTitle("Base des tirages");

  // Gestion du QTableView
  qtv_Tirages->setSelectionMode(QAbstractItemView::SingleSelection);
  qtv_Tirages->setSelectionBehavior(QAbstractItemView::SelectItems);
  qtv_Tirages->setStyleSheet("QTableView {selection-background-color: red;}");
  qtv_Tirages->setEditTriggers(QAbstractItemView::NoEditTriggers);
  qtv_Tirages->setAlternatingRowColors(true);

  // Ratacher cette sous fenetre
  zoneCentrale->addSubWindow(qw_Tirages);
  //QMdiSubWindow *sousFenetre1 = zoneCentrale->addSubWindow(AfficherBase);

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

}
void MainWindow::fen_Voisins(void)
{
  int  i;
  QWidget *qw_Voisins = new QWidget;
  qtv_Voisins = new QTableView;
  int zn = 0;
  qsim_Voisins = new QStandardItemModel(configJeu.limites[zn].max,6);

  // entete du modele
  qsim_Voisins->setHeaderData(0,Qt::Horizontal,"B");
  qsim_Voisins->setHeaderData(1,Qt::Horizontal,"V:r0");
  qsim_Voisins->setHeaderData(2,Qt::Horizontal,"V:+1");
  qsim_Voisins->setHeaderData(3,Qt::Horizontal,"V:+2");
  qsim_Voisins->setHeaderData(4,Qt::Horizontal,"V:-1");
  qsim_Voisins->setHeaderData(5,Qt::Horizontal,"V:-2");

  // Ecriture du numero de boule
  for(i=1;i<=configJeu.limites[zn].max;i++)
  {
	QStandardItem *item = new QStandardItem( QString::number(222));
	item->setData(i,Qt::DisplayRole);
	qsim_Voisins->setItem(i-1,0,item);
  }

  qtv_Voisins->setModel(qsim_Voisins);
  qtv_Voisins->setColumnWidth(0,60);
  qtv_Voisins->setColumnWidth(1,60);
  qtv_Voisins->setColumnWidth(2,60);
  qtv_Voisins->setColumnWidth(3,60);
  qtv_Voisins->setColumnWidth(4,60);
  qtv_Voisins->setColumnWidth(5,60);
  //tblVoisin->setMaximumWidth(260);
  qtv_Voisins->setMinimumHeight(367);
  qtv_Voisins->setSortingEnabled(true);
  qtv_Voisins->sortByColumn(0,Qt::AscendingOrder);
  qtv_Voisins->setAlternatingRowColors(true);
  qtv_Voisins->setEditTriggers(QAbstractItemView::NoEditTriggers);
  //qtv_Voisins->hideColumn(3);
  //qtv_Voisins->hideColumn(4);

  nbSortie = new QLabel;
  nbSortie->setText("Nb total de sorties:");

  QFormLayout *layVoisin = new QFormLayout;
  layVoisin->addWidget(nbSortie);
  layVoisin->addWidget(qtv_Voisins);

  qw_Voisins->setMinimumHeight(367);
  qw_Voisins->setLayout(layVoisin);
  qw_Voisins->setWindowTitle("Voisins de selection");
  //QMdiSubWindow *sousFenetre2 =
  zoneCentrale->addSubWindow(qw_Voisins);

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
  qtv_Ecarts->setMinimumHeight(367);

  QFormLayout *layCouverture = new QFormLayout;
  layCouverture->addWidget(qtv_Ecarts);


  qw_Ecarts->setMinimumHeight(367);
  qw_Ecarts->setLayout(layCouverture);
  qw_Ecarts->setWindowTitle("Couverture boules");
  //QMdiSubWindow *sousFenetre3 =
  zoneCentrale->addSubWindow(qw_Ecarts);

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
  qtv_MaSelection->setMinimumWidth(320);
  QFormLayout *layCouverture = new QFormLayout;
  layCouverture->addWidget(qtv_MaSelection);

  for(j=0;j<10;j++)
  {
	qtv_MaSelection->setColumnWidth(j,30);
  }
  qw_MaSelection->setMinimumHeight(183);


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
  qtv_MesPossibles->setMinimumHeight(367);

  QFormLayout *layCouverture = new QFormLayout;
  layCouverture->addWidget(qtv_MesPossibles);


  qw_MesPossibles->setMinimumHeight(367);
  qw_MesPossibles->setLayout(layCouverture);
  qw_MesPossibles->setWindowTitle("Mes possibles");
  //QMdiSubWindow *sousFenetre3 =
  zoneCentrale->addSubWindow(qw_MesPossibles);

}

void MainWindow::fen_Parites(void)
{
  QWidget *qw_Parites = new QWidget;
  QTabWidget *tabWidget = new QTabWidget;



  int zn = 0;

  qsim_Parites = new QStandardItemModel(configJeu.nbElmZone[zn],configJeu.nb_zone+1);

  qsim_Parites->setHeaderData(0,Qt::Horizontal,"Bpair");
  qsim_Parites->setHeaderData(1,Qt::Horizontal,"Tot z1");
  qsim_Parites->setHeaderData(2,Qt::Horizontal,"Tot z2");

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
  qsim_Ensemble_1->setHeaderData(1,Qt::Horizontal,"Tot z1");
  qsim_Ensemble_1->setHeaderData(2,Qt::Horizontal,"Tot z2");

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

  tabWidget->addTab(qtv_Parites,tr("Parite"));
  tabWidget->addTab(qtv_E1,tr("Repartition"));


  QFormLayout *mainLayout = new QFormLayout;
  mainLayout->addWidget(tabWidget);
  qw_Parites->setWindowTitle("Parites des tirages");
  qw_Parites->setLayout(mainLayout);
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
#if 0
  QMessageBox::information(this, "",
						   "Cell at row "+QString::number(index.row())+
						   " column "+QString::number(index.column())+
						   " was double clicked.");
#endif
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
	DB_tirages->MontreMesPossibles(index,&configJeu,qsim_MesPossibles);
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

  QStandardItem *item1 = qsim_MaSelection->itemFromIndex(index);
#ifndef QT_NO_DEBUG
  qDebug()<< item1->background();
#endif

  if(item1->background() == Qt::red){
	if(index.row()%2==0){
	  QBrush macouleur(Qt::white);
	  item1->setBackground(macouleur);
	}
	else
	{
	  QBrush macouleur (Qt::gray);
	  item1->setBackground(macouleur);
	}
  }
  else
  {
	QBrush macouleur(Qt::red);
	item1->setBackground(macouleur);
  }


}

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
  }
#ifndef QT_NO_DEBUG
  qDebug() << QString::number(index.row());
#endif
  if(cellule){
	qtv_Tirages->clearSelection();
	DB_tirages->MontrerLaBoule(val,qtv_Tirages);
	DB_tirages->MLB_DansLaQtTabView(val,qtv_Voisins);
	DB_tirages->MLB_DansLaQtTabView(val,qtv_Ecarts);
	qtv_LstCouv->clearSelection();
	DB_tirages->MontrerBouleCouverture(val,qtv_LstCouv);
  }
}


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

void MainWindow::tablev_customContextMenu(QPoint pos){
  //int col_id = qtv_MesPossibles->columnAt(pos.x());


  menuTrieMesPossibles->popup(qtv_MesPossibles->viewport()->mapToGlobal(pos));
}

void MainWindow::ft1(void)
{

}

void MainWindow::ft2(void)
{
  // Trie a +1
  QVariant  hdata =  qtv_MesPossibles->model()->headerData(0,Qt::Horizontal);
  QString msg = hdata.toString();

  if (!msg.contains("C")){
	msg = msg.split("b").at(1);
	int b_id = msg.toInt();
	QStandardItem *tmp_itm = qsim_MesPossibles->item(0,0);
	tmp_itm->setData(100,Qt::DisplayRole);
	qsim_MesPossibles->setItem(0,0,tmp_itm);
	//effectuer le trie p1
	//EffectuerTrieMesPossibles(2,b_id,qtv_MesPossibles);
  }

}
void MainWindow::ft3(void)
{

}
void MainWindow::ft4(void)
{

}
void MainWindow::ft5(void)
{

}
void MainWindow::ft6(void)
{

}
