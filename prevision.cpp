#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QtGui>
#include "mainwindow.h"

#if 0
-- comptage boule sur ligne v1
select tbleft.blgn as B,count(tbleft.blgn) as T, MAX(tbleft.sortie) as Smax from
(
select b1k as blgn, b1r as sortie  from BNEXT_b_D1 where id = 1
union all
select b2k as blgn, b2r as sortie  from BNEXT_b_D1 where id = 1
union all
select b3k as blgn, b3r as sortie   from BNEXT_b_D1 where id = 1
union all
select b4k as blgn, b4r as sortie   from BNEXT_b_D1 where id = 1
union all
select b5k as blgn, b5r as sortie   from BNEXT_b_D1 where id = 1
) as tbleft
left join
(
select id,z1 from Bnrz where id <= 49
)as tbright
on
(
tbright.z1 = tbleft.blgn
) group by tbleft.blgn;


-- comptage boule sur ligne v2
select tbleft.z1 as B,count(tbright.blgn) as T, MAX (tbright.sortie) as Smax from (select z1 from Bnrz where id <= 49) as tbleft
left join
(
select b1k as blgn, b1r as sortie  from BNEXT_b_D1 where id = 1
union all
select b2k as blgn, b2r as sortie  from BNEXT_b_D1 where id = 1
union all
select b3k as blgn, b3r as sortie   from BNEXT_b_D1 where id = 1
union all
select b4k as blgn, b4r as sortie   from BNEXT_b_D1 where id = 1
union all
select b5k as blgn, b5r as sortie   from BNEXT_b_D1 where id = 1
) as tbright
on
(
tbleft.z1 = tbright.blgn
) group by tbleft.z1;

#endif

void MainWindow::NEW_ChoixPourTiragesSuivant(QString tb_reponse, int nbTirPrecedent,stTiragesDef *pConf)
{
    bool ret = true;
    int zone = 0;

    // selectionner les derniers tirages
    for (int loop = 1; (loop <= nbTirPrecedent) && ret == true; loop ++)
    {
        ret = NEW_AnalyserCeTirage(loop,tb_reponse,zone,pConf);
    }

}


bool MainWindow::NEW_AnalyserCeTirage(int idTirage,  QString stTblRef,int zone, stTiragesDef *pConf)
{
    bool status = false;
    QString msg1 = "";
    QSqlQuery query ;


    // Creation de la table des analyses pour ce tirage
    QString nomTable = stTblRef
            + "_"
            + pConf->nomZone[zone]
            +"_D" + QString::number(idTirage);

    msg1 =  "create table "+nomTable+" (id Integer primary key,";

#ifdef NUM_IN_COLNAME
    msg1 = msg1 + NEW_ColHeaderName(idTirage,zone,pConf);
#else

    int nb_elem = pConf->nbElmZone[zone];
    for(int bpos=1;bpos<= nb_elem;bpos++)
    {
        msg1 = msg1 +
                "b" + QString::number(bpos)+"v int, " +
                "b" + QString::number(bpos)+"k int, " +
                "b" + QString::number(bpos)+"r int";

        if(bpos< nb_elem)
        {
          msg1 = msg1 + ",";
        }

    }

#endif

    msg1 = msg1 + ");";
#ifndef QT_NO_DEBUG
    qDebug() << msg1;
#endif
    status = query.exec(msg1);
    query.finish();



    msg1 = "insert into "+nomTable+" (id) "
                                   "select z"+QString::number(zone+1)+
            " from "+TB_BNRZ+" where id <="
            +QString::number(pConf->limites[zone].max)+";";

#ifndef QT_NO_DEBUG
    qDebug() << msg1;
#endif
    status = query.exec(msg1);


    // Recuperer les boules du tirage
    msg1 = "select t1.id from "
           "(select id from "  TB_BNRZ  " where id <= "
            +QString::number(pConf->limites[zone].max)+
            ") as t1 "
            "inner join "
            "(select tirages.* from tirages where id = "
            +QString::number(idTirage)+
            ") as t2 "
            "on "
            "("
            "t1.id = t2.b1 or "
            "t1.id = t2.b2 or "
            "t1.id = t2.b3 or "
            "t1.id = t2.b4 or "
            "t1.id = t2.b5 "
            ");";

#ifndef QT_NO_DEBUG
    qDebug() << msg1;
#endif

    status = query.exec(msg1);

    // Prendre chacune des boules une a une
    if(status)
    {
        query.first();
        if(query.isValid())
        {
            int posBoule = 0;
            do
            {
                posBoule++;
                int uneBoule = query.value(0).toInt();

                status = NEW_SyntheseDeBoule(uneBoule, posBoule, idTirage,nomTable, pConf);

            }while(query.next() && status == true);
        }
    }
    return status;
}

