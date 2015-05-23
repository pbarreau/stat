#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QSqlDatabase>
#include <QSqlRecord>

#include <QSqlQuery>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QMenu>
#include <QAction>

#include "gererbase.h"
#include "tirages.h"
#include "labelclickable.h"

QString req_msg(int zone, int boule, stTiragesDef *ref);
// 1,$s/ r"/ r_"\+pConf->nomZone[zone]\+"_"/g

void GererBase::TotalApparitionBoule(int boule, stTiragesDef *pConf, int zone, QStandardItemModel *modele)
{
    QStringList lst_boule;
    const int d[3]={-1,-2,0};
    const int col[3]={6,7,8};
    //const QColor fond[3]={QColor(255,156,86,167),Qt::green,Qt::cyan};
    const QColor fond[3]={QColor(255,156,86,167),QColor(140,255,124,167),QColor(10,255,50,167)};


#if 0
    bool status = false;

    QSqlQuery query;
    QString msg="";

    // Recherche du maximum pour cette boule
    msg= TST_ZoneRequete(pConf, zone,"or",boule,"=");
    msg = "select count (*)  from tirages where (" +msg+ ");";
    status = query.exec(msg);

    if(status){
        query.first();

        if(query.isValid())
        {
            QSqlRecord rec  = query.record();
            int value = rec.value(0).toInt();

            // Mettre le resultat dans tableau voisins
            QStandardItem *item2 = modele->item(boule-1,8);
            item2->setData(value,Qt::DisplayRole);
            item2->setBackground(QBrush(Qt::cyan));
            modele->setItem(boule-1,8,item2);

        }
        query.finish();
    }
#endif

    // Total a n+1 et n+2 et n
    // Rechercher pour la boule les voisins identique
    lst_boule<< QString::number(boule);
    for(int ref = 0; ref <3 ; ref ++)
    {
        // Recuperer pointeur de cellule
        QStandardItem *item1 = modele->item(boule-1,col[ref]);
        int val = 0;
        // Rechercher max pour la boule
        if(d[ref]==0)
        {
            bool status = false;

            QSqlQuery query;
            QString msg="";

            // Recherche du maximum pour cette boule
            msg= TST_ZoneRequete(pConf, zone,"or",boule,"=");
            msg = "select count (*)  from tirages where (" +msg+ ");";
            status = query.exec(msg);

            if(status){
                query.first();

                if(query.isValid())
                {
                    QSqlRecord rec  = query.record();
                    val = rec.value(0).toInt();
                }
                query.finish();
            }

        }
        else
        {
            val = TST_TotalRechercheVoisinADistanceDe(zone,pConf,d[ref],boule,lst_boule);
        }

        // Mettre la valeur trouvee dans le tableau des voisins
        item1->setData(val,Qt::DisplayRole);
        item1->setBackground(QBrush(fond[ref]));
        modele->setItem(boule-1,col[ref],item1);
    }
}

void GererBase::DistributionSortieDeBoule(int boule, QStandardItemModel *modele,stTiragesDef *pRef)
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


    // recuperation du nombre de tirage total
    msg= "select count (*)  from tirages";
    status = query.exec(msg);
    query.first();
    nbTirages = query.value(0).toInt();
    query.finish();


    // creation d'une table temporaire
    msg = "DROP table IF EXISTS tmp_couv";
    status = query.exec(msg);
#ifndef QT_NO_DEBUG
    if(!status){
        qDebug() << "ERROR:" << query.executedQuery()  << "-" << query.lastError().text();
    }
#endif

    msg =  "create table tmp_couv (id INTEGER PRIMARY KEY, depart int, fin int, taille int)";
    status = query.exec(msg);
#ifndef QT_NO_DEBUG
    if(!status){
        qDebug() << "ERROR:" << query.executedQuery()  << "-" << query.lastError().text();
    }
#endif



    // requete a effectuer
    msg = "insert into tmp_couv (id, depart, fin, taille) values (:id, :depart, :fin, :taille)";
    query.prepare(msg);

    // Recuperation des lignes ayant la boule
    msg = req_msg(0,boule,pRef);
    status = selection.exec(msg);


    //Partir de la fin des tirages trouves
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
        //selection.finish();
        //query.finish();
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
    msg = "select count (*)  from tmp_couv";
    status = query.exec(msg);
    query.first();
    nbTotCouv = query.value(0).toInt();

    // Moyenne
    if(a_loop>0)
        EcartMoyen = double(nbTirages)/a_loop;

    // recherche l'ecart le plus grand
    msg = "select max(taille)  from tmp_couv";
    status = query.exec(msg);
    query.first();
    EcartMax = query.value(0).toInt();

    //recherche de l'ecart courant et suivant
    msg = "select taille  from tmp_couv";
    status = query.exec(msg);
    query.last();
    EcartCourant = query.value(0).toInt();
    query.previous();
    EcartPrecedent = query.value(0).toInt();

    QStandardItem *item1 = new QStandardItem;
    item1->setData(EcartCourant,Qt::DisplayRole);
    modele->setItem(boule-1,1,item1);

    QStandardItem *item2 = new QStandardItem( QString::number(222));
    item2->setData(EcartPrecedent,Qt::DisplayRole);
    modele->setItem(boule-1,2,item2);

    QStandardItem *item3 = new QStandardItem( );
    QString valEM = QString::number(EcartMoyen,'g',2);
    //item3->setData(EcartMoyen,Qt::DisplayRole);
    item3->setData(valEM.toDouble(),Qt::DisplayRole);
    modele->setItem(boule-1,3,item3);


    QStandardItem *item4 = new QStandardItem( QString::number(222));
    item4->setData(EcartMax,Qt::DisplayRole);
    modele->setItem(boule-1,4,item4);
}

