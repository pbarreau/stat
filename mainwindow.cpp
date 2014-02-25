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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "choixjeux.h"
#include "tirages.h"
#include "gererbase.h"

//QStandardItemModel *GererBase::modele2_0 ;

MainWindow::MainWindow(QWidget *parent,NE_FDJ::E_typeJeux leJeu, bool load) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    DB_tirages = new GererBase;
    int i;
    tirages tmp(leJeu);
    stTiragesDef configJeu;
    QString ficSource;
    QTableView *qtv_MesChoix = new QTableView;

    ui->setupUi(this);

    zoneCentrale = new QMdiArea;
    zoneCentrale->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    zoneCentrale->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Creation sous fenetre pour mettre donnees de base
    fen_Tirages();

    // Creation sous fenetre des voisins
    fen_Voisins();

    // Creation sous fenetre des ecarts
    fen_Ecarts();

    // Creation fenetre pour memoriser a selection
    fen_MaSelection(qtv_MesChoix);

    // Preparer la base de données
    DB_tirages->CreerBaseEnMemoire(false);

    // Recuperation des contantes du type de jeu
    tmp.getConfig(&configJeu);

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





    setCentralWidget(zoneCentrale);

    // Double click dans sous fenetre base
    connect( qtv_Tirages, SIGNAL( doubleClicked(QModelIndex)) ,
             this, SLOT( TirageBouleRechercheVoisins( QModelIndex) ) );

    // Double click dans sous fenetre ecart
    connect( qtv_Ecarts, SIGNAL( doubleClicked(QModelIndex)) ,
             this, SLOT( EcartBouleRechercheVosins( QModelIndex) ) );

    // click dans fenetre voisin pour afficher boule
    connect( qtv_MesChoix, SIGNAL( doubleClicked(QModelIndex)) ,
             this, SLOT( UneSelectionActivee( QModelIndex) ) );


    // Double click dans sous fenetre ma selection
    connect( qtv_Voisins, SIGNAL( clicked(QModelIndex)) ,
             this, SLOT( MontrerBouleDansBase( QModelIndex) ) );

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

void MainWindow::fen_Voisins(void)
{
    int  i;
    QWidget *qw_Voisins = new QWidget;
    qtv_Voisins = new QTableView;
    qsim_Voisins = new QStandardItemModel(50,6);

    // entete du modele
    qsim_Voisins->setHeaderData(0,Qt::Horizontal,"B");
    qsim_Voisins->setHeaderData(1,Qt::Horizontal,"V:r0");
    qsim_Voisins->setHeaderData(2,Qt::Horizontal,"V:p1");
    qsim_Voisins->setHeaderData(3,Qt::Horizontal,"V:p2");
    qsim_Voisins->setHeaderData(4,Qt::Horizontal,"V:n1");
    qsim_Voisins->setHeaderData(5,Qt::Horizontal,"V:n2");

    // Ecriture du numero de boule
    for(i=1;i<=50;i++)
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
    qsim_Ecarts = new QStandardItemModel(50,5);
    //modele2 = GererBase::modele2_0;

    qsim_Ecarts->setHeaderData(0,Qt::Horizontal,"B"); // Boules
    qsim_Ecarts->setHeaderData(1,Qt::Horizontal,"Ec"); // Ecart en cours
    qsim_Ecarts->setHeaderData(2,Qt::Horizontal,"Ep"); // ECart precedent
    qsim_Ecarts->setHeaderData(3,Qt::Horizontal,"Em"); // Ecart Moyen
    qsim_Ecarts->setHeaderData(4,Qt::Horizontal,"EM"); // Ecart Maxi

    for(i=1;i<=50;i++)
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

    qsim_MaSelection = new QStandardItemModel(5,10);
#if 0
    qsim_Ecarts->setHeaderData(0,Qt::Horizontal,"B"); // Boules
    qsim_Ecarts->setHeaderData(1,Qt::Horizontal,"Ec"); // Ecart en cours
    qsim_Ecarts->setHeaderData(2,Qt::Horizontal,"Ep"); // ECart precedent
    qsim_Ecarts->setHeaderData(3,Qt::Horizontal,"Em"); // Ecart Moyen
    qsim_Ecarts->setHeaderData(4,Qt::Horizontal,"EM"); // Ecart Maxi
#endif

    for(i=1;i<=5;i++)
    {
        for(j=1;j<=10;j++)
        {
            cell_val = j+(i-1)*10;
            if(cell_val<=50){
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

    //qDebug () <<  "H=" + QString::number (qtv_MaSelection->minimumHeight());
    //qDebug () << "W=" + QString::number(qtv_MaSelection->minimumWidth());

    qw_MaSelection->setLayout(layCouverture);
    qw_MaSelection->setWindowTitle("Ma Selection");
    //QMdiSubWindow *sousFenetre3 =
    zoneCentrale->addSubWindow(qw_MaSelection);

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

void MainWindow::TirageBouleRechercheVoisins(const QModelIndex & index)
{
    int val;
#if 0
    QMessageBox::information(this, "",
                             "Cell at row "+QString::number(index.row())+
                             " column "+QString::number(index.column())+
                             " was double clicked.");
#endif
    if(index.column()>0 && index.column()<6){
        val = index.data().toInt();
        qtv_Voisins->sortByColumn(0,Qt::AscendingOrder);
        DB_tirages->RechercheVoisin(val,nbSortie,qsim_Voisins);
    }

}

void MainWindow::EcartBouleRechercheVosins(const QModelIndex & index)
{
    int val = 0;

    if(index.column()==0){
        val = index.data().toInt();
        qtv_Voisins->sortByColumn(0,Qt::AscendingOrder);
        DB_tirages->RechercheVoisin(val,nbSortie,qsim_Voisins);
    }
}

void MainWindow::UneSelectionActivee(const QModelIndex & index)
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

void MainWindow::MontrerBouleDansBase(const QModelIndex & index)
{
    int val = 0;
#if 0
    QModelIndex item1 = qtv_Tirages->currentIndex();
    item1 = item1.model()->index(0,3);
    qtv_Tirages->setCurrentIndex(item1);
#endif

    val = qsim_Voisins->index(index.row(),0).data().toInt();
#ifndef QT_NO_DEBUG
    qDebug() << QString::number(index.row());
#endif
    DB_tirages->MontrerLaBoule(val,qtv_Tirages);
}
