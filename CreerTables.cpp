#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QString>

#include "mainwindow.h"
#include "tirages.h"

QString tirages::col;

bool MainWindow::CreerTables()
{
    QSqlQuery query(MaBaseDesTirages);
    QString msg1 = "";
    QString msg2 = "";
    char tab[2]={'b','e'};
    int i =0, j=0;
    int max_zone = tirages::nb_zone;
    int *zn_conf = tirages::nb_elem_zone;
    int elem;

    // creation du message pour les colonnes
    for(i=0;i<max_zone;i++)
    {
        elem = zn_conf[i];
        for(j=0;j<elem;j++)
        {
            msg1 = msg1 + tab[i]+QString::number(j+1) +" int,";
            msg2 = msg2 + tab[i]+QString::number(j+1) +",";
        }
    }

    if(msg1.length() != 0){
        msg1.remove(msg1.size()-1,1);
        msg2.remove(msg2.size()-1,1);
        msg1 =  "create table tirages (id int primary key,jour TEXT, " +
                msg1 + ")";
        msg2 = "jour, " + msg2;
        tirages::col = msg2;
        query.exec(msg1);

#if 0
        if(query.isValid())
        {
            QMessageBox::information(this, "OK", "Requête effectuée.");
        }
        else
        {
            QMessageBox::warning(this, tr("erreur"), tr("Requete NON effectuée") + MaBaseDesTirages.lastError().text());
        }
#endif
    }
    else
    {
        return false;
    }

    return true;
}

#if  0
bool MainWindow::CreerTables(NE_FDJ::E_typeJeux eNatureJeu)
{
    QSqlQuery query(MaBaseDesTirages);
    QString msg;

    switch(eNatureJeu)
    {
    case NE_FDJ::fdj_loto:
        msg = "create table tirages (id int primary key,jour TEXT, date TEXT, "
                "b1 int, b2 int, b3 int, b4 int, b5 int, e1 int";
        break;

    case NE_FDJ::fdj_euro:
        msg = "create table tirages (id int primary key, jour TEXT, date TEXT,"
                "b1 int, b2 int, b3 int, b4 int, b5 int, e1 int, e2 int";

        break;

    default:
        msg ="";
        break;
    }

    if(msg ==""){
        return false;
    }
    else
    {
        query.exec(msg);
        return true;
    }

}



#endif

