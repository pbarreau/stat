#ifndef QT_NO_DEBUG
#include <QDebug>
#endif


#include <QtGui>
#include <QSqlRecord>

#include <QMessageBox>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QSortFilterProxyModel>

#include "refresultat.h"
#include "SyntheseDetails.h"

//
#include "mainwindow.h"

//extern MainWindow w;


QGridLayout *SyntheseGenerale::GetDisposition(void)
{
    return disposition;
}
QTableView *SyntheseGenerale::GetListeTirages(void)
{
    return  tbv_LesTirages;
}

SyntheseGenerale::SyntheseGenerale(GererBase *pLaBase, int zn, stTiragesDef *pConf, QMdiArea *visuel)
{
    disposition = new QGridLayout;
    bdd=pLaBase;
    pEcran = visuel;
    pMaConf = pConf;
    curzn = zn;

    st_bdTirages = new QString;
    *st_bdTirages = OrganiseChampsDesTirages("tirages", pConf);

    st_JourTirageDef = new QString;
    *st_JourTirageDef = CompteJourTirage(pConf);

    uneDemande.st_baseDef = st_bdTirages;
    uneDemande.st_bdAll = st_bdTirages;
    uneDemande.st_jourDef = st_JourTirageDef;
    DoTirages();
    DoComptageTotal();
    DoBloc2();
    //DoBloc3();
}

void SyntheseGenerale::DoTirages(void)
{
    sqm_LesTirages = new QSqlQueryModel;
    QTableView *qtv_tmp = new QTableView;


    QString st_sqlReq = *st_bdTirages;

    sqm_LesTirages->setQuery(st_sqlReq);

    qtv_tmp->setAlternatingRowColors(true);
    qtv_tmp->setStyleSheet("QTableView {selection-background-color: red;}");
    qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtv_tmp->setFixedSize(470,200);

    qtv_tmp->setModel(sqm_LesTirages);


    qtv_tmp->hideColumn(0);
    qtv_tmp->hideColumn(1);
    for(int j=0;j<5;j++)
        qtv_tmp->setColumnWidth(j,70);

    //qtv_tmp->setColumnWidth(4,60);

    for(int j=5;j<=sqm_LesTirages->columnCount();j++)
        qtv_tmp->setColumnWidth(j,35);


    // Ne pas modifier largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    QVBoxLayout *vb_tmp = new QVBoxLayout;
    QLabel * lab_tmp = new QLabel;
    lab_tmp->setText("Tirages");
    vb_tmp->addWidget(lab_tmp,0,Qt::AlignLeft|Qt::AlignTop);
    vb_tmp->addWidget(qtv_tmp,0,Qt::AlignLeft|Qt::AlignTop);
    disposition->addLayout(vb_tmp,0,0,2,2,Qt::AlignLeft|Qt::AlignTop);


    tbv_LesTirages=qtv_tmp;
    // Simple click dans sous fenetre base
    connect( qtv_tmp, SIGNAL( clicked(QModelIndex)) ,
             pEcran->parent(), SLOT( slot_MontreTirageDansGraph( QModelIndex) ) );

    // Simple click dans sous fenetre base
    connect( qtv_tmp, SIGNAL( clicked(QModelIndex)) ,
             this, SLOT( slot_AnalyseCeTirage( QModelIndex) ) );

#if 0
    // double click dans fenetre  pour afficher details boule
    connect( tbv_bloc1, SIGNAL(doubleClicked(QModelIndex)) ,
             this, SLOT(slot_MontreLesTirages( QModelIndex) ) );
#endif


}

void SyntheseGenerale::slot_AnalyseCeTirage(const QModelIndex & index)
{

    int id = index.row()+1;
    int nb_col = gsim_AnalyseUnTirage->columnCount();

    for(int i = 2; i<=nb_col; i++)
    {
        QStandardItem *item = gsim_AnalyseUnTirage->item(0,i-2);
        int val = 0;

        // Recherche de la config pour le  tirage
        val=RechercheInfoTirages(id,i-2);

        // affectation
        item->setData(val,Qt::DisplayRole);

        // visualisation
        gsim_AnalyseUnTirage->setItem(0,i-2,item);
    }


}

