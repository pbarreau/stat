#ifndef QT_NO_DEBUG
#include <QDebug>
#endif


#include <QtGui>

#include <QHeaderView>

#include <QSortFilterProxyModel>
#include <QLineEdit>
#include <QComboBox>
#include <QTreeView>
#include <QStandardItemModel>
#include <QLabel>
#include <QSqlQuery>

#include "SyntheseDetails.h"
#include "filtrecombinaisons.h"
#include "monQview.h"

//------------- Local Prototypes ------------
QComboBox *ComboPerso(int id);


//--------------Fin local prototypes ----------------------
// ------------Local Fn definitions -----------------
QComboBox *ComboPerso(int id)
{
    QComboBox * tmp_combo = new QComboBox;

    QTreeView *sourceView = new QTreeView;
    sourceView->setRootIsDecorated(false);
    sourceView->setAlternatingRowColors(true);
    tmp_combo->setView(sourceView);

    //sourceView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded); //fine tuning of some options
    sourceView->setSelectionMode(QAbstractItemView::SingleSelection);
    sourceView->setSelectionBehavior(QAbstractItemView::SelectRows);
    sourceView->setAutoScroll(false);

    QStandardItemModel *model = new QStandardItemModel(0, 2);
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Filtre"));

    QStringList ChoixCol;
    ChoixCol<<"J"<<"D"<<"C"<<"E"<<"P"<<"G";
    for(int i = ChoixCol.size()-1; i>=0;i--)
    {
        model->insertRow(0);
        model->setData(model->index(0, 0), ChoixCol.at(i));
        model->setData(model->index(0, 1), id);
    }

    sourceView->header()->hide();
    sourceView->resizeColumnToContents(0);
    tmp_combo->setModel(model);

    return tmp_combo;
}

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

//---------------- Fin Local Fns ------------------------
SyntheseDetails::SyntheseDetails(stCurDemande *uneEtude, QMdiArea *visuel)
{
    pLaDemande = uneEtude;
    pEcran = visuel;
    MontreRechercheTirages(uneEtude);
}

