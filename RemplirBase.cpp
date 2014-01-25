#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include "mainwindow.h"
#include "tirages.h"

bool MainWindow::RemplireLaBase(bool load)
{
    QString fileName_2 ="";
    QString msg = "";
    NE_FDJ::E_typeJeux eNatureJeu = tirages::nature;

    // Choix du fichier contenant les tirages
    switch(eNatureJeu)
    {
    case NE_FDJ::fdj_euro:
        msg = "Fichier pour Euro million";
        fileName_2 = "euromillions_2.csv";
        break;
    case NE_FDJ::fdj_loto:
        msg="Fichier pour Loto";
        fileName_2="nouveau_loto.csv";
        break;
    default:
        break;
    }

    // Selection d'un fichier et non utilisation du defaut
    if(!load)
    {
        // Menu selection fichier avec chemin
        fileName_2=QFileDialog::getOpenFileName(this,
                                                tr(qPrintable(msg)), QDir::currentPath(), tr("Fdj (*.csv);;Perso (*.*)"));

    }

    QFile fichier(fileName_2);

    // On ouvre notre fichier en lecture seule et on vérifie l'ouverture
    if (!fichier.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox msgBox;
        msg = "Auto chargement : " + fileName_2 + "\nEchec!!";
        msgBox.setText(msg);
        msgBox.exec();
        return false;
    }


    QTextStream flux(&fichier);
    QString ligne;
    QStringList list1;
    QStringList list2;
    QString col_info = tirages::col;
    QSqlQuery query(MaBaseDesTirages);


    col_info.replace(QRegExp("\\s+"),""); // supression des espaces
    col_info.replace(",",", :");
    list2 = col_info.split(",");

    col_info = "INSERT INTO tirages (id," + tirages::col + ")VALUES (:" + col_info + ")";
    query.prepare(col_info);


    // Variable pour garder valeur des lignes
    QString jour;
    tirages tmp_tir(tirages::nb_zone, tirages::nb_elem_zone);
    int zone = 0;
    int ElmZone = 0;
    int max_zone = tirages::nb_zone;
    int maxElmZone = 0;
    int start = 4;

    // Passer la premiere ligne
    ligne = flux.readLine();


    // Analyse des suivantes
    int nb_lignes=0;
    while(! flux.atEnd())
    {
        ligne = flux.readLine();
        nb_lignes++;

        //traitement de la ligne
        list1 = ligne.split(";");

        // Recuperation du jour
        jour= list1.at(2);
        query.bindValue(":jour", jour);

        // Recuperation des boules
        start = 4;
        for(zone=0;zone< max_zone;zone++)
        {
            maxElmZone = tmp_tir.nb_elem_zone[zone];

            for(ElmZone=0;ElmZone < maxElmZone;ElmZone++)
            {
                tmp_tir.val_zone[zone][ElmZone]= list1.at(start).toInt();
                query.bindValue(list2.at(start-3), list1.at(start).toInt());
                start++;
            }
        }
        // Mettre dans la base
        query.exec();
    }
    return true;
}
