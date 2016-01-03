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
                   stTiragesDef *pDef):p_stRefTirages(stFiltreTirages),p_conf(pDef),sCouv(zn,pDef)
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
#define XLenTir 500
#define YLenTir 200

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
    int zn = 0;
    if(RechercheCouverture(zn))
    {
        ;// Assocaition recherche avec qttable !
    }

    return returnLayout;
}

bool RefEtude::RechercheCouverture(int zn)
{
    QSqlQuery query;
    bool status = false;
    bool uneCouvDePlus = false;
    QList<sCouv *> lstCouv;

    status = query.exec(p_stRefTirages);

    // Se positionner au debut des tirages du jeu
    if (status)
        status = query.last();

    if(status){
        // Premiere recherche de couverture
        sCouv *tmpCouv = new sCouv(zn,p_conf);
        lstCouv.append(tmpCouv);

        do
        {
            if(uneCouvDePlus)
            {
                sCouv *tmpCouv = new sCouv(zn,p_conf);
                lstCouv.append(tmpCouv);
            }

            QSqlRecord rec  = query.record();
            int bId = 0;
            uneCouvDePlus = AnalysePourCouverture(rec,&bId,zn,lstCouv.last());

            // Une couverture c'est produite en cours d'analyse du tirage
            if(uneCouvDePlus)
            {
                sCouv *tmpCouv = new sCouv(zn,p_conf);
                lstCouv.append(tmpCouv);

              uneCouvDePlus = AnalysePourCouverture(rec,&bId,zn,lstCouv.last());
            }


        }while(query.previous());
    }

    return status;
}

bool RefEtude::AnalysePourCouverture(QSqlRecord unTirage, int *bIdStart, int zn,sCouv *memo)
{
    static bool depart=true;
    static int total = 0;
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
        if((total+1) <= memo->p_conf->limites[zn].max)
        {
            // non

            // Boule deja connue ?
            if(memo->p_val[b_val-1][0])
            {
                // recuperer position
                int pos = memo->p_val[b_val-1][0];

                // incrementer la decouverte
                memo->p_val[pos][2]++;


            }
            else
            {
                // memoriser a l'indice de la boule sa position
                memo->p_val[b_val-1][0]=total;
                // sauver sa position
                memo->p_val[total][1]=b_val;
                memo->p_val[total][2]=1;
                total++;
            }
        }
        else
        {
            // oui
            retVal = true;

            total=0;
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

    return returnLayout;
}
