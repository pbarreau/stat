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
#include "gererbase.h"

MainWindow::MainWindow(QWidget *parent,NE_FDJ::E_typeJeux leJeu, bool load) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    GererBase DB_tirages;
    tirages tmp(leJeu);

    DB_tirages.CreerBaseEnMemoire(false);
    DB_tirages.CreerTableTirages(&tmp);

    ui->setupUi(this);



}

MainWindow::~MainWindow()
{
    delete ui;
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