QString GererBase::TST_ZoneRequete(stTiragesDef *pConf, int zone, QString operateur, int boule, QString critere)
{
    QString ret_msg = "";

    // Operateur : or | and
    // critere : = | <>
    // b1=0 or b2=0 or ..
    for(int i = 0; i<pConf->nbElmZone[zone];i++)
    {
        ret_msg = ret_msg
                + pConf->nomZone[zone]+QString::number(i+1)
                + critere + QString::number(boule)
                + " " + operateur+ " ";
    }
    int len_flag = operateur.length();
    ret_msg.remove(ret_msg.length()-len_flag-1, len_flag+1);

    return ret_msg;
}

void GererBase::AfficherMaxOccurenceBoule(int boule, int zn, stTiragesDef *pConf, LabelClickable *l_nb)
{
    QSqlQuery query(db);
    QString msg;
    int value = 0;
    bool status = false;

    msg= TST_ZoneRequete(pConf, zn,"or",boule,"=");

    msg = "select count (*)  from"
          "("
          "select *  from tirages where ("+ msg+ ")"
                                                 ");";
#if 0
    select count (*)  from (select *  from tirages where (b1=40 or b2=40 or b3=40 or b4=40 or b5=40 ));
#endif
    status = query.exec(msg);
    if(status)
    {
        query.first();

        if(query.isValid())
        {

            QSqlRecord rec  = query.record();
            value = rec.value(0).toInt();

            l_nb->setText(QString("Boule %1 : %2 fois ").arg( boule ).arg(value) );
        }
    }

#if 0
    // Recherche du maximum pour cette boule
#if 0
    msg = "create view r_boul as select *  from tirages where (b1=" +
            QString::number(boule) + " or b2=" + QString::number(boule)
            + " or b3=" + QString::number(boule) + " or b4=" + QString::number(boule) + " or b5=" + QString::number(boule) + ");";
#endif
    msg = "create view r_boul as select *  from tirages where ("
            +msg+");";

    status = query.exec(msg);

    if(status){
        msg = "select count (*) from r_boul";
        status = query.exec(msg);

        if(status){
            query.first();

            if(query.isValid()){

                QSqlRecord rec  = query.record();
                value = rec.value(0).toInt();

                l_nb->setText(QString("Boule %1 : %2 fois ").arg( boule ).arg(value) );
            }
        }
    }
#endif
}

void GererBase::MontrerResultatRechercheVoisins(QStandardItemModel *modele,int zone, stTiragesDef *pConf,int b_id)
{
    QSqlQuery query;
    QString msg;
    bool status = true;

    msg = "select * from r_"+pConf->nomZone[zone]+"_"+QString::number(b_id) + " ;";
    status = query.exec(msg);

    if(status)
    {
        status = query.first();
        if(query.isValid())
        {
            int position = 0;
            modele->sort(0);//
            do{
                QSqlRecord ligne = query.record();
                int r0 = ligne.value(1).toInt();
                int rp1 = ligne.value(2).toInt();
                int rp2 = ligne.value(3).toInt();
                int rn1 = ligne.value(4).toInt();
                int rn2= ligne.value(5).toInt();

                QStandardItem *item_0 = modele->item(position,0);
                item_0->setData(position+1,Qt::DisplayRole);
                modele->setItem(position,0,item_0);

                QStandardItem *item_1 = modele->item(position,1);
                item_1->setData(r0,Qt::DisplayRole);
                modele->setItem(position,1,item_1);

                QStandardItem *item_2 = modele->item(position,2);
                item_2->setData(rp1,Qt::DisplayRole);
                modele->setItem(position,2,item_2);

                QStandardItem *item_3 = modele->item(position,3);
                item_3->setData(rp2,Qt::DisplayRole);
                modele->setItem(position,3,item_3);

                QStandardItem *item_4 = modele->item(position,4);
                item_4->setData(rn1,Qt::DisplayRole);
                modele->setItem(position,4,item_4);

                QStandardItem *item_5 = modele->item(position,5);
                item_5->setData(rn2,Qt::DisplayRole);
                modele->setItem(position,5,item_5);

                position++;
            }while(query.next());
        }

    }

}
//------------
void GererBase::MontrerResultatRechercheAbsent(QStandardItemModel *modele,int zone, stTiragesDef *pConf,int b_id)
{
    QSqlQuery query;
    QString msg;
    bool status = true;

    msg = "select * from abs_"+pConf->nomZone[zone]+"_"+QString::number(b_id) + " ;";
    status = query.exec(msg);

    if(status)
    {
        status = query.first();
        if(query.isValid())
        {
            int position = 0;
            modele->sort(0);//
            do{
                QSqlRecord ligne = query.record();
                int r0 = ligne.value(1).toInt();
                int rp1 = ligne.value(2).toInt();
                int rp2 = ligne.value(3).toInt();
                int rn1 = ligne.value(4).toInt();
                int rn2= ligne.value(5).toInt();

                QStandardItem *item_0 = modele->item(position,0);
                item_0->setData(position+1,Qt::DisplayRole);
                modele->setItem(position,0,item_0);

                QStandardItem *item_1 = modele->item(position,1);
                item_1->setData(r0,Qt::DisplayRole);
                modele->setItem(position,1,item_1);

                QStandardItem *item_2 = modele->item(position,2);
                item_2->setData(rp1,Qt::DisplayRole);
                modele->setItem(position,2,item_2);

                QStandardItem *item_3 = modele->item(position,3);
                item_3->setData(rp2,Qt::DisplayRole);
                modele->setItem(position,3,item_3);

                QStandardItem *item_4 = modele->item(position,4);
                item_4->setData(rn1,Qt::DisplayRole);
                modele->setItem(position,4,item_4);

                QStandardItem *item_5 = modele->item(position,5);
                item_5->setData(rn2,Qt::DisplayRole);
                modele->setItem(position,5,item_5);

                position++;
            }while(query.next());
        }

    }

}


