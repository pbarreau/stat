#ifndef QT_NO_DEBUG
#include <QDebug>
#include <QSqlError>
#endif

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

#include <QGridLayout>
#include <QSqlQueryModel>
#include <QStandardItemModel>
#include <QTableView>
#include <QHeaderView>
#include <QAbstractItemModel>
#include <QToolTip>

#include "barcalculprevision.h"
#include "game.h"

BarCalculPrevision::BarCalculPrevision(QModelIndex index, QSqlDatabase db, B_Game game)
{
    m_index = index;
    dbInUse = db;
    onGame = game;
    tirId = index.model()->index(index.row(),0).data().toInt();


    QSqlQuery query(db);
    bool status = false;
    QString msg1 = "";
    QString msg2 = "";


    int zn = 0;
    int max = game.limites[zn].len;
    QString ref = "t2."+game.names[zn].abv+"%1";
    for(int i = 0; i<max;i++){
        msg2 = msg2 +  ref.arg(i+1);
        if(i<max-1){
            msg2 = msg2 + ",";
        }
    }

    msg1 = "select group_concat(t1.id) as b from B_elm as t1 "
           "inner join B_fdj as t2 "
           "on (t2.id="
            +QString::number(tirId)
            + QString(" and (t1.id in (")+msg2+")))";

#ifndef QT_NO_DEBUG
    qDebug()<< msg1;
#endif

    status = query.exec(msg1);
    if(status){
        query.first();
        tirRef = query.record().value("b").toString();
    }
    this->setWindowTitle(tirRef);

    // creation tableau resultat
    demareRecherche();
}

void BarCalculPrevision::demareRecherche(void)
{
    QSqlQuery query(dbInUse);
    bool status = false;

    QGridLayout *lay_return = new QGridLayout;
    QSqlQueryModel *sqm_tmp = new QSqlQueryModel;
    QStandardItemModel * tmpStdItem = NULL;
    QTableView *qtv_tmp = new QTableView;
    int zn = 0;
    int max = onGame.limites[zn].len;

    QString head = "B,-2,-1,0,+1,+2";
    QStringList lstHead = head.split(",");
    QStringList b = tirRef.split(",");

    int nbCol = lstHead.size();
    int nbLgn = onGame.limites[zn].len;
    tmpStdItem =  new QStandardItemModel(nbLgn,nbCol);
    tmpStdItem->setHorizontalHeaderLabels(lstHead);

    for(int lgn=0;lgn<nbLgn;lgn++)
    {
        for(int pos=0;pos <=nbCol;pos++)
        {
            QStandardItem *item = new QStandardItem();
            tmpStdItem->setItem(lgn,pos,item);
        }
    }
    qtv_tmp->setModel(tmpStdItem);

    // Gestion du QTableView
    qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setStyleSheet("QTableView {selection-background-color: #939BFF;}");

    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtv_tmp->setAlternatingRowColors(true);
    //qtv_tmp->setFixedSize(CLargeur1*1.8,CHauteur1);

    qtv_tmp->setSortingEnabled(true);
    qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
    qtv_tmp->verticalHeader()->hide();

    QHeaderView *htop = qtv_tmp->horizontalHeader();
    htop->setSectionResizeMode(QHeaderView::ResizeToContents);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    qtv_tmp->setMouseTracking(true);
    connect(qtv_tmp,
            SIGNAL(entered(QModelIndex)),this,SLOT(slot_AideToolTip(QModelIndex)));

    // Remplir le tableau
    status = true;
    int resultat = 0;
    int value = 0;
    QString tmp = "";
    QString msg = "";
    for(int req=1;(req<nbCol)&&status;req++){
        tmp = lstHead.at(req);
        value = tmp.toInt();
        msg = Recherche(value);
#ifndef QT_NO_DEBUG
        qDebug()<< msg;
#endif
        status = query.exec(msg);
        if(status){
            query.first();

            for(int i = 0; (i <nbLgn)&& status ;i++){
                if(req == 1){
                    QStandardItem * boule = tmpStdItem->item(i,0);
                    int b_val = query.record().value("b").toInt();
                    boule->setData(b_val,Qt::DisplayRole);
                    tmpStdItem->setItem(i, 0,boule);
                }

                resultat = query.record().value("R").toInt();
                QStandardItem * item_1 = tmpStdItem->item(i,req);
                item_1->setData(resultat,Qt::DisplayRole);
                tmpStdItem->setItem(i, req,item_1);
                status = query.next();
            }
            status = true;
        }
    }

    lay_return->addWidget(qtv_tmp);
    this->setLayout(lay_return);
}

void BarCalculPrevision::slot_AideToolTip(const QModelIndex & index)
{
    QTableView *view = qobject_cast<QTableView *>(sender());

    QBrush C = index.data(Qt::BackgroundRole).value<QBrush>();
    QBrush vide;

    QString msg = "";
    QString msgAdd = "";
    int val = index.model()->index(index.row(),0).data().toInt();
    int cln = index.column();

    if(cln > 0 && cln<6)
    {
        const QAbstractItemModel * pModel = index.model();
        QVariant cellVal = index.data();
        QVariant vCol = pModel->headerData(cln,Qt::Horizontal);
        QString colName = vCol.toString();
        int colval = colName.toInt();

        msg = "Boule = " + QString::number(val+colval)+"\n";
        msgAdd = cellVal.toString()+ " fois en t+1";
    }

    msg = msg + msgAdd;
    QToolTip::showText (QCursor::pos(), msg);

}
QString BarCalculPrevision::Recherche(int increment)
{
    QString   tmp = "";

    tmp = "select t1.b as B, count (t2.id) as 'R' from "
          "( "
          "select t1.id as b from B_elm as t1 "
          "inner join B_fdj as t2 "
          "on "
          "( "
          " t2.id = "+QString::number(tirId)+
          " and "
          " ( "
          " t1.id in (t2.b1,t2.b2,t2.b3,t2.b4,t2.b5) "
          " ) "
          ") order by b asc "
          ")as t1 "
          " "
          "left join "
          "( "
          "select t1.* from B_fdj as t1 inner join "
          "( "
          "select * from B_fdj as t1  "
          "where ( "
          "(t1.b1  in ("+tirRef+")) "
          "or "
          "(t1.b2  in ("+tirRef+")) "
          "or "
          "(t1.b3  in ("+tirRef+")) "
          "or "
          "(t1.b4  in ("+tirRef+")) "
          "or "
          "(t1.b5  in ("+tirRef+")) "
          ") "
          ") as t2 "
          "on  "
          "( "
          "(t1.id = t2.id -1) "
          ") "
          ")as t2 "
          " "
          "on "
          "( "
          " b = (t2.b1 + ("+QString::number(increment)+")) "
          " or "
          " b = (t2.b2 +("+QString::number(increment)+")) "
          " or "
          " b = (t2.b3 +("+QString::number(increment)+")) "
          " or "
          " b = (t2.b4 +("+QString::number(increment)+")) "
          " or "
          " b = (t2.b5 +("+QString::number(increment)+")) "
          ") "
          "group by b;";

    return tmp;
}
