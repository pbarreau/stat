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
            qtv_tmp->setColumnWidth(i,45);
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
    RemplirTableauEcart(zn,tmpStdItem);

    return returnLayout;
}

void RefEtude::RemplirTableauEcart(int zn, QStandardItemModel *sim_tmp)
{
    // Montrer les boules "non" encore sorties
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

    // Montrer boules pas encore sorties.
    MontrerBoulesNonSorties(zn,sim_tmp,curCouv,memo_last_boule);

    for(int i=1;i<=nbBoule;i++){
        // Remplir Sous Fen les ecarts
        DistributionSortieDeBoule_v2(zn, i,sim_tmp);

        // Montrer les valeurs probable
        CouvMontrerProbable_v2(i,sim_tmp);
    }



}

void RefEtude::MontrerBoulesNonSorties(int zn, QStandardItemModel *sim_tmp, sCouv *curCouv,int memo_last_boule)
{
    int nbBoule = p_conf->limites[zn].max;

    for(int i = 0; i< nbBoule ;i++)
    {
        int maVal_0 = curCouv->p_val[i][0];

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

void RefEtude::DistributionSortieDeBoule_v2(int zn,int boule, QStandardItemModel *modele)
{
    bool status = false;

    QSqlQuery query;
    QString msg;


    QSqlQuery selection;
    QString msg1;
    QStringList tmp;
    double EcartMoyen = 0.0;
    int nbTirages=0;
    int calcul;
    int lgndeb=0, lgnfin=0;
    int nbTotCouv = 0, EcartMax=0, EcartCourant = 0, EcartPrecedent=0;
    int a_loop = 0;
    stTiragesDef *pRef = p_conf;

    QString useBase = p_stRefTirages;
    useBase = useBase.remove(";");
    // recuperation du nombre de tirage total
    msg= "select count (*)  from ("+useBase+") as r1;";
    status = query.exec(msg);
    query.first();
    nbTirages = query.value(0).toInt();
    query.finish();


    // creation d'une table temporaire
    msg = "DROP table IF EXISTS tmp_couv";
    status = query.exec(msg);
#ifndef QT_NO_DEBUG
    if(!status){
        qDebug() << "ERROR:" << query.executedQuery()  << "-" << query.lastError().text();
    }
#endif

    msg =  "create table tmp_couv (id INTEGER PRIMARY KEY, depart int, fin int, taille int)";
    status = query.exec(msg);
#ifndef QT_NO_DEBUG
    if(!status){
        qDebug() << "ERROR:" << query.executedQuery()  << "-" << query.lastError().text();
    }
#endif



    // requete a effectuer
    msg = "insert into tmp_couv (id, depart, fin, taille) values (:id, :depart, :fin, :taille)";
    query.prepare(msg);

    // Recuperation des lignes ayant la boule
    msg = req_msg(useBase,zn,boule,pRef);
    status = selection.exec(msg);


    //Partir de la fin des tirages trouves
    selection.last(); // derniere ligne ayant le numero
    if(selection.isValid()){
        lgndeb = nbTirages;
        nbTirages = 0; //calcul des intervales
        a_loop = 1;
        do
        {
            QSqlRecord rec  = selection.record();
            calcul = rec.value(0).toInt();
            lgnfin = selection.value(0).toInt();

            query.bindValue(":depart", lgndeb);
            query.bindValue(":fin", lgnfin);
            query.bindValue(":taille", lgndeb-lgnfin+1);
            // Mettre dans la base
            status = query.exec();

            nbTirages += (lgndeb-lgnfin);
            lgndeb = lgnfin-1;
            a_loop++;
        }while(selection.previous());
        //selection.finish();
        //query.finish();
    }

    // Rajouter une ligne pour ecart le plus recent
    lgnfin = 1;
    query.bindValue(":depart", lgndeb);
    query.bindValue(":fin", lgnfin);
    query.bindValue(":taille", lgndeb-lgnfin+1);
    // Mettre dans la base
    status = query.exec();
    nbTirages += (lgndeb-lgnfin);

    // calcul des ecarts pour la boule
    msg = "select count (*)  from tmp_couv";
    status = query.exec(msg);
    query.first();
    nbTotCouv = query.value(0).toInt();

    // Moyenne
    if(a_loop>0)
        EcartMoyen = double(nbTirages)/a_loop;

    // recherche l'ecart le plus grand
    msg = "select max(taille)  from tmp_couv";
    status = query.exec(msg);
    query.first();
    EcartMax = query.value(0).toInt();

    //recherche de l'ecart courant et suivant
    msg = "select taille  from tmp_couv";
    status = query.exec(msg);
    query.last();
    EcartCourant = query.value(0).toInt();
    query.previous();
    EcartPrecedent = query.value(0).toInt();

    QStandardItem *item1 = new QStandardItem;
    item1->setData(EcartCourant,Qt::DisplayRole);
    modele->setItem(boule-1,1,item1);

    QStandardItem *item2 = new QStandardItem( QString::number(222));
    item2->setData(EcartPrecedent,Qt::DisplayRole);
    modele->setItem(boule-1,2,item2);

    QStandardItem *item3 = new QStandardItem( );
    QString valEM = QString::number(EcartMoyen,'g',2);
    //item3->setData(EcartMoyen,Qt::DisplayRole);
    item3->setData(valEM.toDouble(),Qt::DisplayRole);
    modele->setItem(boule-1,3,item3);


    QStandardItem *item4 = new QStandardItem( QString::number(222));
    item4->setData(EcartMax,Qt::DisplayRole);
    modele->setItem(boule-1,4,item4);
}

void RefEtude::CouvMontrerProbable_v2(int i,QStandardItemModel *dest)
{

    double rayon = 1.5;
    const QColor fond[4]={QColor(255,156,86,190),
                          QColor(140,255,124,190),
                          QColor(70,160,220,190),
                          QColor(255,40,180,190)
                         };

    QStandardItem *cellule[4];

    for(int j =1; j<= 4 ;j++)
    {
        cellule[j-1] = dest->item(i-1,j);
    }
    int Ec = cellule[0]->data(Qt::DisplayRole).toInt();
    int Ep = cellule[1]->data(Qt::DisplayRole).toInt();
    int Em = cellule[2]->data(Qt::DisplayRole).toInt();
    double EM = cellule[3]->data(Qt::DisplayRole).toDouble();

    int d1 = abs(Ep-Ec);
    int d2 = abs(Em-Ec);
    int d3 = abs(EM-Ec);
    int d4 = abs(Em-Ep);
    if(d1 <= rayon)
    {
        cellule[1]->setBackground(QBrush(fond[0]));
    }
    if(d2 <= rayon)
    {
        cellule[2]->setBackground(QBrush(fond[1]));
    }
    if(d3 <= rayon)
    {
        cellule[3]->setBackground(QBrush(fond[2]));
    }
    if(d4 <= rayon)
    {
        cellule[0]->setBackground(QBrush(fond[3]));
    }

}