void SyntheseGenerale::DoComptageTotal(void)
{

    // Onglet pere
    QTabWidget *tab_Top = new QTabWidget;

    QString stNames[]={"Ecarts","Details"};
    int nbItems = sizeof(stNames)/sizeof(QString);

    QGridLayout **design_onglet=new QGridLayout* [nbItems];
    QWidget **wid_ForTop = new QWidget*[nbItems];

    QGridLayout * (SyntheseGenerale::*ptrFunc[])()=
    {&SyntheseGenerale:: Presente_SyntheseEcarts,
            &SyntheseGenerale:: MonLayout_SyntheseDetails};

    stTiragesDef *pConf = pMaConf;

    for(int i =0; i<nbItems;i++)
    {
        wid_ForTop[i]=new QWidget;
        tab_Top->addTab(wid_ForTop[i],tr(stNames[i].toUtf8()));

        // Recherche a une distance de 0 sans critere de filtre
        design_onglet[i] = (this->*ptrFunc[i])();
        wid_ForTop[i]->setLayout(design_onglet[i]);
    }

    QFormLayout *mainLayout = new QFormLayout;
    mainLayout->addWidget(tab_Top);
    disposition->addLayout(mainLayout,0,1,3,1,Qt::AlignLeft|Qt::AlignTop);


}

QGridLayout * SyntheseGenerale::MonLayout_SyntheseTotalEtoiles(int dst)
{
#if 0
#endif

    QGridLayout *lay_return = new QGridLayout;

    sqm_bloc1_2 = new QSqlQueryModel;
    QTableView *qtv_tmp ;
    tbv_bloc1_2 = new QTableView;
    qtv_tmp=tbv_bloc1_2;

    QString st_baseUse = "";
    st_baseUse = st_bdTirages->remove(";");
    QString st_cr1 = "";
    QStringList lst_tmp;
    lst_tmp << "tb2.e";
    int loop = pMaConf->nbElmZone[1];
    st_cr1 =  GEN_Where_3(loop,"tb1.boule",false,"=",lst_tmp,true,"or");
    QString st_msg1 =
            "select tb1.boule as B, count(tb2.id) as T, "
            + *st_JourTirageDef +
            " "
            "from  "
            "("
            "select id as boule from Bnrz where (z2 not null ) "
            ") as tb1 "
            "left join "
            "("
            "select tb2.* from "
            "("
            +st_baseUse+
            " )as tb1"
            ","
            "("
            +st_baseUse+
            ")as tb2 "
            "where"
            "("
            "tb2.id=tb1.id + "
            +QString::number(dst) +
            ")"
            ") as tb2 "
            "on "
            "("
            +st_cr1+
            ") group by tb1.boule;";

#ifndef QT_NO_DEBUG
    qDebug()<< st_msg1;
#endif

    sqm_bloc1_2->setQuery(st_msg1);

    qtv_tmp->setSortingEnabled(true);
    qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
    qtv_tmp->setAlternatingRowColors(true);


    qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtv_tmp->setFixedSize(225,400);

    QSortFilterProxyModel *m=new QSortFilterProxyModel();
    m->setDynamicSortFilter(true);
    m->setSourceModel(sqm_bloc1_2);
    qtv_tmp->setModel(m);
    qtv_tmp->verticalHeader()->hide();
    for(int j=0;j<2;j++)
        qtv_tmp->setColumnWidth(j,30);
    for(int j=2;j<=sqm_bloc1_2->columnCount();j++)
        qtv_tmp->setColumnWidth(j,40);


    // Ne pas modifier largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    QVBoxLayout *vb_tmp = new QVBoxLayout;
    QLabel * lab_tmp = new QLabel;
    lab_tmp->setText("Repartitions");
    vb_tmp->addWidget(lab_tmp,0,Qt::AlignLeft|Qt::AlignTop);
    vb_tmp->addWidget(qtv_tmp,0,Qt::AlignLeft|Qt::AlignTop);

    lay_return->addLayout(vb_tmp,0,0);

    // simple click dans fenetre  pour selectionner boule
    connect( tbv_bloc1_2, SIGNAL(clicked(QModelIndex)) ,
             this, SLOT(slot_SelectionneBoules( QModelIndex) ) );


    // double click dans fenetre  pour afficher details boule
    connect( tbv_bloc1_2, SIGNAL(doubleClicked(QModelIndex)) ,
             this, SLOT(slot_MontreLesTirages( QModelIndex) ) );

    return lay_return;

}

