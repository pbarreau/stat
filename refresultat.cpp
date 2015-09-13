#include <QtGui>

#include <QSortFilterProxyModel>
#include <QHeaderView>

#include "refresultat.h"
#include "mainwindow.h"

QGridLayout *RefResultat::GetDisposition(void)
{
    return disposition;
}

QTableView *RefResultat::GetTable(void)
{
    return(tbv_bloc1);
}

RefResultat::RefResultat(int zn, stTiragesDef *pConf)
{
    disposition = new QGridLayout;
    DoBloc1(zn,pConf);
    DoBloc2(zn,pConf);
    DoBloc3(zn,pConf);
}

void RefResultat::DoBloc1(int zn, stTiragesDef *pConf)
{
#if 0
    select tb1.boule as B, count(tb2.id) as T
            from
            (
                select id as boule from Bnrz where (z1 not null )
                ) as tb1
            left join
            (
                select * from tirages
                ) as tb2
            on
            (
                tb1.boule = tb2.b1 or tb1.boule = tb2.b2 or tb1.boule = tb2.b3 or tb1.boule = tb2.b4 or tb1.boule = tb2.b5
            ) group by tb1.boule;
#endif


    int lgn = pConf->limites[zn].max;
    //sim_bloc1 = new QStandardItemModel(lgn,2);

    sqm_bloc1 = new QSqlQueryModel;
    QTableView *qtv_tmp = new QTableView;

    QString st_msg1 = "select tb1.boule as B, count(tb2.id) as T "
                      "from  "
                      "("
                      "select id as boule from Bnrz where (z1 not null ) "
                      ") as tb1 "
                      "left join "
                      "("
                      "select * from tirages "
                      ") as tb2 "
                      "on "
                      "("
                      "tb1.boule = tb2.b1 or tb1.boule = tb2.b2 or tb1.boule = tb2.b3 or tb1.boule = tb2.b4 or tb1.boule = tb2.b5 "
                      ") group by tb1.boule;";

    sqm_bloc1->setQuery(st_msg1);

    qtv_tmp->setSortingEnabled(true);
    qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
    qtv_tmp->setAlternatingRowColors(true);


    qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtv_tmp->setFixedSize(105,485);

    QSortFilterProxyModel *m=new QSortFilterProxyModel();
    m->setDynamicSortFilter(true);
    m->setSourceModel(sqm_bloc1);
    qtv_tmp->setModel(m);

    for(int j=0;j<=sqm_bloc1->columnCount();j++)
        qtv_tmp->setColumnWidth(j,30);

    // Ne pas modifier largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    disposition->addWidget(qtv_tmp,0,0,2,1,Qt::AlignLeft|Qt::AlignTop);


    tbv_bloc1=qtv_tmp;

    // double click dans fenetre  pour afficher details boule
    connect( tbv_bloc1, SIGNAL(doubleClicked(QModelIndex)) ,
             this, SLOT(slot_MontreLesTirages( QModelIndex) ) );


}

void RefResultat::DoBloc2(int zn, stTiragesDef *pConf)
{
    QSqlTableModel *tblModel = new QSqlTableModel;
    tblModel->setTable("repartition_bh");
    tblModel->select();
    sqtblm_bloc2 = tblModel;

    // Associer toutes les valeurs a la vue
    while (tblModel->canFetchMore())
    {
        tblModel->fetchMore();
    }

    // Attach it to the view
    QTableView *qtv_tmp  = new QTableView;

    // Gestion du QTableView
    qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    //qtv_tmp->setStyleSheet("QTableView {selection-background-color: red;}");
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtv_tmp->setAlternatingRowColors(true);
    qtv_tmp->setFixedSize(412,205);

    qtv_tmp->setModel(tblModel);
    qtv_tmp->setSortingEnabled(true);
    qtv_tmp->hideColumn(0);
    qtv_tmp->sortByColumn(1,Qt::AscendingOrder);

    for(int i=0;i<tblModel->columnCount();i++)
        qtv_tmp->setColumnWidth(i,30);

    QHeaderView *htop = qtv_tmp->horizontalHeader();
    htop->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);


    disposition->addWidget(qtv_tmp,0,1,Qt::AlignLeft|Qt::AlignTop);
    tbv_bloc2=qtv_tmp;
}

void RefResultat::DoBloc3(int zn, stTiragesDef *pConf)
{
    QSqlTableModel *tblModel = new QSqlTableModel;
    tblModel->setTable("repartition_bv");
    tblModel->select();
    sqtblm_bloc3 = tblModel;

    // Associer toutes les valeurs a la vue
    while (tblModel->canFetchMore())
    {
        tblModel->fetchMore();
    }

    // Attach it to the view
    QTableView *qtv_tmp  = new QTableView;

    // Gestion du QTableView
    qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtv_tmp->setAlternatingRowColors(true);
    qtv_tmp->setFixedSize(285,270);

    qtv_tmp->setModel(tblModel);
    qtv_tmp->setSortingEnabled(true);
    //qtv_tmp->hideColumn(0);
    qtv_tmp->sortByColumn(0,Qt::AscendingOrder);

    for(int i=0;i<tblModel->columnCount();i++)
        qtv_tmp->setColumnWidth(i,30);

    QHeaderView *htop = qtv_tmp->horizontalHeader();
    htop->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    //htop=qtv_tmp->verticalHeader();
    //htop->setUserData();

    disposition->addWidget(qtv_tmp,1,1,Qt::AlignLeft|Qt::AlignTop);
    tbv_bloc3=qtv_tmp;
}

void RefResultat::slot_MontreLesTirages(const QModelIndex & index)
{
    int col = 0;
    int rch = 0;


//    if(index.internalPointer() == sqm_bloc1->index(index.row(),index.column()).internalPointer())
    if(index.internalPointer() == tbv_bloc1->model()->index(index.row(),index.column()).internalPointer())
    {
        // Lecture du tableau bloc1
        col = index.column();
        //rch = sqm_bloc1->index(index.row(),0).data().toInt();
        rch = tbv_bloc1->model()->index(index.row(),0).data().toInt();
        if(rch)
            col++;

    }
}

