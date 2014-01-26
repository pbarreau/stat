#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#
#include "gererbase.h"

bool GererBase::CreerTableTirages(tirages *pRef)
{

QString msg1, msg2;
stTiragesDef ref;
bool ret = false;

pRef->getConfig(&ref);


msg1 = pRef->LabelColonnePourBase(&ref);


if(msg1.length() != 0){
    // Retirer le premier element
    msg1.remove("jour, ");
    msg1.replace(",", " int,");
    msg1 = msg1 + " int";

    msg1 =  "create table tirages (id integer primary key,jour TEXT, " +
            msg1 + ")";

    if (db.isOpen())
    {
        QSqlQuery query;
        ret = query.exec(msg1);

    }

}
return ret;
}

#if 0
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
        msg1 =  "create table tirages (id INTEGER PRIMARY KEY,jour TEXT, " +
                msg1 + ")";
        msg2 = "jour, " + msg2;
        tirages::col = msg2;
        query.exec(msg1);

    }
    else
    {
        return false;
    }

    return true;
}
#endif