QGridLayout * SyntheseGenerale::MonLayout_SyntheseTotalRepartitions(int dst)
{
#if 0
#endif

    QGridLayout *lay_return = new QGridLayout;

    QTableView *qtv_tmp ;
    QSqlQueryModel *sqm_tmp = new QSqlQueryModel;

    tbv_bloc1_3 = new QTableView;
    sqm_bloc1_3 = new QSqlQueryModel;
    qtv_tmp=tbv_bloc1_3;
    sqm_tmp=sqm_bloc1_3;

    QString st_baseUse = "";
    st_baseUse = st_bdTirages->remove(";");
    QString st_cr1 = "";
    QStringList lst_tmp;
    lst_tmp << "tb2.e";
    int loop = pMaConf->nbElmZone[1];
    st_cr1 =  "tb1.id=tb2.pid";
    QString st_msg1 =
            "select tb1.id as Id, tb1.tip as Repartition, count(tb2.id) as T, "
            + *st_JourTirageDef +
            " "
            "from  "
            "("
            "select id,tip from lstcombi"
            ") as tb1 "
            "left join "
            "("
            "select tb2.* from "
            "("
            +st_baseUse+
            " )as tb1"
            ","
            "("
            +st_baseUse+
            ")as tb2 "
            "where"
            "("
            "tb2.id=tb1.id+"
            +QString::number(dst) +
            ")"
            ") as tb2 "
            "on "
            "("
            +st_cr1+
            ") group by tb1.id;";

#ifndef QT_NO_DEBUG
    qDebug()<< st_msg1;
#endif

    sqm_tmp->setQuery(st_msg1);

    qtv_tmp->setSortingEnabled(true);
    qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
    qtv_tmp->setAlternatingRowColors(true);


    qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtv_tmp->setFixedSize(290,400);

    QSortFilterProxyModel *m=new QSortFilterProxyModel();
    m->setDynamicSortFilter(true);
    m->setSourceModel(sqm_tmp);
    qtv_tmp->setModel(m);
    qtv_tmp->verticalHeader()->hide();
    qtv_tmp->setColumnWidth(0,30);
    qtv_tmp->setColumnWidth(1,70);
    for(int j=2;j<=sqm_tmp->columnCount();j++)
        qtv_tmp->setColumnWidth(j,40);


    // Ne pas modifier largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);


    // Filtre
    QFormLayout *FiltreLayout = new QFormLayout;
    FiltreCombinaisons *fltComb_tmp = new FiltreCombinaisons();
    fltComb_tmp->setFiltreConfig(sqm_tmp,qtv_tmp,1);
    FiltreLayout->addRow("&Filtre Repartition", fltComb_tmp);

    lay_return->addLayout(FiltreLayout,0,0,Qt::AlignLeft|Qt::AlignTop);
    lay_return->addWidget(qtv_tmp,1,0,Qt::AlignLeft|Qt::AlignTop);

    // simple click dans fenetre  pour selectionner boule
    connect( tbv_bloc1_3, SIGNAL(clicked(QModelIndex)) ,
             this, SLOT(slot_SelectionneBoules( QModelIndex) ) );


    // double click dans fenetre  pour afficher details boule
    connect( tbv_bloc1_3, SIGNAL(doubleClicked(QModelIndex)) ,
             this, SLOT(slot_MontreLesTirages( QModelIndex) ) );

    return lay_return;
}

