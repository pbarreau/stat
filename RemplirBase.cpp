#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QSqlDatabase>
#include <QSqlRecord>

#include <QSqlQuery>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

#include "gererbase.h"
#include "tirages.h"

QString NormaliseJour(QString input);

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
    QString col_info = pRef->s_LibColBase(&ref);
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
        jour= NormaliseJour(list1.at(2));
        //jour = list1.at(2);

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

#endif

void GererBase::RechercheCouverture(int boule, QStandardItemModel *modele)
{
    db.open();
    QSqlQuery query(db);
    QSqlQuery selection(db);
    QString msg, msg1;
    double EcartMoyen = 0.0;
    int SommeTotal;
    int calcul;
    int lgndeb, lgnfin;
    int nbTotCouv = 0, EcartMax=0, EcartCourant = 0, EcartPrecedent=0;

    // Effacer donnees de la table de couverture
    msg =  "drop  table tmp_couv ";
    query.exec(msg);
    msg =  "create table tmp_couv (id INTEGER PRIMARY KEY, depart int, fin int, taille int)";
    query.exec(msg);

    // recuperation du nombre de tirage total
    msg= "select count (*) from tirages";
    calcul = query.exec(msg);
    query.first();
    SommeTotal = query.value(0).toInt();

    msg = "insert into tmp_couv (id, depart, fin, taille) values (:id, :depart, :fin, :taille)";
    query.prepare(msg);

    msg1 = "select id from tirages where (b1=" +
            QString::number(boule) + " or b2=" + QString::number(boule)
            + " or b3=" + QString::number(boule) + " or b4=" + QString::number(boule) + " or b5=" + QString::number(boule) + ")";
    calcul = selection.exec(msg1);


    //ligne actuelle
    lgndeb = SommeTotal;
    SommeTotal = 0; //calcul des intervals
    selection.last();
    do
    {
        QSqlRecord rec  = selection.record();
        calcul = rec.value(0).toInt();

        lgnfin = selection.value(0).toInt();
        query.bindValue(":depart", lgndeb);
        query.bindValue(":fin", lgnfin);
        query.bindValue(":taille", lgndeb-lgnfin+1);
        SommeTotal += (lgndeb-lgnfin+1);
        // Mettre dans la base
        query.exec();
        lgndeb = lgnfin-1;

    }while(selection.previous());

    // calcul des ecarts pour la boule
    msg = "select count (*) from tmp_couv";
    query.exec(msg);
    query.first();
    nbTotCouv = query.value(0).toInt();

    // Moyenne
    if(nbTotCouv>0)
        EcartMoyen = SommeTotal/nbTotCouv;

    // recherche l'ecart le plus grand
    msg = "select max(taille) from tmp_couv";
    query.exec(msg);
    query.first();
    EcartMax = query.value(0).toInt();

    //recherche de l'ecart courant et suivant
    msg = "select taille from tmp_couv";
    query.exec(msg);
    query.last();
    EcartCourant = query.value(0).toInt();
    query.previous();
    EcartPrecedent = query.value(0).toInt();
#if 0
    if(lgndeb ==  0){
        EcartCourant = 0;
    }
    else{
        EcartCourant = query.value(2).toInt() -1;
    }
    //query.previous();
    EcartPrecedent = query.value(3).toInt();;
#endif

    QStandardItem *item1 = new QStandardItem;
    item1->setData(EcartCourant,Qt::DisplayRole);
    modele->setItem(boule-1,1,item1);

#if 0
    QStandardItem *item2 = new QStandardItem( QString::number());
    item2->setData(EcartPrecedent,Qt::DisplayRole);
    modele->setItem(boule-1,2,item2);

    QStandardItem *item3 = new QStandardItem( QString::number(222));
    item3->setData(EcartMoyen,Qt::DisplayRole);
    modele->setItem(boule-1,3,item3);

    QStandardItem *item4 = new QStandardItem( QString::number(222));
    item4->setData(EcartMax,Qt::DisplayRole);
    modele->setItem(boule-1,4,item4);
#endif
    // Recherche terminee finir avec cette vue
    //msg = "drop view r_couv";
    //query.exec(msg);

}

