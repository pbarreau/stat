#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <math.h> //floor

#include <QtGui>
#include <QSplitter>
#include <QMessageBox>

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

int SyntheseDetails::detail_id = 0;

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
    ChoixCol<<"J"<<"D"<<"C"<<"B"<<"E";
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

    //ret_msg = "(" +ret_msg+")";
    return ret_msg;
}

QString FiltreLaBaseSelonSelectionUtilisateur(QModelIndexList indexes, int niveau, int maxElem,QString tmpTab, QString sin)
{
    QString msg = "";
    QString sn = QString::number(niveau);
    QString rtab = "n"+sn;

    int taille = indexes.size();

    if (taille > maxElem)
    {
        QMessageBox::warning(0,"Selection","Depassement de bornes !",QMessageBox::Yes);
        return  msg;
    }

    // retirer le ; si il existe
    sin.remove(";");

    if(taille)
    {
        QStringList lstBoules;
        QString scritere = "";
        QString headName = "";
        QModelIndex un_index;
        bool putIndice = true;

        // Analyse de chaque indexe
        foreach(un_index, indexes)
        {
            const QAbstractItemModel * pModel = un_index.model();
            int col = un_index.column();
            int lgn = un_index.row();
            int use = un_index.model()->index(lgn,0).data().toInt();
            int val = un_index.data().toInt();
            QVariant vCol = pModel->headerData(col,Qt::Horizontal);
            headName = vCol.toString();

            if(niveau>=2)
            {
                putIndice = false;
            }

            if(niveau==3)
            {
                use = lgn;
            }

            // Construire la liste des boules
            lstBoules << QString::number(use);
        }

        // Creation du critere de filtre
        QString tab = rtab + "." + tmpTab;
        scritere = GEN_Where_3(maxElem,tab,putIndice,"=",lstBoules,false,"or");
        if(headName != "T" and headName !="")
        {
            scritere = scritere + " and (J like '%" + headName +"%')";
        }


        msg = "/* DEBUT niveau " + sn
                + " */ select " + rtab + ".* from ("
                + sin + ") as " + rtab + " where ("
                + scritere +"); /* FIN niveau " + sn + " */";
    }
    else
    {
        msg = sin;
    }


#ifndef QT_NO_DEBUG
    qDebug()<<"";
    qDebug()<<"Une Requete";
    qDebug()<<msg;
    qDebug()<<"";
#endif

    return msg;
}

QString FiltreLesTirages(stCurDemande *pEtude)
{
    QString table = *(pEtude->st_baseDef);

#ifndef QT_NO_DEBUG
    qDebug() << table;
#endif

    for (int i = 0; i< 3 ;i++)
    {
        QModelIndexList indexes = pEtude->selection[i];

        if(!indexes.size())
            continue;

        QString requete = "";
        QString champ = "";
        int max = 0;

        if(i<2){
            max = pEtude->ref->nbElmZone[i];
            champ = pEtude->ref->nomZone[i];
        }

        if(i==2)
        {
            max = 1;
            champ = "pid";
        }

        requete = FiltreLaBaseSelonSelectionUtilisateur(indexes,i,max,champ,table);

        table = requete;
    }

    // Filtre sur les occurences
    if(pEtude->selection[3].size())
    {
        QModelIndex un_index;
        foreach (un_index, pEtude->selection[3])
        {
            QString requete = "";

            requete = PBAR_Req2(pEtude,table,un_index,0);

            table = requete;

        }

    }

    // Sauvegarde de la table des reponses a d=0
    *(pEtude->st_bdAll) = table;

    return table;
}

QString sql_RegroupeSelonCritere(QString st_tirages, QString st_cri)
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
            "select * from (" + st_tirages.remove(";")
            +") as r1 "
             ") as tb1 "
             "left join "
             "("
             "select id as B from Bnrz where (z1 not null  and ("+st_cri
            +")) "
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
//---------------- Fin Local Fns ------------------------
SyntheseDetails::~SyntheseDetails()
{
    detail_id--;
}

SyntheseDetails::SyntheseDetails(stCurDemande *pEtude, QMdiArea *visuel,QTabWidget *tab_Top)
{
    pLaDemande = pEtude;
    pEcran = visuel;
    gMemoTab = tab_Top;
    detail_id++;

    QString stRequete = "";
    stRequete = FiltreLesTirages(pEtude);

    // Creation des onglets reponses
    QWidget *uneReponse = PBAR_CreerOngletsReponses(pEtude,visuel,stRequete);
    QString st_titre = "R "+QString::number(detail_id);

    int laFeuille = tab_Top->addTab(uneReponse,st_titre);
    tab_Top->activateWindow();
    tab_Top->setCurrentIndex(laFeuille);

}

QWidget * SyntheseDetails::PBAR_ComptageFiltre(stCurDemande *pEtude, QString ReqTirages, int ongPere)
{
    QWidget * qw_retour = new QWidget;
    QFormLayout *frm_tmp = new QFormLayout;

    //QLabel * titre = new QLabel;

    QString ongNames[]={"b","e","c","g"};
    int maxOnglets = sizeof(ongNames)/sizeof(QString);

    QTabWidget *tab_Top = new QTabWidget;
    gtab_splitter_2[ongPere] = tab_Top;

    QWidget **wid_ForTop = new QWidget*[maxOnglets];
    QGridLayout **dsgOnglet = new QGridLayout * [maxOnglets];

    QGridLayout * (SyntheseDetails::*ptrFunc[])(stCurDemande *pEtude, QString ReqTirages, int zn,int distance)=
    {
            &SyntheseDetails::MonLayout_CompteBoulesZone,
            &SyntheseDetails::MonLayout_CompteBoulesZone,
            &SyntheseDetails::MonLayout_CompteCombi,
            &SyntheseDetails::MonLayout_CompteDistribution
};

    //titre->setText("Position "+labNames[i]);

    for(int j =0; j< maxOnglets;j++)
    {
        wid_ForTop[j]= new QWidget;
        tab_Top->addTab(wid_ForTop[j],ongNames[j]);

        dsgOnglet[j]= (this->*ptrFunc[j])(pEtude,ReqTirages,j,ongPere);
        wid_ForTop[j]->setLayout(dsgOnglet[j]);
    }

    //frm_tmp->addWidget(titre);
    frm_tmp->addWidget(tab_Top);
    qw_retour->setLayout(frm_tmp);

    connect(tab_Top, SIGNAL(tabBarClicked(int)),
            this, SLOT(slot_ClickSurOnglet(int)));


    return qw_retour;
}

QWidget * SyntheseDetails::SPLIT_Voisin(int i)
{
    QWidget * qw_retour = new QWidget;
    QFormLayout *frm_tmp = new QFormLayout;

    QLabel * titre = new QLabel;

    gtab_splitter_2[i] = new QTabWidget;
    QTabWidget *tab_Top = gtab_splitter_2[i];
    QWidget **wid_ForTop = new QWidget*[3];
    QGridLayout **dsgOnglet = new QGridLayout * [3];
    QString labNames[]={"0","+1","-1","?"};
    QString ongNames[]={"b","e","c"};

    QGridLayout * (SyntheseDetails::*ptrFunc[])(int, int)={
            &SyntheseDetails::Synthese_1,
            &SyntheseDetails::Synthese_2,
            &SyntheseDetails::MonLayout_pFnDetailsMontrerRepartition
};

    titre->setText("Position "+labNames[i]);

    for(int j =0; j< 3;j++)
    {
        wid_ForTop[j]= new QWidget;

        tab_Top->addTab(wid_ForTop[j],ongNames[j]);

        dsgOnglet[j]= (this->*ptrFunc[j])(j,dst[i]);
        wid_ForTop[j]->setLayout(dsgOnglet[j]);
    }

    frm_tmp->addWidget(titre);
    frm_tmp->addWidget(tab_Top);
    qw_retour->setLayout(frm_tmp);

    connect(tab_Top, SIGNAL(tabBarClicked(int)),
            this, SLOT(slot_ClickSurOnglet(int)));

    return qw_retour;
}