#if 0
QGridLayout * SyntheseGenerale::MonLayout_SyntheseTotalBoules(int dst)
{
    QGridLayout *lay_return = new QGridLayout;

    sqm_bloc1_1 = new QSqlQueryModel;
    QTableView *qtv_tmp ;
    tbv_bloc1_1 = new QTableView;
    qtv_tmp=tbv_bloc1_1;

    QString st_baseUse = "";
    st_baseUse = st_bdTirages->remove(";");
    QString st_cr1 = "";
    QStringList lst_tmp;
    lst_tmp << "tb2.b";
    st_cr1 =  GEN_Where_3(5,"tb1.boule",false,"=",lst_tmp,true,"or");
    QString st_msg1 =
            "select tb1.boule as B, count(tb2.id) as T, "
            + *st_JourTirageDef +
            " "
            "from  "
            "("
            "select id as boule from Bnrz where (z1 not null ) "
            ") as tb1 "
            "left join "
            "("
            "select tb2.* from "
            "("
            +st_baseUse+
            " )as tb1"
            ","
            "("
            +st_baseUse+
            ")as tb2 "
            "where"
            "("
            "tb2.id=tb1.id + "
            +QString::number(dst) +
            ")"
            ") as tb2 "
            "on "
            "("
            +st_cr1+
            ") group by tb1.boule;";

#ifndef QT_NO_DEBUG
    qDebug()<< st_msg1;
#endif

    sqm_bloc1_1->setQuery(st_msg1);

    qtv_tmp->setSortingEnabled(true);
    qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
    qtv_tmp->setAlternatingRowColors(true);


    qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtv_tmp->setFixedSize(225,400);

    QSortFilterProxyModel *m=new QSortFilterProxyModel();
    m->setDynamicSortFilter(true);
    m->setSourceModel(sqm_bloc1_1);
    qtv_tmp->setModel(m);
    qtv_tmp->verticalHeader()->hide();
    for(int j=0;j<2;j++)
        qtv_tmp->setColumnWidth(j,30);
    for(int j=2;j<=sqm_bloc1_1->columnCount();j++)
        qtv_tmp->setColumnWidth(j,40);


    // Ne pas modifier largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    QVBoxLayout *vb_tmp = new QVBoxLayout;
    QLabel * lab_tmp = new QLabel;
    lab_tmp->setText("Repartitions");
    vb_tmp->addWidget(lab_tmp,0,Qt::AlignLeft|Qt::AlignTop);
    vb_tmp->addWidget(qtv_tmp,0,Qt::AlignLeft|Qt::AlignTop);

    lay_return->addLayout(vb_tmp,0,0);

    // simple click dans fenetre  pour selectionner boule
    connect( tbv_bloc1_1, SIGNAL(clicked(QModelIndex)) ,
             this, SLOT(slot_SelectionneBoules( QModelIndex) ) );


    // double click dans fenetre  pour afficher details boule
    connect( tbv_bloc1_1, SIGNAL(doubleClicked(QModelIndex)) ,
             this, SLOT(slot_MontreLesTirages( QModelIndex) ) );

    return lay_return;

}

#endif

void SyntheseGenerale::DoBloc2(void)
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
    QTableView *qtv_tmp ;
    tbv_bloc2 = new QTableView;
    qtv_tmp=tbv_bloc2;

    // Gestion du QTableView
    qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    //qtv_tmp->setStyleSheet("QTableView {selection-background-color: red;}");
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtv_tmp->setAlternatingRowColors(true);
    qtv_tmp->setFixedSize(470,222);

    qtv_tmp->setModel(tblModel);
    qtv_tmp->setSortingEnabled(true);
    qtv_tmp->hideColumn(0);
    qtv_tmp->sortByColumn(1,Qt::AscendingOrder);
    qtv_tmp->verticalHeader()->hide();

    for(int i=0;i<tblModel->columnCount();i++)
        qtv_tmp->setColumnWidth(i,30);

    QHeaderView *htop = qtv_tmp->horizontalHeader();
    htop->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);


    QVBoxLayout *vb_tmp = new QVBoxLayout;
    QLabel * lab_tmp = new QLabel;
    lab_tmp->setText("Groupement");
    vb_tmp->addWidget(lab_tmp,0,Qt::AlignLeft|Qt::AlignTop);
    vb_tmp->addWidget(qtv_tmp,0,Qt::AlignLeft|Qt::AlignTop);
    disposition->addLayout(vb_tmp,2,0,Qt::AlignLeft|Qt::AlignTop);


    // double click dans fenetre  pour afficher details boule
    connect( tbv_bloc2, SIGNAL(doubleClicked(QModelIndex)) ,
             this, SLOT(slot_MontreLesTirages( QModelIndex) ) );

}

