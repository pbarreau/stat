#ifndef QT_NO_DEBUG
#include <QDebug>
#endif


#include <QtGui>

#include <QSortFilterProxyModel>
#include <QHeaderView>
#include <QLineEdit>

#include "refresultat.h"
#include "mainwindow.h"

extern MainWindow w;
//QString GEN_Where_1(int zn, stTiragesDef *pConf, QStringList &boules, QString op1, QString op2, QString alias)
//;
//QString GEN_Where_2(stTiragesDef *pConf, int zone, QString operateur, int boule, QString critere,QString alias)
//;

QString GEN_Where_3(int loop, QString tb1, bool inc1,
                    QString op1, QStringList &tb2, bool inc2,
                    QString op2);

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
        QStringList stl_tmp;

        int boule_id = index.row()+1;
        stl_tmp << QString::number(boule_id);
        // Lecture du tableau bloc1
        //col = index.column();
        //rch = tbv_bloc1->model()->index(index.row(),0).data().toInt();
        MontreRechercheTirages(NE_Analyses::bToutes,stl_tmp);

    }
}
void RefResultat::MontreRechercheTirages(NE_Analyses::E_Syntese typeAnalyse,QStringList & lst_boules)
{
    QWidget *qw_main = new QWidget;
    QTabWidget *tab_Top = new QTabWidget;
    QTabWidget **wid_ForTop_1 = new QTabWidget*[4];
    QString stNames_1[4]={"0","1","-1","?"};
    int distance[4]={0,-1,1,-2};
    bool spe[4]={false,false,false,true};
    //QGridLayout *design_onglet_1[4];

    for(int i =0; i<4;i++)
    {
        wid_ForTop_1[i]=new QTabWidget;
        tab_Top->addTab(wid_ForTop_1[i],tr(stNames_1[i].toUtf8()));

        QWidget **wid_ForTop_2 = new QWidget*[2];
        QString stNames_2[2]={"Tirages","Repartition"};
        QGridLayout *design_onglet_2[2];

        // Tableau de pointeur de fonction
        QGridLayout *(RefResultat::*ptrFunc[2])(NE_Analyses::E_Syntese table,QStringList & lst_boules, int dist, bool spe)=
        {&RefResultat::MonLayout_pFnDetailsTirages,&RefResultat::MonLayout_pFnSyntheseDetails};

        // Verifier si on est sur l'onglet n° 4
        if(i==3)
        {
            // Memoriser les layouts de l'onglet
            G_design_onglet_2 =new QGridLayout*[2];

        }

        for(int j =0; j<2;j++)
        {
            wid_ForTop_2[j]=new QWidget;
            wid_ForTop_1[i]->addTab(wid_ForTop_2[j],tr(stNames_2[j].toUtf8()));

            //
            design_onglet_2[j] = (this->*ptrFunc[j])(typeAnalyse, lst_boules, distance[i],spe[i]);
            wid_ForTop_2[j]->setLayout(design_onglet_2[j]);

            // Verifier si on est sur l'onglet n° 4
            if(i==3)
            {
                // Memoriser les layouts de l'onglet
                G_design_onglet_2 [j]= design_onglet_2[j];

            }

        }

    }

    QFormLayout *mainLayout = new QFormLayout;
    QString st_titre = "Details Boule : " + lst_boules.at(0);
    mainLayout->addWidget(tab_Top);
    qw_main->setWindowTitle(st_titre);
    qw_main->setLayout(mainLayout);


    QMdiSubWindow *subWindow = pEcran->addSubWindow(qw_main);
    //subWindow->resize(493,329);
    //subWindow->move(737,560);
    qw_main->setVisible(true);
    qw_main->show();
}

#if 0
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
    QString st_titre = "Details Boule : " + QString::number(boule);
    mainLayout->addWidget(tab_Top);
    qw_main->setWindowTitle(st_titre);
    qw_main->setLayout(mainLayout);


    QMdiSubWindow *subWindow = pEcran->addSubWindow(qw_main);
    //subWindow->resize(493,329);
    //subWindow->move(737,560);
    qw_main->setVisible(true);
    qw_main->show();
}

