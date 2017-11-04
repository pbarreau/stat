#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QFile>
#include <QString>
#include <QStringList>
#include <math.h>
#include <QProcess>

#include <QTextStream>

#include <QMessageBox>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

#include "tirages.h"

QString DateAnormer(QString input);
QString JourFromDate(QString LaDate, QString verif);

// pour trier la fusion superloto + loto :
// sort -o mysort.txt -t ";" -r -n -k3.9 -k3.4 -k3 nouveau_loto.csv
// http://www.cyberciti.biz/faq/linux-unix-sort-date-data-using-sortcommand/
bool GererBase::LireLesTirages(QString fileName_2, tirages *pRef)
{
    QFile fichier(fileName_2);
    QString msg = "";
    QString clef_1= "";
    QString clef_2= "";
    bool ret = false;

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
    QString ligne = "";
    QStringList list1;
    stTiragesDef ref;
    pRef->getConfigFor(&ref);

    QString str_1 = "";
    QString str_2 = "";
    //QString cln_tirages = pRef->s_LibColBase(&ref);
    //QString LesColonnes = cln_tirages;

    QSqlQuery sql_1(db);
    QSqlQuery sql_2(db);

    int nbPair = 0;
    int nbE1 = 0;

    // Variable pour garder valeur des lignes
    QString date_tirage = "";
    QString jour_tirage = "";
    int zone = 0;
    int ElmZone = 0;
    int max_zone = ref.nb_zone;
    int maxElmZone = 0;
    int val1 = 0;
    int val2 = 0;

    // Table des tirages
    str_1 = pRef->s_LibColBase(&ref);
    str_2 = str_1;
#ifndef QT_NO_DEBUG
    qDebug() << str_1;
#endif

    str_1.replace(QRegExp("\\s+"),""); // suppression des espaces
    str_1.replace(",",", :");
#ifndef QT_NO_DEBUG
    qDebug() << str_1;
#endif

    str_1 = "INSERT INTO tirages (id," + str_2 + ")VALUES (:id, :" + str_1 + ")";
    sql_1.prepare(str_1);
#ifndef QT_NO_DEBUG
    qDebug() << str_1;
#endif

    // Table des analyses
    str_1 = pRef->s_LibColAnalyse(&ref);
    str_2 = str_1;
#ifndef QT_NO_DEBUG
    qDebug() << str_1;
#endif

    str_1.replace(QRegExp("\\s+"),""); // suppression des espaces
    str_1.replace(",",", :");
#ifndef QT_NO_DEBUG
    qDebug() << str_1;
#endif

    str_1 = "INSERT INTO analyses (id," + str_2 + ")VALUES (:id, :" + str_1 + ")";
    sql_2.prepare(str_1);
#ifndef QT_NO_DEBUG
    qDebug() << str_1;
#endif


    // Construction d'une variable en fonction du max /10
    int **pRZone = new int *[max_zone]; // Pointeur de repartition des zones
    for(zone=0;zone< max_zone;zone++)
    {
        pRZone[zone]=new int[(ref.limites[zone].max/10)+1];
    }

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
#if 0
        // recuperation du jour de la semaine
        jour_tirage = list1.at(1);
        sql_1.bindValue(":jour_tirage", jour_tirage);
#endif
        // Recuperation du date_tirage
        date_tirage= DateAnormer(list1.at(2));
        sql_1.bindValue(":date_tirage", date_tirage);

        // Calcul du jour de la semaine
        jour_tirage = JourFromDate(date_tirage, list1.at(1));
        sql_1.bindValue(":jour_tirage", jour_tirage);

        // Recuperation des boules
        for(zone=0;zone< max_zone;zone++)
        {
            for(int j = 0; j < (ref.limites[zone].max/10)+1; j++)
                pRZone[zone][j]=0;

            maxElmZone = ref.nbElmZone[zone];

            for(ElmZone=0;ElmZone < maxElmZone;ElmZone++)
            {
                // Recuperation de la valeur
                if(fileName_2!="euromillions_4.csv")
                {
                    val1 = list1.at(ref.offsetFichier[zone]+ElmZone).toInt();
                }
                else
                {
                    val1 = list1.at(ref.offsetFichier[zone]+1+ElmZone).toInt();
                }
                pRef->value.valBoules[zone][ElmZone]= val1;

                // Preparation pour affectation variable sql (tirages)
                clef_1 = ":"+ref.nomZone[zone]+QString::number(ElmZone+1);
                clef_1.replace(QRegExp("\\s+"),"");
                sql_1.bindValue(clef_1,val1);

                // incrementation du compteur unite/dizaine
                pRZone[zone][val1/10]++;
            }

            for(int j = 0; j < (ref.limites[zone].max/10)+1; j++)
            {
                val2 = pRZone[zone][j];
                // Preparation pour affectation variable sql (analyses)
                clef_2 = ":"+ref.nomZone[zone]+"d"+QString::number(j);
                clef_2.replace(QRegExp("\\s+"),"");
                sql_2.bindValue(clef_2,val2);
            }

            // Calcul perso a mettre dans la base
            // Automatisation possible ?????
            nbPair = pRef->RechercheNbBoulesPairs(zone);
            clef_1 = " :" + ref.nomZone[zone]+ CL_PAIR;
            sql_1.bindValue(clef_1.replace(QRegExp("\\s+"),""),nbPair);

            nbE1 = pRef->RechercheNbBoulesDansGrp1(zone);
            clef_1 = " :" + ref.nomZone[zone]+ CL_SGRP ;
            sql_1.bindValue(clef_1.replace(QRegExp("\\s+"),""),nbE1);

        }
        // Mettre dans la base
        ret = sql_1.exec();
        ret = sql_2.exec();
    }
    return true;
}