void GererBase::RechercheVoisin(int boule, QLabel *l_nb, QStandardItemModel *modele)//QStandardItemModel *modele)
{
    QSqlQuery query(db);
    QString msg;
    int calcul, voisin;

    // Recherche du maximum pour cette boule
    msg = "create view r_boul as select * from tirages where (b1=" +
            QString::number(boule) + " or b2=" + QString::number(boule)
            + " or b3=" + QString::number(boule) + " or b4=" + QString::number(boule) + " or b5=" + QString::number(boule) + ")";
    calcul = query.exec(msg);
    msg = "select count (*) from r_boul";
    calcul = query.exec(msg);
    query.first();
    QSqlRecord rec  = query.record();
    calcul = rec.value(0).toInt();

    l_nb->setText(QString("Boule %1 : %2 fois ").arg( boule ).arg(calcul) );

    // Recherche des voisins de la boule
    for(voisin=1;voisin<=50;voisin++)
    {
        msg = "select count (*) from r_boul where (b1=" + QString::number(voisin) + " or b2=" + QString::number(voisin)
                + " or b3=" + QString::number(voisin) + " or b4=" + QString::number(voisin) + " or b5=" + QString::number(voisin) + ")";
        calcul = query.exec(msg);
        query.first();
        //QSqlRecord rec  = query.record();
        calcul = query.value(0).toInt();

        QStandardItem *item = new QStandardItem( QString::number(222));
        item->setData(calcul,Qt::DisplayRole);
        modele->setItem(voisin-1,1,item);

        // Recherche de l'ecart de cette boule
        msg = "select * from tirages where (b1=" + QString::number(voisin) +
                " or b2=" + QString::number(voisin)+
                " or b3=" + QString::number(voisin) +
                " or b4=" + QString::number(voisin) +
                " or b5=" + QString::number(voisin) + ") LIMIT 1";
        calcul = query.exec(msg);
        query.first();
        calcul = query.value(0).toInt() -1 ;
        QStandardItem *item2 = new QStandardItem( QString::number(222));
        item2->setData(calcul,Qt::DisplayRole);
        modele->setItem(voisin-1,2,item2);

        // recherche des voisins a n-1
        msg = "create view rn1 as select * from tirages inner join r_boul on tirages.id = r_boul.id + 1";
        calcul = query.exec(msg);
        msg = "select count (*) from rn1 where (b1=" + QString::number(voisin) + " or b2=" + QString::number(voisin)
                + " or b3=" + QString::number(voisin) + " or b4=" + QString::number(voisin) + " or b5=" + QString::number(voisin) + ")";
        calcul = query.exec(msg);
        query.first();
        //QSqlRecord rec  = query.record();
        calcul = query.value(0).toInt();

        QStandardItem *item3 = new QStandardItem( QString::number(222));
        item3->setData(calcul,Qt::DisplayRole);
        modele->setItem(voisin-1,3,item3);
        msg = "drop view rn1";
        query.exec(msg);


        // recherche des voisins a n-2
        msg = "create view rn1 as select * from tirages inner join r_boul on tirages.id = r_boul.id + 2";
        calcul = query.exec(msg);
        msg = "select count (*) from rn1 where (b1=" + QString::number(voisin) + " or b2=" + QString::number(voisin)
                + " or b3=" + QString::number(voisin) + " or b4=" + QString::number(voisin) + " or b5=" + QString::number(voisin) + ")";
        calcul = query.exec(msg);
        query.first();
        //QSqlRecord rec  = query.record();
        calcul = query.value(0).toInt();

        QStandardItem *item4 = new QStandardItem( QString::number(222));
        item4->setData(calcul,Qt::DisplayRole);
        modele->setItem(voisin-1,4,item4);
        msg = "drop view rn1";
        query.exec(msg);

    }

    // Recherche terminee finir avec cette vue
    msg = "drop view r_boul";
    query.exec(msg);

}