#endif


#if EXEMPLE_SQL
--debut requete tb3
select tb3.id as Tid, tb5.id as Pid,
tb3.jour_tirage as J,
substr(tb3.date_tirage,-2,2)||'/'||substr(tb3.date_tirage,6,2)||'/'||substr(tb3.date_tirage,1,4) as D,
tb5.tip as C,
tb3.b1 as b1, tb3.b2 as b2,tb3.b3 as b3,tb3.b4 as b4,tb3.b5 as b5,
tb3.e1 as e1,
tb3.bp as P,
tb3.bg as G
from tirages as tb3, analyses as tb4, lstcombi as tb5
inner join
(
        select *  from tirages as tb1
        where
        (
            (
                tb1.b1=27 or
        tb1.b2=27 or
        tb1.b3=27 or
        tb1.b4=27 or
        tb1.b5=27
        )
            )
        ) as tb2
on (
        (tb3.id = tb2.id + 0)
        and
        (tb4.id = tb3.id)
        and
        (tb4.id_poids = tb5.id)
        )
;
--Fin requete tb3


-- Requete comptage du resultat precedent
select tbleft.boule as B, count(tbright.Tid) as T,
count(CASE WHEN  J like 'lundi%' then 1 end) as LUN, count(CASE WHEN  J like 'mercredi%' then 1 end) as MER, count(CASE WHEN  J like 'same%' then 1 end) as SAM
from
(
        select id as boule from Bnrz where (z1 not null )
        ) as tbleft
left join
(
        --debut requete tb3
        select tb3.id as Tid,
        tb3.jour_tirage as J,
        substr(tb3.date_tirage,-2,2)||'/'||substr(tb3.date_tirage,6,2)||'/'||substr(tb3.date_tirage,1,4) as D,
        tb5.tip as C,
        tb3.b1 as b1, tb3.b2 as b2,tb3.b3 as b3,tb3.b4 as b4,tb3.b5 as b5,
        tb3.e1 as e1,
        tb3.bp as P,
        tb3.bg as G
        from tirages as tb3, analyses as tb4, lstcombi as tb5
        inner join
        (
            select *  from tirages as tb1
            where
            (
                (
                    tb1.b1=27 or
        tb1.b2=27 or
        tb1.b3=27 or
        tb1.b4=27 or
        tb1.b5=27
        )
                )
            ) as tb2
        on (
            (tb3.id = tb2.id + 0)
            and
            (tb4.id = tb3.id)
            and
            (tb4.id_poids = tb5.id)
            )
        --Fin requete tb3
        ) as tbright
on
(
        (
            tbleft.boule = tbright.b1 or
        tbleft.boule = tbright.b2 or
        tbleft.boule = tbright.b3 or
        tbleft.boule = tbright.b4 or
        tbleft.boule = tbright.b5
        )
        and
        (
            tbleft.boule != 27
        )
        ) group by tbleft.boule;
#endif


