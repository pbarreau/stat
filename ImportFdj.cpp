#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QFile>
#include <QString>
#include <QTextStream>

#include <QMessageBox>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

#include <math.h>

#include "tirages.h"

QString NormaliseJour(QString input);

// pour trier la fusion superloto + loto :
// sort -o mysort.txt -t ";" -r -n -k3.9 -k3.4 -k3 nouveau_loto.csv
// http://www.cyberciti.biz/faq/linux-unix-sort-date-data-using-sortcommand/

bool GererBase::LireLesTirages(QString fileName_2, tirages *pRef)
{
    QFile fichier(fileName_2);
    QString msg = "";
    bool ret = true;

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
    QString date_tirage = "";
    QString jour_tirage = "";
    QStringList list1;

    stTiragesDef ref;
    pRef->getConfig(&ref);

    QString sql_msg = "insert into analyses (id) values (null);";
    QString st_sqrq= "";
    QString st_bind= "";
    QString st_conf= "";

    QSqlQuery sql_1;
    QSqlQuery sql_2;
    QSqlQuery sql_3;


    int zone = 0;
    int max_zone = ref.nb_zone;

    // traitement Table 1
    st_conf= GEN_FieldsForTables(&ref);
    st_conf.remove("int");
    st_conf.replace(QRegExp("\\s+"),"");
    st_bind=st_conf;
    st_bind.replace(",",", :");
    st_sqrq = "INSERT INTO tirages (id,jour_tirage,date_tirage," +
            st_conf + ")VALUES (:id, :jour, :date, :" + st_bind + ")";
    sql_1.prepare(st_sqrq);

#ifndef QT_NO_DEBUG
    //qDebug() << st_sqrq;
#endif

    // traitement Table 2
    st_conf= GEN_FieldsForTables(&ref,"t");
    st_conf.remove("int");
    st_conf.replace(QRegExp("\\s+"),"");
    st_sqrq = "INSERT INTO tirages_trie (id," +
            st_conf + ")VALUES (:id, :" + st_bind + ")";
    sql_2.prepare(st_sqrq);

#ifndef QT_NO_DEBUG
    //qDebug() << st_sqrq;
#endif


    // Construction d'une variable en fonction du max /10
    int **pRZone = new int *[max_zone]; // Pointeur de repartition des zones
    for(zone=0;zone< max_zone;zone++)
    {
        pRZone[zone]=new int[(ref.limites[zone].max/10)+1];
    }


    // Passer la premiere ligne
    ligne = flux.readLine();

#if 0
    // Analyse des suivantes
    QStringList lstConf[2];
    int nbBoules = floor(ref.limites[0].max/10);
    GEN_ListForAnalyse(lstConf,nbBoules,&ref);
#endif

    int nb_lignes=0;
    while(! flux.atEnd() && ret)
    {
        ligne = flux.readLine();
        nb_lignes++;

        //traitement de la ligne
        list1 = ligne.split(";");

        // recuperation du jour de la semaine
        jour_tirage = list1.at(1).simplified();
        sql_1.bindValue(":jour", jour_tirage);

        // Recuperation du date_tirage
        date_tirage= NormaliseJour(list1.at(2).simplified());
        sql_1.bindValue(":date", date_tirage);

        // Recuperation des boules

        int posTrie = ref.offsetFichier[1]+ref.nbElmZone[1];

        QStringList lst_trie ;
        switch(ref.choixJeu)
        {
        case NE_FDJ::fdj_euro:
            lst_trie <<  list1.at(posTrie);
            lst_trie <<  list1.at(posTrie+1);
            break;

        case NE_FDJ::fdj_loto:
            lst_trie = list1.at(posTrie).split("+");
            break;

        default:
            return false;
            break;
        }

        for(zone=0;zone< max_zone;zone++)
        {
            for(int j = 0; j < (ref.limites[zone].max/10)+1; j++)
                pRZone[zone][j]=0;

            int maxElmZone = ref.nbElmZone[zone];
            for(int ElmZone=0;ElmZone < maxElmZone;ElmZone++)
            {
                // Recuperation de la valeur
                int boule = list1.at(ref.offsetFichier[zone]+ElmZone).toInt();
                pRef->value.valBoules[zone][ElmZone]= boule;

                // Preparation pour affectation variable sql (tirages)
                QString clef_1 = ":"+ref.nomZone[zone]+QString::number(ElmZone+1);
                clef_1.replace(QRegExp("\\s+"),"");
                sql_1.bindValue(clef_1,boule);

                // Table 2
                if(lst_trie[zone].startsWith("-"))
                    lst_trie[zone].remove(0,1);

                boule=lst_trie[zone].split("-").at(ElmZone).toInt();
                sql_2.bindValue(clef_1,boule);
            }

        }

        // Mettre dans la table Tirages
        ret = sql_1.exec();
        if (ret)
        {
            // Mettre dans table ordre croissant
            ret = sql_2.exec();

            if (ret)
            {
                //nouvelle ligne dans analyse
                ret = sql_3.exec(sql_msg);

            }

#if 0
            // Analyser ce tirage
            if (ret)
            {
                ret = AnalyserCeTirage(nb_lignes, lstConf, &ref);
            }
#endif
        }

    }

    return ret;
}