void SyntheseDetails::MontreRechercheTirages(stCurDemande *pEtude)
{
    int bouleId = pEtude->boule;
    QString colName = pEtude->st_col;
    int curVal=pEtude->val;

    QWidget *qw_main = new QWidget;
    QTabWidget *tab_Top = new QTabWidget;
    QTabWidget **wid_ForTop_1 = new QTabWidget*[4];
    QString stNames_1[4]={"0","1","-1","?"};
    const int Ref_D_Onglet[4]={0,-1,1,-2};

    onglets = tab_Top;

    for(int i =0; i<4;i++)
    {
        wid_ForTop_1[i]=new QTabWidget;
        tab_Top->addTab(wid_ForTop_1[i],tr(stNames_1[i].toUtf8()));

        QWidget **wid_ForTop_2 = new QWidget*[2];
        QString stNames_2[2]={"Tirages","Repartition"};
        QGridLayout *design_onglet_2[2];

        // Tableau de pointeur de fonction
        QGridLayout *(SyntheseDetails::*ptrFunc[2])(int ,stCurDemande *, int )=
        {&SyntheseDetails::MonLayout_pFnDetailsTirages,&SyntheseDetails::MonLayout_pFnSyntheseDetails};

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
            design_onglet_2[j] = (this->*ptrFunc[j])(i, pEtude, Ref_D_Onglet[i]);
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

    QString st_titre =
            "Analyse B:"+QString::number(bouleId)+
            ",Col:"+colName +
            ",(V="+QString::number(curVal)+").";

    mainLayout->addWidget(tab_Top);
    qw_main->setWindowTitle(st_titre);
    qw_main->setLayout(mainLayout);


    QMdiSubWindow *subWindow = pEcran->addSubWindow(qw_main);
    //subWindow->resize(493,329);
    //subWindow->move(737,560);
    qw_main->setVisible(true);
    qw_main->show();
}
// --------------------------
QGridLayout * SyntheseDetails::MonLayout_pFnDetailsTirages(int curId, stCurDemande *pEtude, int val)
{
    QGridLayout *lay_return = new QGridLayout;

    QStringList stl_tmp = pEtude->lst_boules;

    const int Ref_D_Onglet[4]={0,-1,1,-2};
    const bool Ref_A_Onglet[4]={false,false,false,true};

    int distance = Ref_D_Onglet[curId];
    bool ongSpecial = Ref_A_Onglet[curId];

    QString sql_msgRef = "";
    QSqlQueryModel *sqm_tmp = new QSqlQueryModel;
    QTableView *qtv_tmp = new QTableView;


    if(curId==3)
    {
        distance = val;
    }

    // Fonction Pour La requete de base (obtenir les tirages)
    QString (SyntheseDetails::*ptrFunc[2])(QStringList &, int)=
    {&SyntheseDetails::DoSqlMsgRef_Tb1,&SyntheseDetails::DoSqlMsgRef_Tb2};

    // Creer le code de la requete Sql
    int origine = pLaDemande->origine;
    if(origine > 1 && origine <=3)
    {
        origine --;
    }
    else
    {
        origine = 0;
    }
    sql_msgRef = (this->*ptrFunc[origine])(stl_tmp,distance);

    sqm_tmp->setQuery(sql_msgRef);

    qtv_tmp->setSortingEnabled(false);
    //qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
    qtv_tmp->setAlternatingRowColors(true);


    qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);


    // Filtre
    QHBoxLayout *tmp_hLay = new QHBoxLayout();

    QComboBox *tmp_combo = ComboPerso(curId);

    QLabel *tmp_lab = new QLabel(tr("Filtre :"));
    tmp_lab->setBuddy(tmp_combo);

    connect(tmp_combo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slot_FiltreSurNewCol(int)));


    QFormLayout *FiltreLayout = new QFormLayout;
    FiltreCombinaisons *fltComb_tmp = new FiltreCombinaisons;
    fltComb_tmp->setFiltreConfig(sqm_tmp,qtv_tmp,2);


    FiltreLayout->addRow("&Recherche", fltComb_tmp);

    // Mettre combo + line dans hvbox
    tmp_hLay->addWidget(tmp_lab);
    tmp_hLay->addWidget(tmp_combo);
    tmp_hLay->addLayout(FiltreLayout);

    qtv_tmp->hideColumn(0);
    qtv_tmp->hideColumn(1);

    // Formattage de largeur de colonnes
    for(int j=0;j<=4;j++)
        qtv_tmp->setColumnWidth(j,75);

    for(int j=5;j<=sqm_tmp->columnCount();j++)
        qtv_tmp->setColumnWidth(j,30);
    qtv_tmp->setFixedSize(525,205);


    // Ne pas modifier largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    // Double click dans fenetre pour details et localisation dans base Tirages
    connect( qtv_tmp, SIGNAL( doubleClicked(QModelIndex)) ,
             pEcran->parent(), SLOT(slot_MontreLeTirage( QModelIndex) ) );

    //int pos_x = 0;
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
    lay_return->addLayout(tmp_hLay,pos_y,0,Qt::AlignLeft|Qt::AlignTop);
    lay_return->addWidget(qtv_tmp,pos_y+1,0,Qt::AlignLeft|Qt::AlignTop);

    // Associer la combo de selection au filtre pour
    // cette distance (ie cet onglet) et la Qtview associee
    pCritere[curId] = tmp_combo;
    pFiltre[curId] = fltComb_tmp;

    return(lay_return);
}



void SyntheseDetails::slot_FiltreSurNewCol(int colNum)
{
    int colId[6]={2,3,4,10,11,12};
    int ColReal = colId[colNum];

    int val = onglets->currentIndex();
    pFiltre[val]->slot_setFKC(ColReal);
}

void SyntheseDetails::slot_NouvelleDistance(void)
{
    QString msg = "";
    int new_distance = dist->text().toInt();

    // pour Coherence par rapport a graphique et requete
    new_distance *=-1;

    // Tableau de pointeur de fonction
    QGridLayout *(SyntheseDetails::*ptrFunc[2])(int ,stCurDemande *,  int )=
    {&SyntheseDetails::MonLayout_pFnDetailsTirages,&SyntheseDetails::MonLayout_pFnSyntheseDetails};

    for(int j =0; j<2;j++)
    {
        // Effacer l'ancien layout et mettre le nouveau
        QGridLayout * oldOne = G_design_onglet_2[j];
        QGridLayout * monTest;

        monTest = (this->*ptrFunc[j])(3,pLaDemande,new_distance);

        // nouveau dessin ok.
        // Rechercher l'ancien pour suppression et reaffectation;
        QWidget *onp = oldOne->parentWidget();
        delete(oldOne);

        onp->setLayout(monTest);
        G_design_onglet_2[j]=monTest;
    }
}

QGridLayout * SyntheseDetails::MonLayout_pFnSyntheseDetails(int curId, stCurDemande *pEtude, int /*val*/)
{
    QGridLayout *lay_return = new QGridLayout;

    QStringList stl_tmp = pEtude->lst_boules;

    const int Ref_D_Onglet[4]={0,-1,1,-2};
    const bool Ref_A_Onglet[4]={false,false,false,true};

    int distance = Ref_D_Onglet[curId];
    bool ongSpecial = Ref_A_Onglet[curId];

    Synthese_1(lay_return,stl_tmp,distance,ongSpecial);
    Synthese_2(lay_return,stl_tmp,distance,ongSpecial);


    return(lay_return);
}

