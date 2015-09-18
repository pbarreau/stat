#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QtGui>
#include "mainwindow.h"

#if 0
-- comptage boule sur ligne v1
select tbleft.id as R, tbleft.blgn as B,count(tbleft.blgn) as T, MAX(tbleft.sortie) as Smax from
(
select id, b1k as blgn, b1r as sortie  from BNEXT_b_D1 where id = 1
union all
select id, b2k as blgn, b2r as sortie  from BNEXT_b_D1 where id = 1
union all
select id, b3k as blgn, b3r as sortie   from BNEXT_b_D1 where id = 1
union all
select id, b4k as blgn, b4r as sortie   from BNEXT_b_D1 where id = 1
union all
select id,b5k as blgn, b5r as sortie   from BNEXT_b_D1 where id = 1
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

-- synthese
select r as R, b as B, sum(t) as T, max (s) as Smax from tmpBilan group by b order by r asc, T desc, b desc ;

#endif

void MainWindow::NEW_RepartionBoules(stTiragesDef *pConf)
{
    bool status = true;
    QString msg1 = "";
    QSqlQuery query ;
    int zone = 0;

    // Boule Finissant par xx
    QString str_fxx = "";
    //QString str_dxx = "";
    for(int i=0; i<10;i++)
    {
       str_fxx = str_fxx + "F" +QString::number(i)+ " int,";
    }
    for(int i=0; i<6;i++)
    {
       str_fxx = str_fxx + "U" +QString::number(i)+ " int,";
    }
    //enlever la derniere ,
    str_fxx.remove(str_fxx.length()-1,1);

    // Creer table synthese Horizontale
    msg1 =  "create table if not exists repartition_bh "
            "("
            "id INTEGER PRIMARY KEY,"
            "Nb int,"
            "P int,G int,"
            + str_fxx +
            ");";
    status = query.exec(msg1);
    query.finish();
    msg1 =  "insert into repartition_bh  (id) select id from Bnrz  where Bnrz.id <= 6;";
    status = query.exec(msg1);
    query.finish();




    QStringList cri_msg;
    QStringList cri_lab;

    cri_msg <<"z1%2=0"<<"z1<26";
    cri_lab << "P" << "G";

    for(int j=0;j<=9;j++)
    {
      cri_msg<< "z1 like '%" + QString::number(j) + "'";
      cri_lab << "F"+ QString::number(j);
    }

    for(int j=0;j<6;j++)
    {
      cri_msg<< "z1 >="+QString::number(10*j)+ " and z1<="+QString::number((10*j)+9);
      cri_lab << "U"+ QString::number(j);
    }

    // Creer table synthese Verticale
    msg1 =  "create table if not exists repartition_bv "
            "("
            "id INTEGER PRIMARY KEY, L TEXT,"
            "N0 int,N1 int,N2 int, "
            "N3 int,N4 int,N5 int );";
    status = query.exec(msg1);
    query.finish();
    msg1 =  "insert into repartition_bv  (id) select id from Bnrz  where Bnrz.id <="
            +QString::number(cri_lab.count())+";";
    status = query.exec(msg1);
    query.finish();


    // Parite/Groupe
    status = true;
    for(int i=0; (i< cri_msg.count()) && (status == true);i++)
    {
        // Creer Requete pour compter items
        msg1 = cri_msg[i];
        msg1 = GetSql(msg1);

#ifndef QT_NO_DEBUG
    qDebug() << msg1;
#endif

        status = query.exec(msg1);

        // Mise a jour de la tables des resultats
        if(status)
        {
            query.first();
            int key = 1;
            do
            {
                QSqlQuery sq2;
                int nb = query.value(0).toInt();
                int tot = query.value(1).toInt();
                msg1 = "update repartition_bh set Nb="
                        +QString::number(nb)+ ", "
                        + cri_lab[i]
                        + "="+QString::number(tot) + " where id="
                        +QString::number(key)+";";
#ifndef QT_NO_DEBUG
    qDebug() << msg1;
#endif

                status = sq2.exec(msg1);

                if(status)
                {

                    // table verticale
                    msg1 = "update repartition_bv set L='"
                            + cri_lab[i] +
                            "',N"
                            +QString::number(key-1)+ "="
                            +QString::number(tot) + " where id="
                            +QString::number(i+1)+";";
    #ifndef QT_NO_DEBUG
        qDebug() << msg1;
    #endif

                    status = sq2.exec(msg1);

                }
                key++;
            }while(query.next() && status);
        }

        query.finish();

    }
}

QString MainWindow::GetSql(QString st_cri)
{
#if 0
    --- Requete recherche parite sur base pour tirages
    -- requete de groupement des paritees
    select Nb, count(Nb) as Tp from
    (
    -- Req_1 : pour compter le nombre de boules pair par tirages
    select tb1.id as Tid, count(tb2.B) as Nb from
    (
    select * from tirages
    ) as tb1
    left join
    (
    select id as B from Bnrz where (z1 not null  and (z1%2 = 0))
    ) as tb2
    on
    (
     tb2.B = tb1.b1 or
     tb2.B = tb1.b2 or
     tb2.B = tb1.b3 or
     tb2.B = tb1.b4 or
     tb2.B = tb1.b5
    ) group by tb1.id order by Nb desc
    -- fin req_1
    )
    group by Nb;
#endif
  QString st_return =
          "select Nb, count(Nb) as Tp from "
          "("
          "select tb1.id as Tid, count(tb2.B) as Nb from "
          "("
          "select * from tirages "
          ") as tb1 "
          "left join "
          "("
          "select id as B from Bnrz where (z1 not null  and ("+st_cri+")) "
          ") as tb2 "
          "on "
          "("
          "tb2.B = tb1.b1 or "
          "tb2.B = tb1.b2 or "
          "tb2.B = tb1.b3 or "
          "tb2.B = tb1.b4 or "
          "tb2.B = tb1.b5 "
          ") group by tb1.id order by Nb desc "
          ")"
          "group by Nb;";

  return(st_return);
}

void MainWindow::NEW_ChoixPourTiragesSuivant(QString tb_reponse, int nbTirPrecedent,stTiragesDef *pConf)
{
    bool status = true;
    QString msg1 = "";
    QSqlQuery query ;
    int zone = 0;

    // selectionner les derniers tirages
    for (int loop = 1; (loop <= nbTirPrecedent) && status == true; loop ++)
    {
        status = NEW_AnalyserCeTirage(loop,tb_reponse,zone,pConf);
        if (status)
        {
            status = NEW_FaireBilan (loop,tb_reponse,zone,pConf,5);
        }
    }
    // On a fini tout les rangs
    // Creer table synthese finale
    msg1 =  "create table if not exists Bilan (id Integer primary key, r int, b int, t int, s int);";
    status = query.exec(msg1);
    query.finish();

    // Populer la table
    msg1 = "insert into Bilan (r, b, t, s) select R, B, T, Smax from "
           "("
           "select r as R, b as B, sum(t) as T, max (s) as Smax "
           "from tmpBilan group by r,b order by r asc, T desc, b asc "
            ")"
           ";";
    status = query.exec(msg1);
    query.finish();


}


bool MainWindow::NEW_AnalyserCeTirage(int idTirage,  QString stTblRef,int zone, stTiragesDef *pConf)
{
    bool status = false;
    QString msg1 = "";
    QSqlQuery query ;

#ifndef QT_NO_DEBUG
    qDebug() << msg1;
#endif
    status = query.exec(msg1);
    query.finish();

    // Creation de la table des analyses pour ce tirage
    QString nomTable = stTblRef
            + "_"
            + pConf->nomZone[zone]
            +"_D" + QString::number(idTirage);

    msg1 =  "create table "+nomTable+" (id Integer primary key, d int,";

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

bool MainWindow::NEW_FaireBilan(int idTirage, QString stTblRef,int zone, stTiragesDef *pConf, int nbRang)
{
    bool status = false;
    QString msg1 = "";
    QSqlQuery query ;

    // Creation table pour synthese des recherches
    // d: distance, r: rang dans la distance,
    // b: boule etudiee, t: nb de boule commune dans cette etude du rang
    // s: nb max de fois ou la boule est sortie
    msg1 =  "create table if not exists tmpBilan (id Integer primary key, d int, r int, b int, t int, s int);";
    status = query.exec(msg1);
    query.finish();

    // Lecture table des analyses pour ce tirage
    QString nomTable = stTblRef
            + "_"
            + pConf->nomZone[zone]
            +"_D" + QString::number(idTirage);
    for(int i = 1; (i<=nbRang) && status == true;i++)
    {
        msg1 = "insert into tmpBilan (d, r, b, t, s) select D, R, B, T,Smax from "
                "("
                "select tbleft.d as D, tbleft.id as R, tbleft.blgn as B,count(tbleft.blgn) as T, MAX(tbleft.sortie) as Smax from "
                "("
                "select d, id, b1k as blgn, b1r as sortie  from "+nomTable+" where id =" +QString::number(i)+ " "
                "union all "
                "select d, id, b2k as blgn, b2r as sortie  from "+nomTable+" where id =" +QString::number(i)+ " "
                "union all "
                "select d, id, b3k as blgn, b3r as sortie   from "+nomTable+" where id =" +QString::number(i)+ " "
                "union all "
                "select d, id, b4k as blgn, b4r as sortie   from "+nomTable+" where id =" +QString::number(i)+ " "
                "union all "
                "select d, id,b5k as blgn, b5r as sortie   from "+nomTable+" where id =" +QString::number(i)+ " "
                ") as tbleft "
                "left join "
                "("
                "select id,z1 from Bnrz where id <="
                +QString::number(pConf->limites[zone].max)+" "
                ")as tbright "
                "on"
                "("
                "tbright.z1 = tbleft.blgn "
                ") group by tbleft.blgn "
                ");";
#ifndef QT_NO_DEBUG
    qDebug() << msg1;
#endif

        status = query.exec(msg1);
        query.finish();

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
                        " set d=" + QString::number(loop)
                        + ", b"+QString::number(colId)+"v=" + QString::number(uneBoule)
                        + ", b"+QString::number(colId)+"k="  +QString::number(id)
                        + ", b"+QString::number(colId)+"r="  +QString::number(nb)+
                        " where "+ nomTable+".id="+QString::number(boule)+";";
#endif

                status = req_2.exec(msg1);
            }while (query.next() && status);
        }
    }


    return status;
}