bool GererBase::AnalyserCeTirage(int tirId, QStringList pList[], stTiragesDef *pRef)
{
    bool status = true;
    return status;
}

bool GererBase::AnalyserTousLesTirage(stTiragesDef *pRef)
{
    bool status = true;

    // Analyse globale
    QStringList pList[2];
    int nbBoules = floor(pRef->limites[0].max/10);
    GEN_ListForAnalyse(pList,nbBoules,pRef);

    int tirId = 1;

    if(status)
        status = FdjAnalyse_1(tirId,pRef);

    if(status)
        status = FdjAnalyse_2(tirId,pList,pRef);

    if(status)
        status = FdjAnalyse_3(tirId,pRef);


#if 0
    int total_cri = pList[0].size();
    for(int cri_id = 0 ; (cri_id < total_cri) && status; cri_id ++)
    {
        int val = RechercheInfoTirages(tirId, cri_id,pRef);
        sql_msg = "update analyses set "
                +pList[0].at(cri_id) +
                "="
                +QString::number(val) +
                " "
                "where id ="
                +QString::number(tirId)+
                ";";
#ifndef QT_NO_DEBUG
        //qDebug() << sql_msg;
#endif

        status = sql_1.exec(sql_msg);
    }

    // Recherche de consecutif dans les boules du tirage
    status = RechercheConsecutifs(tirId,pRef);
#endif

    return status;
}

bool GererBase::FdjAnalyse_1(int tirId, stTiragesDef *pRef)
{
#if EXEMPLE_SQL
    update analyses set N2= 1
            where( analyses.id in  (
                       /* debut recherche des suivants */
                       select distinct id from tirages_trie
                       where (
                           ((tb2=tb1+1))or((tb3=tb2+1))or((tb4=tb3+1))or((tb5=tb4+1)))
                       /* Fin recherche des suivants */
                       )
                   );
#endif

    bool status = true;

    QSqlQuery sql_1;
    QString st_msg = "";
    QString st_cri = "" ;
    QString st_ed1 = "or";
    QString st_ed2 = "and";


    int nbBoules = floor(pRef->limites[0].max/10);


    for(int nbSuivant=1; (nbSuivant <= nbBoules) && status; nbSuivant++)
    {
        st_cri="";

        for(int j=0;j<=nbBoules-nbSuivant;j++)
        {
            st_cri =  st_cri + "(" ;

            for(int k=j;k<j+nbSuivant;k++)
            {

                st_cri = st_cri +
                        "("
                        "tb"
                        +QString::number(k+2)
                        +"=tb"
                        +QString::number(k+1)
                        +"+1"
                         ")"
                        +st_ed2;
            }
            // Suppresion dernier mot
            st_cri.remove(st_cri.length()-st_ed2.length(),st_ed2.length());
            st_cri = st_cri + ")";

            // Rajout de or
            st_cri = st_cri + st_ed1;

        }
        // Suppresion dernier mot
        st_cri.remove(st_cri.length()-st_ed1.length(),st_ed1.length());

        st_msg = "update analyses set N"
                +QString::number(nbSuivant+1)
                + "= 1 "
                  "where( "
                  "analyses.id in  "
                  "( "
                  "/* debut recherche des suivants */ "
                  "select distinct id from tirages_trie "
                  "where "
                  "( "
                + st_cri +
                ") "
                "/* Fin recherche des suivants */ "
                ") "
                "); ";

#ifndef QT_NO_DEBUG
        //qDebug() << st_msg;
#endif

        status = sql_1.exec(st_msg);
    }

    return status;
}


bool GererBase::FdjAnalyse_2(int tirId, QStringList pList[], stTiragesDef *pRef)
{
#if EXEMPLE_SQL
    update analyses
            set P=(select y.N
                   from
                   (
                       /* Debut requete comptage */
                       select tb1.id as id, count(tb2.B) as N
                       from (select * from tirages )as tb1
                       left join
                       (select id as B from Bnrz where (z1 not null and ( z1%2=0)))as tb2
                       on
                       (
                           tb2.B = tb1.b1 or
            tb2.B = tb1.b2 or
            tb2.B = tb1.b3 or
            tb2.B = tb1.b4 or
            tb2.B = tb1.b5
            ) group by tb1.id
                       /*Fin requete comptage */
                       ) as y
                   where (y.id = analyses.id));
#endif

    bool status = true;

    QSqlQuery sql_1;
    QString st_msg = "";
    QStringList lst_col = pList[0];
    QStringList lst_cri = pList[1];


    for(int i = 0; i< lst_col.size() && status;i++)
    {
        st_msg = "update analyses set "
                + lst_col.at(i)
                +"=("
                 "select y.N from "
                 "("
                 "/* Debut requete comptage */ "
                 "select tb1.id as id, count(tb2.B) as N from "
                 "(select * from tirages )as tb1 "
                 "left join "
                 "("
                 "select id as B from Bnrz "
                 "where (z1 not null and ( "
                +lst_cri.at(i)
                +"))"
                 ")as tb2 "
                 "on "
                 "("
                 " tb2.B = tb1.b1 or "
                 " tb2.B = tb1.b2 or "
                 " tb2.B = tb1.b3 or "
                 " tb2.B = tb1.b4 or "
                 " tb2.B = tb1.b5 "
                 " ) group by tb1.id"
                 " /*Fin requete comptage */"
                 ") as y "
                 "where (y.id = analyses.id));";

#ifndef QT_NO_DEBUG
        //qDebug() << st_msg;
#endif

        status = sql_1.exec(st_msg);
    }


    return status;
}