QWidget * SyntheseDetails::PBAR_CreerOngletsReponses(stCurDemande *pEtude, QMdiArea *visuel,QString stRequete)
{
    QWidget * qw_retour = new QWidget;
    QGridLayout *frm_tmp = new QGridLayout;
    QTabWidget *tab_Top = new QTabWidget;

    onglets = tab_Top;

    QLabel * titre = new QLabel;

    QString ongNames[]={"0","+1","-1","?"};
    QString sqlReq = "";

    int maxOnglets = sizeof(d)/sizeof(int);

    gtab_splitter_2 = new QTabWidget *[maxOnglets];

    QWidget **wid_ForTop = new QWidget*[maxOnglets];
    QGridLayout **dsgOnglet = new QGridLayout * [maxOnglets];

    if(pEtude->st_titre !="")
        titre->setText(pEtude->st_titre);

    for(int id_Onglet = 0; id_Onglet<maxOnglets; id_Onglet++)
    {
        // -- tableau tirages
        wid_ForTop[id_Onglet]= new QWidget;
        tab_Top->addTab(wid_ForTop[id_Onglet],ongNames[id_Onglet]);

        sqlReq = PBAR_Req3(pEtude->st_baseDef,stRequete,d[id_Onglet]);
        QGridLayout *lay_tmp = MonLayout_MontrerTiragesFiltres(visuel,sqlReq,id_Onglet,d);

        // -- onglet comptage avec tableau
        QGridLayout *MonComptage = new  QGridLayout;
        QWidget * qw_tmp = PBAR_ComptageFiltre(pEtude,sqlReq,id_Onglet);

        MonComptage->addLayout(lay_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);
        MonComptage->addWidget(qw_tmp,0,1,Qt::AlignLeft|Qt::AlignTop);

        dsgOnglet[id_Onglet]=MonComptage;
        wid_ForTop[id_Onglet]->setLayout(dsgOnglet[id_Onglet]);
    }

    frm_tmp->addWidget(titre);
    frm_tmp->addWidget(tab_Top);
    qw_retour->setLayout(frm_tmp);

    return qw_retour;
}


QWidget * SyntheseDetails::SPLIT_Tirage(void)
{
    QWidget * qw_retour = new QWidget;
    QFormLayout *frm_tmp = new QFormLayout;

    gtab_tirages = new QTabWidget;

    QTabWidget *tab_Top = gtab_tirages;
    QWidget **wid_ForTop = new QWidget*[4];
    QGridLayout **dsgOnglet = new QGridLayout * [4];
    QString ongNames[]={"0","+1","-1","?"};

    //QGridLayout * (MainWindow::*ptrFunc[])()={};
    stCurDemande uneRecherche;


    for(int i =0; i< 4;i++)
    {
        wid_ForTop[i]= new QWidget;

        tab_Top->addTab(wid_ForTop[i],ongNames[i]);

        QGridLayout *lay_tmp= MonLayout_pFnDetailsMontrerTirages(i,dst[i]);
        QGridLayout *MonTest = new  QGridLayout;
        QWidget * qw_tmp = SPLIT_Voisin(i);
        MonTest->addLayout(lay_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);
        MonTest->addWidget(qw_tmp,0,1,Qt::AlignLeft|Qt::AlignTop);
        dsgOnglet[i]=MonTest;
        wid_ForTop[i]->setLayout(dsgOnglet[i]);
    }

    frm_tmp->addWidget(tab_Top);
    qw_retour->setLayout(frm_tmp);

#if 0
    connect(tab_Top, SIGNAL(tabBarClicked(int)),
            this, SLOT(slot_ClickSurOnglet(int)));
#endif

    return qw_retour;
}

void SyntheseDetails::MontreRechercheTirages(stCurDemande *pEtude)
{
    int bouleId = pEtude->lgn[0];
    QString colName = pEtude->stc[0];

    QWidget *qw_main = new QWidget;
    QTabWidget *tab_Top = new QTabWidget;
    QTabWidget **wid_ForTop_1 = new QTabWidget*[4];

    //onglets = tab_Top;
    onglets = tab_Top;

    for(int i =0; i<4;i++)
    {
        wid_ForTop_1[i]=new QTabWidget;
        tab_Top->addTab(wid_ForTop_1[i],tr(ong[i].toUtf8()));

        QWidget **wid_ForTop_2 = new QWidget*[3];
        QString stNames_2[3]={"Tirages","Voisins","Combinaison"};
        QGridLayout *design_onglet_2[3];

        // Tableau de pointeur de fonction
        QGridLayout *(SyntheseDetails::*ptrFunc[])(int , int )=
        {&SyntheseDetails::MonLayout_pFnDetailsMontrerTirages,
                &SyntheseDetails::MonLayout_pFnDetailsMontrerSynthese,
                &SyntheseDetails::MonLayout_pFnDetailsMontrerRepartition};



        // Verifier si on est sur l'onglet n° 4
        if(i==3)
        {
            // Memoriser les layouts de l'onglet
            G_design_onglet_2 =new QGridLayout*[3];

        }

        for(int j =0; j<3;j++)
        {
            wid_ForTop_2[j]=new QWidget;
            wid_ForTop_1[i]->addTab(wid_ForTop_2[j],tr(stNames_2[j].toUtf8()));

            //
            design_onglet_2[j] = (this->*ptrFunc[j])(i, dst[i]);
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
    QString st_titre = "";


    if(pEtude->origine == 1)
    {
        st_titre =  CreationTitre_1(pEtude);
    }
    else
    {
        st_titre =  CreationTitre_2(pEtude);
    }

    if(pEtude->st_titre != "")
    {
        st_titre = "Depart:("+pEtude->st_titre+")," + st_titre;
    }
    else
    {
        pEtude->st_titre = st_titre;
    }


    mainLayout->addWidget(tab_Top);
    qw_main->setWindowTitle(st_titre);
    qw_main->setLayout(mainLayout);


    QMdiSubWindow *subWindow = pEcran->addSubWindow(qw_main);
    //subWindow->resize(493,329);
    //subWindow->move(737,560);
    qw_main->setVisible(true);
    qw_main->show();
}

QString SyntheseDetails::CreationTitre_1(stCurDemande *pEtude)
{

    QString st_tmp = "Tab1->";

    int ordre[3]={2,0,1};
    QString Nom[3]={"B:","E:","C:"};

    for(int i=0;i<3;i++)
    {
        int clef = ordre[i];
        if(pEtude->lst_boules[clef].size())
        {
            if(pEtude->stc[clef] != "")
            {
                st_tmp =  st_tmp +
                        Nom[clef] +"("
                        + pEtude->stc[clef] +
                        ") ";
            }
            else
            {
                st_tmp =  st_tmp + Nom[clef];
            }


            for(int j=0; j<pEtude->lst_boules[clef].size();j++)
                st_tmp = st_tmp + pEtude->lst_boules[clef].at(j) + ",";

        }
    }
    st_tmp.remove(st_tmp.length()-1,1);


    return st_tmp;
}

QString SyntheseDetails::CreationTitre_2(stCurDemande *pEtude)
{

    QString st_tmp = "Tab2->";

    st_tmp =  st_tmp + "Nb:"+
            QString::number(pEtude->lgn[0])+
            ","+pEtude->stc[0] +
            ":"+
            QString::number(pEtude->val[0])
            ;


    return st_tmp;
}

QGridLayout * SyntheseDetails::MonLayout_pFnDetailsMontrerTirages(int ref,
                                                                  int dst)
{
    QGridLayout *lay_return = new QGridLayout;


    QString sql_msgRef = "";
    QSqlQueryModel *sqm_tmp = new QSqlQueryModel;
    QTableView *qtv_tmp = new QTableView;


    // Fonction Pour La requete de base (obtenir les tirages)
    QString (SyntheseDetails::*ptrFunc[3])(int)=
    {&SyntheseDetails::DoSqlMsgRefGenerique,
            &SyntheseDetails::DoSqlMsgRefGenerique,
            &SyntheseDetails::DoSqlMsgRefGenerique};

    // Creer le code de la requete Sql

    sql_msgRef = (this->*ptrFunc[0])(dst);

    sqm_tmp->setQuery(sql_msgRef);

    qtv_tmp->setSortingEnabled(false);
    //qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
    qtv_tmp->setAlternatingRowColors(true);


    qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);


    // Filtre
    QHBoxLayout *tmp_hLay = new QHBoxLayout();

    QComboBox *tmp_combo = ComboPerso(ref);

    QLabel *tmp_lab = new QLabel(tr("Filtre :"));
    tmp_lab->setBuddy(tmp_combo);

    connect(tmp_combo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slot_FiltreSurNewCol(int)));


    QFormLayout *FiltreLayout = new QFormLayout;
    FiltreCombinaisons *fltComb_tmp = new FiltreCombinaisons;
    QList<qint32> colid;
    colid << 2;
    fltComb_tmp->setFiltreConfig(sqm_tmp,qtv_tmp,colid);


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
    qtv_tmp->setFixedSize(XLenTir,YLenTir);


    // Ne pas modifier largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    // Double click dans fenetre pour details et localisation dans base Tirages
    connect( qtv_tmp, SIGNAL( doubleClicked(QModelIndex)) ,
             pEcran->parent(), SLOT(slot_MontreLeTirage( QModelIndex) ) );

    //int pos_x = 0;
    int pos_y = 0;
    if(ref==3)
    {
        // Dernier onglet
        QFormLayout *distLayout = new QFormLayout;
        dist = new DistancePourTirage((dst*-1),sqm_tmp,qtv_tmp);

        distLayout->addRow("&Distance", dist);
        lay_return->addLayout(distLayout,0,0,Qt::AlignLeft|Qt::AlignTop);

        // Connection du line edit
        connect(dist, SIGNAL(returnPressed()),
                this, SLOT(slot_NouvelleDistance()));

        pos_y++;
    }
    lay_return->addLayout(tmp_hLay,pos_y,0,Qt::AlignLeft|Qt::AlignTop);
    lay_return->addWidget(qtv_tmp,pos_y+1,0,Qt::AlignLeft|Qt::AlignTop);

    // Associer la combo de selection au filtre pour
    // cette distance (ie cet onglet) et la Qtview associee
    pCritere[ref] = tmp_combo;
    pFiltre[ref] = fltComb_tmp;

    return(lay_return);
}