//-----
void GererBase::RechercherAbsentDeLaBoule(int b_id, int zone, stTiragesDef *pConf, int max_absents)
{
    QSqlQuery query;
    QString msg;
    bool status = true;
    int r0 =0, rn1=0,rn2=0,rp1=0,rp2=0;

    // Recherche des voisins de la boule
    for(int absent=1;(absent<=max_absents) && status;absent++)
    {
        if(absent != b_id)
        {
            // Boule absente ? 1:oui, 0:non
            r0 = RechercheAbsentADistanceDe(0,b_id,zone,pConf, absent
                                            );
        }
        else
        {
            r0 = 1;
        }

        rn1 = RechercheAbsentADistanceDe(1,b_id,zone,pConf,absent);
        rp1 = RechercheAbsentADistanceDe(-1,b_id,zone,pConf,absent);
        rn2 = RechercheAbsentADistanceDe(2,b_id,zone,pConf,absent);
        rp2 = RechercheAbsentADistanceDe(-2,b_id,zone,pConf,absent);

        // mise a jour dans la base
        msg = "update abs_"+pConf->nomZone[zone]+"_"+QString::number(b_id) + " " +
                "set r0=" +QString::number(r0)+ ", " +
                "rp1=" +QString::number(rp1)+ ", " +
                "rp2=" +QString::number(rp2)+ ", " +
                "rn1=" +QString::number(rn1)+ ", " +
                "rn2=" +QString::number(rn2)+ " " +
                "where (id="+QString::number(absent)+");";
        status = query.exec(msg);
    }
}
//------
void GererBase::RechercherVoisinDeLaBoule(int b_id, int zone, stTiragesDef *pConf, int max_voisins)
{
    QSqlQuery query;
    QString msg;
    bool status = true;
    int r0 =0, rn1=0,rn2=0,rp1=0,rp2=0;

    // Recherche des voisins de la boule
    for(int voisin=1;(voisin<=max_voisins) && status;voisin++)
    {
        if(voisin != b_id)
        {
            // Boule sortant avec
            r0 = TotalRechercheVoisinADistanceDe(0,b_id,zone,pConf, voisin);
        }
        else
        {
            r0 = 0;
        }

        rn1 = TotalRechercheVoisinADistanceDe(1,b_id,zone,pConf,voisin);
        rp1 = TotalRechercheVoisinADistanceDe(-1,b_id,zone,pConf,voisin);
        rn2 = TotalRechercheVoisinADistanceDe(2,b_id,zone,pConf,voisin);
        rp2 = TotalRechercheVoisinADistanceDe(-2,b_id,zone,pConf,voisin);

        // mise a jour dans la base
        msg = "update r_"+pConf->nomZone[zone]+"_"+QString::number(b_id) + " " +
                "set r0=" +QString::number(r0)+ ", " +
                "rp1=" +QString::number(rp1)+ ", " +
                "rp2=" +QString::number(rp2)+ ", " +
                "rn1=" +QString::number(rn1)+ ", " +
                "rn2=" +QString::number(rn2)+ " " +
                "where (id="+QString::number(voisin)+");";
        status = query.exec(msg);
    }
}
//-----
bool GererBase::CreerTableAbsentDeBoule(int b_id, int zone, stTiragesDef *pConf, int max_voisins)
{
    QSqlQuery query;
    QString msg;
    bool status = false;

    // Creation d'une table si pas encore cree pour memoriser la recherche
    msg =  "create table if not exists abs_"+pConf->nomZone[zone]+"_"+QString::number(b_id)+ " " +
            "(id INTEGER PRIMARY KEY, r0 int, rp1 int, rp2 int, rn1 int, rn2 int);";
    status = query.exec(msg);

    if(status)
    {
        // On essai de recuperer une valeur pour voir si les infos dans table
        msg = "select count (*) from abs_"+pConf->nomZone[zone]+"_" +QString::number(b_id)+ "; ";
        status = query.exec(msg);

        if(status)
        {
            status = query.first();
            if(query.isValid())
            {
                int calcul = query.value(0).toInt();
                if(!calcul){
                    // Table pas encore cree mettre a 0 les donnees
                    for(int absent=1;(absent<=max_voisins) && status;absent++)
                    {
                        msg = "insert into abs_"+pConf->nomZone[zone]+"_"+QString::number(b_id)+
                                " (id, r0, rp1, rp2, rn1, rn2) values (null, 0,0,0,0,0);";
                        status = query.exec(msg);
                    }
                }
                else
                {
                    // La table existe deja et a des infos des voisins
                    status = false;
                }
            }
        }
    }

    return status;
}

