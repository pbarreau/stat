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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "choixjeux.h"
#include "tirages.h"
#include "gererbase.h"

MainWindow::MainWindow(QWidget *parent,NE_FDJ::E_typeJeux leJeu, bool load) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    GererBase DB_tirages;
    tirages tmp(leJeu);
    QString ficSource;

    ui->setupUi(this);

    zoneCentrale = new QMdiArea;
    zoneCentrale->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    zoneCentrale->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    QWidget *AfficherBase = new QWidget;
    QTableView *PourLaBase = new QTableView;
    QFormLayout *layout = new QFormLayout;
    layout->addWidget(PourLaBase);
    AfficherBase->setLayout(layout);
    AfficherBase->setWindowTitle("Base des tirages");
    PourLaBase->setSelectionMode(QAbstractItemView::SingleSelection);
    PourLaBase->setStyleSheet("QTableView {selection-background-color: red;}");
    PourLaBase->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QMdiSubWindow *sousFenetre1 = zoneCentrale->addSubWindow(AfficherBase);


    QStandardItemModel *modele = new QStandardItemModel(50,2);
    modele->setHeaderData(0,Qt::Horizontal,"Boules");
    modele->setHeaderData(1,Qt::Horizontal,"Voisin");
    //QAbstractItemView::NoEditTriggers

    QStandardItem *item = new QStandardItem( QString::number(222));
    modele->setItem(10,0,item);
    QStandardItem *item2 = new QStandardItem( QString::number(240));
    modele->setItem(8,0,item2);

    QWidget *qwVoisin = new QWidget;
    QTableView *tblVoisin = new QTableView;
    tblVoisin->setModel(modele);
    tblVoisin->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tblVoisin->sortByColumn(1);
    QLabel *nbSortie = new QLabel;
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

    setCentralWidget(zoneCentrale);



    DB_tirages.CreerBaseEnMemoire(true);
    //tmp.getConfig(&ref);
    DB_tirages.CreerTableTirages(&tmp);
    ficSource = tmp.SelectSource(load);
    DB_tirages.LireLesTirages(ficSource,&tmp);

    // Lecture de l'ancienne base des tirages
    ficSource="euromillions.csv";
    DB_tirages.LireLesTirages(ficSource,&tmp);

    // Recherche de couverture
    //DB_tirages.RechercheCouverture(&tmp);


    //DB_tirages.AfficherBase(this,PourLaBase);
    DB_tirages.AfficherBase(AfficherBase,PourLaBase);

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
