#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QtGui>
#include <QFormLayout>
#include "labelclickable.h"
#include "pointtirage.h"

//#include <QtPlugin>
//Q_IMPORT_PLUGIN (QWindowsIntegrationPlugin);

#if 0
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
#include <QLabel>
#include <QStandardItemModel>
#include <QTableWidget>
#include <QModelIndex>
#include <QBrush>
#include <QTabWidget>
#endif

#include <math.h>

#include "mygraphicsview.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "choixjeux.h"
#include "tirages.h"
#include "gererbase.h"

static stTiragesDef configJeu;

void MainWindow::EtudierJeu(NE_FDJ::E_typeJeux leJeu, bool load, bool dest_bdd)
{
    DB_tirages = new GererBase;
    int i;
    tirages tmp(leJeu);
    QString ficSource;

    // Recuperation des contantes du type de jeu
    tmp.getConfig(&configJeu);

    // Creation sous fenetre pour mettre donnees de base
    fen_Tirages();

    /// --------- rem 1

    //fen_LstCouv();

    // Creation sous fenetre des voisins
    fen_Voisins();

    // Creation sous fenetre des ecarts
    //fen_Ecarts();

    // Creation fenetre pour memoriser a selection
    fen_MaSelection();

    // Creation fenetre resultat
    fen_MesPossibles();

    // Creation fenetre pour parite
    fen_Parites();
    /// --- Fim rem 1

    // Preparer la base de donn�es
    DB_tirages->CreerBaseEnMemoire(dest_bdd,leJeu);


    // Creation des tables pour ce type jeu
    DB_tirages->CreerTableTirages(&tmp);

    // Recuperation des donn�es fdj
    ficSource = tmp.SelectSource(load);
    if (DB_tirages->LireLesTirages(ficSource,&tmp) == false)
    {
        this->close();
    }

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

    /// ------- rem 2
    ///---------------------
    // Recherche de combinaison A deplacer ?
    //DB_tirages->RechercheCombinaison(&configJeu,tabWidget,zoneCentrale);
    TST_EtoileCombi(&configJeu,G_tbw_MontabWidget);
    TST_RechercheCombi(&configJeu,G_tbw_MontabWidget);
    //QApplication::quit();

    //return;

    // Ordre arrivee des boules ?
    DB_tirages->CouvertureBase(G_sim_Ecarts,&configJeu);



    //// GARDER L'ORDRE D'APPEL DES FONCTIONS PB VERROU SUR LA BASE
    // --
    int zn = 0;
    for(i=1;i<=configJeu.limites[zn].max;i++){
        // Remplir Sous Fen les ecarts
        DB_tirages->DistributionSortieDeBoule(i,G_sim_Ecarts,&configJeu);
        // Montrer les valeurs probable
        DB_tirages->CouvMontrerProbable(i,3,1,G_sim_Ecarts);


        // Calcul occurence de cette boule
        DB_tirages->TotalApparitionBoule(i,&configJeu, zn, G_sim_Voisins[zn]);
    }

    // --
    zn=1;
    for(i=1;i<=configJeu.limites[zn].max;i++)
    {
        // Calcul occurence de cette boule
        DB_tirages->TotalApparitionBoule(i,&configJeu, zn, G_sim_Voisins[zn]);
    }

    // ----------------------------

    /// fin rem 2
    // Remplir la sousfenetre base de donn�es
    DB_tirages->AfficherBase(&configJeu,G_w_Tirages,G_tbv_Tirages);
    /// rem 3


    // Remplir la sous fenetre resultat couverture
    DB_tirages->AfficherResultatCouverture(&configJeu,G_w_CouvTirages,G_tbv_CouvTirages);


    // Remplir la sous fenetre de parite
    DB_tirages->MLP_DansLaQtTabView(&configJeu, CL_PAIR, G_sim_Parites);
    DB_tirages->MLP_DansLaQtTabView(&configJeu, CL_SGRP, G_sim_Ensemble_1);
    DB_tirages->MLP_UniteDizaine(&configJeu, G_sim_ud);
    // Prevision des prochains tirage basee sur la parite du dernier tirage
    TST_PrevisionType(NE_FDJ::critere_parite,&configJeu);
    // idem en regardant la valeur de n/2
    TST_PrevisionType(NE_FDJ::critere_enemble,&configJeu);


    TST_Graphe(&configJeu);
    /// ---- fin rem 3
    //setCentralWidget(zoneCentrale);
}