void SyntheseDetails::Synthese_2(QGridLayout *lay_return, QStringList &stl_tmp, int distance, bool ongSpecial)
{
    //-------------------
    QLabel *titre_2 = new QLabel("Etoiles");
    QTableView *qtv_tmp_2 = new QTableView;
    qtv_tmp_2->setFixedSize(340,230);

    MonQtViewDelegate *la = new MonQtViewDelegate(dist,stl_tmp, onglets);

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

void SyntheseDetails::Synthese_1(QGridLayout *lay_return, QStringList &stl_tmp, int distance, bool ongSpecial)
{
    QString sql_msgRef = "";
    QSqlQueryModel *sqm_tmp = new QSqlQueryModel;
    QTableView *qtv_tmp = new QTableView;

    // Fonction Pour La requete de base (obtenir les tirages)
    QString (SyntheseDetails::*ptrFunc[2])(QStringList &, int)=
    {&SyntheseDetails::DoSqlMsgRef_Tb1,&SyntheseDetails::DoSqlMsgRef_Tb2};

    // Creer le code de la requete Sql
    int origine = pLaDemande->origine;
    if(origine > 1 && origine <=3)
    {
        origine --;
    }
    else
    {
        origine = 0;
    }
    sql_msgRef = (this->*ptrFunc[origine])(stl_tmp,distance);
    // Retirer le ; de la fin
    sql_msgRef.replace(";","");

    sql_msgRef = SD_Tb1(stl_tmp,sql_msgRef,distance);
#ifndef QT_NO_DEBUG
    qDebug() << sql_msgRef;
#endif


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
QString SyntheseDetails::SD_Tb1(QStringList boules, QString sqlTblRef,int dst)
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

QString SyntheseDetails::DoSqlMsgRef_Tb1(QStringList &boules, int dst)
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
                    and
                    ( tb1.jour_tirage like '%LUN%')
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
    QString st_cri2 = "";

    int loop = 5;//pMaConf->nbElmZone[curzn];
    st_cri1= GEN_Where_3(loop,"tb1.b",true,"=",boules,false,"or");

        if(pLaDemande->col>1)
        {
          st_cri2 =
                  "and (tb1.jour_tirage like '%"
                  +pLaDemande->st_col+"%')" ;
        }

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
            +st_cri1
            +st_cri2+
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

QString SyntheseDetails::DoSqlMsgRef_Tb2(QStringList &boules, int dst)
{
#if 0
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


( -- Recherche des lignes des tirages
select * from
(
-- 2 : comptage des boules paires par lignes
select tb1.*, count(tb2.B) as N from tirages as tb1
left join
(
select id as B from Bnrz where (z1 not null and z1%2=0)
)as tb2
on
(
tb1.b1 = tb2.B or
tb1.b2 = tb2.B or
tb1.b3 = tb2.B or
tb1.b4 = tb2.B or
tb1.b5 = tb2.B
)
group by tb1.id
) as ensemble_1
where
(
ensemble_1.N = 2
)
)as tb2
       on (
   (tb3.id = tb2.id +
   0
   )
    and
    (tb4.id = tb3.id)
    and
    (tb4.id_poids = tb5.id)
    );

#endif
    QString st_msg = "";
    QString st_cri1 = "";
    QString st_cri2 = "";


    int val = pLaDemande->boule;

    int col=pLaDemande->col;


    //---------------------
    QStringList cri_msg;

    cri_msg <<"z1%2=0"<<"z1<26";

    for(int j=0;j<=9;j++)
    {
      cri_msg<< "z1 like '%" + QString::number(j) + "'";
    }

    for(int j=0;j<6;j++)
    {
      cri_msg<< "z1 >="+QString::number(10*j)+ " and z1<="+QString::number((10*j)+9);
    }
    //---------------------

    if(col>=2 && col <= 2+cri_msg.size())
    {
        col=col-2;
    }
    else
    {
        col=0;
    }
    st_cri1= cri_msg.at(col);

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
            "select *  from "
            "("
            "select tb1.*, count(tb2.B) as N from tirages as tb1 "
            "left join"
            "("
            "select id as B from Bnrz where (z1 not null and ("
            +st_cri1+"))"
            ")as tb2 "
            "on"
            "("
            "tb1.b1 = tb2.B or "
            "tb1.b2 = tb2.B or "
            "tb1.b3 = tb2.B or "
            "tb1.b4 = tb2.B or "
            "tb1.b5 = tb2.B"
            ")"
            "group by tb1.id"
            ") as ensemble_1 "
            "where"
            "("
            "ensemble_1.N ="
            +QString::number(val)+
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
#ifndef QT_NO_DEBUG
    qDebug() << st_msg;
#endif

    return(st_msg);
}