QString DateAnormer(QString input)
{
    // La fonction doit retourner une date au format  AAAA-MM-JJ
    // http://fr.wikipedia.org/wiki/ISO_8601
    QString ladate = "";

    // regarder la taille de la date
    if (input.size()==10){
        // fdj euro million v2 -> JJ/MM/AAAA
        QStringList tmp = input.split("/");
        ladate = tmp.at(2) + "-"
                + tmp.at(1)+ "-"
                + tmp.at(0);
    }
    else
    {
        if(input.contains("/"))
        {
            // fdj euro million v2 -> JJ/MM/AA
            QStringList tmp = input.split("/");
            ladate = "20" + tmp.at(2) + "-"
                    + tmp.at(1)+ "-"
                    + tmp.at(0);
        }
        else
        {
            // fdj euro million v1 -> AAAAMMJJ
            ladate =input.left(4) + "-"
                    + input.mid(4,2)+ "-"
                    + input.right(2);

        }
    }
    return ladate;
}

QString JourFromDate(QString LaDate, QString verif)
{
    // http://algor.chez.com/date/date.htm
    QString tab[] = {"MARDI","MERCREDI","JEUDI","VENDREDI","SAMEDI","DIMANCHE","LUNDI"};
    QStringList tmp = LaDate.split("-");
    QString retval = "";

    int anne = tmp.at(0).toInt();
    int mois = tmp.at(1).toInt();
    int date = tmp.at(2).toInt();

    int JS = 0;
    double JD = 0.0;
    double  s = 0.0;

    if(mois < 3)
    {
        mois+=12;
        anne--;
    }

    s = anne/100;
    JD = (1720996.5 - s) + (s / 4) + floor(365.25*anne) + floor(30.6001*(mois+1)) + date;
    JD = JD - floor(JD/7)*7;

    JS = (int)floor(JD)%7;

    retval = tab [JS];

    if(retval != verif.trimmed())
    {
        s = s+1;
    }

    return retval;
}

bool GererBase::NEW_AnalyseLesTirages(tirages *pRef)
{
    bool status = false;
    QSqlQuery sql_1;
    QSqlQuery sql_all;
    QString clef_1= "";
    QString sAllTirages = "";

    stTiragesDef ref;
    pRef->getConfigFor(&ref);

    // Table des analyses
    QString str_1 = pRef->s_LibColAnalyse(&ref);
    QString str_2 = str_1;
#ifndef QT_NO_DEBUG
    qDebug() << str_1;
#endif

    str_1.replace(QRegExp("\\s+"),""); // suppression des espaces
    str_1.replace(",",", :");
#ifndef QT_NO_DEBUG
    qDebug() << str_1;
#endif


    str_1 = "INSERT INTO analyses (id," + str_2 + ")VALUES (:id, :" + str_1 + ")";
    sql_1.prepare(str_1);
#ifndef QT_NO_DEBUG
    qDebug() << str_1;
#endif

    int max_zone = ref.nb_zone;
    // Construction d'une variable en fonction du max /10
    int **pRZone = new int *[max_zone]; // Pointeur de repartition des zones
    for(int zone=0;zone< max_zone;zone++)
    {
        pRZone[zone]=new int[(ref.limites[zone].max/10)+1];
    }


    // Requete pour recuperer tous les tirages
    sAllTirages = "select * from tirages; ";

    status = sql_all.exec(sAllTirages);
    if (!status)
    {
        return status;
    }

    status = sql_all.first();
    while(status)
    {
        // Recuperation des boules
        for(int zone=0;zone< max_zone;zone++)
        {
            for(int j = 0; j < (ref.limites[zone].max/10)+1; j++)
                pRZone[zone][j]=0;

            int maxElmZone = ref.nbElmZone[zone];

            for(int ElmZone=0;ElmZone < maxElmZone;ElmZone++)
            {
                QSqlRecord ligne = sql_all.record();
                QString champ = ref.nomZone[zone]+QString::number(ElmZone+1);
                int val1 = ligne.value(champ).toInt();

                // incrementation du compteur unite/dizaine
                pRZone[zone][val1/10]++;
            }

            for(int j = 0; j < (ref.limites[zone].max/10)+1; j++)
            {
                int val2 = pRZone[zone][j];
                // Preparation pour affectation variable sql (analyses)
                clef_1 = ":"+ref.nomZone[zone]+"d"+QString::number(j);
                clef_1.replace(QRegExp("\\s+"),"");
                sql_1.bindValue(clef_1,val2);
            }

        }
        // Mettre dans la base
        status = sql_1.exec();

        //passer a ligne suivante
        status = sql_all.next();
    }

    return status;
}