bool GererBase::FdjAnalyse_3(int tirId, stTiragesDef *pRef)
{
#if EXEMPLE_SQL
    update analyses set C =
            (select y.pid from
             (
                 /* Debut recherche du pid des lignes des tirages */
                 select tb1.id as id, tb2.id as pid, tb2.tip as tip from analyses as tb1
                 left join (select * from lstcombi) as tb2
                 on (
                     (case when tb1.bd0=0 then tb2.b1 is null else tb1.bd0=tb2.b1 end)
                     and
                     (case when tb1.bd1=0 then tb2.b2 is null else tb1.bd1=tb2.b2 end)
                     and
                     (case when tb1.bd2=0 then tb2.b3 is null else tb1.bd2=tb2.b3 end)
                     and
                     (case when tb1.bd3=0 then tb2.b4 is null else tb1.bd3=tb2.b4 end)
                     and
                     (case when tb1.bd4=0 then tb2.b5 is null else tb1.bd4=tb2.b5 end))
                 /* fin recherche pid */
                 ) as y where(y.id = analyses.id));
#endif

    QSqlQuery sql_1;
    QString st_msg = "";
    QString st_cri = "" ;
    QString st_fin = "and";

    bool status = false;
    int nbBoules = floor(pRef->limites[0].max/10);



    for(int j=0;j<=nbBoules;j++)
    {
        //(case when tb1.U0=0 then tb2.b1 is null else tb1.U0=tb2.b1 end)
        st_cri = st_cri +
                "("
                "case when tb1.bd"
                +QString::number(j)
                +"=0 then tb2.b"
                +QString::number(j+1)
                +" is null else tb1.bd"
                +QString::number(j)
                +"=tb2.b"
                +QString::number(j+1)
                +" end"
                 ")"
                +st_fin;
    }
    // Suppresion dernier mot
    st_cri.remove(st_cri.length()-st_fin.length(),st_fin.length());

    st_msg = "update analyses set C = "
             "(select y.pid from "
             "( "
             "/* Debut recherche du pid des lignes des tirages */ "
             "select tb1.id as id, tb2.id as pid, tb2.tip as tip from analyses as tb1 "
             "left join "
             "(select * from lstcombi) as tb2 "
             "on "
             "( "
            + st_cri +
            ") "
            "/* fin recherche pid */ "
            ") as y "
            "where(y.id = analyses.id)); ";

#ifndef QT_NO_DEBUG
    //qDebug() << st_msg;
#endif

    status = sql_1.exec(st_msg);

    return status;
}




bool GererBase::OLD_LireLesTirages(QString fileName_2, tirages *pRef)
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
    pRef->getConfig(&ref);

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
    str_1.replace(QRegExp("\\s+"),""); // suppression des espaces
    str_1.replace(",",", :");
    str_1 = "INSERT INTO tirages (id," + str_2 + ")VALUES (:id, :" + str_1 + ")";
    sql_1.prepare(str_1);

    // Table des analyses
    str_1 = pRef->s_LibColAnalyse(&ref);
    str_2 = str_1;
    str_1.replace(QRegExp("\\s+"),""); // suppression des espaces
    str_1.replace(",",", :");
    str_1 = "INSERT INTO analyses (id," + str_2 + ")VALUES (:id, :" + str_1 + ")";
    sql_2.prepare(str_1);

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

        // recuperation du jour de la semaine
        jour_tirage = list1.at(1);
        sql_1.bindValue(":jour_tirage", jour_tirage);

        // Recuperation du date_tirage
        date_tirage= NormaliseJour(list1.at(2));
        sql_1.bindValue(":date_tirage", date_tirage);

        // Recuperation des boules
        for(zone=0;zone< max_zone;zone++)
        {
            for(int j = 0; j < (ref.limites[zone].max/10)+1; j++)
                pRZone[zone][j]=0;

            maxElmZone = ref.nbElmZone[zone];

            for(ElmZone=0;ElmZone < maxElmZone;ElmZone++)
            {
                // Recuperation de la valeur
                val1 = list1.at(ref.offsetFichier[zone]+ElmZone).toInt();
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

#ifndef QT_NO_DEBUG
        qDebug() << str_1;
        qDebug() << str_2;
#endif

        // Mettre dans la base
        ret = sql_1.exec();
        ret = sql_2.exec();
    }
    return ret;
}

QString NormaliseJour(QString input)
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
        // fdj euro million v1 -> AAAAMMJJ
        ladate =input.left(4) + "-"
                + input.mid(4,2)+ "-"
                + input.right(2);
    }
    return ladate;
}


