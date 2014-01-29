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


    //QLineEdit *nom = new QLineEdit;
    QFormLayout *layout = new QFormLayout;
    layout->addWidget(PourLaBase);
    AfficherBase->setLayout(layout);

    QMdiSubWindow *sousFenetre1 = zoneCentrale->addSubWindow(AfficherBase);


    AfficherBase->setWindowTitle("Base des tirages");
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
