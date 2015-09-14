#ifndef QT_NO_DEBUG
#include <QDebug>
#endif


#include <QtGui>

#include <QSortFilterProxyModel>
#include <QHeaderView>

#include "refresultat.h"
#include "mainwindow.h"

extern MainWindow w;

QGridLayout *RefResultat::GetDisposition(void)
{
    return disposition;
}

QTableView *RefResultat::GetTable(void)
{
    return(tbv_bloc1);
}

RefResultat::RefResultat(int zn, stTiragesDef *pConf, QMdiArea *visuel)
{
    disposition = new QGridLayout;
    pEcran = visuel;
    pMaConf = pConf;
    curzn = zn;

    DoBloc1();
    DoBloc2();
    DoBloc3();
}

void RefResultat::DoBloc1(void)
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



    sqm_bloc1 = new QSqlQueryModel;
    QTableView *qtv_tmp = new QTableView;

    QString st_msg1 =
            "select tb1.boule as B, count(tb2.id) as T, "
            "count(CASE WHEN  jour_tirage like 'lundi%' then 1 end) as LUN,"
            "count(CASE WHEN  jour_tirage like 'mercredi%' then 1 end) as MER,"
            "count(CASE WHEN  jour_tirage like 'same%' then 1 end) as SAM "
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
    qtv_tmp->setFixedSize(225,485);

    QSortFilterProxyModel *m=new QSortFilterProxyModel();
    m->setDynamicSortFilter(true);
    m->setSourceModel(sqm_bloc1);
    qtv_tmp->setModel(m);

    for(int j=0;j<2;j++)
        qtv_tmp->setColumnWidth(j,30);
    for(int j=2;j<=sqm_bloc1->columnCount();j++)
        qtv_tmp->setColumnWidth(j,40);


    // Ne pas modifier largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    disposition->addWidget(qtv_tmp,0,0,2,1,Qt::AlignLeft|Qt::AlignTop);


    tbv_bloc1=qtv_tmp;

    // double click dans fenetre  pour afficher details boule
    connect( tbv_bloc1, SIGNAL(doubleClicked(QModelIndex)) ,
             this, SLOT(slot_MontreLesTirages( QModelIndex) ) );


}

void RefResultat::DoBloc2(void)
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

void RefResultat::DoBloc3(void)
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

    if(index.internalPointer() == tbv_bloc1->model()->index(index.row(),index.column()).internalPointer())
    {
        // Lecture du tableau bloc1
        //col = index.column();
        //rch = tbv_bloc1->model()->index(index.row(),0).data().toInt();
        MontreRechercheTirages(NE_Analyses::bToutes,tbv_bloc1,index);

    }
}

void RefResultat::MontreRechercheTirages(NE_Analyses::E_Syntese typeAnalyse,const QTableView *pTab,const QModelIndex & index)
{
    QWidget *qw_main = new QWidget;
    QTabWidget *tab_Top = new QTabWidget;
    QWidget **wid_ForTop = new QWidget*[2];
    QString stNames[2]={"Tirages","Repartition"};
    QGridLayout *design_onglet[2];

    // Tableau de pointeur de fonction
    QGridLayout *(RefResultat::*ptrFunc[2])(NE_Analyses::E_Syntese table,const QTableView *ptab,const QModelIndex & index)=
    {&RefResultat::MonLayout_pFnDetailsTirages,&RefResultat::MonLayout_pFnSyntheseDetails};


    for(int i =0; i<2;i++)
    {
        wid_ForTop[i]=new QWidget;
        tab_Top->addTab(wid_ForTop[i],tr(stNames[i].toUtf8()));

        //
        design_onglet[i] = (this->*ptrFunc[i])(typeAnalyse,pTab, index);
        wid_ForTop[i]->setLayout(design_onglet[i]);
    }

    int boule = index.row()+1;
    QFormLayout *mainLayout = new QFormLayout;
    QString st_titre = "Details Boule:" + QString::number(boule);
    mainLayout->addWidget(tab_Top);
    qw_main->setWindowTitle(st_titre);
    qw_main->setLayout(mainLayout);


    QMdiSubWindow *subWindow = pEcran->addSubWindow(qw_main);
    //subWindow->resize(493,329);
    //subWindow->move(737,560);
    qw_main->setVisible(true);
    qw_main->show();
}

