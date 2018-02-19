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
#include <QItemDelegate>
#include <cmath>

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
            +"_"+ QString::fromLatin1(cClc_elm)
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
    BSqmColorizeEcart *sqm_tmp = new BSqmColorizeEcart;
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

    qtv_tmp->setItemDelegate(new BDlgEcart); /// Delegation

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

    //qtv_tmp->setFixedHeight(hCommon);
    qtv_tmp->setFixedHeight(CEL2_H*7);

    return   qtv_tmp;
}

bool BCountEcart::createThatTable(QString tblName, int zn)
{
    bool isOk = true;
    QString msg = "";
    QSqlQuery query(dbToUse);
    QString tmpTbl = "tmp_"+ tblName;

    /// B   : boule
    /// Ec  : ecart courant
    /// Ep  : ecart precedent
    /// Em  : ecart moyen
    /// E   : ecart maxi
    /// T   : total
    /// A   : apparition (deja sortie ?)
    msg = "create table if not exists "
            +tblName
            +"(B int, Ec int, Ep int, Em real, E int, T int, A int)";
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
                             "printf(\"%.1f\",avg(E))as Em,max(E) as E,"
                             "count(B) as T from "
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

///-----------------------------
/// Delegation couleur sur tableau ecart
void BDlgEcart::paint(QPainter *painter, const QStyleOptionViewItem &option,
                      const QModelIndex &index) const
{
    int col = index.column();
    int row = index.row();
    int nbCol = index.model()->columnCount();
    int valCel = 0;
    int valAna = 0;
    double radix = 1.5;
    int calc = 0;
    //int ecart = 0;
    QStyleOptionViewItem maModif(option);
    /*QColor fond[]= {QColor(255,106,0,255),
                 QColor(255,191,0,255),
                 QColor(101,148,255,255)};*/
    const QColor fond[4]={QColor(255,156,86,190),
                          QColor(140,255,124,190),
                          QColor(70,160,220,190),
                          QColor(255,40,180,190)
                         };


    //index.data().ca
    //if(index.data().canConvert(QMetaType::Int))
    {
        valCel = index.data().toInt();
    }
    valAna = index.model()->index(index.row(),1).data().toInt();

    if(valCel)
        calc = abs(valAna-valCel)% valCel;

    if(col==1){
        calc = index.model()->index(index.row(),2).data().toInt();
        valAna = index.model()->index(index.row(),3).data().toInt();
        calc = abs(valAna-calc)% valAna;
        if(valAna && calc<=radix){
            painter->fillRect(option.rect, fond[2]);
        }
    }

    if(col==2){
        /// colonne Ep
        if(valAna && calc<=radix){
            painter->fillRect(option.rect, fond[0]);
        }
    }
    if(col==3){
        /// colonne Em
        if(valAna && calc<=radix){
            painter->fillRect(option.rect, fond[1]);
        }
    }
    if(col==4){
        /// colonne Em
        if(valAna && calc<=radix){
            painter->fillRect(option.rect, fond[2]);
        }
    }

    /*

    /// Regarder la valeur de la derniere colonne
    /// Elle indique que mettre comme couleur
    if(index.model()->index(index.row(),nbCol-1).data().canConvert(QMetaType::Int))
    {
        val =  index.model()->index(index.row(),nbCol-1).data().toInt();
    }


    if(col>0 && col<(nbCol-1)){
        /// bit actif
        if(val & (1<<col-1)){
            painter->fillRect(option.rect, u[0]);
        }
    }
    */
    QItemDelegate::paint(painter, maModif, index);
}


QVariant BSqmColorizeEcart::data(const QModelIndex &index, int role) const
{
    QColor u[]= {
        Qt::black,
        Qt::red,
        Qt::green,
        QColor(255,216,0,255),
        QColor(255,106,0,255),
        QColor(178,0,255,255)};

    if(index.column()== 0 )
    {
        //int nbCol=index.model()->columnCount();
        double radix = 1.5;
        /// recuperation des l'info de la ligne
        QModelIndex Ec = index.sibling(index.row(),1);
        QModelIndex Ep = index.sibling(index.row(),2);
        QModelIndex Em = index.sibling(index.row(),3);
        QModelIndex ET = index.sibling(index.row(),4);


        int ec = Ec.data().toInt();
        int ep = Ep.data().toInt();
        int em = Em.data().toInt();
        int et = ET.data().toInt();

        if(ec &&((abs(ec-ep)%ep<=radix)||
                 (abs(ec-em)%em<=radix)||
                 (abs(ec-et)%et<=radix)
                 )){
            if (role == Qt::TextColorRole){
                return (u[1]);
            }
        }
    }

    return QSqlQueryModel::data(index,role);
}