//______
void GererBase::CreerTablePonderationAbsentDeBoule(int b_id, int zn, stTiragesDef *pConf)
{
    QSqlQuery query;
    QString msg;
    bool status = true;
    QString names[5]={"r0","rp1","rp2","rn1","rn2"};
    int ecart[5]={0,-1,-2,1,2};

    for(int i=0;(i<(pConf->nbElmZone[zn])) && status;i++)
    {
        // voir si la table existe
        msg = "select * from tabs_"
                + pConf->nomZone[zn]
                +"_"
                + QString::number(b_id)+names[i]+";";
        status = query.exec(msg);

        if(!query.isValid())
        {
            // Creer la table
            msg = "create table tabs_"
                    + pConf->nomZone[zn]
                    +"_"
                    +QString::number(b_id)+names[i]+
                    " (id INTEGER PRIMARY KEY, b int, nb int);";
            status = query.exec(msg);
            if(status)
            {
                // La table est cree
                // recuperer les absents et donner leur rapport a la boule
                // 1: select id from abs_b_13 where (r0 = 1)
                msg = "select id from abs_"
                        + pConf->nomZone[zn]
                        +"_"
                        +QString::number(b_id)
                        +" where ("+names[i]+"=1);";
                status = query.exec(msg);
                if(status)
                {
                    query.first();
                    if(query.isValid())
                    {
                        do
                        {
                            // On va parcourir chaque valeur et trouver le total dans la base
                            QSqlRecord ligne = query.record();
                            int boule = ligne.value(0).toInt();
                            QString msg2= TST_ZoneRequete(pConf, zn,"or",b_id,"=");;
                            QString msg3= TST_ZoneRequete(pConf, zn,"or",boule,"=");

                            msg = "select count (*) from "
                                  "("
                                  "select * from tirages "
                                  "inner join "
                                  "("
                                  "select *  from tirages where ("+ msg2 + ") "
                                    + ")as r_boul "
                                      "on tirages.id = r_boul.id + %1)"
                                      "as rn1 "
                                      "where("+msg3+");";

                            msg = (msg).arg(ecart[i]);
#ifndef QT_NO_DEBUG
                            //qDebug()<< msg;
#endif

                            QSqlQuery query_2;
                            status = query_2.exec(msg);
                            if(status)
                            {
                                int total = 0;
                                query_2.first();

                                if(query_2.isValid())
                                {
                                    total = query_2.value(0).toInt();
                                    msg = "insert into tabs_"+pConf->nomZone[zn]+"_"+QString::number(b_id)+names[i]+
                                            " (id, b, nb) values (null,"
                                            +QString::number(boule)
                                            +","
                                            + QString::number(total)
                                            +");";
                                    status = query_2.exec(msg);

                                }
                            }

                        }while(query.next());
                    }
                }// Absent present
            }// Recup absent
        }// Fin creation table
    } // Fin for
    //return status;
}
//-----
bool GererBase::CreerTableVoisinsDeBoule(int b_id, int zone, stTiragesDef *pConf, int max_voisins)
{
    QSqlQuery query;
    QString msg;
    bool status = false;

    // Creation d'une table si pas encore cree pour memoriser la recherche
    msg =  "create table if not exists r_"+pConf->nomZone[zone]+"_"+QString::number(b_id)+ " " +
            "(id INTEGER PRIMARY KEY, r0 int, rp1 int, rp2 int, rn1 int, rn2 int);";
    status = query.exec(msg);

    if(status)
    {
        // On essai de recuperer une valeur pour voir si les infos dans table
        msg = "select count (*) from r_"+pConf->nomZone[zone]+"_" +QString::number(b_id)+ "; ";
        status = query.exec(msg);

        if(status)
        {
            status = query.first();
            if(query.isValid())
            {
                int calcul = query.value(0).toInt();
                if(!calcul){
                    // Table pas encore cree mettre a 0 les donnees
                    for(int voisin=1;(voisin<=max_voisins) && status;voisin++)
                    {
                        msg = "insert into r_"+pConf->nomZone[zone]+"_"+QString::number(b_id)+
                                " (id, r0, rp1, rp2, rn1, rn2) values (null, 0,0,0,0,0);";
                        status = query.exec(msg);
                    }
                }
                else
                {
                    // La table existe deja et a des infos des voisins
                    status = false;
                }
            }
        }
    }

    return status;
}