QString MainWindow::NEW_ColHeaderName(int idTirage,int zone, stTiragesDef *pConf)
{
    bool status = false;
    QString msg1 = "";
    QSqlQuery query ;


    // Recuperer les boules du tirage
    QString st_tb = "t1.id";
    QString st_msg2 = " from "
                      "(select id from "  TB_BNRZ  " where id <= "
                       +QString::number(pConf->limites[zone].max)+
                       ") as t1 "
                       "inner join "
                       "(select tirages.* from tirages where id = "
                       +QString::number(idTirage)+
                       ") as t2 "
                       "on "
                       "("
                       "t1.id = t2.b1 or "
                       "t1.id = t2.b2 or "
                       "t1.id = t2.b3 or "
                       "t1.id = t2.b4 or "
                       "t1.id = t2.b5 "
                       ");";

    msg1 = "select "+ st_tb+ st_msg2;

#ifndef QT_NO_DEBUG
    qDebug() << msg1;
#endif

    status = query.exec(msg1);
    msg1="";

    // Prendre chacune des boules une a une
    if(status)
    {
        query.first();
        if(query.isValid())
        {
            int bpos = 0;
            //int nb_elem = query.size();

            // Pb size as sqlite :int nb_elem = query.size();
            QSqlQuery sql2;
            msg1 = "select count("+st_tb+") " + st_msg2;
            status = sql2.exec(msg1);
            sql2.first();
            int nb_elem = sql2.value(0).toInt();

            msg1="";
            do
            {
                int val = query.value(0).toInt();

                bpos++;

                msg1 = msg1 +
                        "id_" + QString::number(val)+" int, " +
                        "nb_" + QString::number(val)+" int";

                if(bpos< nb_elem)
                {
                  msg1 = msg1 + ",";
                }

            }while(query.next());
        }
    }

    return msg1;
}

bool MainWindow::NEW_SyntheseDeBoule(int uneBoule, int colId, int loop, QString nomTable, stTiragesDef *pConf)
{
    bool status = false;

    QStringList demande;
    QString msg1 = "";
    QSqlQuery query ;

    demande << QString::number(uneBoule);
    msg1 = NEW_ChercherTotalBoulesAUneDistance(demande,loop,pConf,1);

    status = query.exec(msg1);

    // Sauvegarder les reponses dans la bonne colonne de la bonne table
    if(status)
    {
        query.first();
        if(query.isValid())
        {
            int boule = 0;
            do{
                QSqlQuery req_2;
                int id=query.value(0).toInt();
                int nb=query.value(1).toInt(); // val a mettre dans a table

                boule++;

#ifdef NUM_IN_COLNAME

                msg1 = "update "+ nomTable+" set id_"+QString::number(uneBoule)+"="
                        +QString::number(id)
                        + ", nb_"+QString::number(uneBoule)+"="
                        +QString::number(nb)+
                        " where "+ nomTable+".id="+QString::number(boule)+";";
#else
                msg1 = "update "+ nomTable+
                        " set b"+QString::number(colId)+"v=" + QString::number(uneBoule)
                        + ", b"+QString::number(colId)+"k="  +QString::number(id)
                        + ", b"+QString::number(colId)+"r="  +QString::number(nb)+
                        " where "+ nomTable+".id="+QString::number(boule)+";";
#endif

#ifndef QT_NO_DEBUG
                qDebug() << msg1;
#endif

                status = req_2.exec(msg1);
            }while (query.next() && status);
        }
    }


    return status;
}
