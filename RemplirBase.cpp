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
            maclef = " :" + ref.nomZone[zone]+ CL_PAIR;
            //val1 = list2.indexOf(maclef);
            query.bindValue(maclef.replace(QRegExp("\\s+"),""),nbPair);

            nbE1 = pRef->NbDansE1(zone);
            maclef = " :" + ref.nomZone[zone]+ CL_SGRP ;
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



void GererBase::RechercheCouverture(int boule, QStandardItemModel *modele)
{
    bool status = false;

    QSqlQuery query;
    QString msg;


    QSqlQuery selection;
    QString msg1;
    QStringList tmp;
    double EcartMoyen = 0.0;
    int nbTirages=0;
    int calcul;
    int lgndeb=0, lgnfin=0;
    int nbTotCouv = 0, EcartMax=0, EcartCourant = 0, EcartPrecedent=0;
    int a_loop = 0;

    CouvertureBase(modele);
    // recuperation du nombre de tirage total
    msg= "select count (*) from tirages";
    status = query.exec(msg);
    query.first();
    nbTirages = query.value(0).toInt();
    query.finish();


    // creation d'une table temporaire
    msg = "DROP table IF EXISTS tmp_couv";
    status = query.exec(msg);
    if(!status){
        qDebug() << "ERROR:" << query.executedQuery()  << "-" << query.lastError().text();
    }
    //status = query.isActive();
    //query.finish();
    //status = query.isActive();

    msg =  "create table tmp_couv (id INTEGER PRIMARY KEY, depart int, fin int, taille int)";
    status = query.exec(msg);
    if(!status){
        qDebug() << "ERROR:" << query.executedQuery()  << "-" << query.lastError().text();
    }
    //status = query.isActive();
    //status = query.next();
    //status = query.isValid();
    //query.finish();
    //status = query.isActive();
    //query.clear();
    //status = query.isActive();
    //status = db.commit();



    // requete a effectuer
    msg = "insert into tmp_couv (id, depart, fin, taille) values (:id, :depart, :fin, :taille)";
    query.prepare(msg);

    // Recuperation des lignes ayant la boule
    msg = "select id from tirages where (b1=" + QString::number(boule) +
            " or b2=" + QString::number(boule) +
            " or b3=" + QString::number(boule) +
            " or b4=" + QString::number(boule) +
            " or b5=" + QString::number(boule) + ")";
    status = selection.exec(msg);


    //Partir de la fin des tirages connus
    selection.last(); // derniere ligne ayant le numero
    if(selection.isValid()){
        lgndeb = nbTirages;
        nbTirages = 0; //calcul des intervales
        a_loop = 1;
        do
        {
            QSqlRecord rec  = selection.record();
            calcul = rec.value(0).toInt();
            lgnfin = selection.value(0).toInt();

            query.bindValue(":depart", lgndeb);
            query.bindValue(":fin", lgnfin);
            query.bindValue(":taille", lgndeb-lgnfin+1);
            // Mettre dans la base
            status = query.exec();

            nbTirages += (lgndeb-lgnfin);
            lgndeb = lgnfin-1;
            a_loop++;
        }while(selection.previous());
        selection.finish();
        query.finish();
    }

    // Rajouter une ligne pour ecart le plus recent
    lgnfin = 1;
    query.bindValue(":depart", lgndeb);
    query.bindValue(":fin", lgnfin);
    query.bindValue(":taille", lgndeb-lgnfin+1);
    // Mettre dans la base
    status = query.exec();
    nbTirages += (lgndeb-lgnfin);

    // calcul des ecarts pour la boule
    msg = "select count (*) from tmp_couv";
    status = query.exec(msg);
    query.first();
    nbTotCouv = query.value(0).toInt();

    // Moyenne
    if(a_loop>0)
        EcartMoyen = nbTirages/a_loop;

    // recherche l'ecart le plus grand
    msg = "select max(taille) from tmp_couv";
    status = query.exec(msg);
    query.first();
    EcartMax = query.value(0).toInt();

    //recherche de l'ecart courant et suivant
    msg = "select taille from tmp_couv";
    status = query.exec(msg);
    query.last();
    EcartCourant = query.value(0).toInt();
    query.previous();
    EcartPrecedent = query.value(0).toInt();

    //QBrush macouleur(Qt::green);
    QStandardItem *item1 = new QStandardItem;
    item1->setData(EcartCourant,Qt::DisplayRole);
    //item1->setBackground(macouleur);
    modele->setItem(boule-1,1,item1);

    QStandardItem *item2 = new QStandardItem( QString::number(222));
    item2->setData(EcartPrecedent,Qt::DisplayRole);
    modele->setItem(boule-1,2,item2);

    QStandardItem *item3 = new QStandardItem( QString::number(EcartMoyen,'g',6));
    item3->setData(EcartMoyen,Qt::DisplayRole);
    modele->setItem(boule-1,3,item3);


    QStandardItem *item4 = new QStandardItem( QString::number(222));
    item4->setData(EcartMax,Qt::DisplayRole);
    modele->setItem(boule-1,4,item4);


}