QString RefResultat::DoSqlMsgRef_Tb1(int boule)
{
#if 0
    select t2.id as Tid,
            t2.jour_tirage as J,
            substr(t2.date_tirage,-2,2)||'/'||substr(t2.date_tirage,6,2)||'/'||substr(t2.date_tirage,1,4) as D,
            t3.tip as C,
            t2.b1 as b1, t2.b2 as b2,t2.b3 as b3,t2.b4 as b4,t2.b5 as b5,
            t2.e1 as e1,
            t2.bp as P,
            t2.bg as G
            from tirages as t2,
            lstcombi as t3,
            analyses as t4
            where
            (
                (t3.id = t4.id_poids and t4.id=t2.id)
                and
                (
                    t2.b1 = 1 or
            t2.b2 = 1 or
            t2.b3 = 1 or
            t2.b4 = 1 or
            t2.b5 = 1
            )
                );
#endif
    QString st_msg = "";

    st_msg = "select t2.id as Tid, "
             "t2.jour_tirage as J,"
             "substr(t2.date_tirage,-2,2)||'/'||substr(t2.date_tirage,6,2)||'/'||substr(t2.date_tirage,1,4) as D,"
             "t3.tip as C,"
             "t2.b1 as b1, t2.b2 as b2,t2.b3 as b3,t2.b4 as b4,t2.b5 as b5,"
             "t2.e1 as e1,"
             "t2.bp as P,"
             "t2.bg as G "
             "from tirages as t2,"
             "lstcombi as t3,"
             "analyses as t4 "
             "where"
             "("
             "(t3.id = t4.id_poids and t4.id=t2.id)"
             "and"
             "("
             "t2.b1 =" +QString::number(boule)
            + " or "
              "t2.b2 =" +QString::number(boule)
            + " or "
              " t2.b3 =" +QString::number(boule)
            + " or "
              " t2.b4 =" +QString::number(boule)
            + " or "
              " t2.b5 =" +QString::number(boule)
            + ")"
              ");";

    return(st_msg);
}

