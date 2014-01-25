#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QApplication>
#include <QSqlDatabase>
#include <QMessageBox>
#include "mainwindow.h"

bool MainWindow::OuvrirBase(QSqlDatabase *db)
{
    *db = QSqlDatabase::addDatabase("QSQLITE");


    //db->setDatabaseName(":memory:");
    db->setDatabaseName("mabase.sqlite");
    if (!db->open()) {
        QMessageBox::critical(0, qApp->tr("Echec ouverture base !!"),
                              qApp->tr("Base Sql Lite, probleme de creation !!.\n\n"
                                       "Cancel pour finir."), QMessageBox::Cancel);
        return false;
    }
    else{
#ifndef QT_NO_DEBUG
    qDebug()<< "Ouverture Base Ok!\n";
#endif
        return true;
    }
}
