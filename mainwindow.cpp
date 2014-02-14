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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "choixjeux.h"
#include "tirages.h"
#include "gererbase.h"

MainWindow::MainWindow(QWidget *parent,NE_FDJ::E_typeJeux leJeu, bool load) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    DB_tirages = new GererBase;
    int i;
    tirages tmp(leJeu);
    QString ficSource;

    ui->setupUi(this);

    zoneCentrale = new QMdiArea;
    zoneCentrale->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    zoneCentrale->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Creation sous fenetre pour mettre donnees de base
    mabase();

    // Creation sous fenetre des voisins
    voisins();

    // Creation sous fenetre des ecarts
    couverture();

    // Preparer la base de données
    DB_tirages->CreerBaseEnMemoire(true);
    DB_tirages->CreerTableTirages(&tmp);



    // Recuperation des données
    ficSource = tmp.SelectSource(load);
    DB_tirages->LireLesTirages(ficSource,&tmp);
    // Lecture des anciennes base des tirages
    ficSource = "euromillions_2.csv";
    DB_tirages->LireLesTirages(ficSource,&tmp);
    ficSource="euromillions.csv";
    DB_tirages->LireLesTirages(ficSource,&tmp);

    //// GARDER L'ORDRE D'APPEL DES FONCTIONS PB VERROU SUR LA BASE
    // Remplir Sous Fen les ecarts
    for(i=1;i<=50;i++){
        DB_tirages->RechercheCouverture(i,modele2);
    }

    // Remplir la sousfenetre base de données
    DB_tirages->AfficherBase(AfficherBase,PourLaBase);




    setCentralWidget(zoneCentrale);
    connect( PourLaBase, SIGNAL( doubleClicked(QModelIndex)) ,
             this, SLOT( cellSelected( QModelIndex) ) );



}

void MainWindow::mabase(void)
{
    AfficherBase = new QWidget;
    PourLaBase = new QTableView;
    QFormLayout *layout = new QFormLayout;

    layout->addWidget(PourLaBase);
    AfficherBase->setLayout(layout);
    AfficherBase->setWindowTitle("Base des tirages");

    // Gestion du QTableView
    PourLaBase->setSelectionMode(QAbstractItemView::SingleSelection);
    PourLaBase->setStyleSheet("QTableView {selection-background-color: red;}");
    PourLaBase->setEditTriggers(QAbstractItemView::NoEditTriggers);
    PourLaBase->setAlternatingRowColors(true);

    // Ratacher cette sous fenetre
    zoneCentrale->addSubWindow(AfficherBase);
    //QMdiSubWindow *sousFenetre1 = zoneCentrale->addSubWindow(AfficherBase);

}

void MainWindow::voisins(void)
{
    int  i;
    QWidget *qwVoisin = new QWidget;
    tblVoisin = new QTableView;
    modele = new QStandardItemModel(50,5);

    // entete du modele
    modele->setHeaderData(0,Qt::Horizontal,"B");
    modele->setHeaderData(1,Qt::Horizontal,"V:r0");
    modele->setHeaderData(2,Qt::Horizontal,"V:r1+r2");
    modele->setHeaderData(3,Qt::Horizontal,"V:n-1");
    modele->setHeaderData(4,Qt::Horizontal,"V:n-2");


    // Ecriture du numero de boule
    for(i=1;i<=50;i++)
    {
        QStandardItem *item = new QStandardItem( QString::number(222));
        item->setData(i,Qt::DisplayRole);
        modele->setItem(i-1,0,item);
    }

    tblVoisin->setModel(modele);
    tblVoisin->setColumnWidth(0,60);
    tblVoisin->setColumnWidth(1,60);
    tblVoisin->setColumnWidth(2,60);
    tblVoisin->setColumnWidth(3,60);
    tblVoisin->setColumnWidth(4,60);
    //tblVoisin->setMaximumWidth(260);
    tblVoisin->setMinimumHeight(367);
    tblVoisin->setSortingEnabled(true);
    tblVoisin->sortByColumn(0,Qt::AscendingOrder);
    tblVoisin->setAlternatingRowColors(true);
    tblVoisin->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tblVoisin->hideColumn(3);
    tblVoisin->hideColumn(4);

    nbSortie = new QLabel;
    nbSortie->setText("Nb total de sorties:");

    QFormLayout *layVoisin = new QFormLayout;
    layVoisin->addWidget(nbSortie);
    layVoisin->addWidget(tblVoisin);

    qwVoisin->setMinimumHeight(367);
    qwVoisin->setLayout(layVoisin);
    qwVoisin->setWindowTitle("Voisins de selection");
    //QMdiSubWindow *sousFenetre2 =
    zoneCentrale->addSubWindow(qwVoisin);

}

void MainWindow::couverture(void)
{
    int  i;
    QWidget *qwCouverture = new QWidget;
    tblCouverture = new QTableView;
    modele2 = new QStandardItemModel(50,5);

    modele2->setHeaderData(0,Qt::Horizontal,"B"); // Boules
    modele2->setHeaderData(1,Qt::Horizontal,"Ec"); // Ecart en cours
    modele2->setHeaderData(2,Qt::Horizontal,"Ep"); // ECart precedent
    modele2->setHeaderData(3,Qt::Horizontal,"Em"); // Ecart Moyen
    modele2->setHeaderData(4,Qt::Horizontal,"EM"); // Ecart Maxi

    for(i=1;i<=50;i++)
    {
        QStandardItem *item = new QStandardItem( QString::number(i));
        item->setData(i,Qt::DisplayRole);
        modele2->setItem(i-1,0,item);
    }

    tblCouverture->setModel(modele2);
    tblCouverture->setColumnWidth(0,45);
    tblCouverture->setColumnWidth(1,45);
    tblCouverture->setColumnWidth(2,45);
    tblCouverture->setColumnWidth(3,45);
    tblCouverture->setColumnWidth(4,45);
    tblCouverture->setSortingEnabled(true);
    tblCouverture->sortByColumn(0,Qt::AscendingOrder);
    tblCouverture->setAlternatingRowColors(true);
    tblCouverture->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //tblCouverture->setMaximumWidth(500);
    tblCouverture->setMinimumHeight(367);

    QFormLayout *layCouverture = new QFormLayout;
    layCouverture->addWidget(tblCouverture);


    qwCouverture->setMinimumHeight(367);
    qwCouverture->setLayout(layCouverture);
    qwCouverture->setWindowTitle("Couverture boules");
    //QMdiSubWindow *sousFenetre3 =
    zoneCentrale->addSubWindow(qwCouverture);

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

void MainWindow::cellSelected(const QModelIndex & index)
{
    int val;
#if 0
    QMessageBox::information(this, "",
                             "Cell at row "+QString::number(index.row())+
                             " column "+QString::number(index.column())+
                             " was double clicked.");
#endif
    val = index.data().toInt();
    tblVoisin->sortByColumn(0,Qt::AscendingOrder);
    DB_tirages->RechercheVoisin(val,nbSortie,modele);

}