void SyntheseGenerale::DoBloc3(void)
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
    qtv_tmp->setFixedSize(285,222);

    qtv_tmp->setModel(tblModel);
    qtv_tmp->setSortingEnabled(true);
    //qtv_tmp->hideColumn(0);
    qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
    qtv_tmp->verticalHeader()->hide();

    for(int i=0;i<tblModel->columnCount();i++)
        qtv_tmp->setColumnWidth(i,30);

    QHeaderView *htop = qtv_tmp->horizontalHeader();
    htop->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    //htop=qtv_tmp->verticalHeader();
    //htop->setUserData();

    QVBoxLayout *vb_tmp = new QVBoxLayout;
    QLabel * lab_tmp = new QLabel;
    lab_tmp->setText("Autre");
    vb_tmp->addWidget(lab_tmp,0,Qt::AlignLeft|Qt::AlignTop);
    vb_tmp->addWidget(qtv_tmp,0,Qt::AlignLeft|Qt::AlignTop);
    disposition->addLayout(vb_tmp,2,1,Qt::AlignLeft|Qt::AlignTop);

    //disposition->addWidget(qtv_tmp,1,1,Qt::AlignLeft|Qt::AlignTop);
    tbv_bloc3=qtv_tmp;
}

void SyntheseGenerale::slot_ChangementEnCours(const QItemSelection &selected,
                                              const QItemSelection &deselected)
{
    QItemSelectionModel *selection;
    QModelIndexList les_indexes =  selected.indexes();
    QModelIndex un_index;

    int col = -1;

    selection = tbv_bloc1_1->selectionModel();
    foreach(un_index, les_indexes) {
        col = un_index.column();
        if(col != 0)
        {
            selection->select(un_index,  QItemSelectionModel::Toggle);
        }

    }

}

void SyntheseGenerale::slot_SelectionneBoules(const QModelIndex & index)
{
    void *pSource = index.internalPointer();
    int col = index.column();
    int val = index.data().toInt();

    // Boules
    if(pSource == tbv_bloc1_1->model()->index(0,0).internalPointer())
    {
        MemoriserChoixUtilisateur(0, tbv_bloc1_1, index);
    }

    // Etoiles
    if(pSource == tbv_bloc1_2->model()->index(0,0).internalPointer())
    {
        MemoriserChoixUtilisateur(1, tbv_bloc1_2, index);
    }

    // Combinaison
    if(pSource == tbv_bloc1_3->model()->index(0,0).internalPointer())
    {
        if(col>=2 && val)
            MemoriserChoixUtilisateur(2, tbv_bloc1_3, index);
    }

}