#if 0
void MainWindow::Prev_MainWindow(QWidget *parent,NE_FDJ::E_typeJeux leJeu, bool load, bool dest_bdd)// :
//  QMainWindow(parent),
//  ui(new Ui::MainWindow)
{
    DB_tirages = new GererBase;
    int i;
    tirages tmp(leJeu);
    QString ficSource;

    // Recuperation des contantes du type de jeu
    tmp.getConfig(&configJeu);

    //stTiragesDef ess;
    //ess = configJeu;

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
    fen_MaSelection();

    // Creation fenetre resultat
    fen_MesPossibles();

    // Creation fenetre pour parite
    fen_Parites();

    // Preparer la base de donn�es
    DB_tirages->CreerBaseEnMemoire(dest_bdd,leJeu);


    // Creation des tables pour ce type jeu
    DB_tirages->CreerTableTirages(&tmp);

    // Recuperation des donn�es fdj
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
    TST_EtoileCombi(&configJeu,tabWidget);
    TST_RechercheCombi(&configJeu,tabWidget);
    //QApplication::quit();

    //return;

    // Ordre arrivee des boules ?
    DB_tirages->CouvertureBase(qsim_Ecarts,&configJeu);



    //// GARDER L'ORDRE D'APPEL DES FONCTIONS PB VERROU SUR LA BASE
    // --
    int zn = 0;
    for(i=1;i<=configJeu.limites[zn].max;i++){
        // Remplir Sous Fen les ecarts
        DB_tirages->DistributionSortieDeBoule(i,qsim_Ecarts,&configJeu);
        // Montrer les valeurs probable
        DB_tirages->CouvMontrerProbable(i,3,1,qsim_Ecarts);


        // Calcul occurence de cette boule
        DB_tirages->TotalApparitionBoule(i,&configJeu, zn, qsimT_Voisins[zn]);
    }

    // --
    zn=1;
    for(i=1;i<=configJeu.limites[zn].max;i++)
    {
        // Calcul occurence de cette boule
        DB_tirages->TotalApparitionBoule(i,&configJeu, zn, qsimT_Voisins[zn]);
    }

    // ----------------------------


    // Remplir la sousfenetre base de donn�es
    DB_tirages->AfficherBase(&configJeu,qw_Tirages,qtv_Tirages);


    // Remplir la sous fenetre resultat couverture
    DB_tirages->AfficherResultatCouverture(&configJeu,qw_LstCouv,qtv_LstCouv);


    // Remplir la sous fenetre de parite
    DB_tirages->MLP_DansLaQtTabView(&configJeu, CL_PAIR, qsim_Parites);
    DB_tirages->MLP_DansLaQtTabView(&configJeu, CL_SGRP, qsim_Ensemble_1);
    DB_tirages->MLP_UniteDizaine(&configJeu, qsim_ud);

    TST_Graphe(&configJeu);

    setCentralWidget(zoneCentrale);


    ///-----------------
#if 0
    // Selection a change
    connect(qtv_LstCouv->selectionModel(), SIGNAL(selectionChanged (const QItemSelection&, const QItemSelection&)),
            this, SLOT(slot_CouvertureSelChanged(QItemSelection,QItemSelection)));

#endif

}
#endif

void MainWindow::fen_Tirages(void)
{
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
    //Gtv_Tirages->setFixedSize(600,600);

    lay_ForTop[0]->addWidget(G_tbv_Tirages);
    wid_ForTop[0]->setLayout(lay_ForTop[0]);


    // Gestion du QTableView de Couvertures Tirages
    G_tbv_CouvTirages->setSelectionMode(QAbstractItemView::MultiSelection);
    G_tbv_CouvTirages->setSelectionBehavior(QAbstractItemView::SelectItems);
    G_tbv_CouvTirages->setStyleSheet("QTableView {selection-background-color: red;}");
    G_tbv_CouvTirages->setEditTriggers(QAbstractItemView::NoEditTriggers);
    G_tbv_CouvTirages->setAlternatingRowColors(true);

    lay_ForTop[1]->addWidget(G_tbv_CouvTirages);
    wid_ForTop[1]->setLayout(lay_ForTop[1]);

    //lay_couvTirages->addWidget(Gtv_CouvTirages);
    //Gw_CouvTirages->setLayout(lay_couvTirages);

    lay_tirages->addWidget(tab_Top);
    G_w_Tirages->setLayout(lay_tirages);
    G_w_Tirages->setWindowTitle("Tirages");
    //Gw_Tirages->setMinimumHeight(420);
    G_w_Tirages->setFixedSize(600,500);

    zoneCentrale->addSubWindow(G_w_Tirages);
    G_w_Tirages->setVisible(true);
    //QMdiSubWindow *sousFenetre1 = zoneCentrale->addSubWindow(AfficherBase);

    // Double click dans sous fenetre base
    connect( G_tbv_Tirages, SIGNAL( doubleClicked(QModelIndex)) ,
             this, SLOT( slot_ChercheVoisins( QModelIndex) ) );


}

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

