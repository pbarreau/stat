#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QtGui>
#include <QWidget>
#include <QString>
#include <QGridLayout>
#include <QTabWidget>

#include <QSqlQuery>
#include <QSqlRecord>

#include <QSqlQueryModel>
#include <QTableView>
#include <QHeaderView>

#include "refetude.h"
#include "SyntheseDetails.h"

//#define XLenTir 500
//#define YLenTir 200

sCouv::sCouv(int zn, stTiragesDef *pDef):p_conf(pDef),p_deb(-1),p_fin(-1)
{
    int maxItems = p_conf->limites[zn].max;
    p_val = new int *[maxItems];

    for(int i=0;i<maxItems;i++)
    {
        p_val[i]=new int[3];

        if(p_val[i])
            memset(p_val[i],0,3*sizeof(int));
    }
}

sCouv::~sCouv()
{
    for(int i=0;i<3;i++)
    {
        delete p_val[i];
    }
    delete p_val;
}

RefEtude::RefEtude(QString stFiltreTirages,int zn,
                   stTiragesDef *pDef):p_stRefTirages(stFiltreTirages),p_conf(pDef)
{
    QWidget *uneReponse = CreationOnglets();

#ifndef QT_NO_DEBUG
    qDebug()<< stFiltreTirages;
#endif

    uneReponse->show();
}

QWidget *RefEtude::CreationOnglets()
{
    QWidget * qw_retour = new QWidget;
    QGridLayout *frm_tmp = new QGridLayout;
    QTabWidget *tab_Top = new QTabWidget;

    QString ongNames[]={"Tirages","Couvertures","Ecarts"};
    int maxOnglets = sizeof(ongNames)/sizeof(QString);

    QWidget **wid_ForTop = new QWidget*[maxOnglets];
    QGridLayout **dsgOnglet = new QGridLayout * [maxOnglets];

    QGridLayout * (RefEtude::*ptrFunc[])()={
            &RefEtude::MonLayout_TabTirages,
            &RefEtude::MonLayout_TabCouvertures,
            &RefEtude::MonLayout_TabEcarts};

    for(int id_Onglet = 0; id_Onglet<maxOnglets; id_Onglet++)
    {
        wid_ForTop[id_Onglet]= new QWidget;
        tab_Top->addTab(wid_ForTop[id_Onglet],ongNames[id_Onglet]);

        dsgOnglet[id_Onglet]=(this->*ptrFunc[id_Onglet])();
        wid_ForTop[id_Onglet]->setLayout(dsgOnglet[id_Onglet]);
    }

    frm_tmp->addWidget(tab_Top);
    qw_retour->setLayout(frm_tmp);

    return qw_retour;
}

QGridLayout *RefEtude::MonLayout_TabTirages()
{
    QGridLayout *lay_return = new QGridLayout;

    QSqlQueryModel *sqm_tmp = new QSqlQueryModel;
    QTableView *qtv_tmp = new QTableView;

    sqm_tmp->setQuery(p_stRefTirages);
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

    lay_return->addWidget(qtv_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);

    return lay_return;
}

QGridLayout *RefEtude::MonLayout_TabCouvertures()
{
    QGridLayout *returnLayout = new QGridLayout;
    //QList<sCouv *> *lstCouv = p_MaListe;

    int zn = 0;
    if(RechercheCouverture(&p_MaListe, zn))
    {
        ;// Association recherche avec qttable !
        QTableView *tbv_tmp1 = TablePourLstcouv(&p_MaListe, zn);
        QTableView *tbv_tmp2 = DetailsLstcouv(zn);
        returnLayout->addWidget(tbv_tmp1,0,0);
        returnLayout->addWidget(tbv_tmp2,0,1);
    }

    return returnLayout;
}

QTableView * RefEtude::DetailsLstcouv(int zn)
{
    QTableView *qtv_tmp = new QTableView;

    int nb_lgn = p_conf->limites[zn].max;
    QStandardItemModel * tmpStdItem =  new QStandardItemModel(nb_lgn,3);

    QString colName[]={"A","B","T"};
    qtv_tmp->setModel(tmpStdItem);

    int nbcol = sizeof(colName)/sizeof(QString);

    for(int i=0;i<nbcol;i++)
    {
        tmpStdItem->setHeaderData(i,Qt::Horizontal,colName[i]);
        // Remplir resultat
        for(int pos=0;pos <nb_lgn;pos++)
        {
            QStandardItem *item = new QStandardItem();

            if(i == 0){
                item->setData(pos+1,Qt::DisplayRole);
            }
            tmpStdItem->setItem(pos,i,item);
            qtv_tmp->setColumnWidth(i,35);
        }
    }

    qtv_tmp->setSortingEnabled(true);
    qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
    qtv_tmp->setAlternatingRowColors(true);
    qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Bloquer largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->hide();

    // Taille tableau
    qtv_tmp->setFixedSize(XLenTir/3,YLenTir);
    qtv_tmp->setEnabled(false);

    p_tbv_2 = qtv_tmp;
    p_qsim_2 = tmpStdItem;
    return qtv_tmp;
}

