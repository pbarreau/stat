#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QSqlDatabase>
#include <QSqlRecord>

#include <QSqlQuery>
#include <QMessageBox>
#include <QFile>

#include "gererbase.h"
#include "tirages.h"



bool GererBase::LireLesTirages(QString fileName_2, tirages *pRef)
{
    QFile fichier(fileName_2);
    QString msg;
    QString maclef;

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
    //QStringList list2;
    stTiragesDef ref;
    pRef->getConfig(&ref);
    QString col_info = pRef->LabelColonnePourBase(&ref);
    QString LesColonnes = col_info;
    QSqlQuery query(db);
    int nbPair = 0;
    int nbE1 = 0;

    // Variable pour garder valeur des lignes
    QString jour;
    int zone = 0;
    int ElmZone = 0;
    int max_zone = ref.nb_zone;
    int maxElmZone = 0;
    //int val = 0;
    int val1 = 0;

    col_info.replace(QRegExp("\\s+"),""); // suppression des espaces
    col_info.replace(",",", :");
    //list2 = col_info.split(",");

    col_info = "INSERT INTO tirages (id," + LesColonnes + ")VALUES (:id, :" + col_info + ")";
    query.prepare(col_info);

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
        for(zone=0;zone< max_zone;zone++)
        {
            maxElmZone = ref.nbElmZone[zone];

            for(ElmZone=0;ElmZone < maxElmZone;ElmZone++)
            {
                val1 = list1.at(ref.offsetFichier[zone]+ElmZone).toInt();
                pRef->value.valBoules[zone][ElmZone]= val1;
                maclef = ":"+ref.nomZone[zone]+QString::number(ElmZone+1);
                maclef.replace(QRegExp("\\s+"),"");
                query.bindValue(maclef,val1);
            }

            // Calcul perso a mettre dans la base
            // Automatisation possible ?????
            nbPair = pRef->NbPairs(zone);
            maclef = " :" + ref.nomZone[zone]+ "_pair";
            //val1 = list2.indexOf(maclef);
            query.bindValue(maclef.replace(QRegExp("\\s+"),""),nbPair);

            nbE1 = pRef->NbDansE1(zone);
            maclef = " :" + ref.nomZone[zone]+ "_E1";
            //val1 = list2.indexOf(maclef);
            query.bindValue(maclef.replace(QRegExp("\\s+"),""),nbE1);

        }
        // Mettre dans la base
        query.exec();
    }
    return true;
}

#if 0
void GererBase::RechercheCouverture(tirages *pRef)
{
    stTiragesDef ref;
    pRef->getConfig(&ref);
    int zone = 0;
    int boule = 0;
    int depart = 2;
    int first = 0;
    int memo[3][ref.limites[zone].max];
    //0 compteur
    for(boule=0;boule<ref.limites[zone].max;boule++)
    {
        memo[0][boule]=0;
        memo[1][boule]=0;
        memo[2][boule]=0;
    }

    QSqlQuery query(db);
    QString msg = "select * from tirages";
    query.exec(msg);

    int val ;

    query.last();
    do
    {
        QSqlRecord rec  = query.record();
        for(boule=depart;boule<(depart+ref.nbElmZone[zone]);boule++)
        {
            val = rec.value(boule).toInt();
            pRef->value.valBoules[zone][boule-depart]=val;

            if(!memo[0][val-1]){
                // une boule trouvee
                memo[0][val-1]++;
                memo[1][first]=val;
                first++;
            }
            if(start >= ref.limites[zone].max)
            {
                // une couverture trouvee
                start = 0;
                for(boule=0;boule<ref.limites[zone].max;boule++)
                {
                    memo[1][boule]=0;
                }

            }
        }


    }while(query.previous());

}

#endif

void GererBase::RechercheCouverture(tirages *pRef)
{
    stTiragesDef ref;
    pRef->getConfig(&ref);
    int zone = 0;
    int depart = 2;
    int max_boule = ref.limites[zone].max;
    int nb_boule =ref.nbElmZone[zone];
    int boule;
    int memo_boule= 0;
    int max = 0;
    int i;
    int val;
    int calcul;

    QSqlQuery query(db);
    QString msg = "select * from tirages";
    query.exec(msg);

    int nbSortie[5][50];

    // mise a zero des compteurs
    for(i=0;i<5;i++)
    {
        for(boule=0;boule<50;boule++)
        {
            nbSortie[i][boule]=0;
        }
    }

    // Recuperer dernier tirage
    query.first();
    QSqlRecord rec  = query.record();
    for(i=depart;i< nb_boule;i++)
    {
        val = rec.value(i).toInt();
        pRef->value.valBoules[zone][i-depart]=val;

        // Recherche du maximum pour cette boule
        msg = "create view r_boul as select * from tirages where (b1=" +
                QString::number(val) + " or b2=" + QString::number(val)
                + " or b3=" + QString::number(val) + " or b4=" + QString::number(val) + " or b5=" + QString::number(val) + ")";
        calcul = query.exec(msg);
        msg = "select count (*) from r_boul";
        calcul = query.exec(msg);
        query.first();
        QSqlRecord rec  = query.record();
        calcul = rec.value(0).toInt();

        for(boule = 1; boule<=max_boule;boule++)
        {
            msg = "select count (*) from r_boul where (b1=" + QString::number(boule) + " or b2=" + QString::number(boule)
                    + " or b3=" + QString::number(boule) + " or b4=" + QString::number(boule) + " or b5=" + QString::number(boule) + ")";
            calcul = query.exec(msg);
            query.first();
            QSqlRecord rec  = query.record();
            calcul = rec.value(0).toInt();

            nbSortie[i-depart][boule-1] = calcul;

            if(calcul > max)
            {
                memo_boule = boule;
                max = calcul;
            }
        }

        // Recherche terminee finir avec cette vue
        msg = "drop view r_boul";
        query.exec(msg);
    }
    // resultat
}