bool GererBase::NEW_LireLesTirages(int file_id,tiragesFileFormat *unTableau, tirages *pRef)
{
    bool status = false;
    QSqlQuery sql_1;
    QString clef_1= "";

    QFile fichier(unTableau->name);
    if (!fichier.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox msgBox;
        clef_1 = "Auto chargement : " + (unTableau->name) + "\nEchec!!";
        msgBox.setText(clef_1);
        msgBox.exec();
        return false;

        // Tentative de telechargemnt
        //wget https://media.fdj.fr/generated/game/loto/loto2017.zip  --no-check-certificate
        //unzip -o loto2017.zip
        QProcess process;
        QString maCommande = "wget https://media.fdj.fr/generated/game/loto/";
        QString file = ((unTableau->name).split(".")).at(0);
        maCommande = maCommande + file+".zip --no-check-certificate";
        process.start(maCommande);
        maCommande = "unzip -o "+file+".zip";
        process.start(maCommande);
        if (!fichier.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            return true;
        }
    }


    QTextStream flux(&fichier);
    QString ligne = "";
    QStringList list1;

    stTiragesDef ref;
    pRef->getConfigFor(&ref);

    QString str_1 = pRef->s_LibColBase(&ref);
    QString str_2 = str_1;
#ifndef QT_NO_DEBUG
    qDebug() << str_1;
#endif
    str_1.replace(QRegExp("\\s+"),""); // suppression des espaces
    str_1.replace(",",", :");
#ifndef QT_NO_DEBUG
    qDebug() << str_1;
#endif

    str_1 = "INSERT INTO v_tirages (" + str_2 + ",file)VALUES (:" + str_1 + ", :file);";
    sql_1.prepare(str_1);
#ifndef QT_NO_DEBUG
    qDebug() << str_1;
#endif

    // --- DEBUT ANALYSE DU FICHIER
    QString date_tirage = "";
    QString jour_tirage = "";

    // Passer la premiere ligne
    ligne = flux.readLine();


    // Analyse des suivantes
    int nb_lignes=0;
    status = true;
    while((! flux.atEnd()) && (status == true))
    {
        ligne = flux.readLine();
        nb_lignes++;

        //traitement de la ligne
        list1 = ligne.split(";");


        // Recuperation du date_tirage
        date_tirage= DateAnormer(list1.at(unTableau->o_date));
        sql_1.bindValue(":date_tirage", date_tirage);

        // Calcul du jour de la semaine
        jour_tirage = JourFromDate(date_tirage, list1.at(unTableau->o_day));
        sql_1.bindValue(":jour_tirage", jour_tirage);

        // Recuperation des boules
        for(int zone=0;zone< (unTableau->nbZone);zone++)
        {
            int maxElmZone = unTableau->o_znsz[zone];
            for(int ElmZone=0;ElmZone < maxElmZone;ElmZone++)
            {
                int val1 = list1.at(unTableau->o_zone[zone]+ElmZone).toInt();

                pRef->value.valBoules[zone][ElmZone]= val1;

                // Preparation pour affectation variable sql (tirages)
                clef_1 = ":"+ref.nomZone[zone]+QString::number(ElmZone+1);
                clef_1.replace(QRegExp("\\s+"),"");
                sql_1.bindValue(clef_1,val1);
            }

            // Toutes les clefs sont faites ?
            sql_1.bindValue(":file",file_id);

            // Calcul perso a mettre dans la base
            // Automatisation possible ?????
            int nbPair = pRef->RechercheNbBoulesPairs(zone);
            clef_1 = " :" + ref.nomZone[zone]+ CL_PAIR;
            sql_1.bindValue(clef_1.replace(QRegExp("\\s+"),""),nbPair);

            int nbE1 = pRef->RechercheNbBoulesDansGrp1(zone);
            clef_1 = " :" + ref.nomZone[zone]+ CL_SGRP ;
            sql_1.bindValue(clef_1.replace(QRegExp("\\s+"),""),nbE1);
        }

        // Mettre dans la base
        status = sql_1.exec();

    }


    return status;
}