QTableView * RefEtude::TablePourLstcouv(QList<sCouv *> *lstCouv,int zn)
{
    QTableView *qtv_tmp = new QTableView;

    int nb_colH = lstCouv->size();
    int nb_lgn = p_conf->limites[zn].max;
    QStandardItemModel * tmpStdItem =  new QStandardItemModel(nb_lgn,nb_colH);
    qtv_tmp->setModel(tmpStdItem);

    for(int i=nb_colH;i>0;i--)
    {
        QString colName = "C" +QString::number(i);
        int curcol = nb_colH - i;
        tmpStdItem->setHeaderData(curcol,Qt::Horizontal,colName);
        // Remplir resultat
        for(int pos=0;pos <nb_lgn;pos++)
        {
            QStandardItem *item = new QStandardItem();
            int b_val = lstCouv->at(i-1)->p_val[pos][1];
            item->setData(b_val,Qt::DisplayRole);
            tmpStdItem->setItem(pos,curcol,item);
            qtv_tmp->setColumnWidth(curcol,35);
        }
    }

    qtv_tmp->setSortingEnabled(false);
    qtv_tmp->setAlternatingRowColors(true);
    qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Bloquer largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    // Taille tableau
    qtv_tmp->setFixedSize((XLenTir)*2/3,YLenTir);

    // simple click dans fenetre  pour selectionner boule
    connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
             this, SLOT(slot_Couverture( QModelIndex) ) );

    p_tbv_1 = qtv_tmp;


    return qtv_tmp;
}

void RefEtude::slot_Couverture(const QModelIndex & index)
{
    int col = index.column();
    static int previous = -1;

    QItemSelectionModel *selectionModel = p_tbv_1->selectionModel();
    if(selectionModel->selectedIndexes().size() == 0)
    {
        p_tbv_2->setEnabled(false);
        previous = -1;
    }
    else
    {
       if(previous != col)
       {
           previous = col;
       }
       else
       {
           return;
       }
        int nbItem = p_MaListe.size();
        sCouv *pUndetails = p_MaListe.at(nbItem-1-col);

        p_tbv_2->sortByColumn(0,Qt::AscendingOrder);

        int zn = 0;
        int nbBoules = p_conf->limites[zn].max;
        for(int i=0;i<nbBoules;i++)
        {
            QStandardItem * item_1 = p_qsim_2->item(i,1);
            QStandardItem * item_2 = p_qsim_2->item(i,2);

            item_1->setData(pUndetails->p_val[i][1],Qt::DisplayRole);
            item_2->setData(pUndetails->p_val[i][2],Qt::DisplayRole);

            p_qsim_2->setItem(i,1,item_1);
            p_qsim_2->setItem(i,2,item_2);
        }
        p_tbv_2->setEnabled(true);
    }

}

bool RefEtude::RechercheCouverture(QList<sCouv *> *lstCouv,int zn)
{
    QSqlQuery query;
    bool status = false;
    bool uneCouvDePlus = false;


    status = query.exec(p_stRefTirages);

    // Se positionner au debut des tirages du jeu
    if (status)
        status = query.last();

    if(status){
        // Premiere recherche de couverture
        sCouv *tmpCouv = new sCouv(zn,p_conf);
        lstCouv->append(tmpCouv);

        do
        {
            if(uneCouvDePlus)
            {
                sCouv *tmpCouv = new sCouv(zn,p_conf);
                lstCouv->append(tmpCouv);
            }

            QSqlRecord rec  = query.record();
            int bId = 0;
            uneCouvDePlus = AnalysePourCouverture(rec,&bId,zn,lstCouv->last());

            // Une couverture c'est produite en cours d'analyse du tirage
            if(uneCouvDePlus)
            {
                sCouv *tmpCouv = new sCouv(zn,p_conf);
                lstCouv->append(tmpCouv);

                uneCouvDePlus = AnalysePourCouverture(rec,&bId,zn,lstCouv->last());
            }


        }while(query.previous());
    }

    return status;
}