//----------------
//http://www.qtcentre.org/threads/11247-how-to-color-a-cell-in-a-QTableView
MaSQlModel::MaSQlModel(QObject *parent):QSqlQueryModel(parent)
{

}

QVariant MaSQlModel::data(const QModelIndex &index, int role) const
{
    if(index.column()>4 )
    {
        int val = QSqlQueryModel::data(index,role).toInt();

        if(role == Qt::DisplayRole)
        {
            if(val <=9)
            {
                QString sval = "0"+QString::number(val);

                return sval;
            }
        }

    }


    if(index.column()>=10)
    {
        if((role == Qt::TextColorRole))
        {
            return QColor(Qt::red);
        }
    }

    return QSqlQueryModel::data(index,role);
}
//----------------



QGridLayout * SyntheseDetails::MonLayout_MontrerTiragesFiltres(QMdiArea *visuel,QString sql_msgRef,
                                                               int ref,int *dst)
{
    QGridLayout *lay_return = new QGridLayout;

    //QSqlQueryModel *sqm_tmp = new QSqlQueryModel;
    MaSQlModel *sqm_tmp = new MaSQlModel;
    QTableView *qtv_tmp = new QTableView;

    sqm_tmp->setQuery(sql_msgRef);
    qtv_tmp->setModel(sqm_tmp);

    qtv_tmp->setSortingEnabled(false);
    qtv_tmp->setAlternatingRowColors(true);
    qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtv_tmp->hideColumn(0);
    qtv_tmp->hideColumn(1);

    // Formattage de largeur de colonnes
    for(int j=0;j<=4;j++)
        qtv_tmp->setColumnWidth(j,75);

    for(int j=5;j<=sqm_tmp->columnCount();j++)
        qtv_tmp->setColumnWidth(j,30);

    // Bloquer largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    // Taille tableau
    qtv_tmp->setFixedSize(XLenTir,YLenTir);

    // Double click dans fenetre pour details et localisation dans base Tirages
    connect( qtv_tmp, SIGNAL( doubleClicked(QModelIndex)) ,
             visuel->parent(), SLOT(slot_MontreLeTirage( QModelIndex) ) );


    // Zone Filtre
    QHBoxLayout *tmp_hLay = new QHBoxLayout();
    QComboBox *tmp_combo = ComboPerso(ref);
    QLabel *tmp_lab = new QLabel(tr("Filtre :"));
    tmp_lab->setBuddy(tmp_combo);
    connect(tmp_combo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slot_FiltreSurNewCol(int)));

    QFormLayout *FiltreLayout = new QFormLayout;
    FiltreCombinaisons *fltComb_tmp = new FiltreCombinaisons;
    QList<qint32> colid;
    colid << 2;
    fltComb_tmp->setFiltreConfig(sqm_tmp,qtv_tmp,colid);
    FiltreLayout->addRow("&Recherche", fltComb_tmp);

    // Associer la combo de selection au filtre pour
    // cette distance (ie cet onglet) et la Qtview associee
    pCritere[ref] = tmp_combo;
    pFiltre[ref] = fltComb_tmp;

    QLabel *tmp_lab2 = fltComb_tmp->getLabel();

    // Mettre combo + line dans hvbox
    tmp_hLay->addWidget(tmp_lab);
    tmp_hLay->addWidget(tmp_combo);
    tmp_hLay->addLayout(FiltreLayout);
    tmp_hLay->addWidget(tmp_lab2);


    int pos_y = 0;
    if(ref==3)
    {
        // Dernier onglet
        QFormLayout *distLayout = new QFormLayout;
        int val = dst[ref]*-1;
        dist = new DistancePourTirage(val,
                                      sqm_tmp,qtv_tmp);

        distLayout->addRow("&Distance", dist);
        lay_return->addLayout(distLayout,0,0,Qt::AlignLeft|Qt::AlignTop);

        // Connection du line edit
        connect(dist, SIGNAL(returnPressed()),
                this, SLOT(slot_NouvelleDistance()));

        pos_y++;
    }
    lay_return->addLayout(tmp_hLay,pos_y,0,Qt::AlignLeft|Qt::AlignTop);
    lay_return->addWidget(qtv_tmp,pos_y+1,0,Qt::AlignLeft|Qt::AlignTop);


    return(lay_return);
}


void SyntheseDetails::slot_FiltreSurNewCol(int colNum)
{
    int z1 = pLaDemande->ref->nbElmZone[0];
    int z2 = pLaDemande->ref->nbElmZone[1];
    int ConfZn[5][2]={
        {2,1},
        {3,1},
        {4,1},
        {5,z1},
        {5+z1,z2}
    };

    int ColReal = ConfZn[colNum][0];
    int nbCol = ConfZn[colNum][1];

    int val = onglets->currentIndex();
    pFiltre[val]->clear();
    pFiltre[val]->slot_setFKC(ColReal,nbCol);
}

void SyntheseDetails::slot_NouvelleDistance(void)
{
    QString msg = "";
    int new_distance = dist->getValue();

    // pour Coherence par rapport a graphique et requete
    new_distance *=-1;

    // Recherche sur nouveau critere
    msg = PBAR_Req3(pLaDemande->st_baseDef,*(pLaDemande->st_bdAll),new_distance);

    // Application de la requete pour le tableau des tirages
    QSqlQueryModel *sqlmodel = dist->getAssociatedModel();
    QTableView *tab =dist->getAssociatedVue();


    // recalcul
    sqlmodel->setQuery(msg);
    tab->setModel(sqlmodel);

    // Aplication pour les onglets de comptage
    FiltreCombinaisons *pCombi = NULL;
    for(int id=0;id<3;id++)

    {
        QString compte = "";

        if(id<2)
        {
            compte =  PBAR_ReqComptage(pLaDemande, msg, id, new_distance);
        }
        else
        {
            compte = PBAR_ReqNbCombi(pLaDemande, msg);
            pCombi = dist->GetFiltre();
        }

        sqlmodel = dist->getAssociatedModel(id);
        tab =dist->getAssociatedVue(id);
        QSortFilterProxyModel *m = dist->GetProxyModel(id);

        // recalcul
        sqlmodel->setQuery(compte);
        if(m !=NULL)
        {
            m->setDynamicSortFilter(true);
            m->setSourceModel(sqlmodel);
            tab->setModel(m);
        }
        else
        {
            tab->setModel(sqlmodel);
        }

        if(pCombi != NULL){
            QList<qint32> colid;
            colid << 1;
            pCombi->setFiltreConfig(sqlmodel,tab,colid);
        }

    }
    // Memorisation
    dist->setValue(new_distance *-1);
    d[3]= new_distance;
}

QGridLayout * SyntheseDetails::MonLayout_pFnDetailsMontrerSynthese(int ref, int dst)
{
    QGridLayout *lay_return = new QGridLayout;
    QGridLayout *lay_1;
    QGridLayout *lay_2;

    lay_1 = Synthese_1(ref,dst);
    lay_2 = Synthese_2(ref,dst);

    lay_return->addLayout(lay_1,0,0);
    lay_return->addLayout(lay_2,0,1);

    return(lay_return);
}