void SyntheseGenerale::MemoriserChoixUtilisateur(int zn, QTableView *ptbv, const QModelIndex & index)
{
    QItemSelectionModel *selection;
    QItemSelection une_cellule_choisie(index,index);
    QModelIndexList indexes ;
    QModelIndex un_index;

    int nbZone = pMaConf->nb_zone;
    int nb_element_max_zone = 0;
    QString stNomZone = 0;

    static int col_depart [3]= {-1,-1,-1};
    int cur_col = index.column();
    int nb_items = 0;

    int ligne = index.row();
    int val = index.data().toInt();
    const QAbstractItemModel * pModel = index.model();
    QVariant vCol = pModel->headerData(cur_col,Qt::Horizontal);
    QString headName = vCol.toString();


    if (zn>=nbZone)
    {
        nb_element_max_zone = 1;
        col_depart[zn] = -1;
        stNomZone = "Combinaison";
        uneDemande.lst_boules[zn].clear();
    }
    else
    {
        nb_element_max_zone = pMaConf->nbElmZone[zn];
        stNomZone = pMaConf->nomZone[zn];
    }

    selection = ptbv->selectionModel();
    indexes = selection->selectedIndexes();

    // L'utilisateur a t il tout deselectionnee
    if(indexes.size() == 0)
    {
        col_depart[zn] = -1;
        uneDemande.lst_boules[zn].clear();
        return;
    }

    // Cas selection de combinaison
    if (zn == 2){
        col_depart[zn] == -1;
        uneDemande.lst_boules[zn].clear();
    }

    if (col_depart[zn] == -1)
    {
        col_depart[zn] =  cur_col;
        uneDemande.col[zn] = cur_col;
        uneDemande.stc[zn]=headName;
        uneDemande.val[zn]=val;
        uneDemande.lgn[zn]=ligne;

    }

    // La colonne numero de boule
    if(cur_col == col_depart[zn])
    {

        nb_items = indexes.size();
        if(nb_items <= nb_element_max_zone)
        {
            QStringList lst_tmp;
            QString boule;
            foreach(un_index, indexes)
            {
#ifndef QT_NO_DEBUG
                qDebug() << stNomZone
                         <<" -> Nb items:"<<nb_items
                        <<"Col:" << un_index.column()
                       <<", Ligne:" << un_index.row();
#endif
                boule = un_index.model()->index(un_index.row(),0).data().toString();
                lst_tmp = lst_tmp << boule;
            }
            uneDemande.lst_boules[zn]=lst_tmp;
        }
        else
        {
            //un message d'information
            QMessageBox::warning(0, stNomZone, tr("Attention, maximum deja selectionne !"),QMessageBox::Yes);
        }

    }
    else
    {
        // deselectionner l'element
        selection->select(une_cellule_choisie, QItemSelectionModel::Deselect);
    }
}

void SyntheseGenerale::slot_MontreLesTirages(const QModelIndex & index)
{
    void *pSource = index.internalPointer();
    int col = index.column();
    int val = index.data().toInt();
    const QAbstractItemModel * pModel = index.model();
    QVariant vCol = pModel->headerData(col,Qt::Horizontal);
    QString headName = vCol.toString();

#if 0
    if(pSource == tbv_bloc1_3->model()->index(0,0).internalPointer())
    {

        if (col < 2 || !val)
            return;
    }
#endif
    // Le simple click a construit la liste des boules
    stCurDemande *etude = new stCurDemande;

    // Boules
    //||
    //(pSource == tbv_bloc1_3->model()->index(0,0).internalPointer())

    if(
            (pSource == tbv_bloc1_1->model()->index(0,0).internalPointer())
            ||
            (pSource == tbv_bloc1_2->model()->index(0,0).internalPointer())
            )
    {
        uneDemande.origine = 1;
    }

    if(
            pSource == tbv_bloc2->model()->index(0,0).internalPointer()
            )
    {
        uneDemande.origine = 2;
        uneDemande.lgn[0]=index.model()->index(index.row(),1).data().toInt();
        uneDemande.col[0]=col;
        uneDemande.stc[0]=headName;
        uneDemande.val[0]=val;

    }


    // recopie de la config courante
    uneDemande.cur_dst = 0;
    *etude = uneDemande;

    // Nouvelle de fenetre de detail de cette boule
    SyntheseDetails *unDetail = new SyntheseDetails(etude,pEcran);
}