void GererBase::TST_RechercheVoisin(QStringList &boules, int zn,stTiragesDef *pConf,
                                    LabelClickable *l_nb, QStandardItemModel *modele)
{
    QSqlQuery query;
    QString msg = "select count (*)  from tirages where (";
    bool status = false;
    QString lstBoules =  boules.join(",");
    //int zn = 0;
    const int d[5]={0,-1,-2,1,2};

    if(boules.isEmpty())
    {
        l_nb->setText("Nb total de sorties:");

        // Remettre a zero
        for(int i = 0 ; i<pConf->limites[zn].max;i++)
        {
            for(int ref = 0; ref <5 ; ref ++)
            {
                QStandardItem *item1 = modele->item(i,ref+1);
                item1->setData("",Qt::DisplayRole);
            }
        }
        return;
    }

    QString w_msg = TST_ConstruireWhereData(zn,pConf,boules);

    msg = msg + w_msg;
    msg = msg + ");";

    status = query.exec(msg);

    if(status)
    {
        status= query.first();
        if(query.isValid())
        {
            int val = query.value(0).toInt();
            l_nb->setText(QString("Boule %1 : %2 fois ").arg( lstBoules ).arg(val) );
        }
    }
    query.finish();

    // Trier les boules de la fenetre voisins
    //modele->sortByColumn(0,Qt::AscendingOrder);


    // Rechercher pour chaque boules les voisins
    for(int i = 0 ; i<pConf->limites[zn].max;i++)
    {
        for(int ref = 0; ref <5 ; ref ++)
        {
            QStandardItem *item1 = modele->item(i,ref+1);
            int val = TST_TotalRechercheVoisinADistanceDe(zn,pConf,d[ref],i+1,boules);

            // Mettre la valeur trouvee dans le tableau des voisins
            item1->setData(val,Qt::DisplayRole);
#if 0
            if(ref == 1 || ref == 2)
            {
                QColor fond[2]={Qt::red,Qt::green};
                // Mettre le resultat dans tableau voisins a la bonne place
                QStandardItem *item2 = modele->item(i,ref+5);
                item2->setData(val,Qt::DisplayRole);
                item2->setBackground(QBrush(fond[ref-1]));
                modele->setItem(i,ref+5,item2);

            }
#endif
        }
    }

}
QString GererBase::TST_GetIntervalCouverture(int etendue)
{
    QString msg= "" ;
    QSqlQuery query;
    bool status = false;
    int nb_couverture = 0;
    int id = 0;
    int deb = 0;
    int fin = 0;

    if (etendue < 0)
    {
        etendue = 0 - etendue;
    }

    msg = "select  *  from czb;";
    status = query.exec(msg);
    if(status){
        // se positionner sur dernier enregistrement
        query.last();
        QSqlRecord rec;

        if(query.isValid())
        {
            rec = query.record();
            id = rec.value(0).toInt();
            deb = rec.value(1).toInt();
            fin = rec.value(2).toInt();

            if(fin !=1)
            {
                fin = fin -1;
                deb=1;
            }

            msg = "id between " + QString::number(deb) + " and " + QString::number(fin);
        }
    }
    else
    {
        msg="";
    }
    return msg;
}

QString GererBase::TST_ConstruireWhereData(int zn, stTiragesDef *pConf, QStringList &boules)
{
    QString msg= "" ;
    QString flag = " and ";

    for(int i=0; i< boules.size();i++)
    {
        int val_boule = boules.at(i).toInt();
        QString msg1 = TST_ZoneRequete(pConf, zn,"or",val_boule,"=");
        msg = msg + "(" +msg1+ ")"
                + flag;
    }

    msg.remove(msg.length()-flag.length(),flag.length());

    return msg;
}
void GererBase::RechercheAbsent(int boule, int zn, stTiragesDef *pConf,
                                QLabel *l_nb, QStandardItemModel *modele)
{
    QSqlQuery query(db);
    QString msg;
    bool status = false;
    int max_absent = pConf->limites[zn].max;
    status = CreerTableAbsentDeBoule(boule, zn, pConf, max_absent);

    if(status)
    {
        RechercherAbsentDeLaBoule(boule, zn, pConf, max_absent);
    }
    // Affichage des resultats dans la vue
    //MontrerResultatRechercheAbsent(modele, zn, pConf, boule);
    CreerTablePonderationAbsentDeBoule(boule, zn, pConf);

#if 0
    if(status)
    {
        PondererAbsentDeLaBoule(boule, zn, pConf, max_absent);
    }
#endif
}

void GererBase::RechercheVoisin(int boule, int zn, stTiragesDef *pConf,
                                LabelClickable *l_nb, QStandardItemModel *modele)
{
    QSqlQuery query(db);
    QString msg;
    bool status = false;
    //int calcul = 0, voisin = 0;
    //int rp1 = 0, rp2 = 0, rn1 = 0, rn2 = 0;
    //int resu = 0;
    //int zn = 0;
    QString mvoisins[5]={"r0","rp1","rp2","rn1","rn2"};

    if (iAffichageVoisinEnCoursDeLaBoule[zn] != boule)
    {
        iAffichageVoisinEnCoursDeLaBoule[zn] = boule;
        AfficherMaxOccurenceBoule(boule, zn, pConf, l_nb);

        int max_voisin = pConf->limites[zn].max;
        status = CreerTableVoisinsDeBoule(boule, zn, pConf, max_voisin);

        if(status)
        {
            RechercherVoisinDeLaBoule(boule, zn, pConf, max_voisin);
        }

        // Affichage des resultats dans la vue
        MontrerResultatRechercheVoisins(modele, zn, pConf, boule);
    }

    // creer les tables avec les meilleurs de facon ordonne
    for(int i=0;(i<(pConf->nbElmZone[zn])) && status;i++)
    {
        // voir si la table existe
        msg = "select * from tb_"
                + pConf->nomZone[zn]
                +"_"
                + QString::number(boule)+mvoisins[i]+";";
        status = query.exec(msg);

        if(!query.isValid())
        {
            // La table n'existe pas encore
            msg = "create table tb_"
                    + pConf->nomZone[zn]
                    +"_"
                    +QString::number(boule)+mvoisins[i]+
                    " as select id,"+mvoisins[i]+" from " +
                    "r_"+pConf->nomZone[zn]+"_"+QString::number(boule)
                    + " order by " + mvoisins[i]+" desc limit 10;";
            status = query.exec(msg);
        }
    }

    // Selection les meilleurs resultats de chacun
    msg = "select * from union_" + pConf->nomZone[zn]+ QString::number(boule)+";";
    status = query.exec(msg);

    if(!query.isValid())
    {
        QString flag = " union ";
        msg="";
        for(int i=0;i<pConf->nbElmZone[zn];i++)
        {
            msg = msg
                    + "select id from tb_"
                    + pConf->nomZone[zn]
                    + "_"
                    + QString::number(boule)+mvoisins[i]+ flag
                    ;
        }
        msg.remove(msg.length()-flag.length(),flag.length());

        // La table n'existe pas encore
        msg = "create table union_" + pConf->nomZone[zn]+ QString::number(boule)+
                " as select r_"+pConf->nomZone[zn]+"_"+ QString::number(boule)+".id,"+
                "(r0+rp1+rp2+rn1+rn2) 'T' from r_"+pConf->nomZone[zn]+"_"+ QString::number(boule)+
                " where id in ("+msg+") order by T desc;";
        status = query.exec(msg);
    }

#if 0

    "select id from tb_"
    + pConf->nomZone[zn]
            +"_"
            +QString::number(boule)+mvoisins[0]+ " union " +
            "select id from tb_"+QString::number(boule)+mvoisins[1]+ " union " +
            "select id from tb_"+QString::number(boule)+mvoisins[2]+ " union " +
            "select id from tb_"+QString::number(boule)+mvoisins[3]+ " union " +
            "select id from tb_"+QString::number(boule)+mvoisins[4]+ " " +

        #endif
            // Recherche terminee finir avec cette vue
            //msg = "drop view r_boul";
            //query.exec(msg);

}