QGridLayout * SyntheseDetails::MonLayout_CompteCombi(stCurDemande *pEtude, QString ReqTirages, int zn, int ongPere)
{
    QGridLayout *lay_return = new QGridLayout;


    QString sql_msgRef = "";
    QSqlQueryModel *sqm_tmp = new QSqlQueryModel;
    QTableView *qtv_tmp = new QTableView;

    // Filtre
    QFormLayout *FiltreLayout = new QFormLayout;
    FiltreCombinaisons *fltComb_1 = new FiltreCombinaisons();
    QList<qint32> colid;
    colid << 1;
    fltComb_1->setFiltreConfig(sqm_tmp,qtv_tmp,colid);

    //fltComb_1->setFiltreConfig(sqm_tmp,qtv_tmp,1);
    FiltreLayout->addRow("&Filtre Repartition", fltComb_1);

    // Memorisation des pointeurs
    if(ongPere == 3)
    {
        dist->keepPtr(2,sqm_tmp,qtv_tmp, NULL);
        dist->keepFiltre(fltComb_1);
    }


    sql_msgRef = PBAR_ReqNbCombi(pEtude,ReqTirages);

    sqm_tmp->setQuery(sql_msgRef);

    // Mettre le nom des colonnes sur 2 lettres
    int nbcol = sqm_tmp->columnCount();
    for(int i = 0; i<nbcol;i++)
    {
        QString headName = sqm_tmp->headerData(i,Qt::Horizontal).toString();
        if(headName.size()>2)
        {
            sqm_tmp->setHeaderData(i,Qt::Horizontal,headName.left(2));
        }
    }


    qtv_tmp->setSortingEnabled(true);
    qtv_tmp->sortByColumn(1,Qt::AscendingOrder);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->hideColumn(0); // don't show the ID
    qtv_tmp->verticalHeader()->hide();
    qtv_tmp->setFixedSize(CLargeur1,CHauteur1);

    // Taille/Nom des colonnes
    qtv_tmp->setColumnWidth(1,70);
    for(int j=2;j<sqm_tmp->columnCount();j++)
    {
        qtv_tmp->setColumnWidth(j,LCELL);
    }
    // Ne pas modifier largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    // Connection du double click dans table voisins
    // double click dans fenetre  pour afficher details boule
    connect( qtv_tmp, SIGNAL(doubleClicked(QModelIndex)) ,
             this, SLOT(slot_ZoomTirages( QModelIndex) ) );


    int pos_y = 0;


    lay_return->addLayout(FiltreLayout,pos_y,0,Qt::AlignLeft|Qt::AlignTop);
    lay_return->addWidget(qtv_tmp,pos_y+1,0,Qt::AlignLeft|Qt::AlignTop);

    return(lay_return);
}

QGridLayout * SyntheseDetails::MonLayout_CompteDistribution(stCurDemande *pEtude, QString ReqTirages, int laPos, int ongPere)
{
    QGridLayout *lay_return = new QGridLayout;

    int zn=0;
    int maxElems = pEtude->ref->limites[zn].max;
    int nbBoules = floor(maxElems/10)+1;

    QStringList *maRef = LstCritereGroupement(zn,pEtude->ref);
    int nbCol = maRef[0].size();
    int nbLgn = pEtude->ref->nbElmZone[zn] + 1;

    QTableView *qtv_tmp = new QTableView;
    QStandardItemModel * tmpStdItem = NULL;
    QSqlQuery query ;

    //Creer un tableau d'element standard
    if(nbCol)
    {
        tmpStdItem =  new QStandardItemModel(nbLgn,nbCol);
        qtv_tmp->setModel(tmpStdItem);

        QStringList tmp=maRef[1];
        tmp.insert(0,"Nb");
        tmpStdItem->setHorizontalHeaderLabels(tmp);
        for(int lgn=0;lgn<nbLgn;lgn++)
        {
            for(int pos=0;pos <=nbCol;pos++)
            {
                QStandardItem *item = new QStandardItem();

                if(pos == 0){
                    item->setData(lgn,Qt::DisplayRole);
                }
                tmpStdItem->setItem(lgn,pos,item);
                qtv_tmp->setColumnWidth(pos,LCELL);
            }
        }
        // Gestion du QTableView
        qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
        qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
        qtv_tmp->setStyleSheet("QTableView {selection-background-color: #939BFF;}");

        qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
        qtv_tmp->setAlternatingRowColors(true);
        qtv_tmp->setFixedSize(CLargeur1*1.8,CHauteur1);

        qtv_tmp->setSortingEnabled(true);
        qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
        qtv_tmp->verticalHeader()->hide();

        QHeaderView *htop = qtv_tmp->horizontalHeader();
        htop->setSectionResizeMode(QHeaderView::Fixed);
        qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);


        QVBoxLayout *vb_tmp = new QVBoxLayout;
        QLabel * lab_tmp = new QLabel;
        lab_tmp->setText("Groupement par memoire");
        vb_tmp->addWidget(lab_tmp,0,Qt::AlignLeft|Qt::AlignTop);
        vb_tmp->addWidget(qtv_tmp,0,Qt::AlignLeft|Qt::AlignTop);
        lay_return->addLayout(vb_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);
    }
    else
    {
        return lay_return;
    }

    bool status = true;
    for(int i=0; (i< nbCol) && (status == true);i++)
    {
        // Creer Requete pour compter items
        QString msg1 = maRef[0].at(i);
        QString sqlReq = "";
        sqlReq = sql_RegroupeSelonCritere(ReqTirages,msg1);

#ifndef QT_NO_DEBUG
        qDebug() << sqlReq;
#endif

        status = query.exec(sqlReq);

        // Mise a jour de la tables des resultats
        if(status)
        {
            query.first();
            do
            {
                int nb = query.value(0).toInt();
                int tot = query.value(1).toInt();

                QStandardItem * item_1 = tmpStdItem->item(nb,i+1);
                item_1->setData(tot,Qt::DisplayRole);
                tmpStdItem->setItem(nb,i+1,item_1);
            }while(query.next() && status);
        }
    }

    return(lay_return);
}


QGridLayout * SyntheseDetails::MonLayout_pFnDetailsMontrerRepartition(int ref, int dst)
{
    QGridLayout *lay_return = new QGridLayout;


    QString sql_msgRef = "";
    QSqlQueryModel *sqm_tmp = new QSqlQueryModel;

    QTableView *qtv_tmp = new QTableView;
    qtv_local[ref][2] = qtv_tmp;

    sql_msgRef = DoSqlMsgRefGenerique(dst);
    // Retirer le ; de la fin
    sql_msgRef.replace(";","");



    sql_msgRef =
            "select tbleft.id as id, tbleft.tip as Repartition, count(tbright.id) as T, "
            +*(pLaDemande->st_jourDef)
            + " "
              "from "
              "( "
              "select id , tip  from lstcombi "
              ") as tbleft "
              "left join "
              "( "
            +sql_msgRef+
            ") as tbright "
            "on "
            "( "
            "( "
            "tbleft.id=tbright.pid"
            ") "
            ") group by tbleft.id order by T desc, tbleft.tip asc;"
            ;
#ifndef QT_NO_DEBUG
    qDebug() << sql_msgRef;
#endif

    sqm_tmp->setQuery(sql_msgRef);

    // Filtre
    QFormLayout *FiltreLayout = new QFormLayout;
    FiltreCombinaisons *fltComb_1 = new FiltreCombinaisons();
    QList<qint32> colid;
    colid << 1;
    fltComb_1->setFiltreConfig(sqm_tmp,qtv_tmp,colid);

    //fltComb_1->setFiltreConfig(sqm_tmp,qtv_tmp,1);
    FiltreLayout->addRow("&Filtre Repartition", fltComb_1);

    qtv_tmp->setSortingEnabled(true);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->hideColumn(0); // don't show the ID
    qtv_tmp->verticalHeader()->hide();
    qtv_tmp->setFixedSize(290,CHauteur1);

    // Taille/Nom des colonnes
    qtv_tmp->setColumnWidth(1,70);
    for(int j=2;j<sqm_tmp->columnCount();j++)
    {
        qtv_tmp->setColumnWidth(j,40);
    }
    // Ne pas modifier largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    // Connection du double click dans table voisins
    // double click dans fenetre  pour afficher details boule
    connect( qtv_tmp, SIGNAL(doubleClicked(QModelIndex)) ,
             this, SLOT(slot_ZoomTirages( QModelIndex) ) );


    int pos_y = 0;


    lay_return->addLayout(FiltreLayout,pos_y,0,Qt::AlignLeft|Qt::AlignTop);
    lay_return->addWidget(qtv_tmp,pos_y+1,0,Qt::AlignLeft|Qt::AlignTop);

    return(lay_return);
}

QString PBAR_ReqNbCombi(stCurDemande *pEtude, QString ReqTirages)
{
    QString msg = "";

    msg =
            "select tbleft.id as id, tbleft.tip as Repartition, count(tbright.id) as T, "
            +*(pEtude->st_jourDef)
            + " "
              "from "
              "( "
              "select id , tip  from lstcombi "
              ") as tbleft "
              "left join "
              "( "
            +ReqTirages.remove(";")+
            ") as tbright "
            "on "
            "( "
            "( "
            "tbleft.id=tbright.pid"
            ") "
            ") group by tbleft.id order by T desc, tbleft.tip asc;"
            ;
#ifndef QT_NO_DEBUG
    qDebug() << msg;
#endif

    return msg;
}