void GererBase::RechercheVoisin(int boule, QLabel *l_nb, QStandardItemModel *modele)//QStandardItemModel *modele)
{
    QSqlQuery query(db);
    QString msg;
    int calcul = 0, voisin = 0;
    int rp1 = 0, rp2 = 0, rn1 = 0, rn2 = 0;
    int resu = 0;

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
        // Boule sortant avec
        msg = "select count (*) from r_boul where (b1=" + QString::number(voisin) + " or b2=" + QString::number(voisin)
                + " or b3=" + QString::number(voisin) + " or b4=" + QString::number(voisin) + " or b5=" + QString::number(voisin) + ")";
        calcul = query.exec(msg);
        query.first();
        //QSqlRecord rec  = query.record();
        resu = query.value(0).toInt();

        QStandardItem *item = new QStandardItem( QString::number(222));
        item->setData(resu,Qt::DisplayRole);
        modele->setItem(voisin-1,1,item);

        rn1 = TotalRechercheVoisinADistanceDe(1,voisin);
        rp1 = TotalRechercheVoisinADistanceDe(-1,voisin);
        rn2 = TotalRechercheVoisinADistanceDe(2,voisin);
        rp2 = TotalRechercheVoisinADistanceDe(-2,voisin);

        calcul = rp1 + rp2 + rn1 + rn2;
        QStandardItem *item2 = new QStandardItem( QString::number(222));
        item2->setData(calcul,Qt::DisplayRole);
        modele->setItem(voisin-1,2,item2);

        calcul =calcul + resu;
        QStandardItem *item3 = new QStandardItem( QString::number(222));
        item3->setData(calcul,Qt::DisplayRole);
        modele->setItem(voisin-1,3,item3);
    }

    // Recherche terminee finir avec cette vue
    msg = "drop view r_boul";
    query.exec(msg);

}

int GererBase::TotalRechercheVoisinADistanceDe(int dist, int voisin)
{
    QString msg = "";
    QSqlQuery query(db);
    int calcul = 0;
    bool status;

    // Selection des lignes voisines de celle de reference
    msg = "create view rn1 as select * from tirages inner join r_boul on tirages.id = r_boul.id + %1";
    msg = (msg).arg(dist);
    status = query.exec(msg);

    // Comptage des lignes ayant la boule comme voisine
    msg = "select count (*) from rn1 where (b1=" + QString::number(voisin) + " or b2=" + QString::number(voisin)
            + " or b3=" + QString::number(voisin) + " or b4=" + QString::number(voisin) + " or b5=" + QString::number(voisin) + ")";
    status = query.exec(msg);
    query.first();
    calcul = query.value(0).toInt();

    // On detruit la vue des resultats de cette boule
    msg = "drop view rn1";
    status = query.exec(msg);

    return calcul;
}

void GererBase::CouvertureBase(QStandardItemModel *dest)
{
    QSqlQuery query;
    QSqlQuery sauve;
    QString msg = "";
    bool depart_couverture = false;
    int lgndeb=0, lgnfin=0;
    int memo_boule[50]= {0};
    int nb_boules = 0;
    int i = 0, j= 0;
    bool status;

    // recuperer tous les tirages
    msg= "select * from tirages";
    status = query.exec(msg);

    // requete a effectuer
    msg = "insert into couverture (id, depart, fin, taille) values (:id, :depart, :fin, :taille)";
    sauve.prepare(msg);

    // Se positionner au debut des tirages du jeu
    query.last();
    depart_couverture = true;
    if(query.isValid())
    {
        do
        {
            QSqlRecord rec  = query.record();

            // Sauvegarder le debut de couverture
            if(depart_couverture){
                lgndeb = rec.value(0).toInt();
                sauve.bindValue(":depart", lgndeb);

                depart_couverture = false;
            }

            for(i = 0; i<5;i++)
            {
                int boule = rec.value(2+i).toInt();

                if(!memo_boule[boule-1])
                {
                    nb_boules++;
                }

                memo_boule[boule-1]++;

                if(nb_boules == 50)
                {
                    // remettre a zero compteur apparition
                    nb_boules = 0;
                    memset(memo_boule, 0, 50*sizeof(int));

                    lgnfin = rec.value(0).toInt();
                    sauve.bindValue(":fin", lgnfin);
                    sauve.bindValue(":taille", lgndeb-lgnfin+1);
                    // Mettre dans la base
                    status = sauve.exec();
                    if(!status){
                        qDebug() << "ERROR:" << query.executedQuery()  << "-" << query.lastError().text();
                    }

                    if( i != 4){
                        lgndeb = lgnfin;
                        for(j=0;j<i;j++){
                            if(i !=j)
                            {
                                nb_boules++;
                                memo_boule[rec.value(2+j).toInt()-1]++;
                            }
                        }
                    }
                    else
                    {
                        lgndeb = lgnfin-1;
                    }
                    sauve.bindValue(":depart", lgndeb);
                }
            }
        }while(query.previous());
        // On a parcouru toute la base
        // indiquer les absents
        for(i = 0; i< 50 ;i++)
        {
            if (!memo_boule[i])
            {
                QBrush macouleur(Qt::green);
                QStandardItem *item1 = dest->item(i);
                item1->setBackground(macouleur);
            }
        }
    }
}
