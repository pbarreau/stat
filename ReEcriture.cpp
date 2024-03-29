#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QtGui>
#include "mainwindow.h"

void MainWindow::NEW_ChercherTotalBoules(QStringList choix, stTiragesDef *pConf)
{
    QSqlQuery sql_1(db_0);
    QString msg = "";
    bool status = true;
    QString ColNames[4]={"r0","r1","rn1","tot"};
    int val [3] = {0,-1,1};

    for(int distance=0;(distance<4)&&(status ==true);distance++)
    {
        if(distance < 3)
        {
            msg = NEW_ChercherTotalBoulesAUneDistance(choix,val[distance],pConf);
        }
        else
        {
            msg= "select id, (r0+r1+rn1) from VoisinSql;";
        }
        status = sql_1.exec(msg);

        if (status)
        {
            sql_1.first();
            if(sql_1.isValid())
            {
                do
                {
                    QSqlQuery req_2(db_0);
                    int bId = sql_1.value(0).toInt();
                    int tot = sql_1.value(1).toInt();

                    msg = "update VoisinSql set "+ColNames[distance]+"="
                            +QString::number(tot)
                            + " "
                              "where VoisinSql.id="+QString::number(bId)+";";
                    status = req_2.exec(msg);

                }while(sql_1.next() && status);

            }
        }
    }
    // Mettre a jour la vue de la table
    G_tblPrev_v2->select();

}

QString MainWindow::NEW_ChercherTotalBoulesAUneDistance(QStringList selectionBoule, int distance, stTiragesDef *pConf,int trie)
{
#if 0 //exemple de requete executable sur la base
    select t3.boule as B1, table_2.T as T1
            from (select oazb.boule from oazb)as t3
            left join
            (
                select t1.boule as B, count (t1.boule) as T
                from (select oazb.boule from oazb where(oazb.boule != 1))as t1
                left join
                (
                    select *
                    from
                    (select id as id1 from tirages
                     where
                     (
                         tirages.b1=1 or
            tirages.b2=1 or
            tirages.b3=1 or
            tirages.b4=1 or
            tirages.b5=1
            )
                     ) as tabl4
                    left join tirages on tabl4.id1=tirages.id + 1
            ) as t2
                on
                (
                    (
                        t1.boule = t2.b1 or
            t1.boule = t2.b2 or
            t1.boule = t2.b3 or
            t1.boule = t2.b4 or
            t1.boule = t2.b5
            )
                    ) group by boule) as table_2
            on (B1=table_2.B) group by B1 order by B1 asc;

    // --------- autre requete meilleure
    select tb1.boule, count(tb2.id) as T from oazb as tb1
            left join
            (
                select tb6.* from
                (select * from tirages as base1 where
                 (
                     (b1=1 or b2=1 or b3=1 or b4=1 or b5=1 )
                     and
                     (b1=2 or b2=2 or b3=2 or b4=2 or b5=2 )
                     )
                 ) as tb5
                left join tirages as tb6  on (tb6.id = tb5.id + -1)
                )as tb2 on
            (
                (tb1.boule != 1 and tb1.boule != 2)
                and
                (
                    tb1.boule = tb2.b1 or
            tb1.boule = tb2.b2 or
            tb1.boule = tb2.b3 or
            tb1.boule = tb2.b4 or
            tb1.boule = tb2.b5
            )
                ) group by tb1.boule;


#endif

    // ------------------
    QString st_msg_1 = "";
    QString st_msg_2 = "";
    QString st_msg_3 = "";
    QString st_msg_4 = "";


    bool status = false;
    QSqlQuery sql_req(db_0);
    int zn =0;


    if(distance == 0)
    {
        st_msg_1 = NEW_ExceptionBoule(zn,pConf,selectionBoule);
        st_msg_1 = st_msg_1 + " and ";
    }

    if (trie)
    {
      st_msg_4 = " order  by T desc, tb1.boule asc";
    }

    st_msg_2 = DB_tirages->TST_ConstruireWhereData(zn,pConf,selectionBoule);

    st_msg_3 =  "select tb1.boule, count(tb2.id) as T from oazb as tb1 "
                "left join "
                "("
                "select tb6.* from "
                "(select * from tirages as base1 where "
                "("
            + st_msg_2 +
            ")"
            ") as tb5 "
            "left join tirages as tb6  on (tb5.id = tb6.id + " +QString::number(distance)+ ") "
                                                                                           ")as tb2 on "
                                                                                           "("
            + st_msg_1 +
            "("
            "tb1.boule = tb2.b1 or "
            "tb1.boule = tb2.b2 or "
            "tb1.boule = tb2.b3 or "
            "tb1.boule = tb2.b4 or "
            "tb1.boule = tb2.b5 "
            ")"
            ") group by tb1.boule "
            + st_msg_4 + ";";

#ifndef QT_NO_DEBUG
    qDebug() << st_msg_3;
#endif

    return(st_msg_3);
}

QString MainWindow::NEW_ExceptionBoule(int zn, stTiragesDef *pConf,QStringList &boules)
{
    //QString col(QString::fromLocal8Bit(CL_TOARR) + pConf->nomZone[zn]);
    QString msg= "" ;
    QString flag = " and ";

    for(int i=0; i< boules.size();i++)
    {
        int val_boule = boules.at(i).toInt();
        msg = msg + "(tb1.boule !=" +QString::number(val_boule)+ ")"
                + flag;
    }

    msg.remove(msg.length()-flag.length(),flag.length());

    return msg;
}