QGridLayout * RefResultat::MonLayout_pFnDetailsTirages(NE_Analyses::E_Syntese table,const QTableView *ptab,const QModelIndex & index)
{
    QGridLayout *lay_return = new QGridLayout;

    QString sql_msgRef = "";
    QSqlQueryModel *sqm_tmp = new QSqlQueryModel;
    QTableView *qtv_tmp = new QTableView;

    int ligne = index.row()+1;
    //int col = index.column();

    switch(table)
    {
    case NE_Analyses::bToutes:
    {
        sql_msgRef = DoSqlMsgRef_Tb1(ligne);
    }
        break;
    default:
        ; // Rien
    }

    sqm_tmp->setQuery(sql_msgRef);

    qtv_tmp->setSortingEnabled(false);
    //qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
    qtv_tmp->setAlternatingRowColors(true);


    qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //QSortFilterProxyModel *m=new QSortFilterProxyModel();
    //m->setDynamicSortFilter(true);
    //m->setSourceModel(sqm_tmp);
    qtv_tmp->setModel(sqm_tmp);
    qtv_tmp->hideColumn(0);

    for(int j=0;j<=3;j++)
        qtv_tmp->setColumnWidth(j,75);

    for(int j=4;j<=sqm_tmp->columnCount();j++)
        qtv_tmp->setColumnWidth(j,30);
    qtv_tmp->setFixedSize(525,205);


    // Ne pas modifier largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    // Double click dans fenetre de details
    connect( qtv_tmp, SIGNAL( doubleClicked(QModelIndex)) ,
             pEcran->parent(), SLOT(slot_MontreLeTirage( QModelIndex) ) );

    lay_return->addWidget(qtv_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);

    return(lay_return);
}
QGridLayout * RefResultat::MonLayout_pFnSyntheseDetails(NE_Analyses::E_Syntese table, const QTableView *ptab, const QModelIndex &index)
{
    QGridLayout *lay_return = new QGridLayout;
    QString sql_msgRef = "";
    QSqlQueryModel *sqm_tmp = new QSqlQueryModel;
    QTableView *qtv_tmp = new QTableView;

    int ligne = index.row()+1;

    switch(table)
    {
    case NE_Analyses::bToutes:
    {
        sql_msgRef = DoSqlMsgRef_Tb1(ligne);
        // Retirer le ; de la fin
        sql_msgRef.replace(";","");
        sql_msgRef = SD_Tb1(ligne,sql_msgRef);
#ifndef QT_NO_DEBUG
        qDebug() << sql_msgRef;
#endif

    }
        break;
    default:
        ; // Rien
    }

    sqm_tmp->setQuery(sql_msgRef);

    qtv_tmp->setSortingEnabled(true);
    qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
    qtv_tmp->setAlternatingRowColors(true);


    //qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
    qtv_tmp->setSelectionMode(QAbstractItemView::NoSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QSortFilterProxyModel *m=new QSortFilterProxyModel();
    m->setDynamicSortFilter(true);
    m->setSourceModel(sqm_tmp);
    qtv_tmp->setModel(m);

    for(int j=0;j<=sqm_tmp->columnCount();j++)
        qtv_tmp->setColumnWidth(j,30);
    qtv_tmp->setFixedSize(525,205);


    // Ne pas modifier largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);


    lay_return->addWidget(qtv_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);


    return(lay_return);
}

//Synthese detaille table 1
QString RefResultat::SD_Tb1(int boule, QString sqlTblRef)
{
#if 0
    select tb1.boule as B, count(tb2.Tid) as T
            from
            (
                select id as boule from Bnrz where (z1 not null )
                ) as tb1
            left join
            (
                --- Table droite
                select t2.id as Tid,
                t2.jour_tirage as J,
                substr(t2.date_tirage,-2,2)||'/'||substr(t2.date_tirage,6,2)||'/'||substr(t2.date_tirage,1,4) as D,
                t3.tip as C,
                t2.b1 as b1, t2.b2 as b2,t2.b3 as b3,t2.b4 as b4,t2.b5 as b5,
                t2.e1 as e1,
                t2.bp as P,
                t2.bg as G
                from tirages as t2,
                lstcombi as t3,
                analyses as t4
                where
                (
                    (t3.id = t4.id_poids and t4.id=t2.id)
                    and
                    (
                        t2.b1 = 1 or
            t2.b2 = 1 or
            t2.b3 = 1 or
            t2.b4 = 1 or
            t2.b5 = 1
            ))
                -- fin table droite

                ) as tb2
            on
            (
                (tb1.boule = tb2.b1 or tb1.boule = tb2.b2 or tb1.boule = tb2.b3 or tb1.boule = tb2.b4 or tb1.boule = tb2.b5)
                and
                (tb1.boule!=1)
                ) group by tb1.boule;



    --- Table droite
            select t2.id as Tid,
            t2.jour_tirage as J,
            substr(t2.date_tirage,-2,2)||'/'||substr(t2.date_tirage,6,2)||'/'||substr(t2.date_tirage,1,4) as D,
            t3.tip as C,
            t2.b1 as b1, t2.b2 as b2,t2.b3 as b3,t2.b4 as b4,t2.b5 as b5,
            t2.e1 as e1,
            t2.bp as P,
            t2.bg as G
            from tirages as t2,
            lstcombi as t3,
            analyses as t4
            where
            (
                (t3.id = t4.id_poids and t4.id=t2.id)
                and
                (
                    t2.b1 = 1 or
            t2.b2 = 1 or
            t2.b3 = 1 or
            t2.b4 = 1 or
            t2.b5 = 1
            ))
            -- fin table droite
            ;
#endif

    QString sql_msg =
            "select tb1.boule as B, count(tb2.Tid) as T, "
            "count(CASE WHEN  J like 'lundi%' then 1 end) as LUN,"
            "count(CASE WHEN  J like 'mercredi%' then 1 end) as MER,"
            "count(CASE WHEN  J like 'same%' then 1 end) as SAM "
            "from"
            "("
            "select id as boule from Bnrz where (z1 not null)"
            ") as tb1 "
            "left join"
            "("
            +sqlTblRef+
            ") as tb2 "
            "on"
            "("
            "(tb1.boule = tb2.b1 or tb1.boule = tb2.b2 or tb1.boule = tb2.b3 or tb1.boule = tb2.b4 or tb1.boule = tb2.b5) "
            " and "
            "(tb1.boule!="
            +QString::number(boule)
            +")"
             ") group by tb1.boule;";

    return sql_msg;
}