QString RefResultat::DoSqlMsgRef_Tb1(QStringList &boules, int dst)
{
#if 0
    --debut requete tb3
            select tb3.id as Tid, tb5.id as Pid,
            tb3.jour_tirage as J,
            substr(tb3.date_tirage,-2,2)||'/'||substr(tb3.date_tirage,6,2)||'/'||substr(tb3.date_tirage,1,4) as D,
            tb5.tip as C,
            tb3.b1 as b1, tb3.b2 as b2,tb3.b3 as b3,tb3.b4 as b4,tb3.b5 as b5,
            tb3.e1 as e1,
            tb3.bp as P,
            tb3.bg as G
            from tirages as tb3, analyses as tb4, lstcombi as tb5
            inner join
            (
                select *  from tirages as tb1
                where
                (
                    (
                        tb1.b1=27 or
            tb1.b2=27 or
            tb1.b3=27 or
            tb1.b4=27 or
            tb1.b5=27
            )
                    )
                ) as tb2
            on (
                (tb3.id = tb2.id + 0)
                and
                (tb4.id = tb3.id)
                and
                (tb4.id_poids = tb5.id)
                );

    --Fin requete tb3
        #endif
            QString st_msg = "";
    QString st_cri1 = "";

    int loop = pMaConf->nbElmZone[curzn];
    st_cri1= GEN_Where_3(loop,"tb1.b",true,"=",boules,false,"or");

    st_msg =
            "select tb3.id as Tid, tb5.id as Pid,"
            "tb3.jour_tirage as J,"
            "substr(tb3.date_tirage,-2,2)||'/'||substr(tb3.date_tirage,6,2)||'/'||substr(tb3.date_tirage,1,4) as D,"
            "tb5.tip as C,"
            "tb3.b1 as b1, tb3.b2 as b2,tb3.b3 as b3,tb3.b4 as b4,tb3.b5 as b5,"
            "tb3.e1 as e1,"
            "tb3.bp as P,"
            "tb3.bg as G "
            "from tirages as tb3, analyses as tb4, lstcombi as tb5 "
            "inner join"
            "("
            "select *  from tirages as tb1 "
            "where"
            "("
            +st_cri1+
            ")"
            ") as tb2 "
            "on ("
            "(tb3.id = tb2.id + "
            +QString::number(dst)
            +") "
             "and"
             "(tb4.id = tb3.id)"
             "and"
             "(tb4.id_poids = tb5.id)"
             ");"
            ;

    return(st_msg);
}