QString PBAR_ReqComptage(stCurDemande *pEtude, QString ReqTirages, int zn,int distance)
{
    QString msg = "";

    QString st_cri_all = "";
    QStringList boules;

    if(pEtude->selection[zn].size())
    {
        if(distance == 0 ){
            QModelIndex une_selection;
            foreach (une_selection, pEtude->selection[0]) {
                int la_boule = une_selection.model()->index(une_selection.row(),0).data().toInt();
                boules<<QString::number(la_boule);
            }
            // Mettre une exception pour ne pas compter le cas
            st_cri_all= GEN_Where_3(1,"tbleft.boule",false,"!=",boules,false,"and");
            boules.clear();
            st_cri_all= st_cri_all + " and ";
        }
    }

    boules<< "tbright."+pEtude->ref->nomZone[zn];
    int loop = pEtude->ref->nbElmZone[zn];
    st_cri_all= st_cri_all +GEN_Where_3(loop,"tbleft.boule",false,"=",boules,true,"or");
    boules.clear();

    msg =
            "select tbleft.boule as B, count(tbright.id) as T, "
            +*(pEtude->st_jourDef)
            + " "
              "from "
              "( "
              "select id as boule from Bnrz where (z1 not null ) "
              ") as tbleft "
              "left join "
              "( "
            +ReqTirages.remove(";")+
            ") as tbright "
            "on "
            "( "
            + st_cri_all +
            ") group by tbleft.boule; "
            ;

#ifndef QT_NO_DEBUG
    qDebug() << msg;
#endif


    return msg;

}

QGridLayout * SyntheseDetails::MonLayout_CompteBoulesZone(stCurDemande *pEtude, QString ReqTirages, int zn,int ongPere)
{
    QGridLayout *lay_return = new QGridLayout;

    QSqlQueryModel *sqm_tmp = new QSqlQueryModel;
    QTableView *qtv_tmp = new QTableView;

    QString sql_msgRef = PBAR_ReqComptage(pEtude, ReqTirages, zn, ongPere);

    sqm_tmp->setQuery(sql_msgRef);

    // Renommer le nom des colonnes
    int nbcol = sqm_tmp->columnCount();
    for(int i = 0; i<nbcol;i++)
    {
        QString headName = sqm_tmp->headerData(i,Qt::Horizontal).toString();
        if(headName.size()>2)
        {
            sqm_tmp->setHeaderData(i,Qt::Horizontal,headName.left(2));
        }
    }

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

    // Memorisation des pointeurs
    if(ongPere == 3)
    {
        dist->keepPtr(zn,sqm_tmp,qtv_tmp,m);
    }


    qtv_tmp->setModel(m);
    qtv_tmp->verticalHeader()->hide();

    for(int j=0;j<=sqm_tmp->columnCount();j++)
        qtv_tmp->setColumnWidth(j,LCELL);
    qtv_tmp->setFixedSize(CLargeur1,CHauteur1);

    // Ne pas modifier largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    QLabel *titre_1 = new QLabel(pEtude->ref->FullNameZone[zn]);
    //lay_return->addWidget(titre_1,0,0,Qt::AlignCenter|Qt::AlignTop);
    lay_return->addWidget(titre_1,0,0,Qt::AlignLeft|Qt::AlignTop);
    lay_return->addWidget(qtv_tmp,1,0,Qt::AlignLeft|Qt::AlignTop);

    return lay_return;
}

QGridLayout * SyntheseDetails::Synthese_1(int onglet, int distance)
{
    QGridLayout *lay_return = new QGridLayout;

    QString sql_msgRef = "";
    QSqlQueryModel *sqm_tmp = new QSqlQueryModel;

    QString st_cri_all = "";
    QStringList boules;

    QTableView *qtv_tmp = new QTableView;
    qtv_local[onglet][0] = qtv_tmp;

    sql_msgRef = DoSqlMsgRefGenerique(distance);
    // Retirer le ; de la fin
    sql_msgRef.replace(";","");

    if(pLaDemande->lst_boules[0].size())
    {
        if(distance == 0 ){
            // Mettre une exception pour ne pas compter le cas
            st_cri_all= GEN_Where_3(1,"tbleft.boule",false,"!=",pLaDemande->lst_boules[0],false,"and");
            st_cri_all= st_cri_all + " and ";

        }
    }
    boules<< "tbright.b";
    st_cri_all= st_cri_all +GEN_Where_3(5,"tbleft.boule",false,"=",boules,true,"or");




#ifndef QT_NO_DEBUG
    qDebug() << st_cri_all;
#endif

    sql_msgRef =
            "select tbleft.boule as B, count(tbright.id) as T, "
            +*(pLaDemande->st_jourDef)
            + " "
              "from "
              "( "
              "select id as boule from Bnrz where (z1 not null ) "
              ") as tbleft "
              "left join "
              "( "
            +sql_msgRef+
            ") as tbright "
            "on "
            "( "
            + st_cri_all +
            ") group by tbleft.boule; "
            ;

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
    qtv_tmp->verticalHeader()->hide();

    for(int j=0;j<=sqm_tmp->columnCount();j++)
        qtv_tmp->setColumnWidth(j,30);
    qtv_tmp->setFixedSize(200,CHauteur1);


    // Ne pas modifier largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    QLabel *titre_1 = new QLabel("Boules");
    //lay_return->addWidget(titre_1,0,0,Qt::AlignCenter|Qt::AlignTop);
    lay_return->addWidget(titre_1,0,0,Qt::AlignLeft|Qt::AlignTop);
    lay_return->addWidget(qtv_tmp,1,0,Qt::AlignLeft|Qt::AlignTop);

    // Connection du double click dans table voisins
    // double click dans fenetre  pour afficher details boule
#if 0
    connect( qtv_tmp, SIGNAL(doubleClicked(QModelIndex)) ,
             this, SLOT(slot_ZoomTirages( QModelIndex) ) );
#endif
    return lay_return;

}

//------
QGridLayout *  SyntheseDetails::Synthese_2(int onglet, int distance)
{
    QGridLayout *lay_return = new QGridLayout;

    QString sql_msgRef = "";
    QString st_cri_all = "";
    QStringList boules;

    QSqlQueryModel *sqm_tmp = new QSqlQueryModel;

    QTableView *qtv_tmp = new QTableView;
    qtv_local[onglet][1] = qtv_tmp;



    sql_msgRef = DoSqlMsgRefGenerique(distance);
    // Retirer le ; de la fin
    sql_msgRef.replace(";","");

    if(pLaDemande->lst_boules[1].size())
    {
        if(distance == 0 ){
            // Mettre une exception pour ne pas compter le cas
            st_cri_all= st_cri_all +GEN_Where_3(1,"tbleft.boule",false,"!=",pLaDemande->lst_boules[0],false,"and");
            st_cri_all= st_cri_all + " and ";

        }
    }
    boules<< "tbright.e";
    st_cri_all= st_cri_all +GEN_Where_3(1,"tbleft.boule",false,"=",boules,true,"or");



#ifndef QT_NO_DEBUG
    qDebug() << st_cri_all;
#endif


    sql_msgRef =
            "select tbleft.boule as B, count(tbright.id) as T, "
            +*(pLaDemande->st_jourDef)
            + " "
              "from "
              "( "
              "select id as boule from Bnrz where (z2 not null ) "
              ") as tbleft "
              "left join "
              "( "
            +sql_msgRef+
            ") as tbright "
            "on "
            "( "
            "( "
            + st_cri_all +
            ") "
            ") group by tbleft.boule; "
            ;
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
    qtv_tmp->verticalHeader()->hide();

    for(int j=0;j<=sqm_tmp->columnCount();j++)
        qtv_tmp->setColumnWidth(j,30);
    qtv_tmp->setFixedSize(200,CHauteur1);


    // Ne pas modifier largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    QLabel *titre_1 = new QLabel("Etoiles");
    //lay_return->addWidget(titre_1,0,0,Qt::AlignCenter|Qt::AlignTop);
    lay_return->addWidget(titre_1,0,0,Qt::AlignLeft|Qt::AlignTop);
    lay_return->addWidget(qtv_tmp,1,0,Qt::AlignLeft|Qt::AlignTop);

    // Connection du double click dans table voisins
    // double click dans fenetre  pour afficher details boule
    connect( qtv_tmp, SIGNAL(doubleClicked(QModelIndex)) ,
             this, SLOT(slot_ZoomTirages( QModelIndex) ) );

    return lay_return;
}

