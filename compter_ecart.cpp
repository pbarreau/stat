#ifndef QT_NO_DEBUG
#include <QDebug>
#include <QSqlError>
#endif

#include <QTableView>
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSortFilterProxyModel>
#include <QHeaderView>

#include "compter_ecart.h"

int BCountEcart::total = 0;

BCountEcart::~BCountEcart()
{
    total --;
}

BCountEcart::BCountEcart(const QString &in, const int ze, const BGame &pDef,  QSqlDatabase fromDb)
    :BCount(pDef,in,fromDb,NULL,eCountElm)
{
    QString name = "";
    QTableView *qtv_tmp = NULL;
    int nb_zones = myGame.znCount;

    if (ze< nb_zones && ze >=0)
    {
        if(nb_zones == 1){
            hCommon = CEL2_H *(floor(myGame.limites[ze].max/10)+1);
        }
        else{
            if(ze<nb_zones-1)
                hCommon = CEL2_H * BMAX_2((floor(myGame.limites[ze].max/10)+1),(floor(myGame.limites[ze+1].max/10)+1));
        }
    }

    qtv_tmp = Compter(&name,ze);

    qtv_tmp->setParent(this);
    total++;
}

QTableView * BCountEcart::Compter(QString *pname, int zn)
{
    QTableView *qtv_tmp = new QTableView;
    QString qtv_name = QString::fromLatin1(cClc_eca)
            +"_"+ QString::number(total).rightJustified(3,'0')
            + "_z"+QString::number(zn+1);
    qtv_tmp->setObjectName(qtv_name);


    /// Creation de la table dans la base
    /// pour garder les resultat
    if(!createThatTable(qtv_name,zn)){
        /// stopper les calculs
        return   qtv_tmp;
    }

    /// suite du traitement
    QSqlQueryModel *sqm_tmp = new QSqlQueryModel;
    QString st_msg1 = "select * from ("
            + qtv_name
            +") order by B desc;";

    sqm_tmp->setQuery(st_msg1,dbToUse);

    qtv_tmp->setSortingEnabled(true);
    qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
    qtv_tmp->setAlternatingRowColors(true);
    qtv_tmp->setStyleSheet("QTableView {selection-background-color: #939BFF;}");

    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);

    QSortFilterProxyModel *m=new QSortFilterProxyModel();
    m->setDynamicSortFilter(true);
    m->setSourceModel(sqm_tmp);
    qtv_tmp->setModel(m);

    //qtv_tmp->setItemDelegate(new BDelegateElmOrCmb); /// Delegation

    qtv_tmp->verticalHeader()->hide();
    //qtv_tmp->hideColumn(0);
    qtv_tmp->setSortingEnabled(true);
    qtv_tmp->sortByColumn(0,Qt::AscendingOrder);

    //largeur des colonnes
    int nbCol = sqm_tmp->columnCount();
    for(int pos=0;pos<nbCol;pos++)
    {
        qtv_tmp->setColumnWidth(pos,CEL2_L);
    }
    int l = CEL2_L * (nbCol+1);
    qtv_tmp->setFixedWidth(l);

    qtv_tmp->setFixedHeight(hCommon);

    return   qtv_tmp;
}

bool BCountEcart::createThatTable(QString tblName, int zn)
{
    bool isOk = true;
    QString msg = "";
    QSqlQuery query(dbToUse);
    QString tmpTbl = "tmp_"+ tblName;

    msg = "create table if not exists "
            +tblName
            +"(B int, Ec int, Ep int, Em real, E int)";
    isOk = query.exec(msg);

    if(isOk){
        QString ref_1 = myGame.names[zn].abv+"%1 int";
        QString ref_2 = "";
        int tot_items = myGame.limites[zn].len;
        msg="";
        for(int items=0;items<tot_items;items++){
            msg = msg+ref_1.arg(items+1);
            if(items<tot_items-1) msg = msg+",";
        }
        msg = "create table if not exists "
                + tmpTbl
                +"(lgn integer primary key, id int,"
                + msg
                +")";
        QString doSql[]={"drop table if exists " +tmpTbl,
                         msg
                        };
        int nb_sql = sizeof(doSql)/sizeof(QString);
        /// pour chaque boule de l'ensemble
        /// executer le code de traitement de la table temporaire
        msg="";
        QString clause="";
        ref_1 = "t1."+myGame.names[zn].abv+"%1";
        ref_2 = "(t1."+myGame.names[zn].abv+"%1=%2)";
        for(int items=0;items<tot_items;items++){
            msg = msg+ref_1.arg(items+1);
            clause = clause + ref_2.arg(items+1).arg("%1");
            if(items<tot_items-1){
                msg = msg+",";
                clause = clause+"or";
            }
        }
        msg = "insert into "
                + tmpTbl
                +" select null, t1.id,"
                + msg
                +" from("
                +db_data
                +") as t1 where";

        int nb_boule = myGame.limites[zn].max;
        for(int boule=0;(boule<nb_boule)&&isOk;boule++){
            for(int sql_id=0;(sql_id<nb_sql)&&isOk;sql_id++){
                isOk = query.exec(doSql[sql_id]);
            }
            ///remplir la table
            if(isOk){
                QString sql = clause.arg(boule+1);
                sql = msg
                        +"("
                        + sql
                        +")";
                isOk = query.exec(sql);
#ifndef QT_NO_DEBUG
                qDebug() <<sql;
#endif

                /// si la table est remplie
                /// faire le calcul sur ecart
                /// et mettre dans table resultat
                if(isOk){
                    sql = "insert into "
                            + tblName
                            +" select max(B)as B, max(Ec) as Ec, max(Ep) as Ep,"
                             "printf(\"%.1f\",avg(E))as Em,max(E) as E from "
                             "("
                             "select "
                            +QString::number(boule+1)
                            +" as B,(case when t1.lgn=1 then t1.id -1 end)as Ec,"
                             "(case when t1.lgn=1 then (t2.id-t1.id)  end)as Ep,"
                             "(t2.id-t1.id) as E from "
                            +tmpTbl+" as t1, "
                            +tmpTbl+" as t2 "
                                    "where"
                                    "(t2.lgn=t1.lgn+1)"
                                    ")";
                    isOk = query.exec(sql);
#ifndef QT_NO_DEBUG
                    qDebug() <<sql;
#endif
                }
            }
        }
        /// suppression derniere table
        if(isOk){
            isOk = query.exec(doSql[0]);
        }
    }

    return isOk;
}