QGridLayout * RefResultat::MonLayout_pFnDetailsTirages(NE_Analyses::E_Syntese table, QStringList &stl_tmp, int distance, bool ongSpecial)
{
    QGridLayout *lay_return = new QGridLayout;

    QString sql_msgRef = "";
    QSqlQueryModel *sqm_tmp = new QSqlQueryModel;
    QTableView *qtv_tmp = new QTableView;


    switch(table)
    {
    case NE_Analyses::bToutes:
    {
        sql_msgRef = DoSqlMsgRef_Tb1(stl_tmp,distance);
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
    qtv_tmp->hideColumn(1);

    for(int j=0;j<=4;j++)
        qtv_tmp->setColumnWidth(j,75);

    for(int j=5;j<=sqm_tmp->columnCount();j++)
        qtv_tmp->setColumnWidth(j,30);
    qtv_tmp->setFixedSize(525,205);


    // Ne pas modifier largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    // Double click dans fenetre de details
    connect( qtv_tmp, SIGNAL( doubleClicked(QModelIndex)) ,
             pEcran->parent(), SLOT(slot_MontreLeTirage( QModelIndex) ) );

    int pos_x = 0;
    int pos_y = 0;
    if(ongSpecial)
    {
        // Dernier onglet
        QFormLayout *distLayout = new QFormLayout;
        dist = new QLineEdit(QString::number((distance*-1)));

        distLayout->addRow("&Distance", dist);
        lay_return->addLayout(distLayout,0,0,Qt::AlignLeft|Qt::AlignTop);
        bSelection = stl_tmp;
        // Connection du line edit
        connect(dist, SIGNAL(returnPressed()),
                this, SLOT(slot_NouvelleDistance()));

        pos_y++;
    }

    lay_return->addWidget(qtv_tmp,pos_y,pos_x,Qt::AlignLeft|Qt::AlignTop);

    return(lay_return);
}

void RefResultat::slot_NouvelleDistance(void)
{
    QString msg = "";
    int new_distance = dist->text().toInt();

    // pour Coherence par rapport a graphique et requete
    new_distance *=-1;

    // Tableau de pointeur de fonction
    QGridLayout *(RefResultat::*ptrFunc[2])(NE_Analyses::E_Syntese table,QStringList & lst_boules, int dist, bool spe)=
    {&RefResultat::MonLayout_pFnDetailsTirages,&RefResultat::MonLayout_pFnSyntheseDetails};

    for(int j =0; j<2;j++)
    {
        // Effacer l'ancien layout et mettre le nouveau
        QGridLayout * oldOne = G_design_onglet_2[j];
        QGridLayout * monTest;


        monTest = (this->*ptrFunc[j])(
                    NE_Analyses::bToutes,
                    bSelection,
                    new_distance,true);

        // nouveau dessin ok.
        // Rechercher l'ancien pour suppression et reaffectation;
        QWidget *onp = oldOne->parentWidget();
        delete(oldOne);

        onp->setLayout(monTest);
        G_design_onglet_2[j]=monTest;
    }
}

QGridLayout * RefResultat::MonLayout_pFnSyntheseDetails(NE_Analyses::E_Syntese table, QStringList &stl_tmp, int distance, bool ongSpecial)
{
    QGridLayout *lay_return = new QGridLayout;

    Synthese_1(lay_return,table,stl_tmp,distance,ongSpecial);
    Synthese_2(lay_return,table,stl_tmp,distance,ongSpecial);


    return(lay_return);
}

void RefResultat::Synthese_2(QGridLayout *lay_return,NE_Analyses::E_Syntese table, QStringList &stl_tmp, int distance, bool ongSpecial)
{
    //-------------------
    QLabel *titre_2 = new QLabel("Etoiles");
    QTableView *qtv_tmp_2 = new QTableView;
    qtv_tmp_2->setFixedSize(340,230);

    MonQtViewDelegate *la = new MonQtViewDelegate;
    //QStandardItemModel *model=new QStandardItemModel(4, 2);

    QString sql_msgRef = "";
    MaSqlRequeteEditable *model = new MaSqlRequeteEditable;

    sql_msgRef = "select z1 as B, null as etoiles from Bnrz where z1 not null;";
    model->setQuery(sql_msgRef);

    qtv_tmp_2->setSortingEnabled(false);
    //qtv_tmp_2->sortByColumn(0,Qt::AscendingOrder);
    qtv_tmp_2->setAlternatingRowColors(true);


    qtv_tmp_2->setSelectionMode(QAbstractItemView::SingleSelection);
    qtv_tmp_2->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp_2->setEditTriggers(QAbstractItemView::DoubleClicked);

    qtv_tmp_2->setModel(model);
    qtv_tmp_2->setItemDelegate(la);

    // Pb Sqlite sur count
    sql_msgRef = "select count(z1) as B from Bnrz where z1 not null;";
    QSqlQuery qry_tmp;
    int nblignes;
    if(qry_tmp.exec(sql_msgRef))
    {
        qry_tmp.first();
        nblignes = qry_tmp.value(0).toInt();
    }

    for (int row = 0; row < nblignes; ++row) {
        //QModelIndex index = model->index(row, 0, QModelIndex());
        //model->setData(index, QVariant((row + 10)));
        qtv_tmp_2->setRowHeight(row,205);
    }
    qtv_tmp_2->setColumnWidth(0,40);
    qtv_tmp_2->setColumnWidth(1,250);
    //qtv_tmp_2->resizeColumnsToContents();
    // Ne pas modifier largeur des colonnes
    qtv_tmp_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp_2->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    lay_return->addWidget(titre_2,0,1,Qt::AlignLeft|Qt::AlignTop);
    lay_return->addWidget(qtv_tmp_2,1,1,Qt::AlignLeft|Qt::AlignTop);
    //------------------
}

void RefResultat::Synthese_1(QGridLayout *lay_return,NE_Analyses::E_Syntese table, QStringList &stl_tmp, int distance, bool ongSpecial)
{
    QString sql_msgRef = "";
    QSqlQueryModel *sqm_tmp = new QSqlQueryModel;
    QTableView *qtv_tmp = new QTableView;



    switch(table)
    {
    case NE_Analyses::bToutes:
    {

        sql_msgRef = DoSqlMsgRef_Tb1(stl_tmp,distance);
        // Retirer le ; de la fin
        sql_msgRef.replace(";","");

        sql_msgRef = SD_Tb1(stl_tmp,sql_msgRef,distance);
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
    qtv_tmp->setFixedSize(200,230);


    // Ne pas modifier largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    QLabel *titre_1 = new QLabel("Voisins");
    //lay_return->addWidget(titre_1,0,0,Qt::AlignCenter|Qt::AlignTop);
    lay_return->addWidget(titre_1,0,0,Qt::AlignLeft|Qt::AlignTop);
    lay_return->addWidget(qtv_tmp,1,0,Qt::AlignLeft|Qt::AlignTop);
}

//Synthese detaille table 1
QString RefResultat::SD_Tb1(QStringList boules, QString sqlTblRef,int dst)
{
#if 0
    -- Requete comptage du resultat precedent
            select tbleft.boule as B, count(tbright.Tid) as T,
            count(CASE WHEN  J like 'lundi%' then 1 end) as LUN, count(CASE WHEN  J like 'mercredi%' then 1 end) as MER, count(CASE WHEN  J like 'same%' then 1 end) as SAM
            from
            (
                select id as boule from Bnrz where (z1 not null )
                ) as tbleft
            left join
            (
                --debut requete tb3
                select tb3.id as Tid,
                tb3.jour_tirage as J,
                substr(tb3.date_tirage,-2,2)||'/'||substr(tb3.date_tirage,6,2)||'/'||substr(tb3.date_tirage,1,4) as D,
                tb5.tip as C,
                tb3.b1 as b1, tb3.b2 as b2,tb3.b3 as b3,tb3.b4 as b4,tb3.b5 as b5,
                tb3.e1 as e1,
                tb3.bp as P,
                tb3.bg as G
                from tirages as tb3, analyses as tb4, lstcombi as tb5
                inner join
                (
                    select *  from tirages as tb1
                    where
                    (
                        (
                            tb1.b1=27 or
            tb1.b2=27 or
            tb1.b3=27 or
            tb1.b4=27 or
            tb1.b5=27
            )
                        )
                    ) as tb2
                on (
                    (tb3.id = tb2.id + 0)
                    and
                    (tb4.id = tb3.id)
                    and
                    (tb4.id_poids = tb5.id)
                    )
                --Fin requete tb3
                ) as tbright
            on
            (
                (
                    tbleft.boule = tbright.b1 or
            tbleft.boule = tbright.b2 or
            tbleft.boule = tbright.b3 or
            tbleft.boule = tbright.b4 or
            tbleft.boule = tbright.b5
            )
                and
                (
                    tbleft.boule != 27
            )
                ) group by tbleft.boule;
#endif
    QString sql_msg ="";
    QString st_cr2 = "";
    QString st_cr3 = "";
    QStringList stl_tmp;

    stl_tmp << "tbright.b";
    st_cr2 =  GEN_Where_3(5,"tbleft.boule",false,"=",stl_tmp,true,"or");
#ifndef QT_NO_DEBUG
    qDebug() << st_cr2;
#endif

    if(dst == 0)
    {
        st_cr3 =  GEN_Where_3(1,"tbleft.boule",false,"!=",boules,false,"or");

        st_cr3 = " and " + st_cr3 ;
#ifndef QT_NO_DEBUG
        qDebug() << st_cr3;
#endif
    }

    sql_msg =
            "select tbleft.boule as B, count(tbright.Tid) as T, "
            "count(CASE WHEN  J like 'lundi%' then 1 end) as LUN,"
            "count(CASE WHEN  J like 'mercredi%' then 1 end) as MER,"
            "count(CASE WHEN  J like 'same%' then 1 end) as SAM "
            "from"
            "("
            "select id as boule from Bnrz where (z1 not null)"
            ") as tbleft "
            "left join"
            "("
            +sqlTblRef+
            ") as tbright "
            "on"
            "("
            "("
            + st_cr2 +
            ")"
            + st_cr3 +
            ") group by tbleft.boule;";

#ifndef QT_NO_DEBUG
    qDebug() << sql_msg;
#endif

    return sql_msg;
}

//QString RefResultat::SD_Tb2(QStringList boules, QString sqlTblRef,int dst)

QString SD_Tb2(void)
{
#if 0

    --etoiles
            --debut requete tb3
            select tb3.id as Tid,
            tb3.jour_tirage as J,
            substr(tb3.date_tirage,-2,2)||'/'||substr(tb3.date_tirage,6,2)||'/'||substr(tb3.date_tirage,1,4) as D,
            tb5.tip as C,
            tb3.b1 as b1, tb3.b2 as b2,tb3.b3 as b3,tb3.b4 as b4,tb3.b5 as b5,
            tb3.e1 as e1,
            tb3.bp as P,
            tb3.bg as G
            from tirages as tb3, analyses as tb4, lstcombi as tb5
            inner join
            (
                select *  from tirages as tb1
                ) as tb2
            on (
                (tb3.id = tb2.id + 0)
                and
                (tb4.id = tb3.id)
                and
                (tb4.id_poids = tb5.id)
                )
            --Fin requete tb3


            -- Requete comptage du resultat precedent
            select tbleft.boule as B, count(tbright.Tid) as T,
            count(CASE WHEN  J like 'lundi%' then 1 end) as LUN, count(CASE WHEN  J like 'mercredi%' then 1 end) as MER, count(CASE WHEN  J like 'same%' then 1 end) as SAM
            from
            (
                select id as boule from Bnrz where (z2 not null )
                ) as tbleft
            left join
            (
                --debut requete tb3
                select tb3.id as Tid,
                tb3.jour_tirage as J,
                substr(tb3.date_tirage,-2,2)||'/'||substr(tb3.date_tirage,6,2)||'/'||substr(tb3.date_tirage,1,4) as D,
                tb5.tip as C,
                tb3.b1 as b1, tb3.b2 as b2,tb3.b3 as b3,tb3.b4 as b4,tb3.b5 as b5,
                tb3.e1 as e1,
                tb3.bp as P,
                tb3.bg as G
                from tirages as tb3, analyses as tb4, lstcombi as tb5
                inner join
                (
                    select *  from tirages as tb1
                    ) as tb2
                on (
                    (tb3.id = tb2.id + 0)
                    and
                    (tb4.id = tb3.id)
                    and
                    (tb4.id_poids = tb5.id)
                    )
                --Fin requete tb3
                ) as tbright
            on
            (
                (
                    tbleft.boule = tbright.e1
            )
                ) group by tbleft.boule;

#endif

    QString st_msg = "";

    st_msg =
            "select tbleft.boule as B, count(tbright.Tid) as T,"
            "count(CASE WHEN  J like 'lundi%' then 1 end) as LUN,"
            "count(CASE WHEN  J like 'mercredi%' then 1 end) as MER,"
            "count(CASE WHEN  J like 'same%' then 1 end) as SAM "
            "from"
            "("
            "select id as boule from Bnrz where (z2 not null )"
            ") as tbleft "
            "left join"
            "("
            "select tb3.id as Tid,"
            "tb3.jour_tirage as J,"
            "substr(tb3.date_tirage,-2,2)||'/'||substr(tb3.date_tirage,6,2)||'/'||substr(tb3.date_tirage,1,4) as D,"
            "tb5.tip as C,"
            "tb3.b1 as b1, tb3.b2 as b2,tb3.b3 as b3,tb3.b4 as b4,tb3.b5 as b5,"
            "tb3.e1 as e1,"
            "tb3.bp as P,"
            "tb3.bg as G "
            "from tirages as tb3, analyses as tb4, lstcombi as tb5 "
            "inner join"
            "("
            "select *  from tirages as tb1 "
            ") as tb2 "
            "on ("
            "(tb3.id = tb2.id + 0) "
            "and"
            "(tb4.id = tb3.id)"
            "and"
            "(tb4.id_poids = tb5.id)"
            ")"
            ") as tbright "
            "on"
            "("
            "("
            "tbleft.boule = tbright.e1 "
            ")"
            ") group by tbleft.boule;"
            ;

    return (st_msg);
}

QString ComptageGenerique(int zn, int dst, QStringList boules, stTiragesDef *pConf)
{
#if 0
    --debut requete tb3
            select * from tirages as tb3
            inner join
            (
                select *  from tirages as tb1
                where
                ( (
                      tb1.b1=27 or
            tb1.b2=27 or
            tb1.b3=27 or
            tb1.b4=27 or
            tb1.b5=27
            )
                  )
                ) as tb2
            on tb3.id = tb2.id + -1
            --Fin requete tb3
            ;

    --Comptage
            select tb1.boule as B, count(tb2.id) as T,
            count(CASE WHEN  jour_tirage like 'lundi%' then 1 end) as LUN, count(CASE WHEN  jour_tirage like 'mercredi%' then 1 end) as MER, count(CASE WHEN  jour_tirage like 'same%' then 1 end) as SAM
            from
            (
                select id as boule from Bnrz where (z1 not null )
                ) as tb1
            left join
            (
                --debut requete tb3
                select * from tirages as tb3
                inner join
                (
                    select *  from tirages as tb1
                    where
                    ( (
                          tb1.b1=27 or
            tb1.b2=27 or
            tb1.b3=27 or
            tb1.b4=27 or
            tb1.b5=27
            )
                      )
                    ) as tb2
                on tb3.id = tb2.id + 0
            --Fin requete tb3
            ) as tb2
            on
            (
                (
                    tb1.boule = tb2.b1 or
            tb1.boule = tb2.b2 or
            tb1.boule = tb2.b3 or
            tb1.boule = tb2.b4 or
            tb1.boule = tb2.b5
            )
                and
                (
                    tb1.boule != 27
            )
                ) group by tb1.boule;

#endif

    QString st_cr1 = "";
    QString st_cr2 = "";
    QString st_cr3 = "";
    QString st_tmp = "";
    QStringList stl_tmp;

    //exemple dst = 1; loop=5; boules <<1 <<2;
    // st_cr1 => ((tb1.b1=1 or tb1.b2=1 or tb1.b3=1 or tb1.b4=1 or tb1.b5=1 )
    // and (tb1.b1=2 or tb1.b2=2 or tb1.b3=2 or tb1.b4=2 or tb1.b5=2 ))
    int loop = pConf->nbElmZone[zn];
    st_cr1 =  GEN_Where_3(loop,"tb1.b",true,"=",boules,false,"or");
#ifndef QT_NO_DEBUG
    qDebug() << st_cr1;
#endif

    // st_cr2 => ((tb1.boule=tb2.b1 or tb1.boule=tb2.b2 or
    // tb1.boule=tb2.b3 or tb1.boule=tb2.b4 or tb1.boule=tb2.b5 ))
    stl_tmp << "tb2.b";
    st_cr2 =  GEN_Where_3(5,"tb1.boule",false,"=",stl_tmp,true,"or");
#ifndef QT_NO_DEBUG
    qDebug() << st_cr2;
#endif

    if(dst == 0)
    {
        st_cr3 =  GEN_Where_3(1,"tb1.boule",false,"!=",boules,false,"or");

        st_cr3 = " and " + st_cr3 ;
#ifndef QT_NO_DEBUG
        qDebug() << st_cr3;
#endif
    }
    // Creer critere 1 en fonction des boules
    st_tmp =
            "select tb1.boule as B, count(tb2.id) as T,"
            "count(CASE WHEN  jour_tirage like 'lundi%' then 1 end) as LUN,"
            "count(CASE WHEN  jour_tirage like 'mercredi%' then 1 end) as MER,"
            "count(CASE WHEN  jour_tirage like 'same%' then 1 end) as SAM "
            "from"
            "("
            "select id as boule from Bnrz where (z"
            +QString::number(zn+1)
            +" not null )"
             ") as tb1 "
             "left join"
             "("
             " "
             "select * from tirages as tb3 "
             "inner join "
             "("
             "select *  from tirages as tb1 "
             "where"
             "("
            + st_cr1 +
            ")"
            ") as tb2 "
            "on tb3.id = tb2.id + "
            + QString::number(dst)
            +" "
             ") as tb2 "
             "on"
             "("
            + st_cr2
            + st_cr3 +
            ") group by tb1.boule;";

#ifndef QT_NO_DEBUG
    qDebug() << st_tmp;
#endif

    return st_tmp;
}

#if 0
QString GEN_Where_2(stTiragesDef *pConf, int zone, QString operateur, int boule, QString critere,QString alias="def")
{
    QString ret_msg = "";

    // Operateur : or | and
    // critere : = | <>
    // b1=0 or b2=0 or ..
    for(int i = 0; i<pConf->nbElmZone[zone];i++)
    {
        ret_msg = ret_msg
                + alias + "." + pConf->nomZone[zone]+QString::number(i+1)
                + critere + QString::number(boule)
                + " " + operateur+ " ";
    }
    int len_flag = operateur.length();
    ret_msg.remove(ret_msg.length()-len_flag-1, len_flag+1);

    return ret_msg;
}
#endif

QString GEN_Where_3(int loop,
                    QString tb1,
                    bool inc1,
                    QString op1,
                    QStringList &tb2,
                    bool inc2,
                    QString op2
                    )
{
    QString ret_msg = "";
    QString ind_1 = "";
    QString ind_2 = "";

    QString flag = " and ";

    for(int j=0; j< tb2.size();j++)
    {
        ret_msg = ret_msg + "(";
        for(int i = 0; i<loop;i++)
        {
            // Mettre un nombre apres  1er table
            if(inc1)
            {
                ind_1 = tb1+QString::number(i+1);
            }
            else
            {
                ind_1 = tb1;
            }

            // Mettre un nombre apres  2eme table
            if(inc2)
            {
                ind_2 = tb2.at(j)+QString::number(i+1);
            }
            else
            {
                ind_2 = tb2.at(j);
            }

            // Construire message
            ret_msg = ret_msg
                    + ind_1
                    + op1
                    + ind_2
                    + " " + op2 + " ";
        }
        // retirer le dernier operateur (op2)
        ret_msg.remove(ret_msg.length()-op2.length()-1, op2.length()+1);

        ret_msg =  ret_msg + ")";
        ret_msg = ret_msg + flag;
    }
    // retirer le dernier operateur
    ret_msg.remove(ret_msg.length()-flag.length(),flag.length());

    ret_msg = "(" +ret_msg+")";
    return ret_msg;
}

#if 0
QString GEN_Where_1(int zn, stTiragesDef *pConf, QStringList &boules, QString op1, QString op2, QString alias)
{
    QString msg= "" ;
    QString flag = " and ";

    for(int i=0; i< boules.size();i++)
    {
        int val_boule = boules.at(i).toInt();
        //QString msg1 = GEN_Where_2(pConf, zn,op1,val_boule,op2,alias);
        //        QString msg1 = GEN_Where_2(pConf, zn,"or",val_boule,"=");
        int loop = pConf->nbElmZone[zn];
        QString msg1 = GEN_Where_3(loop, "tb1.b",true,"=",boules,false,"or");

        msg = msg + "(" +msg1+ ")"
                + flag;
    }

    msg.remove(msg.length()-flag.length(),flag.length());

    return msg;
}


QString NEW_ExceptionBoule(int zn, stTiragesDef *pConf,QStringList &boules)
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
#endif