//--------
//Synthese detaille table 1
QString SyntheseDetails::SD_Tb1_1(QStringList &boules, QString &sqlTblRef,int dst)
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
    QString st_cr4 = "";
    QStringList stl_etoile;

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

    if(stl_etoile.size())
    {
        st_cr4 = GEN_Where_3(5,"tbleft.boule",false,"=",stl_etoile,true,"or");
        st_cr4 = " and " + st_cr4;
    }

    sql_msg =
            "select tbleft.boule as B, count(tbright.Tid1) as T, "
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

//-------------
QString SyntheseDetails::SD_Tb2_1(QStringList &boules, QString &sqlTblRef,int dst)
{
    QString sql_msg ="";
    QString st_cr2 = "";
    QString st_cr3 = "";
    QStringList stl_tmp;
    QString st_cr4 = "";
    QStringList stl_etoile;

    stl_tmp << "tbright.e";
    st_cr2 =  GEN_Where_3(1,"tbleft.boule",false,"=",stl_tmp,true,"or");
#ifndef QT_NO_DEBUG
    qDebug() << st_cr2;
#endif


    if(stl_etoile.size())
    {
        st_cr4 = GEN_Where_3(5,"tbleft.boule",false,"=",stl_etoile,true,"or");
        st_cr4 = " and " + st_cr4;
    }

    sql_msg =
            "select tbleft.boule as E, count(tbright.Tid1) as T, "
            "count(CASE WHEN  J like 'lundi%' then 1 end) as LUN,"
            "count(CASE WHEN  J like 'mercredi%' then 1 end) as MER,"
            "count(CASE WHEN  J like 'same%' then 1 end) as SAM "
            "from"
            "("
            "select id as boule from Bnrz where (z2 not null)"
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

QString SyntheseDetails::SD_Tb1_2(QStringList &boules, QString &sqlTblRef,int dst)
{
    QString sql_msg ="";

#ifndef QT_NO_DEBUG
    qDebug() << sqlTblRef;
#endif


#ifndef QT_NO_DEBUG
    qDebug() << sql_msg;
#endif

    return sql_msg;
}

QString SyntheseDetails::SD_Tb2_2(QStringList &boules, QString &sqlTblRef,int dst)
{
    QString sql_msg ="";

#ifndef QT_NO_DEBUG
    qDebug() << sqlTblRef;
#endif


#ifndef QT_NO_DEBUG
    qDebug() << sql_msg;
#endif

    return sql_msg;
}
QString SyntheseDetails::SD_Tb1_3(QStringList &boules, QString &sqlTblRef,int dst)
{
#if 0
#endif
    QString sql_msg ="";

#ifndef QT_NO_DEBUG
    qDebug() << sqlTblRef;
#endif


    sql_msg =
            "select tbleft.boule as B, count(tbright.Tid1) as T, "
            +*(pLaDemande->st_jourDef)+
            "from "
            "( "
            "select id as boule from Bnrz where (z1 not null ) "
            ") as tbleft "
            "left join "
            "( "
            +sqlTblRef+
            ") as tbright "
            "on "
            "( "
            "( "
            "tbleft.boule = tbright.b1 or "
            "tbleft.boule = tbright.b2 or "
            "tbleft.boule = tbright.b3 or "
            "tbleft.boule = tbright.b4 or "
            "tbleft.boule = tbright.b5 "
            ") "
            ") group by tbleft.boule; "
            ;

#ifndef QT_NO_DEBUG
    qDebug() << sql_msg;
#endif

    return sql_msg;
}

//-----
QString SyntheseDetails::SD_Tb2_3(QStringList &boules, QString &sqlTblRef,int dst)
{
#if 0
    -- Requete comptage du resultat precedent
            select tbleft.boule as E, count(tbright.Tid1) as T,
            count(CASE WHEN  J like 'lundi%' then 1 end) as LUN, count(CASE WHEN  J like 'mercredi%' then 1 end) as MER, count(CASE WHEN  J like 'same%' then 1 end) as SAM
            from
            (
                select id as boule from Bnrz where (z2 not null )
                ) as tbleft
            left join
            (
                select tb3.id as Tid1, tb5.id as Pid1, tb3.jour_tirage as J, substr(tb3.date_tirage,-2,2)||'/'||substr(tb3.date_tirage,6,2)||'/'||substr(tb3.date_tirage,1,4) as D, tb5.tip as C, tb3.b1 as b1, tb3.b2 as b2,tb3.b3 as b3,tb3.b4 as b4,tb3.b5 as b5, tb3.e1 as e1 from tirages as tb3, analyses as tb4, lstcombi as tb5 inner join ( select tirages.*,  analyses.id_poids from tirages,analyses where ( tirages.id=analyses.id and analyses.id_poids = 115) ) as tb2 on ( (tb3.id = tb2.id + 0) and (tb4.id = tb3.id) and (tb4.id_poids = tb5.id) )
                ) as tbright
            on
            (
                (
                    tbleft.boule = tbright.e1
            )
                ) group by tbleft.boule;
#endif
    QString sql_msg ="";

#ifndef QT_NO_DEBUG
    qDebug() << sqlTblRef;
#endif


    sql_msg =
            "select tbleft.boule as E, count(tbright.Tid1) as T, "
            "count(CASE WHEN J like 'lundi%' then 1 end) as LUN, count(CASE WHEN J like 'mercredi%' then 1 end) as MER, count(CASE WHEN J like 'same%' then 1 end) as SAM "
            "from "
            "( "
            "select id as boule from Bnrz where (z2 not null ) "
            ") as tbleft "
            "left join "
            "( "
            +sqlTblRef+
            ") as tbright "
            "on "
            "( "
            "( "
            "tbleft.boule = tbright.e1 "
            ") "
            ") group by tbleft.boule; "
            ;

#ifndef QT_NO_DEBUG
    qDebug() << sql_msg;
#endif

    return sql_msg;
}


QString SyntheseDetails::DoSqlMsgRef_Tb4(QStringList &boules, int dst)
{
#if 0
    select tg.id as cid, tg.tip as Repartition,count(tr.new_pid) as T,
            count(CASE WHEN  J like 'lundi%' then 1 end) as LUN, count(CASE WHEN  J like 'mercredi%' then 1 end) as MER, count(CASE WHEN  J like 'same%' then 1 end) as SAM
            from lstcombi as tg
            left join
            (
                -- debuftleft
                select tg.id as previous_id,tr.id as new_id, tr.npid as new_pid, tg.*,tr.*from
                (select tirages.*, lstcombi.id as previous_pid from tirages, analyses, lstcombi
                 where
                 (
                     lstcombi.id = 125
            and
            tirages.id = analyses.id
            and
            analyses.id_poids = lstcombi.id
            )
                 ) as tg
                left join
                (
                    select t1.*,t2.id as npid from tirages as t1, lstcombi as t2, analyses as t3
                    where (t1.id=t3.id and t3.id_poids=t2.id)
                    )as tr
                on
                (
                    tg.id = tr.id+0
            )
                -- fin left
                )as tr
            on
            (
                tg.id = tr.new_pid
            )group by tg.id order by tg.id asc
        #endif
            QString st_sqlR = "";
    QString st_cri1 = "";
    //QString st_cri2 = "";
    int val = pLaDemande->lgn[0];

    if(boules.size())
    {
        st_cri1= GEN_Where_3(5,"tirages.b",true,"=",boules,false,"or");
        st_cri1 = " and " + st_cri1;
    }

    st_sqlR =
            "select tg.id as cid, tg.tip as Repartition,count(tr.new_pid) as T, "
            "count(CASE WHEN  J like 'lundi%' then 1 end) as LUN, count(CASE WHEN  J like 'mercredi%' then 1 end) as MER, count(CASE WHEN  J like 'same%' then 1 end) as SAM "
            "from lstcombi as tg "
            "left join "
            "( "
            "/* debuftleft */"
            "select tg.id as previous_id,tr.id as new_id, tr.npid as new_pid, tg.*,tr.*from "
            "(select tirages.*, lstcombi.id as previous_pid from tirages, analyses, lstcombi "
            "where "
            "( "
            "lstcombi.id = "
            + QString::number(val) +
            " "
            "and "
            "tirages.id = analyses.id "
            "and "
            "analyses.id_poids = lstcombi.id "
            + st_cri1 +
            ") "
            ") as tg "
            "left join "
            "( "
            "select t1.id as id,"
            "t1.jour_tirage as J,"
            "substr(t1.date_tirage,-2,2)||'/'||substr(t1.date_tirage,6,2)||'/'||substr(t1.date_tirage,1,4) as D,"
            "t2.tip as C,"
            "t1.b1 as b1, t1.b2 as b2,t1.b3 as b3,t1.b4 as b4,t1.b5 as b5,"
            "t1.e1 as e1,"
            "t2.id as npid from tirages as t1, lstcombi as t2, analyses as t3 "
            "where (t1.id=t3.id and t3.id_poids=t2.id) "
            ")as tr "
            "on  "
            "( "
            "tr.id = tg.id + "
            +QString::number(dst)+
            ") "
            "/* fin left */"
            ")as tr "
            "on "
            "( "
            "tg.id = tr.new_pid "
            ")group by tg.id order by tg.id asc;";

#ifndef QT_NO_DEBUG
    qDebug() << st_sqlR;
#endif
    return st_sqlR;

}

QString SyntheseDetails::ReponsesOrigine_1(int dst)
{
    QString st_sqlR = "";

    int loop[]={5,1,1};
    QString table[]={"tb1.b","tb1.e","tb1.pid"};
    bool inc1[] ={true,true,false};
    QString cond1[]={"=","=","="};
    bool inc2[] ={false,false,false};
    QString cond2[]={"or","or","or"};

    QString msg_req[3] = {""};
    QString st_jour[3] = {""};

    QString st_cri_all = "";

    QString jonc = " and ";

    QStringList boules;

    QString st_baseTirages = "";
    st_baseTirages = pLaDemande->st_bdAll->remove(";");

    QString st_baseUse = "";
    st_baseUse = pLaDemande->st_baseDef->remove(";");

#ifndef QT_NO_DEBUG
    qDebug() << st_baseTirages;
#endif

#ifndef QT_NO_DEBUG
    qDebug() << st_baseUse;
#endif


    int col_day = 1;
    for(int i = 0; i< 3; i++)
    {
        boules=pLaDemande->lst_boules[i];
        msg_req[i] = "";

        // creation du message en fonction d'origine
        if(boules.size())
            msg_req[i]= GEN_Where_3(loop[i],table[i],inc1[i],cond1[i],boules,inc2[i],cond2[i]);

        if (msg_req[i] != "")
        {
            st_cri_all = st_cri_all + msg_req[i] + jonc;
        }

        // Jour demande ?
        if (i == 2)
            col_day = 2;

        if(pLaDemande->col[i]>col_day)
            st_jour[i] =
                    QString::fromLocal8Bit("(tb1.J like '%")
                    +pLaDemande->stc[i]+
                    "%')" ;

        if (st_jour[i] != "")
        {
            st_cri_all = st_cri_all +st_jour[i] +  jonc ;
        }
    }

    // on retire le dernier and
    if(st_cri_all != "")
    {
        st_cri_all.remove(st_cri_all.size()-jonc.size(),jonc.size());
        st_cri_all = " and " + st_cri_all;
    }

#ifndef QT_NO_DEBUG
    qDebug() << st_cri_all;
#endif



    st_sqlR = "select tb2.* from "
              "("
            +st_baseUse+
            " )as tb1"
            ","
            "("
            +st_baseTirages+
            ")as tb2 "
            "where"
            "("
            "(tb2.id=tb1.id + "
            +QString::number(dst) + ") "
            +st_cri_all+
            ");";

#ifndef QT_NO_DEBUG
    qDebug() << st_sqlR;
#endif

    return st_sqlR;
}

QString SyntheseDetails::DoSqlMsgRefGenerique(int dst)
{
    QString st_sqlR = "";

    if(pLaDemande->origine ==1)
    {
        st_sqlR =  ReponsesOrigine_1(dst);
    }
    else
    {
        st_sqlR =  ReponsesOrigine_2(dst);
    }
    return st_sqlR;
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

    if(pLaDemande->col[0]>1)
    {
        st_cri2 =
                "and (tb1.jour_tirage like '%"
                +pLaDemande->stc[0]+"%')" ;
    }

    st_msg =
            "select tb3.id as Tid1, tb5.id as Pid,"
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

QString PBAR_Req2(stCurDemande *pRef,QString baseFiltre,QModelIndex cellule,int zn)
{
    QStringList cri_msg;
    QString     st_cri1 = "";
    QString     st_cri2 = "";
    QString     st_req1 = "";
    QString     st_req2 = "";
    QString     szn = "z" + QString::number(zn+1);

    int col = cellule.column();
#if USE_repartition_bh
    int nbi = cellule.model()->index(cellule.row(),1).data().toInt();
#else
    int nbi = cellule.model()->index(cellule.row(),0).data().toInt();
#endif

    cri_msg <<szn+"%2=0"<<szn+"<"+QString::number((pRef->ref->limites[0].max)/2);

    for(int j=0;j<=9;j++)
    {
        cri_msg<< szn+ " like '%" + QString::number(j) + "'";
    }

    int max = floor(pRef->ref->limites[zn].max/10)+1;;
    for(int j=0;j<max;j++)
    {
        cri_msg<< szn+" >="+QString::number(10*j)+ " and "+szn+"<="+QString::number((10*j)+9);
    }

#if USE_repartition_bh
    if(col>=2 && col <= 2+cri_msg.size())
    {
        col=col-2;
    }
#else
    if(col>=1 && col <= 1+cri_msg.size())
    {
        col=col-1;
    }

#endif
    else
    {
        col=0;
    }
    st_cri1= cri_msg.at(col);

    // Requete filtre sur la colonne
    int loop = pRef->ref->nbElmZone[zn];
    QString tbname1 = "tb1." + pRef->ref->nomZone[zn];
    QStringList Malst;
    Malst << "tb2.B";
    st_cri2 = GEN_Where_3(loop,tbname1,true,"=",Malst,false,"or");
    st_req1 = "/* DEBUT Rq Comptage */ select tb1.*, count(tb2.B) as N from ("
            + baseFiltre.remove(";")
            +")as tb1 "
            +"left join"
            +"("
            +"select id as B from Bnrz where"
            +"("
            + szn + " not null and "
            +"("
            + st_cri1
            +")"
            +")"
            +") as tb2 "
            +"on"
            +"("
            + st_cri2
            +") group by tb1.id; /* Fin Rq Comptage */";

#ifndef QT_NO_DEBUG
    qDebug() << st_req1;
#endif

    st_req2 = "/* DEBUT Filtre Ligne */ select monfiltre.* from ("
            + st_req1.remove(";")
            +")as monfiltre where(monfiltre.N="
            + QString::number(nbi)
            +"); /* Fin Filtre ligne */";

#ifndef QT_NO_DEBUG
    qDebug() << st_req2;
#endif

    return st_req2;
}

QString PBAR_Req3(QString *base, QString baseFiltre,int dst)
{
    QString req = "";

#ifndef QT_NO_DEBUG
    qDebug() << *base;
#endif

    req = "Select Mabdd.* from ("
            + (*base).remove(";")
            +") as Mabdd inner join ("
            +baseFiltre.remove(";")
            +")as filtre on ( Mabdd.id = filtre.id +"
            +QString::number(dst)
            +");";

#ifndef QT_NO_DEBUG
    qDebug() << req;
#endif

    return req;
}

QString SyntheseDetails::ReponsesOrigine_2(int dst)
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
    QString st_sqlR = "";
    QString st_cri1 = ""; // ensemble de depart
    QString st_cri2 = ""; // contrainte sur z1 (boule)
    QString st_cri3 = ""; // contrainte sur z2 (etoiles)
    QString st_criWhere = ""; // contrainte communes
    QString st_cri1_1 = "";

    int loop[]={5,1,1};
    QString table[]={"tb2.b","tb2.e","tb5.id"};
    bool inc1[] ={true,true,false};
    QString cond1[]={"=","=","="};
    bool inc2[] ={false,false,false};
    QString cond2[]={"or","or","or"};

    int lgn = pLaDemande->lgn[0];
    int col=pLaDemande->col[0];
    QStringList boules; //=pLaDemande->lst_boules[0];

    //---------------------
    QStringList cri_msg;

    cri_msg <<"z1%2=0"<<"z1<"+QString::number((pLaDemande->ref->limites[0].max)/2);

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
    st_cri1_1= cri_msg.at(col);



    st_cri1 =
            "select tb3.id as id, tb5.id as pid,"
            "tb3.jour_tirage as J,"
            "substr(tb3.date_tirage,-2,2)||'/'||substr(tb3.date_tirage,6,2)||'/'||substr(tb3.date_tirage,1,4) as D,"
            "tb5.tip as C,"
            "tb3.b1 as b1, tb3.b2 as b2,tb3.b3 as b3,tb3.b4 as b4,tb3.b5 as b5,"
            "tb3.e1 as e1,"
            "tb2.N as N from tirages as tb3,"
            "analyses as tb4, lstcombi as tb5 "
            "inner join"
            "("
            "select *  from "
            "("
            "select tb1.*, count(tb2.B) as N from tirages as tb1 "
            "left join"
            "("
            "select id as B from Bnrz where (z1 not null and ("
            +st_cri1_1+
            "))"
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
            ") as tb2 "
            "on ("
            "(tb3.id = tb2.id + "
            +QString::number(dst)
            +") "
             "and"
             "(tb4.id = tb3.id)"
             "and"
             "(tb4.id_poids = tb5.id)"
             ")"
            ;

    st_cri2 ="(tb2.N ="
            +QString::number(lgn)+")";

    QString msg_req[3];
    QString st_cri_all = "";
    QString jonc = " and ";

    for(int i = 0; i< 3; i++)
    {
        boules=pLaDemande->lst_boules[i];
        msg_req[i] = "";

        // creation du message en fonction d'origine
        if(boules.size())
            msg_req[i]= GEN_Where_3(loop[i],table[i],inc1[i],cond1[i],boules,inc2[i],cond2[i]);

        if (msg_req[i] != "")
        {
            st_cri_all = st_cri_all + msg_req[i] + jonc;
        }

    }
    // on retire le dernier and
    if(st_cri_all != "")
    {
        st_cri_all.remove(st_cri_all.size()-jonc.size(),jonc.size());
        st_cri_all = " and " + st_cri_all;
    }


    // Des boules a rechercher ?
    //if(boules.size())
    //    st_cri2 = st_cri2 + " and " + GEN_Where_3(5,"tb2.b",true,"=",boules,false,"or");

    if(!dst){
        if((st_cri2 !="") || (st_cri_all !=""))
            st_criWhere = " where ("+ st_cri2 + st_cri_all +")";
    }

    st_sqlR = st_cri1
            + st_criWhere +
            ";";

#ifndef QT_NO_DEBUG
    qDebug() << st_sqlR;
#endif

    return(st_sqlR);
}

QString SyntheseDetails::DoSqlMsgRef_Tb3(QStringList &boules, int dst)
{
#if 0
    select tb3.id as Tid1, tb5.id as Pid1,
            tb2.id as Tid2, tb2.id_poids as Pid2,
            tb3.jour_tirage as J,
            substr(tb3.date_tirage,-2,2)||'/'||substr(tb3.date_tirage,6,2)||'/'||substr(tb3.date_tirage,1,4) as D,
            tb5.tip as C,
            tb3.b1 as b1, tb3.b2 as b2,tb3.b3 as b3,tb3.b4 as b4,tb3.b5 as b5,
            tb3.e1 as e1
            from tirages as tb3, analyses as tb4, lstcombi as tb5
            inner join
            (
                select tirages.*,  analyses.id_poids from tirages,analyses
                where
                (
                    tirages.id=analyses.id
            and
            analyses.id_poids = 120
            )
                ) as tb2
            on (
                (tb3.id = tb2.id + -1)
                and
                (tb4.id = tb3.id)
                and
                (tb4.id_poids = tb5.id)
                )
            where
            (
                J like '%sam%'
                );

    --Fin requete tb3
            -----------------
            "select tb3.id as Tid1, tb5.id as Pid1, "
            "tb2.id as Tid2, tb2.id_poids as Pid2, "
            "tb3.jour_tirage as J, "
            "substr(tb3.date_tirage,-2,2)||'/'||substr(tb3.date_tirage,6,2)||'/'||substr(tb3.date_tirage,1,4) as D, "
            "tb5.tip as C, "
            "tb3.b1 as b1, tb3.b2 as b2,tb3.b3 as b3,tb3.b4 as b4,tb3.b5 as b5, "
            "tb3.e1 as e1 "
            "from tirages as tb3, analyses as tb4, lstcombi as tb5 "
            "inner join "
            "( "
            "select tirages.*, analyses.id_poids from tirages,analyses "
            "where "
            "( "
            "(tirages.id=analyses.id) "
            "and "
            "(analyses.id_poids = 121) "
            "and "
            "(tirages.e1 = 1) "
            ") "
            ") as tb2 "
            "on ( "
            "(tb3.id = tb2.id + -1) "
            "and "
            "(tb4.id = tb3.id) "
            "and "
            "(tb4.id_poids = tb5.id) "
            ") "
            "; "
            " "
            "--Fin requete tb3 "
            " "
        #endif
            QString st_sqlR = ""; // Requete complete
    QString st_cri1 = ""; // ensemble de depart
    QString st_cri2 = ""; // contrainte sur z1 (boule)
    QString st_cri3 = ""; // contrainte sur z2 (etoiles)
    QString st_criWhere = ""; // contrainte communes



    int val = pLaDemande->lgn[0];
    int col=pLaDemande->col[0];


    st_cri1 =             "select tb3.id as Tid1, tb5.id as Pid1, "
                          "tb3.jour_tirage as J, "
                          "substr(tb3.date_tirage,-2,2)||'/'||substr(tb3.date_tirage,6,2)||'/'||substr(tb3.date_tirage,1,4) as D, "
                          "tb5.tip as C, "
                          "tb3.b1 as b1, tb3.b2 as b2,tb3.b3 as b3,tb3.b4 as b4,tb3.b5 as b5, "
                          "tb3.e1 as e1 "
                          "from tirages as tb3, analyses as tb4, lstcombi as tb5 "
                          "inner join "
                          "( "
                          "select tirages.*,  analyses.id_poids from tirages,analyses "
                          "where "
                          "( "
                          "tirages.id=analyses.id "
                          "and "
                          "analyses.id_poids = "
            +QString::number(val)+
            ") "
            ") as tb2 "
            "on ( "
            "(tb3.id = tb2.id + "
            +QString::number(dst)+
            ") "
            "and "
            "(tb4.id = tb3.id) "
            "and "
            "(tb4.id_poids = tb5.id) "
            ")";

    // Des boules a rechercher ?
    if(boules.size())
        st_cri2 = GEN_Where_3(5,"tb3.b",true,"=",boules,false,"or");

    if(col>2)
        st_cri2= st_cri2
                +" and "
                 "( "
                 "J like '%"
                +pLaDemande->stc[0]+
                "%' "
                ") ";

    if((st_cri2 !="") || (st_cri3 !=""))
        st_criWhere = " where ("+ st_cri2 + st_cri3 +")";


    st_sqlR = st_cri1
            + st_criWhere +
            ";";

#ifndef QT_NO_DEBUG
    qDebug() << st_sqlR;
#endif

    return(st_sqlR);
}

void SyntheseDetails::slot_FermeLaRecherche(int index)
{
    delete(this);
}

void SyntheseDetails::slot_ClickSurOnglet(int index)
{
#ifndef QT_NO_DEBUG
    qDebug() << "index click ->" << index;
#endif

    for(int i = 0; i<4;i++)
    {
        gtab_splitter_2[i]->setCurrentIndex(index);
    }

    //gtab_tirages->setCurrentIndex(index);
}

void SyntheseDetails::slot_ZoomTirages(const QModelIndex & index)
{
    void *pSource = index.internalPointer();
    int onglet = onglets->currentIndex();

    int ligne = index.row();
    int col = index.column();
    int val = index.data().toInt();
    const QAbstractItemModel * pModel = index.model();
    QVariant vCol = pModel->headerData(col,Qt::Horizontal);
    QString headName = vCol.toString();

    stCurDemande *etude = new stCurDemande;
    int zn = 0;


    // Prendre la config actuelle puis la modifier
    *etude = *pLaDemande;

    //etude->cur_dst += dst[onglet];
    // Nouvelle reference d'ensemble de depart ?
    QString *newBaseRef = new QString;
    *newBaseRef = DoSqlMsgRefGenerique (dst[onglet]);
    etude->st_baseDef = newBaseRef;

    void * pqtv_1 = qtv_local[onglet][0]->model()->index(0,0).internalPointer();
    void * pqtv_2 = qtv_local[onglet][1]->model()->index(0,0).internalPointer();
    void * pqtv_3 = qtv_local[onglet][2]->model()->index(0,0).internalPointer();


    if( pSource == pqtv_1 )
    {
        zn = 0;
    }

    if( pSource ==  pqtv_2)
    {
        zn = 1;
    }

    if( pSource ==  pqtv_3)
    {
        zn = 2;
    }

    etude->lgn[zn] = ligne;
    etude->col[zn] = col;
    etude->stc[zn] = headName;
    etude->val[zn] = val;

    // On regarde si la boule est deja dans les selectionnee
    QStringList lst_tmp = pLaDemande->lst_boules[zn];
    QString boule;
    boule = index.model()->index(index.row(),0).data().toString();

    if(!lst_tmp.contains(boule))
        lst_tmp << boule;

    etude->lst_boules[zn]=lst_tmp;

    // Nouvelle de fenetre de detail de cette boule
    SyntheseDetails *unDetail = new SyntheseDetails(etude,pEcran,gMemoTab);
}