QGridLayout * SyntheseGenerale:: Presente_SyntheseEcarts(void)
{

    QGridLayout *returnLayout = new QGridLayout;

    QStandardItemModel *G_sim_Ecarts;
    int zn = 0;

    G_sim_Ecarts = new QStandardItemModel(pMaConf->limites[zn].max,5);
    returnLayout = MonLayout_SyntheseEcarts(G_sim_Ecarts);

    // Ordre arrivee des boules ?
    bdd->CouvertureBase(G_sim_Ecarts,pMaConf);

    for(int i=1;i<=pMaConf->limites[zn].max;i++){

        // Remplir Sous Fen les ecarts
        bdd->DistributionSortieDeBoule(i,G_sim_Ecarts,pMaConf);

        // Montrer les valeurs probable
        bdd->CouvMontrerProbable(i,G_sim_Ecarts);
    }


    return(returnLayout);
}

QGridLayout * SyntheseGenerale:: MonLayout_SyntheseEcarts(QStandardItemModel *qsim_tmp)
{
    QGridLayout *returnLayout = new QGridLayout;

    int  i;
    int zn = 0;

    QTableView *qtv_tmp;


    qtv_tmp = new QTableView;

    qsim_tmp->setHeaderData(0,Qt::Horizontal,"B"); // Boules
    qsim_tmp->setHeaderData(1,Qt::Horizontal,"Ec"); // Ecart en cours
    qsim_tmp->setHeaderData(2,Qt::Horizontal,"Ep"); // ECart precedent
    qsim_tmp->setHeaderData(3,Qt::Horizontal,"Em"); // Ecart Moyen
    qsim_tmp->setHeaderData(4,Qt::Horizontal,"EM"); // Ecart Maxi

    for(i=1;i<=pMaConf->limites[zn].max;i++)
    {
        QStandardItem *item = new QStandardItem( QString::number(i));
        item->setData(i,Qt::DisplayRole);
        qsim_tmp->setItem(i-1,0,item);
    }
    qtv_tmp->setModel(qsim_tmp);

    qtv_tmp->setColumnWidth(0,45);
    qtv_tmp->setColumnWidth(1,45);
    qtv_tmp->setColumnWidth(2,45);
    qtv_tmp->setColumnWidth(3,45);
    qtv_tmp->setColumnWidth(4,45);
    qtv_tmp->setSortingEnabled(true);
    qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
    qtv_tmp->setAlternatingRowColors(true);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtv_tmp->setFixedSize(280,450);
    qtv_tmp->verticalHeader()->hide();


    returnLayout->addWidget(qtv_tmp);

    return(returnLayout);
}

QGridLayout * SyntheseGenerale::MonLayout_SyntheseDetails()
{
    QGridLayout *lay_return = new QGridLayout;
    QTableView *tmpTblView_1 = new QTableView;
    QTableView *tmpTblView_2 = new QTableView;

    // Compter le nombre de colonne a creer
    QString sqlReq = "";
    QSqlQuery sql_req;
    bool status = false;

    sqlReq = "select * from repartition_bh limit 1;";
    status = sql_req.exec(sqlReq);

    if(status)
    {
        QSqlRecord ligne = sql_req.record();
        int nb_col = ligne.count();
        QStandardItemModel * tmpStdItem =  new QStandardItemModel(1,nb_col-2);
        tmpTblView_1->setModel(tmpStdItem);
        gsim_AnalyseUnTirage = tmpStdItem;
        int val = 0;
        for(int i = 2; i<nb_col; i++)
        {
            tmpStdItem->setHeaderData(i-2,Qt::Horizontal,ligne.fieldName(i));
            QStandardItem *item = new QStandardItem();

            // Recherche de la config pour le dernier tirage
            val=RechercheInfoTirages(1,i-2);
            item->setData(val,Qt::DisplayRole);
            tmpStdItem->setItem(0,i-2,item);
            tmpTblView_1->setColumnWidth(i-2,30);
        }

        tmpTblView_1->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tmpTblView_1->setSelectionBehavior(QAbstractItemView::SelectItems);
        tmpTblView_1->setSelectionMode(QAbstractItemView::SingleSelection);
        tmpTblView_1->verticalHeader()->hide();
        tmpTblView_1->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        tmpTblView_1->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);


    }
    lay_return->addWidget(tmpTblView_1,0,0);

    // Construction table 2
    QStandardItemModel * tmpStdItem_2 =  new QStandardItemModel(6,2);
    tmpTblView_2->setModel(tmpStdItem_2);
    // entete du modele
    QStringList lstheader;
    lstheader << "Q" << "V:+1";
    tmpStdItem_2->setHorizontalHeaderLabels(lstheader);
    for(int y=0;y<6;y++)
    {
        for(int x=0;x<2;x++)
        {
            QStandardItem *item = new QStandardItem();
            if(x==0)
            {
                item->setData(y,Qt::DisplayRole);
            }
            tmpStdItem_2->setItem(y,x,item);
            tmpTblView_2->setColumnWidth(x,50);
        }
    }
    tmpTblView_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tmpTblView_2->setSelectionBehavior(QAbstractItemView::SelectItems);
    tmpTblView_2->setSelectionMode(QAbstractItemView::SingleSelection);
    tmpTblView_2->verticalHeader()->hide();
    tmpTblView_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    tmpTblView_2->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    lay_return->addWidget(tmpTblView_2,1,0);

    return(lay_return);
}


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