#if 0
    // Selection a change
    connect(qtv_LstCouv->selectionModel(), SIGNAL(selectionChanged (const QItemSelection&, const QItemSelection&)),
            this, SLOT(slot_CouvertureSelChanged(QItemSelection,QItemSelection)));

#endif

}

QFormLayout * MainWindow::MonLayout_VoisinsPresent()
{
    QFormLayout *lay_return = new QFormLayout;

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

        // Double click sur libell� recherche boule
        connect( G_lab_nbSorties[zn], SIGNAL( clicked(QString)) ,
                 this, SLOT( slot_RepererLesTirages(QString) ) );


    }

    // Memo du design
    lay_return->addWidget(tabWidget);

    return(lay_return);
}
//----------
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
//--------
void MainWindow::fen_Voisins(void)
{
    QWidget *qw_Voisins = new QWidget;

    // Onglet pere
    QTabWidget *tab_Top = new QTabWidget;
    QWidget **wid_ForTop = new QWidget*[2];

    QWidget *wTop_1 = new QWidget;
    QWidget *wTop_2 = new QWidget;

    wid_ForTop[0]= wTop_1;
    wid_ForTop[1]= wTop_2;

    tab_Top->addTab(wid_ForTop[0],tr("Presents"));
    tab_Top->addTab(wid_ForTop[1],tr("Absents"));
    // ------------------

    QFormLayout * design_onglet_1 = MonLayout_VoisinsPresent();
    QFormLayout * design_onglet_2 = MonLayout_VoisinsAbsent();

    wid_ForTop[0]->setLayout(design_onglet_1);
    wid_ForTop[1]->setLayout(design_onglet_2);

    QFormLayout *mainLayout = new QFormLayout;
    mainLayout->addWidget(tab_Top);

    qw_Voisins->setMinimumWidth(480);
    qw_Voisins->setLayout(mainLayout);
    qw_Voisins->setWindowTitle("Voisins de selection");

    zoneCentrale->addSubWindow(qw_Voisins);
    qw_Voisins->setVisible(true);
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

void MainWindow::fen_MaSelection(void)
{
    QTabWidget *tabWidget = new QTabWidget;
    int nb_zn = configJeu.nb_zone;

    G_tbv_MaSelection = new QTableView*[nb_zn];
    G_sim_MaSelection= new QStandardItemModel*[nb_zn];
    QWidget **tmpT_Widget = new QWidget*[nb_zn];
    QFormLayout **layT_MaSelection = new QFormLayout*[nb_zn];
    int *nbcol = new int [nb_zn];

    int  i=0,j=0, cell_val=0;
    QWidget *qw_MaSelection = new QWidget;

    for(int zn = 0;zn<nb_zn;zn++)
    {
        nbcol[zn] = (configJeu.limites[zn].max)%configJeu.nbElmZone[zn]?
                    (configJeu.limites[zn].max/configJeu.nbElmZone[zn])+1:
                    (configJeu.limites[zn].max/configJeu.nbElmZone[zn]);

        G_sim_MaSelection[zn]= new QStandardItemModel(configJeu.nbElmZone[zn],nbcol[zn]);
        G_tbv_MaSelection[zn] = new QTableView;
        tmpT_Widget[zn] = new QWidget;

        for(i=1;i<=configJeu.nbElmZone[zn];i++)/// Code a verifier en fonction bornes max
        { // Dans le cas max > 50
            for(j=1;j<=nbcol[zn];j++)
            {
                cell_val = j+(i-1)*nbcol[zn];
                if(cell_val<=configJeu.limites[zn].max){
                    QStandardItem *item = new QStandardItem( QString::number(i));
                    item->setData(cell_val,Qt::DisplayRole);
                    G_sim_MaSelection[zn]->setItem(i-1,j-1,item);
                }
            }
        }

        G_tbv_MaSelection[zn]->setModel(G_sim_MaSelection[zn]);
        G_tbv_MaSelection[zn]->setAlternatingRowColors(true);
        G_tbv_MaSelection[zn]->setEditTriggers(QAbstractItemView::NoEditTriggers);
        G_tbv_MaSelection[zn]->setMinimumHeight(190);

        layT_MaSelection[zn] = new QFormLayout;
        layT_MaSelection[zn]->addWidget(G_tbv_MaSelection[zn]);

        for(j=0;j<10;j++)
        {
            G_tbv_MaSelection[zn]->setColumnWidth(j,30);
        }

        tmpT_Widget[zn]->setLayout(layT_MaSelection[zn]);
        tabWidget->addTab(tmpT_Widget[zn],tr(configJeu.nomZone[zn].toLocal8Bit()));

        // click dans fenetre ma selection
        connect( G_tbv_MaSelection[zn], SIGNAL( doubleClicked(QModelIndex)) ,
                 this, SLOT( slot_UneSelectionActivee( QModelIndex) ) );
    }

    QFormLayout *mainLayout = new QFormLayout;
    mainLayout->addWidget(tabWidget);

    //qw_MaSelection->setMinimumHeight(200);
    qw_MaSelection->setFixedSize(390,260);
    qw_MaSelection->setLayout(mainLayout);
    qw_MaSelection->setWindowTitle("Ma Selection");

    //QMdiSubWindow *sousFenetre3 =
    zoneCentrale->addSubWindow(qw_MaSelection);
    qw_MaSelection->setVisible(true);

}
void MainWindow::fen_MesPossibles(void)
{
    QWidget *w_DataFenetre = new QWidget;
    // Onglet pere
    QTabWidget *tab_Top = new QTabWidget;
    QWidget **wid_ForTop = new QWidget*[5];

    QWidget *wTop_1 = new QWidget;
    QWidget *wTop_2 = new QWidget;
    QWidget *wTop_3 = new QWidget;
    QWidget *wTop_4 = new QWidget;
    QWidget *wTop_5 = new QWidget;

    wid_ForTop[0]= wTop_1;
    wid_ForTop[1]= wTop_2;
    wid_ForTop[2]= wTop_3;
    wid_ForTop[3]= wTop_4;
    wid_ForTop[4]= wTop_5;

    tab_Top->addTab(wid_ForTop[0],tr("Possibles"));
    tab_Top->addTab(wid_ForTop[1],tr("Absents"));
    tab_Top->addTab(wid_ForTop[2],tr("Ecarts"));
    tab_Top->addTab(wid_ForTop[3],tr("Parite"));
    tab_Top->addTab(wid_ForTop[4],tr("<n/2"));
    // ------------------

    QFormLayout * design_onglet_1 = MonLayout_ChoixPossible();
    QFormLayout * design_onglet_2 = MonLayout_Absent();
    QFormLayout * design_onglet_3 = MonLayout_Ecarts();
    QFormLayout * design_onglet_4 = MonLayout_Parite();
    QFormLayout * design_onglet_5 = MonLayout_Nsur2();

    wid_ForTop[0]->setLayout(design_onglet_1);
    wid_ForTop[1]->setLayout(design_onglet_2);
    wid_ForTop[2]->setLayout(design_onglet_3);
    wid_ForTop[3]->setLayout(design_onglet_4);
    wid_ForTop[4]->setLayout(design_onglet_5);

    QFormLayout *mainLayout = new QFormLayout;
    mainLayout->addWidget(tab_Top);

    //w_DataFenetre->setMinimumHeight(450);
    //w_DataFenetre->setFixedSize(320,450);
    w_DataFenetre->setLayout(mainLayout);
    w_DataFenetre->setWindowTitle("Boules");

    zoneCentrale->addSubWindow(w_DataFenetre);
    w_DataFenetre->setVisible(true);
}

QFormLayout * MainWindow:: MonLayout_ChoixPossible(void)
{
    QFormLayout *returnLayout = new QFormLayout;

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
QFormLayout * MainWindow:: MonLayout_Absent(void)
{
    QFormLayout *returnLayout = new QFormLayout;

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
QFormLayout * MainWindow:: MonLayout_Ecarts(void)
{
    int  i;
    int zn = 0;
    G_sim_Ecarts = new QStandardItemModel(configJeu.limites[zn].max,5);
    G_tbv_Ecarts = new QTableView;
    QFormLayout *returnLayout = new QFormLayout;

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

    // Double click dans sous fenetre ecart
    connect( G_tbv_Ecarts, SIGNAL( doubleClicked(QModelIndex)) ,
             this, SLOT( slot_qtvEcart( QModelIndex) ) );

    return(returnLayout);
}
// ---------
QFormLayout * MainWindow::MonLayout_Parite()
{
    QFormLayout *lay_return = new QFormLayout;

    int nb_zn = configJeu.nb_zone;
    QTabWidget *tab_conteneur = new QTabWidget;

    G_tbv_PariteVoisin = new QTableView*[nb_zn];
    G_sim_PariteVoisin= new QStandardItemModel*[nb_zn];
    G_lab_PariteVoisin = new QLabel*[nb_zn];
    QFormLayout **layT_Tmp_1 = new QFormLayout*[nb_zn];
    QWidget **tmpT_Widget = new QWidget*[nb_zn];

    QTableView ** fn_refTbv = G_tbv_PariteVoisin;
    QStandardItemModel ** fn_refSim = G_sim_PariteVoisin;
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
QFormLayout * MainWindow::MonLayout_Nsur2()
{
    QFormLayout *lay_return = new QFormLayout;

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


    zoneCentrale->addSubWindow(qw_Parites);
    qw_Parites->setVisible(true);
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

void MainWindow::slot_qtvEcart(const QModelIndex & index)
{
#if 0
    int val = 0;

    if(index.column()==0){
        // Effacer precedente selection
        qtv_LstCouv->clearSelection();
        qtv_Tirages->clearSelection();

        val = index.data().toInt();
        qtvT_Voisins[0]->sortByColumn(0,Qt::AscendingOrder);
        DB_tirages->RechercheVoisin(val,0,&configJeu,qlT_nbSorties[0],qsimT_Voisins[0]);
    }
#endif
}

void MainWindow::slot_UneSelectionActivee(const QModelIndex & index)
{
    //static QStringList *select = new QStringList *[configJeu.nb_zone];
    static QStringList select[2];
    int zn = -1;

    // determination de la table dans l'onglet ayant recu le click
    if (index.internalPointer() == G_sim_MaSelection[0]->index(index.row(),index.column()).internalPointer()){
        zn = 0;
    }
    else
    {
        zn=1;
    }



    QStandardItem *item1 = G_sim_MaSelection[zn]->itemFromIndex(index);
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

void MainWindow::slot_F2_RechercherLesTirages(const QModelIndex & index)
{
    int col=0;
    int val = 0;
    int zn=0;
    QStringList list;

    // determination de la table dans l'onglet ayant recu le click
    if (index.internalPointer() == G_sim_Parites->index(index.row(),index.column()).internalPointer())
    {
        col = index.column();
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

        if(cellule)
        {
            QStringList list;
            val = G_sim_Voisins[zn]->index(index.row(),0).data().toInt();

            col = index.column();

            if(col==CL_IHM_TOT_0)
            {
                list << QString::number(val);
                TST_MontreTirageAyantCritere(NE_FDJ::critere_boule,zn,&configJeu,list);
            }

            if(col >0 && col < CL_IHM_TOT_1)
            {
                QRegExp reg_number ("(\\d+)");
                QString  str_br = G_lab_nbSorties[zn]->text();
                int pos = 0;

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

            if(col >= CL_IHM_TOT_1 && col <= CL_IHM_TOT_2)
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
    if (index.internalPointer() == G_sim_PariteVoisin[0]->index(index.row(),index.column()).internalPointer())
    {
        zn = 0;
    }

    if (index.internalPointer() == G_sim_PariteVoisin[1]->index(index.row(),index.column()).internalPointer())
    {
        zn = 1;
    }

    if(zn != -1)
    {
        // determination de colonne
        col = index.column();
        rch = G_sim_PariteVoisin[zn]->index(index.row(),0).data().toInt();

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
        ptir = new PointTirage(configJeu.choixJeu,eTirage);
        //QGraphicsItem *unPoint = new QGraphicsItem;
        //unPoint->setPos(10);
        ptir->setPos(10,69.8);
        QGraphicsScene *pScene;
        pScene = myview[0]->GetScene();
        pScene->setFocusItem(ptir);

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

void MainWindow::TST_EtoileCombi(stTiragesDef *ref, QTabWidget *onglets)
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


    QSqlQuery sql_1;
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
    TST_PonderationCombi(50);

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

    TST_MontrerDetailCombinaison(msg, &configJeu);
}

void MainWindow::TST_MontrerDetailCombinaison(QString msg, stTiragesDef *pTDef)
{
    QWidget *qw_fenResu = new QWidget;
    QTabWidget *tw_resu = new QTabWidget;
    QSqlQueryModel *sqm_r1 = new QSqlQueryModel;
    QTableView *tv_r1 = new QTableView;
    QString st_msg ="";
    QFormLayout *mainLayout = new QFormLayout;


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
    tw_resu->addTab(tv_r1,tr("Details"));
    tw_resu->addTab(qtv_rep,tr("Synthese"));
    mainLayout->addWidget(tw_resu);
    qw_fenResu->setLayout(mainLayout);
    qw_fenResu->setWindowTitle(msg);

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





    sqm_r1->setQuery(msg_2);
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
    bool status = false;

    if (fen_titre == ""){
        fenetre_titre = "B:"+st_list.join(",");
    }
    else
    {
       fenetre_titre =  fen_titre;
    }

    sqm_r1->setQuery(msg);


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

void MainWindow::slot_MontreLeTirage(const QModelIndex & index)
{
    // recuperer la ligne de la table
    //int col = index.model()->columnCount();//myDataTableModel()->rowCount();
    int val = index.model()->index(index.row(),0).data().toInt();
    QAbstractItemModel *mon_model =G_tbv_Tirages->model();
    QModelIndex item1 = mon_model->index(0,0, QModelIndex());

    if (item1.isValid()){
        item1 = item1.model()->index(val-1,0);
        G_tbv_Tirages->setCurrentIndex(item1);
        G_tbv_Tirages->scrollTo(item1);
        G_tbv_Tirages->selectRow(val-1);

        QItemSelectionModel *selectionModel (G_tbv_Tirages->selectionModel());
        QItemSelection macellule(item1, item1);
        selectionModel->select(macellule, QItemSelectionModel::Select);
    }
}

int MainWindow::TST_TotBidDansGroupememnt(int bId, QString &st_grp)
{
    int ret_val = 0;
    bool status = false;
    QSqlQuery sql_1;

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

void MainWindow::TST_Graphe(stTiragesDef *pConf)
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

    zoneCentrale->addSubWindow(tabWidget);
    tabWidget->setVisible(true);
}

UnConteneurDessin * MainWindow::TST_Graphe_1(stTiragesDef *pConf)
{
    //UnConteneurDessin *
    une_vue[0] = new UnConteneurDessin;
    QString msg_2="";
    QString msg_3 = "";

    myview[0] = new MyGraphicsView(eRepartition,une_vue[0], "Tirages",Qt::white);

    msg_2="select analyses.id, lstcombi.poids, lstcombi.pos from analyses inner join lstcombi on analyses.id_poids = lstcombi.id;";
    myview[0]->DessineCourbeSql(msg_2,pConf->choixJeu,Qt::black);

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

    myview[1] = new MyGraphicsView(eParite,une_vue[1], "Parites");
    msg_2 = "select tirages.id, tirages.bp from tirages";
    myview[1]->DessineCourbeSql(msg_2,pConf->choixJeu,Qt::black,10);


    msg_2 = "select tirages.id, tirages.ep from tirages";
    myview[1]->DessineCourbeSql(msg_2,pConf->choixJeu,Qt::blue,10,100);

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

    myview[2] = new MyGraphicsView(eGroupe,une_vue[2], "b<N/2", Qt::gray);
    msg_2 = "select tirages.id, tirages.bg from tirages";
    myview[2]->DessineCourbeSql(msg_2,pConf->choixJeu,Qt::black,10);


    msg_2 = "select tirages.id, tirages.eg from tirages";
    myview[2]->DessineCourbeSql(msg_2,pConf->choixJeu,Qt::blue,10,100);

    //une_vue[2]->show();
    //zoneCentrale->addSubWindow(une_vue[2]);
    //une_vue[2]->setVisible(true);
    return (une_vue[2]);
}

//----------
void MainWindow::TST_PrevisionType(NE_FDJ::E_typeCritere cri_type, stTiragesDef *pConf)
{
    QStandardItemModel **modele;
    QLabel **label;
    QSqlQuery query;
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
            modele = G_sim_PariteVoisin;
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
