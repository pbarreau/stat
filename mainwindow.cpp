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
    tirages tmp(leJeu);
    QString ficSource;

    ui->setupUi(this);


    mabase();

    DB_tirages->CreerBaseEnMemoire(true);
    DB_tirages->CreerTableTirages(&tmp);

    ficSource = tmp.SelectSource(load);
    DB_tirages->LireLesTirages(ficSource,&tmp);
    // Lecture de l'ancienne base des tirages
    ficSource="euromillions.csv";
    DB_tirages->LireLesTirages(ficSource,&tmp);

    voisins();
    //couverture();
    //DB_tirages.AfficherBase(this,PourLaBase);
    DB_tirages->AfficherBase(AfficherBase,PourLaBase);






    setCentralWidget(zoneCentrale);
    connect( PourLaBase, SIGNAL( doubleClicked(QModelIndex)) ,
             this, SLOT( cellSelected( QModelIndex) ) );



}

void MainWindow::mabase(void)
{
    zoneCentrale = new QMdiArea;
    zoneCentrale->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    zoneCentrale->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    int i;

    //QTableWidget *AfficherBase = new QTableWidget;

    //#if 0
    AfficherBase = new QWidget;
    PourLaBase = new QTableView;
    QFormLayout *layout = new QFormLayout;
    layout->addWidget(PourLaBase);
    AfficherBase->setLayout(layout);
    AfficherBase->setWindowTitle("Base des tirages");
    PourLaBase->setSelectionMode(QAbstractItemView::SingleSelection);
    PourLaBase->setStyleSheet("QTableView {selection-background-color: red;}");
    PourLaBase->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //#endif
    QMdiSubWindow *sousFenetre1 = zoneCentrale->addSubWindow(AfficherBase);

}

void MainWindow::voisins(void)
{
    int  i;
    modele = new QStandardItemModel(50,2);
    modele->setHeaderData(0,Qt::Horizontal,"Boules");
    modele->setHeaderData(1,Qt::Horizontal,"Voisin");

    for(i=1;i<=50;i++)
    {
        QStandardItem *item = new QStandardItem( QString::number(222));
        item->setData(i,Qt::DisplayRole);
        modele->setItem(i-1,0,item);
    }

    QWidget *qwVoisin = new QWidget;
    tblVoisin = new QTableView;
    tblVoisin->setModel(modele);

    tblVoisin->setSortingEnabled(true);
    tblVoisin->sortByColumn(0,Qt::AscendingOrder);

    tblVoisin->setAlternatingRowColors(true);
    tblVoisin->setEditTriggers(QAbstractItemView::NoEditTriggers);

    nbSortie = new QLabel;
    QFormLayout *layVoisin = new QFormLayout;
    tblVoisin->setColumnWidth(0,60);
    tblVoisin->setColumnWidth(1,60);
    tblVoisin->setMaximumWidth(180);
    tblVoisin->setMinimumHeight(367);
    qwVoisin->setMinimumHeight(367);
    nbSortie->setText("Nb total de sorties:");
    layVoisin->addWidget(nbSortie);
    layVoisin->addWidget(tblVoisin);
    qwVoisin->setLayout(layVoisin);
    qwVoisin->setWindowTitle("Voisins de selection");
    QMdiSubWindow *sousFenetre2 = zoneCentrale->addSubWindow(qwVoisin);

}

void MainWindow::couverture(void)
{
    int  i;
    modele2 = new QStandardItemModel(50,5);
    modele2->setHeaderData(0,Qt::Horizontal,"Boules");
    modele2->setHeaderData(1,Qt::Horizontal,"E en cours");
    modele2->setHeaderData(2,Qt::Horizontal,"E precedent");
    modele2->setHeaderData(3,Qt::Horizontal,"E Moy");
    modele2->setHeaderData(4,Qt::Horizontal,"E Max");

    for(i=1;i<=50;i++)
    {
        QStandardItem *item = new QStandardItem( QString::number(i));
        item->setData(i,Qt::DisplayRole);
        modele2->setItem(i-1,0,item);
    }

    QWidget *qwCouverture = new QWidget;
    tblCouverture = new QTableView;
    tblCouverture->setModel(modele2);

    tblCouverture->setSortingEnabled(true);
    tblCouverture->sortByColumn(0,Qt::AscendingOrder);

    tblCouverture->setAlternatingRowColors(true);
    tblCouverture->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QFormLayout *layCouverture = new QFormLayout;
    tblCouverture->setColumnWidth(0,60);
    tblCouverture->setColumnWidth(1,60);
    tblCouverture->setMaximumWidth(500);
    tblCouverture->setMinimumHeight(367);
    qwCouverture->setMinimumHeight(367);

    layCouverture->addWidget(tblCouverture);
    qwCouverture->setLayout(layCouverture);
    qwCouverture->setWindowTitle("Couverture boules");
    QMdiSubWindow *sousFenetre3 = zoneCentrale->addSubWindow(qwCouverture);

    for(i=1;i<=50;i++){
        DB_tirages->RechercheCouverture(i,modele2);
    }
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