int GererBase::TST_TotalRechercheVoisinADistanceDe(int zn,stTiragesDef *pConf,int dist, int v_id,QStringList &boules)
{
#if 0
    select * from
            (
                select * from tirages inner join
                (
                    select *  from tirages where
                    (
                        (b1=27 or b2=27 or b3=27 or b4=27 or b5=27)
                        )
                    ) as r1
                on tirages.id = r1.id + 2
            ) as r2
            where (b1 = 1 or b2 = 1 or b3 = 1 or b4 = 1 or b5 = 1 );
#endif

    int ret_val = 0;

    if(boules.contains(QString::number(v_id)) && !dist)
        return ret_val;


    QSqlQuery query;
    bool status = false;
    QString msg = "select *  from tirages inner join  ( select *  from tirages where (";

    QString w_msg = TST_ConstruireWhereData(zn,pConf,boules);
    msg = msg + w_msg;
    msg = msg + ")) as r1 on tirages.id = r1.id + %1 ) as r2";
    msg = (msg).arg(dist);

    QString msg_2 = TST_ZoneRequete(pConf, zn,"or",v_id,"=");
    msg_2= "select count (*)  from (" +msg+ " where (" +msg_2+ " );" ;
    status = query.exec(msg_2);
    if(status)
    {
        query.first();
        if(query.isValid())
        {
            ret_val = query.value(0).toInt();
        }
    }


    return ret_val;
}


void GererBase::TST_LBcDistBr(int zn,stTiragesDef *pConf,int dist, int br,int bc)
{
    // liste des tirages ayant la boule de reference Br
    // dont les tirages a la distance Dist
    // ont la boule B_Cherche
#if 0
    select * from
            (
                select * from tirages inner join
                (
                    select *  from tirages where
                    (
                        (b1=27 or b2=27 or b3=27 or b4=27 or b5=27)
                        )
                    ) as r1
                on tirages.id = r1.id + 2
            ) as r2
            where (b1 = 1 or b2 = 1 or b3 = 1 or b4 = 1 or b5 = 1 );
#endif

    QSqlQuery query;
    bool status = false;
    QStringList boules;
    QString msg = "select *  from tirages inner join  ( select *  from tirages where (";

    boules << QString::number(bc);

    QString w_msg = TST_ConstruireWhereData(zn,pConf,boules);
    msg = msg + w_msg;
    msg = msg + ")) as r1 on tirages.id = r1.id + %1 ) as r2";
    msg = (msg).arg(dist);

    QString msg_2 = TST_ZoneRequete(pConf, zn,"or",br,"=");
    msg_2= "select * from (" +msg+ " where (" +msg_2+ " );" ;
    status = query.exec(msg_2);
    if(status)
    {
        query.first();
        if(query.isValid())
        {
            int ret_val = 0;
            do
            {
                ret_val++;
            }while(query.next());

            ret_val ++;
        }
    }
}