QString CompteJourTirage(stTiragesDef *pMaConf)
{
    QString st_msg = "";

    int nb_tir = pMaConf->nb_tir_semaine;

    for(int i=0;i<nb_tir;i++)
    {
        st_msg = st_msg +
                "count(CASE WHEN  J like '"
                +pMaConf->jour_tir[i]+
                "%' then 1 end) as "
                +pMaConf->jour_tir[i].left(3)+
                ",";
    }

#ifndef QT_NO_DEBUG
    qDebug()<< st_msg;
#endif

    st_msg.remove(st_msg.length()-1,1);

    return st_msg;
}


QString OrganiseChampsDesTirages(QString st_base_reference, stTiragesDef *pMaConf)
{
#if 0
    select tb3.id as id, tb5.id as pid, tb3.jour_tirage as J,
            substr(tb3.date_tirage,-2,2)||'/'||substr(tb3.date_tirage,6,2)||'/'||substr(tb3.date_tirage,1,4) as D,
            tb5.tip as C,
            tb3.b1 as b1,
            tb3.b2 as b2,
            tb3.b3 as b3,
            tb3.b4 as b4,
            tb3.b5 as b5,
            tb3.e1 as e1
            from tirages as tb3,
            analyses as tb4,
            lstcombi as tb5
            where
            (
                tb4.id = tb3.id
            and
            tb5.id = tb4.id_poids
            );
#endif

    QString st_base = "";
    QString st_tmp = "";
    QString st_cr1 = "";
    QStringList lst_tmp;
    int loop = 0;

    for(int i =0 ; i< pMaConf->nb_zone; i++)
    {
        lst_tmp <<   pMaConf->nomZone[i];
        st_tmp = "tb3."+pMaConf->nomZone[i];
        loop =  pMaConf->nbElmZone[i];
        st_tmp =  GEN_Where_3(loop,st_tmp,true," as ",lst_tmp,true,",");
        st_cr1 = st_cr1 + st_tmp + ",";
        lst_tmp.clear();
    }
#ifndef QT_NO_DEBUG
    qDebug()<< st_cr1;
#endif

    st_cr1.remove(QRegExp("[()]"));
    st_cr1.remove(st_cr1.length()-1,1);

    st_base =
            "select tb3.id as id, tb5.id as pid, tb3.jour_tirage as J, "
            "substr(tb3.date_tirage,-2,2)||'/'||substr(tb3.date_tirage,6,2)||'/'||substr(tb3.date_tirage,1,4) as D, "
            "tb5.tip as C, "
            + st_cr1 +
            " from ("
            +st_base_reference+
            ") as tb3,  "
            "analyses as tb4,  "
            "lstcombi as tb5 "
            "where "
            "( "
            "tb4.id = tb3.id "
            "and "
            "tb5.id = tb4.id_poids "
            "); ";

#ifndef QT_NO_DEBUG
    qDebug()<< st_base;
#endif

    return st_base;
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
