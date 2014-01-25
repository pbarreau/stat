#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QMessageBox>
#include <QSqlQuery>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "choixjeux.h"
#include "tirages.h"


MainWindow::MainWindow(QWidget *parent,NE_FDJ::E_typeJeux leJeu, bool load) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    int nbzone = 2;
    int *def_zones = new int[2];
    //tirages *un_tirage = NULL;
    bool bARetouner = false;


    ui->setupUi(this);

    tirages::nature = leJeu;

    if(leJeu == NE_FDJ::fdj_euro)
    {
        def_zones[0]=5;
        def_zones[1]=2;

    }
    else
    {
        def_zones[0]=5;
        def_zones[1]=1;

    }
    un_tirage = new tirages (nbzone,def_zones);

    bARetouner = CreerBaseDeDonnees();
    if(bARetouner)
    {
        closewindows = !RemplireLaBase(load);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::CreerBaseDeDonnees(void)
{
    bool bARetouner = false;

    bARetouner = OuvrirBase(&MaBaseDesTirages);

    if (bARetouner)
        CreerTables();

    return(bARetouner);
}

void MainWindow::ouvrir_mainwindows(void)
{
    if(closewindows){
        this->~MainWindow();
    }
    else
    {
        this->show();
    }
}