//----------
bool GererBase::RechercheAbsentADistanceDe(int dist, int boule, int zn,stTiragesDef *pConf,int voisin)
{
    QString msg = "";
    QString msg2 = "";
    QString msg_limite = "";
    QSqlQuery query(db);
    bool bEstAbsent = false;
    bool status;

    msg_limite = TST_GetIntervalCouverture(1);

    msg= TST_ZoneRequete(pConf, zn,"or",boule,"=");
    msg2 = TST_ZoneRequete(pConf, zn,"or",voisin,"=");

    msg = "select count (*) from "
          "("
          "select * from tirages "
          "inner join "
          "("
          "select *  from tirages where ("+ msg + ") and (" + msg_limite
            + ")"
              ")as r_boul "
              "on tirages.id = r_boul.id + %1)"
              "as rn1 "
              "where("+msg2+");";

    msg = (msg).arg(dist);

#ifndef QT_NO_DEBUG
    //qDebug()<< msg;
#endif

    status = query.exec(msg);
    if(status)
    {
        int total = 0;
        query.first();

        if(query.isValid())
        {
            total = query.value(0).toInt();
            if(total)
            {
                bEstAbsent=false; // On a trouve au moins 1 donc pas absent
            }
            else
            {
                bEstAbsent = true;
            }
        }
    }
    return bEstAbsent;
}
//----------
int GererBase::TotalRechercheVoisinADistanceDe(int dist, int boule, int zn,stTiragesDef *pConf,int voisin)
{
    QString msg = "";
    QString msg2 = "";
    QSqlQuery query(db);
    int calcul = 0;
    bool status;

#if 0
    msg= TST_ZoneRequete(pConf, zn,"or",boule);
    // Selection des lignes voisines de celle de reference
    msg = "create view rn1 as select * from tirages "
          "inner join "
          "("
          "select *  from tirages where ("+ msg+ ")"
                                                 ")"
                                                 "as r_boul on tirages.id = r_boul.id + %1";
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
#endif

    msg= TST_ZoneRequete(pConf, zn,"or",boule,"=");
    msg2 = TST_ZoneRequete(pConf, zn,"or",voisin,"=");

    msg = "select count (*) from "
          "("
          "select * from tirages "
          "inner join "
          "("
          "select *  from tirages where ("+ msg+ ")"
                                                 ")as r_boul "
                                                 "on tirages.id = r_boul.id + %1)"
                                                 "as rn1 "
                                                 "where("+msg2+");";
    msg = (msg).arg(dist);
    status = query.exec(msg);
    if(status)
    {
        query.first();
        if(query.isValid())
        {
            calcul = query.value(0).toInt();
        }
    }
    return calcul;
}

void GererBase::CouvertureBase(QStandardItemModel *dest,stTiragesDef *pRef)
{
    QSqlQuery query;
    QSqlQuery sauve;
    QSqlQuery position;
    QString msg = "";
    bool depart_couverture = false;
    int lgndeb=0, lgnfin=0;
    int memo_boule[50]= {0};
    int ordr_boule[50]= {0};
    int nb_boules = 0;
    int i = 0;
    int id_couv=0;
    bool status;
    int zn = 0;
    int memo_last_boule = 0;

    stTiragesDef ref = *pRef;

    // recuperer tous les tirages
    msg= "select * from tirages";
    status = query.exec(msg);

    // Se positionner au debut des tirages du jeu
    status = query.last();
    depart_couverture = true;
    lgndeb = query.value(0).toInt();
    if(query.isValid())
    {
        // requete a effectuer
        msg = "insert into " + QString::fromLocal8Bit(CL_TCOUV) + ref.nomZone[zn]  +
                " (id, depart, fin, taille) values (:id, :depart, :fin, :taille)";
        status = sauve.prepare(msg);

        int last_boule=0;
        do
        {
            QSqlRecord rec  = query.record();

            // Sauvegarder le debut de couverture
            if(depart_couverture){
                id_couv++;
                sauve.bindValue(":depart", lgndeb);

                // creer colonne pour ordre d'arrivee
                CreerColonneOrdreArrivee(id_couv, &ref);
                depart_couverture = false;
            }

            int boule = 0;
            // prendre toutes les boules de la zone pour le tirage concerne
            for(i = 0; (i<ref.nbElmZone[zn])&& (nb_boules<ref.limites->max);i++)
            {
                boule = rec.value(2+i).toInt();

                if(boule == last_boule)
                {
                    last_boule = -1;
                }
                else
                {

                    // Cette boule est elle connue
                    if(!memo_boule[boule-1])
                    {
                        // non alors memoriser l'ordre d'arrivee
                        ordr_boule[nb_boules]= boule;
                        msg = "update " + QString::fromLocal8Bit(CL_TOARR) + ref.nomZone[zn] +
                                " set " + QString::fromLocal8Bit(CL_CCOUV) +
                                QString::number(id_couv) + "=" +QString::number(boule)+
                                " where (id="+QString::number(nb_boules+1)+");";
                        status = position.exec(msg);
                        nb_boules++;
                    }

                    // Un boule de plus comporte le numero "boule"
                    memo_boule[boule-1]++;
                }
            }

            // A ton atteind la converture ?
            if(nb_boules == ref.limites->max)
            {
                last_boule = boule;
                memo_last_boule = boule;

                // Oui
                // il faudra une nouvelle colonne pour l'ordre d'arrivee des boules
                depart_couverture = true;

                // remettre a zero compteur apparition
                nb_boules = 0;
                memset(memo_boule, 0, (ref.limites->max)*sizeof(int));
                memset(ordr_boule, 0, (ref.limites->max)*sizeof(int));

                lgnfin = rec.value(0).toInt();
                sauve.bindValue(":fin", lgnfin);
                sauve.bindValue(":taille", lgndeb-lgnfin+1);
                // Mettre dans la base
                status = sauve.exec();

#ifndef QT_NO_DEBUG
                if(!status){
                    qDebug() << "ERROR:" << query.executedQuery()  << "-" << query.lastError().text();
                }
#endif


                // Non, alors indiquer les voisines comme nouvelles
                lgndeb = lgnfin;
                // Avancer pour ensuite reculer
                query.next();

#if 0
                // Est ce la derniere boule du tirage qui a permis la couverture
                if( i != ref.nbElmZone[zn]-1)
                {
                    // Creer une nouvelle colonne couverture
                    // creer colonne pour ordre d'arrivee
                    CreerColonneOrdreArrivee(id_couv+1, &ref);

                    // Non, alors indiquer les voisines comme nouvelles
                    lgndeb = lgnfin;
                    for(j=0;j<i;j++){
                        int boule = rec.value(2+j).toInt();

                        // On ne prends pas celle qui a permis la fin de couverture
                        if(i !=j)
                        {
                            msg = "update " + QString::fromLocal8Bit(CL_TOARR) + ref.nomZone[zn] +
                                    " set " + QString::fromLocal8Bit(CL_CCOUV) +
                                    QString::number(id_couv+1) + "=" +QString::number(boule)+
                                    " where (id="+QString::number(nb_boules+1)+");";
                            status = position.exec(msg);
                            ordr_boule[nb_boules]= boule;
                            nb_boules++;
                            memo_boule[boule-1]++;
                        }
                    }

                }
                else
                {
                    lgndeb = lgnfin-1;
                }
                //sauve.bindValue(":depart", lgndeb);

#endif
            }

        }while(query.previous());

        // On a parcouru toute la base
        // indiquer les absents
        for(i = 0; i< ref.limites->max ;i++)
        {
            if (!memo_boule[i])
            {
                QStandardItem *item1 = dest->item(i);
                QBrush macouleur;
                QColor unecouleur;

                if(i!=memo_last_boule-1)
                {
                    macouleur.setColor(Qt::green);
                    macouleur.setStyle(Qt::SolidPattern);
                }
                else
                {
                    // http://stackoverflow.com/questions/8571059/how-to-generate-new-qcolors-that-are-different
                    // http://goffgrafix.com/pantone-rgb-800.php
                    unecouleur.setRgb(255,127,30);
                    macouleur.setColor(unecouleur);
                    macouleur.setStyle(Qt::SolidPattern);
                }
                item1->setBackground(macouleur);
            }
        }

    }
}

