#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include <QFile>

#include "gererbase.h"
#include "tirages.h"



bool GererBase::LireLesTirages(QString fileName_2, tirages *pRef)
{
    QFile fichier(fileName_2);
    QString msg;

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
    stTiragesDef ref;
    pRef->getConfig(&ref);
    QString col_info = pRef->LabelColonnePourBase(&ref);
    QString LesColonnes = col_info;
    QSqlQuery query(db);


    col_info.replace(QRegExp("\\s+"),""); // suppression des espaces
    col_info.replace(",",", :");
    list2 = col_info.split(",");

    col_info = "INSERT INTO tirages (id," + LesColonnes + ")VALUES (:id, :" + col_info + ")";
    query.prepare(col_info);

    // Variable pour garder valeur des lignes
    QString jour;
    int zone = 0;
    int ElmZone = 0;
    int max_zone = ref.nb_zone;
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
        start = 4; // A verifier pour un autre type de fichier : ie loto
        for(zone=0;zone< max_zone;zone++)
        {
            maxElmZone = ref.nbElmZone[zone];

            for(ElmZone=0;ElmZone < maxElmZone;ElmZone++)
            {
                //pRef->value.valBoules[zone][ElmZone]= list1.at(start).toInt();
                query.bindValue(list2.at(start-3), list1.at(start).toInt());
                start++;
            }
        }
        // Mettre dans la base
        query.exec();
    }
    return true;
}