bool RefEtude::AnalysePourCouverture(QSqlRecord unTirage, int *bIdStart, int zn,sCouv *memo)
{
    static bool depart=true;
    static int total = 1;
    bool retVal = false;

    int id = unTirage.value(0).toInt();

    if(depart)
    {
        memo->p_deb = id;
        depart = false;
    }

    // parcour des boules
    int b_val = 0;
    for(int bId=(*bIdStart);bId<memo->p_conf->nbElmZone[zn];bId++)
    {
        // recuperer une boule
        b_val = unTirage.value(5+bId).toInt();

        // une couverture complete ?
        if(total <= memo->p_conf->limites[zn].max)
        {
            // non

            // Boule deja connue ?
            if(memo->p_val[b_val-1][0])
            {
                // recuperer position
                int pos = memo->p_val[b_val-1][0]-1;

                // incrementer la decouverte
                memo->p_val[pos][2]++;


            }
            else
            {
                // memoriser a l'indice de la boule sa position
                memo->p_val[b_val-1][0]=total;
                // sauver sa position
                memo->p_val[total-1][1]=b_val;
                memo->p_val[total-1][2]=1;

                // Marquer comme vue
                //memo->p_val[total-1][3]=1;
                total++;
            }
        }
        else
        {
            // oui
            retVal = true;

            total=1;
            depart=true;
            memo->p_fin=id;

            // Quelle position dans le tirage termine la couverture
            if(bId<memo->p_conf->nbElmZone[zn]-1)
            {
                *bIdStart = bId;
                break;
            }
        }
    }

    return retVal;
}

QGridLayout *RefEtude::MonLayout_TabEcarts()
{
    QGridLayout *returnLayout = new QGridLayout;

    QTableView *qtv_tmp = new QTableView;

    int zn = 0;
    int nb_lgn = p_conf->limites[zn].max;
    QStandardItemModel * tmpStdItem =  new QStandardItemModel(nb_lgn,5);

    QString colName[]={"B","Ec","Ep","Em","EM"};
    qtv_tmp->setModel(tmpStdItem);

    int nbcol = sizeof(colName)/sizeof(QString);
    for(int i=0;i<nbcol;i++)
    {
        tmpStdItem->setHeaderData(i,Qt::Horizontal,colName[i]);
        // Remplir resultat
        for(int pos=0;pos <nb_lgn;pos++)
        {
            QStandardItem *item = new QStandardItem();

            if(i == 0){
                item->setData(pos+1,Qt::DisplayRole);
            }
            tmpStdItem->setItem(pos,i,item);
            qtv_tmp->setColumnWidth(i,35);
        }
    }

    qtv_tmp->setSortingEnabled(true);
    qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
    qtv_tmp->setAlternatingRowColors(true);
    qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Bloquer largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->hide();

    // Taille tableau
    qtv_tmp->setFixedSize(XLenTir,YLenTir);
    returnLayout->addWidget(qtv_tmp,0,0);

    // Remplir le tableau
    RemplirTableauEcart(tmpStdItem);

    return returnLayout;
}

void RefEtude::RemplirTableauEcart(QStandardItemModel *sim_tmp)
{
  // Montrer les boules "non" encore sorties
    int zn = 0;
    int totCouv = p_MaListe.size();
    int nbBoule = p_conf->limites[zn].max;

    if(!totCouv)
        return;

    sCouv *curCouv = p_MaListe.last();
    sCouv *PrvCouv = NULL;
    int memo_last_boule = 0;

    if(totCouv >1){
        PrvCouv = p_MaListe.at(totCouv-2);
        memo_last_boule = PrvCouv->p_val[nbBoule-1][1];
    }

    for(int i = 0; i< nbBoule ;i++)
    {
        int maVal_0 = curCouv->p_val[i][0];
        int maVal_1 = curCouv->p_val[i][1];
        int maVal_2 = curCouv->p_val[i][2];
        if (!maVal_0)
        {
            QStandardItem *item1 = sim_tmp->item(i);
            QBrush macouleur;
            QColor unecouleur;

            if(i!=memo_last_boule-1)
            {
                macouleur.setColor(Qt::green);
                macouleur.setStyle(Qt::SolidPattern);
            }
            else
            {
                // http://stackoverflow.com/questions/8571059/how-to-generate-new-qcolors-that-are-different
                // http://goffgrafix.com/pantone-rgb-800.php
                unecouleur.setRgb(255,127,30);
                macouleur.setColor(unecouleur);
                macouleur.setStyle(Qt::SolidPattern);
            }
            item1->setBackground(macouleur);
        }
    }

}