void GererBase::CouvMontrerProbable(int i,
                                    int col_m,
                                    int col_v,
                                    QStandardItemModel *dest)
{

    double rayon = 1.5;

    QStandardItem *item1 = dest->item(i-1,col_m);
    QStandardItem *item2 = dest->item(i-1,col_v);
    double v_moyen = item1->data(Qt::DisplayRole).toDouble();
    int v_court = item2->data(Qt::DisplayRole).toInt();


    if((v_court>=v_moyen-rayon) && (v_court <= (v_moyen +rayon)))
    {
        item2->setBackground(QBrush(Qt::magenta));
        item1->setBackground(QBrush(Qt::magenta));
    }

}

QString req_msg(int zone, int boule, stTiragesDef *ref)
{
    int max_elm_zone = ref->nbElmZone[zone];
    int col_id = 0;
    //QString *tab = ref->nomZone;
    QString msg = "select id from tirages where (";


    // Suite msg requete
    for(col_id=1;col_id<=max_elm_zone;col_id++)
    {
        msg = msg +
                ref->nomZone[zone] +
                QString::number(col_id)+ "=" + QString::number(boule)+
                " or ";
    }

    if(msg.length() != 0){
        msg.remove(msg.size()-4,4);
        msg = msg + ")";
    }

    return msg;
}

bool GererBase::CreerColonneOrdreArrivee(int id, stTiragesDef *pConf)
{
    QSqlQuery query;
    QString msg = "";
    stTiragesDef ref = *pConf;
    bool status = false;
    int zn = 0;

    // test pour voir si une colonne existe deja
    msg = "select * from " + QString::fromLocal8Bit(CL_TOARR) + ref.nomZone[zn] +
            "." + QString::fromLocal8Bit(CL_CCOUV) +
            QString::number(id);
    status = query.exec(msg);

    if (status == false)
    {
        msg = "alter table " + QString::fromLocal8Bit(CL_TOARR) + ref.nomZone[zn] +
                " add column " + QString::fromLocal8Bit(CL_CCOUV) +
                QString::number(id) + " int;";
        status = query.exec(msg);
    }
    else
    {
        status = true;
    }

    return status;
}

void GererBase::PopulateCellMenu(int b_id, int v_id,int zone, stTiragesDef *pConf, QMenu *menu,QObject * receiver)
{
    QString msg;
    QSqlQuery query;
    bool status = false;
    int somme = 0;

    // On recupere les valeurs des voisins
    QString Lib[6]={"tot:","r0:","+1:","+2:","-1:","-2:"};
    QString vVoisin[6];

    msg = "select * from r_"+pConf->nomZone[zone]+"_" + QString::number(b_id) +
            " where (id = "+QString::number(v_id) +");";
    status = query.exec(msg);

    if(status)
    {
        status = query.first();
        if(query.isValid())
        {
            QSqlRecord ligne = query.record();
            int nb_col = ligne.count();
            for(int i =1; i< nb_col;i++)
            {
                int val = ligne.value(i).toInt();
                somme+=val;
                vVoisin[i]= Lib[i]+QString::number(val);
            }
            vVoisin[0]= Lib[0]+QString::number(somme);

            for(int i = 0; i<6;i++)
            {
                menu->addAction(new QAction(vVoisin[i], receiver));
            }

        }
    }
}


bool GererBase::TST_Requete(QString &sql_msg, int lgn, QString &col, QStandardItemModel *&qsim_ptr)
{
    QSqlQuery query;
    bool status = false;

    status = query.exec(sql_msg);

    if (status)
    {
        query.first();
        if(query.isValid())
        {
            int val = query.value(0).toInt();
            RangerValeurResultat(lgn,col,val,qsim_ptr);
        }
        query.finish();
    }

    return status;
}
