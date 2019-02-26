#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QApplication>
#include <QMessageBox>

#include <QtGui>
#include <QSqlRecord>
#include <QMenu>
#include <QToolTip>

#include <QHeaderView>
#include <QVBoxLayout>
#include <QSortFilterProxyModel>
#include <QSplitter>
#include <QPushButton>
#include <QDataWidgetMapper>
#include <QSqlRelationalDelegate>
#include <QStackedWidget>
#include <QWidgetAction>

#include <QSqlDatabase>

#include "refetude.h"
#include "SyntheseGenerale.h"
#include "SyntheseDetails.h"
#include "showstepper.h"

#include "compter_groupes.h"
#include "compter_zones.h"
#include "compter_combinaisons.h"
#include "lescomptages.h"
#include "compter.h"

#include "mainwindow.h"

#include "cbarycentre.h"
#include "sqlqmdetails.h"
#include "bvisuresume.h"

//extern MainWindow w;
#ifdef USE_SG_CODE
QString GetBoulesOfTirage(int tir);
#endif

QGridLayout *SyntheseGenerale::MonLayout_TabAuguste(int col, int lgn)
{
    QGridLayout *lay_return = new QGridLayout;

    QTableView *tbv_tmp0 = tbForPrincipeAuguste(col, lgn);


    lay_return->addWidget(tbv_tmp0,0,0,1,3,Qt::AlignLeft|Qt::AlignTop);

    return lay_return;
}

QTableView *SyntheseGenerale::tbForPrincipeAuguste(int nbcol, int nblgn)
{
    QTableView *qtv_tmp = new QTableView;
    int zn = 0;


    QStandardItemModel * tmpStdItem =  new QStandardItemModel(1,nbcol);
    ///p_qsim_3 = tmpStdItem;

    qtv_tmp->setModel(tmpStdItem);

    for(int i=0;i<nbcol;i++)
    {
        tmpStdItem->setHeaderData(i,Qt::Horizontal,QString::number(i));

        for(int j = 0; i< nblgn;j++)
        {
            QStandardItem *item = new QStandardItem();
            tmpStdItem->setItem(j,i,item);
        }
        qtv_tmp->setColumnWidth(i,LCELL);
    }

    qtv_tmp->setSortingEnabled(false);
    //qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
    qtv_tmp->setAlternatingRowColors(true);
    qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Bloquer largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ///qtv_tmp->verticalHeader()->hide();

    // Taille tableau
    qtv_tmp->setFixedSize(XLenTir,CHauteur2);

    // double click dans fenetre  pour afficher details boule
    ///connect( qtv_tmp, SIGNAL(doubleClicked(QModelIndex)) ,
    ///         this, SLOT(slot_Type_G( QModelIndex) ) );

    ///p_tbv_3 = qtv_tmp;
    return qtv_tmp;
}

QGridLayout *SyntheseGenerale::GetDisposition(void)
{
    return disposition;
}
QTableView *SyntheseGenerale::GetListeTirages(void)
{
    return  tbv_LesTirages;
}
RefEtude *SyntheseGenerale::GetTabEcarts(void)
{
    return tabEcarts;
}

void SyntheseGenerale::GetInfoTableau(int onglet, QTableView **pTbl, QSqlQueryModel **pSqm, QSortFilterProxyModel **pSfpm)
{
    // se mettre sur le bon onglet
    ptabComptage->setCurrentIndex(onglet);

    // renvoyer les infos
    *pTbl = tbv_bloc1_1;
    *pSqm = sqm_bloc1_1;
    *pSfpm = mysortModel;
}
void SyntheseGenerale::slot_ShowTotalBoule(const QModelIndex &index)
{
    QAbstractItemModel *mon_model = tbv_LesTirages->model();

    int ecart = 0;

    //recuperer la colonne
    int col_id = index.column();

    // se mettre sur le bon onglet pour montrer le total
    ptabComptage->setCurrentIndex(0);

    int val = index.model()->index(index.row(),0).data().toInt();
    mysortModel->sort(0);
    tbv_bloc1_1->scrollTo(mysortModel->index(val-1,0));



    if((col_id == 1) || (col_id ==2)){
        // Montrer le tirage precedent contenant la boule
        ecart = index.model()->index(index.row(),1).data().toInt();

        if(col_id == 2)
            ecart = ecart + index.model()->index(index.row(),col_id).data().toInt();
        // se Positionner sur un element visible de la table (colonne 2 des tirages: la date)
        QModelIndex item1 = tbv_LesTirages->model()->index(ecart,2);
        tbv_LesTirages->scrollTo(item1,QAbstractItemView::PositionAtCenter);
    }
}

void SyntheseGenerale::slot_ShowBouleForNewDesign(const QModelIndex & index)
{
    /// Pas de test sur la colonne ici
    /// se mettre sur le bon onglet
    ptabComptage->setCurrentIndex(0);///(boules)

    // recuperer la valeur a la colone de la table
    int val = index.model()->index(index.row(),0).data().toInt();
    mysortModel->sort(0);
    tbv_bloc1_1->scrollTo(mysortModel->index(val-1,0));
}

void SyntheseGenerale::slot_ShowBoule(const QModelIndex & index)
{
    int val = 0;



    // recuperer la valeur de la colonne
    int col = index.column();
    QTableView *view = qobject_cast<QTableView *>(sender());

    if(col > 4 && col <= 4 + pMaConf->nbElmZone[0])
    {
        // se mettre sur le bon onglet
        ptabComptage->setCurrentIndex(0);

        // recuperer la valeur a la colone de la table
        val = index.model()->index(index.row(),index.column()).data().toInt();
        mysortModel->sort(0);
        tbv_bloc1_1->scrollTo(mysortModel->index(val-1,0));
    }

    col = 0;
}

SyntheseGenerale::SyntheseGenerale(GererBase *pLaBase, QTabWidget *ptabSynt,int zn, stTiragesDef *pConf, QMdiArea *visuel)
{
    origine=pLaBase;
    QString chk = origine->get_IdCnx();
    db_0 = QSqlDatabase::database(chk);

    disposition = new QGridLayout;

    pEcran = visuel;
    pMaConf = pConf;
    pMaConf->db_cnx=chk;

    ptabTop = ptabSynt;
    //curzn = zn;
    int nb_zones = pConf->nb_zone;
    maRef = new  QStringList* [nb_zones] ;


    QString *st_tmp = new QString;
    QString *st_tmp2 = new QString;
    st_bdTirages = new QString;
    //*st_bdTirages = OrganiseChampsDesTirages("tirages", pConf);
    *st_bdTirages = C_TousLesTirages;

    *st_tmp = *st_bdTirages;
    *st_tmp2 = *st_bdTirages;

    st_JourTirageDef = new QString;
    *st_JourTirageDef = CompteJourTirage(db_0.connectionName());

    uneDemande.st_Ensemble_1 = st_bdTirages;
    uneDemande.st_LDT_Depart = st_tmp2;
    uneDemande.st_LDT_Filtre = st_tmp;
    uneDemande.st_jourDef = st_JourTirageDef;
    uneDemande.ref = pConf;
    DoTirages();
    DoComptageTotal();
#if 0
    /// greffon pour calculer barycentre des tirages
    stNeedsOfBary param;
    param.db = bdd->getMyDb();
    param.ncx = bdd->getMyDb().connectionName();
    param.tbl_in="B_fdj";
    CBaryCentre c(param);
#endif


    disposition->addWidget(ptabTop,1,0,1,2,Qt::AlignLeft|Qt::AlignTop);
}

#ifdef USE_SG_CODE

void SyntheseGenerale::slot_MaFonctionDeCalcul(const QModelIndex &my_index, int cid)
{
    QSqlQuery requete;
    bool status = false;
    QString msg = "";
    int zone = 0;

    int total = pMaConf->limites[zone].max;
    int possible = total/2;

    int val = my_index.model()->index(my_index.row(),0).data().toInt();
    QString table = "stepper_"+QString::number(val);

    msg = "drop table  "+ table  + ";";
    status = requete.exec(msg);

    //cid : couverture id
    //tid : tirage id
    //y : nombre de fois sortie depuis tirage de selection
    //b ; numero de boule
    //c : couleur crayon a mettre pour cette boule
    //bgc : couleur du fond
    msg = "create table if not exists "+ table  + "(id integer primary key, cid int, tid int, y int, b int, c int, bgc int);";
    status = requete.exec(msg);
    if(!status){
        return;
    }

    stMyHeadedList linksInfo[possible];
    memset(linksInfo,-1,sizeof(stMyHeadedList)*possible);

    stMyLinkedList links[total+1];
    memset(links,0,sizeof(stMyLinkedList)*(total+1));

    // Preparation des liens
    int i = 1;
    linksInfo[0].depart= 1;
    linksInfo[0].total = total;

    for(i=1; i< total;i++)
    {
        links[i].n = i+1;
        links[i].p = i-1;
    }
    links[i].p = i-1;

    msg = "select * from tirages where(id between 0 and " + QString::number(val) +");";

    int boule = 0;
    int tid = 0;
    status = requete.exec(msg);
    status = requete.last();
    int maVerif = 0;
    if(requete.isValid())
    {
        int nb = pMaConf->nbElmZone[zone];
        do{
            QSqlRecord record = requete.record();
            tid = record.value(0).toInt();
            maVerif++;
            for (i=1;i<=nb;i++)
            {
                QString champ = pMaConf->nomZone[zone]+QString::number(i);
                boule = record.value(champ).toInt();

                linksInfo[links[boule].y].total--;///nbtot[links[boule].y]--;

                // Enlever le lien
                if(links[boule].p <=0)
                {
                    links[links[boule].n].p=-1;
                    if(links[boule].n){

                        linksInfo[links[boule].y].depart=links[boule].n;///start[links[boule].y]= links[boule].n;
                    }
                    else
                    {


                        linksInfo[links[boule].y].depart=-1;///start[links[boule].y]=-1;
                        linksInfo[links[boule].y].total=0; ///nbtot[links[boule].y]=0;
                    }
                }
                else{
                    links[links[boule].p].n = links[boule].n;
                    links[links[boule].n].p = links[boule].p;
                }

                // indiquer la nouvelle position
                int b= boule;
                do
                {
                    b = links[b].n;
                    if(links[b].x)
                        links[b].x--;
                }while(links[b].n);
                links[boule].n = 0;

                // la boule change de niveau
                links[boule].y++;

                // Positionne la boule dans liste
                if(linksInfo[links[boule].y].depart == -1)///if(start[links[boule].y] == -1)
                {
                    linksInfo[links[boule].y].depart = boule; ///start[links[boule].y] = boule;
                    linksInfo[links[boule].y].total = 1;///nbtot[links[boule].y] = 1;

                    links[boule].x = 1;
                    links[boule].p = -1;
                }
                else
                {
                    linksInfo[links[boule].y].total++;///nbtot[links[boule].y]++;

                    // Mettre la boule a la fin de la liste chainee
                    // dernier element
                    int cur = linksInfo[links[boule].y].depart;///start[links[boule].y];
                    int last = 0;
                    do{
                        last = links[cur].n;
                        if(last)
                            cur = last;
                    }while(last);

                    // Faire add node
                    links[cur].n = boule;
                    links[boule].p = cur;
                    links[boule].x = (links[cur].x)+1;
                }

                if(linksInfo[0].total <=0)///if(nbtot[0]<=0)
                {
                    cid++;
                    break;/// ou continue ?
                }
            }

            // On a parcouru toutes les boules de ce tirage
            // on va lire les listes chainees
            MemoriserProgression(table,&linksInfo[0],links, val,possible, cid, tid);
        }while(requete.previous());

        // Presentation des resultats
        PresenterResultat(0,val);

    }
}

void SyntheseGenerale::PresenterResultat(int cid,int tid)
{
    ShowStepper *unReponse = new ShowStepper(cid,tid);

}

void SyntheseGenerale::MemoriserProgression(QString table,stMyHeadedList *h,stMyLinkedList *l, int start, int y, int cid, int tid)
{
    QSqlQuery sql;
    bool sta = false;

    QString msg = "insert into "+
            table + " (id,cid,tid,y,b,c,bgc) values (null,:cid, :tid, :y, :b, :c, :bgc);";
    sta = sql.prepare(msg);
    ///qDebug() << "Prepare :" << sta;
    sql.bindValue(":cid", cid);
    sql.bindValue(":tid", tid);
    sql.bindValue(":c", 0);
    sql.bindValue(":bgc", 0);

    /// Mise en place des valeurs
    for (int i = 0; i< y;i++)
    {
        if(h[i].total>0)
        {
            sql.bindValue(":y", i);

            // Parcourir la liste chainee
            int data = h[i].depart;
            do
            {
                sql.bindValue(":b", data);
                sta = sql.exec();
                data = l[data].n;
            }while(data && (sta == true));
        }
    }

    // Colorier
    MettreCouleur(start, tid);
}

QString GetBoulesOfTirage(int tir)
{
    bool sta = false;
    QSqlQuery sql;
    QString msg = "";

    msg="select b1,b2,b3,b4,b5 from reftirages where(id =" +
            QString::number(tir) +");";

    // lancer la requete
#ifndef QT_NO_DEBUG
    qDebug() << msg;
#endif
    sta = sql.exec(msg);

    sta = sql.first();
    if(sql.isValid())
    {
        msg= "";
        QSqlRecord resultat = sql.record();
        for(int i =0; i<5; i++)
        {
            msg= msg+ resultat.value(i).toString()+",";
        }
        msg.remove(msg.length()-1,1);
    }

    return msg;
}

void SyntheseGenerale::MettreCouleur(int start, int cur)
{
    bool sta = false;
    QSqlQuery sql;
    QString msg = "";
    QString val = "";
    QString table = "stepper_"+QString::number(start);

    //Mise en place tirage precedent
    if(start - cur > 0)
    {
        val = GetBoulesOfTirage(cur + 1);
        // preparer la requete mise a jour
        msg = "update " + table + " set c=3 where (" + table
                + ".tid = " + QString::number(cur)
                + " and (" + table +".b in ("+val+")));";

        // lancer la requete
#ifndef QT_NO_DEBUG
        qDebug() << msg;
#endif
        sta = sql.exec(msg);
    }

    // Couleur pour tirage courant
    val = GetBoulesOfTirage(cur);
    // preparer la requete mise a jour
    msg = "update " + table + " set c=1 where (" + table
            + ".tid = " + QString::number(cur)
            + " and (" + table +".b in ("+val+")));";

    // lancer la requete
#ifndef QT_NO_DEBUG
    qDebug() << msg;
#endif
    sta = sql.exec(msg);


    // Mise en place indicateur tirage suivant
    if(cur-1>0)
    {

        val = GetBoulesOfTirage(cur - 1);
        // preparer la requete mise a jour
        msg = "update " + table + " set c=2 where (" + table
                + ".tid = " + QString::number(cur)
                + " and (" + table +".b in ("+val+")));";

        // lancer la requete
#ifndef QT_NO_DEBUG
        qDebug() << msg;
#endif
        sta = sql.exec(msg);

    }

}
#endif // code a mettre dans stepper

void SyntheseGenerale::slot_ccmr_TbvLesTirages(QPoint pos)
{
    QTableView *view = qobject_cast<QTableView *>(sender());
    QModelIndex index = view->indexAt(pos);

    QMenu *MonMenu=new QMenu(pEcran);
    QString msg = "Recherche";
    ShowStepper *UnDetail = new ShowStepper(pMaConf);
    // QModelIndex index = tbv_LesTirages->indexAt(pos);

    MonTraitement = new B_ActFrMdlIndex(index,msg);
    MonMenu->addAction(MonTraitement);

    connect(MonTraitement, SIGNAL(sig_SelectionTirage(const QModelIndex,int)),
            UnDetail, SLOT(slot_MaFonctionDeCalcul(const QModelIndex,int)) );

    MonMenu->exec(tbv_LesTirages->viewport()->mapToGlobal(pos));

#if 0
    int val = 0;



    // recuperer la valeur de la colonne
    int col = index.column();

    if(col > 4 && col <= 4 + pMaConf->nbElmZone[0])
    {
        // se mettre sur le bon onglet
        ptabComptage->setCurrentIndex(0);

        // recuperer la valeur a la colone de la table
        val = index.model()->index(index.row(),index.column()).data().toInt();
        mysortModel->sort(0);
        tbv_bloc1_1->scrollTo(mysortModel->index(val-1,0));
    }

    int v_id = G_sim_MesPossibles->index(index.row(),index.column()).data().toInt();
    QVariant  hdata =  index.model()->headerData(index.column(),Qt::Horizontal);
    QString msg = hdata.toString();
    msg = msg.split("b").at(1);
    int b_id = msg.toInt();
    DB_tirages->PopulateCellMenu(b_id, v_id, 0, &configJeu, menu, this);
    menu->popup(G_tbv_MesPossibles->viewport()->mapToGlobal(pos));
#endif

}

#if 1
void SyntheseGenerale::DoTirages(void)
{
    RefEtude *unTest = new RefEtude(origine,*st_bdTirages,0,pMaConf,pEcran,ptabTop);
    QWidget *uneReponse = unTest->CreationOnglets();


    tabEcarts = unTest;

    tbv_LesTirages = unTest->GetListeTirages();
    tbv_LesEcarts = unTest->GetLesEcarts();

    disposition->addWidget(uneReponse,0,0,Qt::AlignLeft|Qt::AlignTop);
#if 0
    /// ------------------------------
    QString st_table = REF_BASE;
    cCompterGroupes *test = new cCompterGroupes(st_table);
    connect(test,SIGNAL(sig_ComptageReady(B_RequeteFromTbv)),
            pEcran->parent(),SLOT(slot_NOUVEAU_Ensemble(B_RequeteFromTbv)));
    connect( tbv_LesTirages, SIGNAL( clicked(QModelIndex)) ,
             test, SLOT( slot_DecodeTirage( QModelIndex) ) );

    cCompterZoneElmts *test2 = new cCompterZoneElmts(st_table,NULL);
    connect(test2,SIGNAL(sig_ComptageReady(B_RequeteFromTbv)),
            pEcran->parent(),SLOT(slot_NOUVEAU_Ensemble(B_RequeteFromTbv)));

    cCompterCombinaisons *test3 = new cCompterCombinaisons(st_table);
    connect(test3,SIGNAL(sig_ComptageReady(B_RequeteFromTbv)),
            pEcran->parent(),SLOT(slot_NOUVEAU_Ensemble(B_RequeteFromTbv)));

    cLesComptages *tous = new cLesComptages(st_table);

    /// -------------------------------------
#endif

    connect( tbv_LesTirages, SIGNAL( clicked(QModelIndex)) ,
             pEcran->parent(), SLOT( slot_MontreTirageDansGraph( QModelIndex) ) );


    connect( tbv_LesTirages, SIGNAL( clicked(QModelIndex)) ,
             this, SLOT( slot_ShowBoule( QModelIndex) ) );

    tbv_LesTirages->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tbv_LesTirages, SIGNAL(customContextMenuRequested(QPoint)),this,
            SLOT(slot_ccmr_TbvLesTirages(QPoint)));


#if 0
    // Double click sur ecart et localisation dans base Tirages
    connect( tbv_LesEcarts, SIGNAL( doubleClicked(QModelIndex)) ,
             pEcran->parent(), SLOT(slot_MontreLeTirage( QModelIndex) ) );
#endif

    connect( tbv_LesEcarts, SIGNAL( clicked(QModelIndex)) ,
             this, SLOT( slot_ShowTotalBoule( QModelIndex) ) );


}

#else
void SyntheseGenerale::DoTirages(void)
{
    sqm_LesTirages = new QSqlQueryModel;
    QTableView *qtv_tmp = new QTableView;


    QString st_sqlReq = *st_bdTirages;

#ifndef QT_NO_DEBUG
    qDebug()<< st_sqlReq;
#endif

    sqm_LesTirages->setQuery(st_sqlReq);

    qtv_tmp->setAlternatingRowColors(true);
    qtv_tmp->setStyleSheet("QTableView {selection-background-color: red;}");
    qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtv_tmp->setFixedSize(475,245);

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
    disposition->addLayout(vb_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);


    tbv_LesTirages=qtv_tmp;
    // Simple click dans sous fenetre base
    connect( qtv_tmp, SIGNAL( clicked(QModelIndex)) ,
             pEcran->parent(), SLOT( slot_MontreTirageDansGraph( QModelIndex) ) );

    // Simple click dans sous fenetre base
    connect( qtv_tmp, SIGNAL( clicked(QModelIndex)) ,
             pEcran->parent(), SLOT( slot_MontreTirageAnalyse( QModelIndex) ) );

#if 0
    // double click dans fenetre  pour afficher details boule
    connect( tbv_bloc1, SIGNAL(doubleClicked(QModelIndex)) ,
             this, SLOT(slot_MontreLesTirages( QModelIndex) ) );
#endif


}
#endif

void SyntheseGenerale::DoComptageTotal(void)
{

    // Onglet pere
    QTabWidget *tab_Top = new QTabWidget;

    QString stNames[]={"b","e","c","g","ba"};
    int items = sizeof(stNames)/sizeof(QString);

    QWidget **wid_ForTop = new QWidget*[items];
    QGridLayout *design_onglet[items];

    ptabComptage = tab_Top;
    // Tableau de pointeur de fonction
    QGridLayout *(SyntheseGenerale::*ptrFunc[])(int)=
    {
            &SyntheseGenerale::MonLayout_SyntheseTotalBoules,
            &SyntheseGenerale::MonLayout_SyntheseTotalEtoiles,
            &SyntheseGenerale::MonLayout_SyntheseTotalRepartitions,
            &SyntheseGenerale::MonLayout_SyntheseTotalGroupement,
            &SyntheseGenerale::MonLayout_SyntheseTotalBarycentre
};

    stTiragesDef *pConf = pMaConf;

    for(int i =0; i<items;i++)
    {
        wid_ForTop[i]=new QWidget;
        tab_Top->addTab(wid_ForTop[i],tr(stNames[i].toUtf8()));

        // Recherche a une distance de 0 sans critere de filtre
        design_onglet[i] = (this->*ptrFunc[i])(0);
        wid_ForTop[i]->setLayout(design_onglet[i]);
    }

    QFormLayout *mainLayout = new QFormLayout;
    selection = new LabelClickable;

    selection->setText(CTXT_SELECTION);
    connect( selection, SIGNAL( clicked(QString)) ,
             this, SLOT( slot_RazSelection(QString) ) );

    mainLayout->addWidget(selection);
    mainLayout->addWidget(tab_Top);
    disposition->addLayout(mainLayout,0,1,Qt::AlignLeft|Qt::AlignTop);


}

void SyntheseGenerale::slot_RazSelection(QString)
{
    tbv_bloc1_1->selectionModel()->clearSelection();
    tbv_bloc1_2->selectionModel()->clearSelection();
    tbv_bloc1_2->selectionModel()->clearSelection();
    tbv_bloc2->selectionModel()->clearSelection();

    for(int i =0; i< 4;i++)
    {
        uneDemande.selection[i].clear();
    }

    selection->setText(CTXT_SELECTION);
}

QGridLayout * SyntheseGenerale::MonLayout_SyntheseTotalEtoiles(int dst)
{
    QGridLayout *lay_return = new QGridLayout;

    sqm_bloc1_2 = new QSqlQueryModel;

    int zn = 1;
    QTableView *qtv_tmp = new QTableView;
    QString qtv_name = QString::fromLatin1(C_TBL_6) + "_z"+QString::number(zn+1);
    qtv_tmp->setObjectName(qtv_name);

    //tbv_bloc1_2 = new QTableView;
    //qtv_tmp=tbv_bloc1_2;
    tbv_bloc1_2 = qtv_tmp;

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

    sqm_bloc1_2->setQuery(st_msg1,db_0);
    // Renommer le nom des colonnes
    QSqlQueryModel *sqm_tmp=sqm_bloc1_2;
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
    qtv_tmp->setStyleSheet("QTableView {selection-background-color: #939BFF;}");


    qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtv_tmp->setFixedSize((nbcol*LCELL)+20,CHauteur1);

    QSortFilterProxyModel *m=new QSortFilterProxyModel();
    m->setDynamicSortFilter(true);
    m->setSourceModel(sqm_bloc1_2);
    qtv_tmp->setModel(m);
    qtv_tmp->verticalHeader()->hide();
    for(int j=0;j<2;j++)
        qtv_tmp->setColumnWidth(j,30);
    for(int j=2;j<=sqm_bloc1_2->columnCount();j++)
        qtv_tmp->setColumnWidth(j,LCELL);


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
    connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
             this, SLOT(slot_ClicDeSelectionTableau( QModelIndex) ) );



    // double click dans fenetre  pour afficher details boule
    connect( tbv_bloc1_2, SIGNAL(doubleClicked(QModelIndex)) ,
             this, SLOT(slot_MontreLesTirages( QModelIndex) ) );

    return lay_return;

}

QGridLayout * SyntheseGenerale::MonLayout_SyntheseTotalRepartitions(int dst)
{
    QGridLayout *lay_return = new QGridLayout;

    int zn = 0;
    QTableView *qtv_tmp = new QTableView;
    QString qtv_name = QString::fromLatin1(C_TBL_7) + "_z"+QString::number(zn+1);
    qtv_tmp->setObjectName(qtv_name);

    QSqlQueryModel *sqm_tmp = new QSqlQueryModel;

    //tbv_bloc1_3 = new QTableView;
    //qtv_tmp=tbv_bloc1_3;
    tbv_bloc1_3 =qtv_tmp;

    sqm_bloc1_3 = new QSqlQueryModel;
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
            "select id,tip from lstCombi_z1"
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

    sqm_tmp->setQuery(st_msg1,db_0);
    int nbcol = sqm_tmp->columnCount();
    for(int i = 0; i<nbcol;i++)
    {
        QString headName = sqm_tmp->headerData(i,Qt::Horizontal).toString();
        if(headName.size()>2)
        {
            sqm_tmp->setHeaderData(i,Qt::Horizontal,headName.left(2));
        }
    }

    qtv_tmp->hideColumn(0);
    qtv_tmp->setSortingEnabled(true);
    qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
    qtv_tmp->setAlternatingRowColors(true);
    qtv_tmp->setStyleSheet("QTableView {selection-background-color: #939BFF;}");
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);


    qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
    //qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    //qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtv_tmp->setFixedSize(250,CHauteur1);

    QSortFilterProxyModel *m=new QSortFilterProxyModel();
    m->setDynamicSortFilter(true);
    m->setSourceModel(sqm_tmp);
    qtv_tmp->setModel(m);
    qtv_tmp->verticalHeader()->hide();
    qtv_tmp->setColumnWidth(0,30);
    qtv_tmp->setColumnWidth(1,70);
    for(int j=2;j<=sqm_tmp->columnCount();j++)
        qtv_tmp->setColumnWidth(j,LCELL);


    // Ne pas modifier largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);


    // Filtre
    QFormLayout *FiltreLayout = new QFormLayout;
    FiltreCombinaisons *fltComb_tmp = new FiltreCombinaisons();
    QList<qint32> colid;
    colid << 1;
    fltComb_tmp->setFiltreConfig(sqm_tmp,qtv_tmp,colid);

    //fltComb_tmp->setFiltreConfig(sqm_tmp,qtv_tmp,1);
    FiltreLayout->addRow("&Filtre Repartition", fltComb_tmp);

    lay_return->addLayout(FiltreLayout,0,0,Qt::AlignLeft|Qt::AlignTop);
    lay_return->addWidget(qtv_tmp,1,0,Qt::AlignLeft|Qt::AlignTop);


    // Mettre le dernier tirage en evidence
    QSqlQuery selection(db_0);
    bool status = false;

    st_msg1 = "select analyses.id, analyses.fk_idCombi_z1 from analyses limit 1;";
    status = selection.exec(st_msg1);
    status = selection.first();
    if(selection.isValid())
    {
        int value = selection.value(1).toInt();
        //tv_r1->setItemDelegate(new MaQtvDelegation(NULL,value-1,1));

        QAbstractItemModel *mon_model = qtv_tmp->model();
        //QStandardItemModel *dest= (QStandardItemModel*) mon_model;
        QModelIndex mdi_item1 = mon_model->index(0,0);

        if (mdi_item1.isValid()){
            //mdi_item1 = mdi_item1.model()->index(value-1,1);
            mdi_item1 = mon_model->index(value-1,1);
            QPersistentModelIndex depart(mdi_item1);

            qtv_tmp->selectionModel()->setCurrentIndex(mdi_item1, QItemSelectionModel::NoUpdate);
            qtv_tmp->scrollTo(mdi_item1);
            qtv_tmp->setItemDelegate(new MaQtvDelegation(depart));
        }
    }


    // simple click dans fenetre  pour selectionner boule
#if 0
    connect( tbv_bloc1_3, SIGNAL(clicked(QModelIndex)) ,
             this, SLOT(slot_Select_C( QModelIndex) ) );
#endif
    connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
             this, SLOT(slot_ClicDeSelectionTableau( QModelIndex) ) );



    // double click dans fenetre  pour afficher details boule
    connect( tbv_bloc1_3, SIGNAL(doubleClicked(QModelIndex)) ,
             this, SLOT(slot_MontreLesTirages( QModelIndex) ) );

    return lay_return;
}

#if 0
QSqlQuery query(db_1);
bool isOk = true;

QString filterDays = CreerCritereJours(db_1.connectionName(),tbl_in);

#ifndef QT_NO_DEBUG
qDebug() << "filterDays:"<<filterDays;
#endif

/// prendre dans les tirages les jours, les boules de la zone
QString str_data = "select id,J,b1,b2,b3,b4,b5 from ("
        +tbl_in+")";

#ifndef QT_NO_DEBUG
qDebug() << "str_data:"<<str_data;
#endif

if((isOk = query.exec(str_data))){
    //Verifier si on a des donnees
    query.first();
    if((isOk=query.isValid())){
        /// Poursuivre les calculs
        /// 1 : Transformation de lignes vers 1 colonne
        QString tbl_refBoules = QStringLiteral("B_elm");
        str_data = "select c1.id as Id, c1.J as J, r1.z1 as b  FROM ("
                +tbl_refBoules
                +") as r1, ("
                +str_data
                +" ) as c1 where (b=b1 or b=b2 or b=b3 or b=b4 or b=b5) order by c1.id ";
#ifndef QT_NO_DEBUG
        qDebug() << "str_data:"<<str_data;
#endif
        /// 2: Calcul du barycentre si calcul toltal de chaque boule
        ///  de la base complete
        QString tbl_totalBoule = "r_B_fdj_0_elm_z1";
        if(isTableTotalBoulleReady(tbl_totalBoule)){
            str_data = "Select c1.id as Id, sum(c2.t)/5 as BC, J From ("
                    +str_data
                    +") as c1, ("
                    +tbl_totalBoule
                    +") as c2 WHERE (c1.b = c2.b) GROUP by c1.id";

#ifndef QT_NO_DEBUG
            qDebug() << "str_data:"<<str_data;
#endif
            /// 3: Creation d'une table regroupant les barycentres
            QString str_tblData = "";
            QString str_tblName = "r_"+tbl_in+"_0_brc_z1";
            str_tblData = "select BC, count(BC) as T, "
                    +filterDays
                    +QString(",NULL as P, NULL as F from (")
                    + str_data
                    + ") as c1 group by BC order by T desc";
            str_tblData = "create table if not exists "
                    +str_tblName
                    +" as "
                    +str_tblData;
#ifndef QT_NO_DEBUG
            qDebug() << "str_tblData:"<<str_tblData;
#endif
            if((isOk = query.exec(str_tblData))){
                /// mettre dans la table analyse le barycentre de chaque tirage
                QString str_tblAnalyse = "";
                if(tbl_in=="E1"){
                    str_tblAnalyse = "U_E1_ana_z1";
                }
                else{
                    str_tblAnalyse = "B_ana_z1";
                }

                if((isOk = mettreBarycentre(str_tblAnalyse, str_data))){
                    /// indiquer le dernier barycentre des tirages fdj
                    isOk = repereDernier(str_tblName);
                }
            }
        }
        else{
            /// Appeller la fonction des sommes de chaque boule
            ;
        }
    }
    else{
        /// On a aucune valeur pour faire les calculs
        ;
    }
}

if(!isOk){
    /// analyser erreur
    QString err_msg = query.lastError().text();
    //un message d'information
    QMessageBox::critical(NULL, "Barycentre", err_msg,QMessageBox::Yes);
#ifndef QT_NO_DEBUG
    qDebug() << err_msg;
#endif
    QApplication::quit();
}

#endif

bool SyntheseGenerale::isTableTotalBoulleReady(QString tbl_total)
{
    bool isOk = true;

    return(isOk);
}

bool SyntheseGenerale::mettreBarycentre(QString tbl_dst, QString src_data)
{
    bool isOK = true;
    QSqlQuery query(db_0);
    QString msg = "";

    /// 1 : Renommer la table resultat
    msg = "ALTER TABLE "+tbl_dst+" RENAME TO old_"+tbl_dst+";";

    if((isOK=query.exec(msg))){
        stJoinArgs param;
        param.arg1 = "tbLeft.*, tbRight.BC as Bc";
        param.arg2 = QString("old_")+tbl_dst;
        param.arg3 = src_data;
        param.arg4 = "tbLeft.id = tbRight.id";

        msg = DB_Tools::leftJoin(param);
#ifndef QT_NO_DEBUG
        qDebug() << msg;
#endif
        //if not exists
        msg = "create table  " + tbl_dst + " as " + msg;

        if((isOK = query.exec(msg))){
            /// Supprimer table old
            msg = "drop table if exists old_"+tbl_dst+";";
            isOK = query.exec(msg);
        }
    }
    return isOK;
}


bool SyntheseGenerale::CreateTable_Barycentre(QString tbl_dest, QString tbl_poids_boules)
{
    QSqlQuery query(db_0);
    bool isOk = true;
    //view_total_boule
    QString filterDays = *st_JourTirageDef;

#ifndef QT_NO_DEBUG
    qDebug()<< filterDays;
#endif

    /// prendre dans les tirages les jours, les boules de la zone
    QString tbl_in = REF_BASE;
    QString str_data = "select id,J,b1,b2,b3,b4,b5 from ("
            +tbl_in+")";

#ifndef QT_NO_DEBUG
    qDebug() << "str_data:"<<str_data;
#endif

    if((isOk = query.exec(str_data))){
        //Verifier si on a des donnees
        query.first();
        if((isOk=query.isValid())){
            /// Poursuivre les calculs
            /// 1 : Transformation de lignes vers 1 colonne
            QString tbl_refBoules = QStringLiteral("Bnrz");
            str_data = "select c1.id as Id, c1.J as J, r1.z1 as b  FROM ("
                    +tbl_refBoules
                    +") as r1, ("
                    +str_data
                    +" ) as c1 where (b=b1 or b=b2 or b=b3 or b=b4 or b=b5) order by c1.id ";
#ifndef QT_NO_DEBUG
            qDebug() << "str_data:"<<str_data;
#endif
            /// 2: Calcul du barycentre si calcul toltal de chaque boule
            ///  de la base complete
            QString tbl_totalBoule = tbl_poids_boules;
            if(isTableTotalBoulleReady(tbl_totalBoule)){
                str_data = "Select c1.id as Id, sum(c2.t)/5 as BC, J From ("
                        +str_data
                        +") as c1, ("
                        +tbl_totalBoule
                        +") as c2 WHERE (c1.b = c2.b) GROUP by c1.id";

#ifndef QT_NO_DEBUG
                qDebug() << "str_data:"<<str_data;
#endif
                /// 3: Creation d'une table regroupant les barycentres
                QString str_tblData = "";
                QString str_tblName = tbl_dest;
                str_tblData = "select BC, count(BC) as T, "
                        +filterDays
                        +QString(",NULL as P, NULL as F from (")
                        + str_data
                        + ") as c1 group by BC order by T desc";
                str_tblData = "create table if not exists "
                        +str_tblName
                        +" as "
                        +str_tblData;
#ifndef QT_NO_DEBUG
                qDebug() << "str_tblData:"<<str_tblData;
#endif
                if((isOk = query.exec(str_tblData))){
                    /// mettre dans la table analyse le barycentre de chaque tirage
                    QString str_tblAnalyse = "analyses";

                    if((isOk = mettreBarycentre(str_tblAnalyse, str_data))){
                        /// indiquer le dernier barycentre des tirages fdj
                        //isOk = repereDernier(str_tblName);
                    }
                }
            }
            else{
                /// Appeller la fonction des sommes de chaque boule
                ;
            }
        }
        else{
            /// On a aucune valeur pour faire les calculs
            ;
        }
    }

    if(!isOk){
        /// analyser erreur
        QString err_msg = query.lastError().text();
        //un message d'information
        QMessageBox::critical(NULL, "Barycentre", err_msg,QMessageBox::Yes);
#ifndef QT_NO_DEBUG
        qDebug() << err_msg;
#endif
        QApplication::quit();
    }
    return isOk;
}

QGridLayout * SyntheseGenerale::MonLayout_SyntheseTotalBarycentre(int dst)
{
    QGridLayout *lay_return = new QGridLayout;
    bool isOk = true;

    QString tbl_in = REF_BASE;
    QString str_tblName = "r_"+tbl_in+"_0_brc_z1";
    QString tbl_totalBoule = "view_total_boule";

    if((isOk = CreateTable_Barycentre(str_tblName,tbl_totalBoule))){
        QTableView *qtv_tmp = new QTableView;
        QSqlQueryModel *sqm_tmp =new QSqlQueryModel;
        QString msg = "select * from "+str_tblName+";";

        sqm_tmp->setQuery(msg,db_0);

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
        //qtv_tmp->setStyleSheet("QTableView {selection-background-color: rgba(100, 100, 100, 150);}");
        qtv_tmp->setStyleSheet("QTableView {selection-background-color: #939BFF;}");

        qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);
        qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
        qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
        qtv_tmp->setFixedSize((nbcol*LCELL)+20,CHauteur1);

        QSortFilterProxyModel *m=new QSortFilterProxyModel();
        m->setDynamicSortFilter(true);
        m->setSourceModel(sqm_tmp);
        qtv_tmp->setModel(m);

        qtv_tmp->verticalHeader()->hide();
        for(int j=0;j<2;j++)
            qtv_tmp->setColumnWidth(j,30);
        for(int j=2;j<=sqm_tmp->columnCount();j++)
            qtv_tmp->setColumnWidth(j,LCELL);


        // Ne pas modifier largeur des colonnes
        qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

        QVBoxLayout *vb_tmp = new QVBoxLayout;
        QLabel * lab_tmp = new QLabel;
        lab_tmp->setText("Barycentres");
        vb_tmp->addWidget(lab_tmp,0,Qt::AlignLeft|Qt::AlignTop);
        vb_tmp->addWidget(qtv_tmp,0,Qt::AlignLeft|Qt::AlignTop);

        lay_return->addLayout(vb_tmp,0,0);

    }



    return lay_return;
}

QTableView * SyntheseGenerale::ConstruireTbvAnalyseBoules(int zn, QString source, QString definition, QString key, QString tb_resultat)
{
    sqm_bloc1_1 = new QSqlQueryModel;

    QTableView *qtv_tmp = new QTableView;
    QString qtv_name = QString("new")+QString::fromLatin1(C_TBL_6) + "_z"+QString::number(zn+1);
    qtv_tmp->setObjectName(qtv_name);


    QSqlQuery query(db_0);
    bool isOk = true;
    QString st_msg1 = "";


    if((isOk = Boules_Details(zn,tb_resultat,definition,key,source))){
        st_msg1 = "select * from "+tb_resultat+";";
    }
    sqm_bloc1_1->setQuery(st_msg1,db_0);

    // Renommer le nom des colonnes

    st_sqlmqDetailsNeeds val;
    int b_min=-1;
    int b_max=-1;
    val.ori = this;
    val.cnx = db_0.connectionName();
    val.sql = st_msg1;
    val.wko = tb_resultat;
    val.view = qtv_tmp;
    val.b_max = &b_max;
    val.b_min = &b_min;
    sqlqmDetails *sqm_tmp= new sqlqmDetails(val,sqm_bloc1_1);

    mysortModel = qobject_cast<QSortFilterProxyModel *>( qtv_tmp->model());

    int nbcol = sqm_tmp->columnCount();

    qtv_tmp->setSortingEnabled(true);
    qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
    qtv_tmp->setAlternatingRowColors(true);
    //qtv_tmp->setStyleSheet("QTableView {selection-background-color: rgba(100, 100, 100, 150);}");
    qtv_tmp->setStyleSheet("QTableView {selection-background-color: #939BFF;}");

    qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtv_tmp->setFixedSize((nbcol*LCELL)+20,CHauteur1);


    qtv_tmp->verticalHeader()->hide();
    for(int j=0;j<2;j++)
        qtv_tmp->setColumnWidth(j,30);
    for(int j=2;j<=sqm_tmp->columnCount();j++)
        qtv_tmp->setColumnWidth(j,28);


    // Ne pas modifier largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    qtv_tmp->hideColumn(0);
    tbv_bloc1_1 = qtv_tmp;

    // simple click dans fenetre  pour selectionner boule
#if 0
    connect( tbv_bloc1_1, SIGNAL(clicked(QModelIndex)) ,
             this, SLOT(slot_Select_B( QModelIndex) ) );
#endif
    connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
             this, SLOT(slot_ClicDeSelectionTableau( QModelIndex) ) );


    qtv_tmp->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(qtv_tmp, SIGNAL(customContextMenuRequested(QPoint)),this,
            SLOT(slot_ccmr_tbForBaseEcart(QPoint)));

    // tbv_bloc1_1
    // double click dans fenetre  pour afficher details boule
    connect( qtv_tmp, SIGNAL(doubleClicked(QModelIndex)) ,
             this, SLOT(slot_MontreLesTirages( QModelIndex) ) );

    return qtv_tmp;
}

QTableView * SyntheseGenerale::ResumeTbvAnalyseBoules(int zn, QString tb_read)
{
    QTableView * qtv_tmp = new QTableView;
    QString tb_write = tb_read + QString("_rsm_z")+QString::number(zn+1);

    QSqlQuery query(db_0);
    QSqlQueryModel a;
    bool isOk = true;
    QString msg[]={
        {"SELECT name FROM sqlite_master "
         "WHERE type='table' AND name='"+tb_read+"';"},
        {"create table if not exists "
         + tb_write
         +" as "
         "select t1.C as C_id,"
         " row_number() over(order by t1.C DESC) as id,"
         "  NULL as C,"
         " count(t1.C) as T,"
         " group_concat(t1.B, ', ') as Boules "
         " from ("+tb_read+") as t1 GROUP by t1.C order by t1.C DESC"
        },
        {"select * from " + tb_write}
    };


    if((isOk = query.exec(msg[0])))
    {
        query.first();
        if(query.isValid())
        {
            /// La table existe faire le resume
            if((isOk = query.exec(msg[1]))){
                QSqlQueryModel *sqm_tmp = new QSqlQueryModel;
                sqm_tmp->setQuery(msg[2],db_0);

                QSortFilterProxyModel *m=new QSortFilterProxyModel();
                m->setDynamicSortFilter(true);
                m->setSourceModel(sqm_tmp);

                qtv_tmp->setModel(m);
                mettreEnConformiteVisuel(qtv_tmp);
            }
        }
        else{
            QMessageBox::critical(NULL,"Table",tb_read+QString(" absente\n"),QMessageBox::Ok);
        }
    }

    if(query.lastError().isValid()){
        DB_Tools::DisplayError("SyntheseGenerale::",&query," ResumeTbvAnalyseBoules ");
    }

    return qtv_tmp;
}

void SyntheseGenerale::mettreEnConformiteVisuel(QTableView *qtv_tmp)
{
    QSortFilterProxyModel *m = qobject_cast<QSortFilterProxyModel *>(qtv_tmp->model()) ;
    QSqlQueryModel *sqm_tmp = qobject_cast<QSqlQueryModel*>(m->sourceModel());

    prmBVisuResume a;
    a.cnx = db_0.connectionName();
    /// TBD comment remonter le nom de la table cree
    /// BDelegateCouleurFond::SauverTableauPriotiteCouleurs()
    a.cld = "pCouleurs_65";

    BVisuResume *color = new BVisuResume(a,qtv_tmp);
    qtv_tmp->setItemDelegate(color);

    qtv_tmp->setSortingEnabled(true);
    qtv_tmp->sortByColumn(1,Qt::AscendingOrder);
    qtv_tmp->setAlternatingRowColors(true);

    qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtv_tmp->setStyleSheet("QTableView {selection-background-color: #939BFF;}");

    qtv_tmp->verticalHeader()->hide();
    qtv_tmp->hideColumn(0);
    int nb_col = sqm_tmp->columnCount();
    for(int i = 0; i< nb_col-1;i++){
        qtv_tmp->setColumnWidth(i,28);
    }

    qtv_tmp->setFixedWidth((nb_col*LCELL)+150);
    // Ne pas modifier largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->horizontalHeader()->setSectionResizeMode(nb_col-1,QHeaderView::ResizeToContents);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

}

QGridLayout * SyntheseGenerale::MonLayout_SyntheseTotalBoules(int dst)
{
    QGridLayout *lay_return = new QGridLayout;

    int zn = 0;
    QString key = "z"+QString::number(zn+1);
    QString cible = "view_total_boule";

    QTableView * qtv_tmp_1 = ConstruireTbvAnalyseBoules(zn,"RefTirages","Bnrz",key,cible);
    QTableView * qtv_tmp_2 = ResumeTbvAnalyseBoules(zn,cible);

    QVBoxLayout *vb_tmp = new QVBoxLayout;
    QLabel * lab_tmp_1 = new QLabel;
    QLabel * lab_tmp_2 = new QLabel;

    lab_tmp_1->setText("Repartitions");
    vb_tmp->addWidget(lab_tmp_1,0,Qt::AlignLeft|Qt::AlignTop);
    vb_tmp->addWidget(qtv_tmp_1,0,Qt::AlignLeft|Qt::AlignTop);

    lab_tmp_2->setText("Selections Possible");
    vb_tmp->addWidget(lab_tmp_2,0,Qt::AlignLeft|Qt::AlignTop);
    vb_tmp->addWidget(qtv_tmp_2,0,Qt::AlignLeft|Qt::AlignTop);

    lay_return->addLayout(vb_tmp,0,0);


    return lay_return;

}

void SyntheseGenerale::slot_ccmr_tbForBaseEcart(QPoint pos)
{
    /// http://www.qtcentre.org/threads/7388-Checkboxes-in-menu-items
    /// https://stackoverflow.com/questions/2050462/prevent-a-qmenu-from-closing-when-one-of-its-qaction-is-triggered

    QTableView *view = qobject_cast<QTableView *>(sender());
    QModelIndex index = view->indexAt(pos);
    int col = view->columnAt(pos.x());

    if(col == 1)
    {
        QString tbl = view->objectName();

        int val = 0;
        if(index.model()->index(index.row(),1).data().canConvert(QMetaType::Int))
        {
            val =  index.model()->index(index.row(),1).data().toInt();
        }

        QMenu *MonMenu = new QMenu(pEcran);
        QMenu *subMenu= ContruireMenu(tbl,val);
        MonMenu->addMenu(subMenu);
        CompleteMenu(MonMenu, tbl, val);


        MonMenu->exec(view->viewport()->mapToGlobal(pos));
    }
}

void SyntheseGenerale::slot_wdaFilter(int val)
{
    //QWidgetAction *wdaFrom = qobject_cast<QWidgetAction *>(sender());
    QCheckBox *chkFrom = qobject_cast<QCheckBox *>(sender());

#ifndef QT_NO_DEBUG
    //qDebug() << "Boule :("<< wdaFrom->objectName()<<") check:"<< wdaFrom->isChecked();
    qDebug() << "Boule :("<< chkFrom->objectName()<<") check:"<< chkFrom->isChecked();
#endif
}

void SyntheseGenerale::CompleteMenu(QMenu *LeMenu,QString tbl, int clef)
{
    int col = 3;
    int niveau = 0;
    bool existe = false;
    existe = VerifierValeur(db_0, clef, tbl,col,&niveau);

    QCheckBox *chkb_1 = new QCheckBox;
    chkb_1->setText("Filtrer");
    QWidgetAction *chk_act_1 = new QWidgetAction(LeMenu);
    chk_act_1->setDefaultWidget(chkb_1);
    connect(chkb_1,SIGNAL(stateChanged(int)),this,SLOT(slot_wdaFilter(int)));

    if((!existe) || (!niveau))
    {
        chkb_1->setChecked(false);
    }
    else
    {
        chkb_1->setChecked(true);
    }

    LeMenu->addAction(chk_act_1);
}

QMenu *SyntheseGenerale::ContruireMenu(QString tbl, int val)
{
    QString msg2 = "Priorite";
    QMenu *menu =new QMenu(msg2, pEcran);
    //menu->setWindowFlags(Qt::Tool);
    //menu->setTitle(msg2);
    QActionGroup *grpPri = new  QActionGroup(menu);

    int col = 2;
    int niveau = 0;
    bool existe = false;
    existe = VerifierValeur(db_0, val, tbl, col, &niveau);



    for(int i =1; i<=5;i++)
    {
        QString name = QString::number(i);
        QAction *radio = new QAction(name,grpPri);
        name = QString::number(existe)+
                ":"+QString::number(niveau)+
                ":"+name+":"+QString::number(val)+
                ":"+tbl;
        radio->setObjectName(name);
        radio->setCheckable(true);
        menu->addAction(radio);
    }

    QAction *uneAction;
    if(niveau)
    {
        uneAction = qobject_cast<QAction *>(grpPri->children().at(niveau-1));
        uneAction->setChecked(true);
    }
    connect(grpPri,SIGNAL(triggered(QAction*)),this,SLOT(slot_ChoosePriority(QAction*)));
    return menu;
}


void SyntheseGenerale::slot_ChoosePriority(QAction *cmd)
{
    QSqlQuery query(db_0);
    QString msg = "";

    QString st_from = cmd->objectName();
    QStringList def = st_from.split(":");
    /// Verifier coherence des donnees
    /// pos 0: ligne trouvee dans table
    /// pos 1: ancie priorite
    /// pos 2: nvlle priorite
    /// pos 3: element selectionne
    /// pos 4:nom de table
    if(def.size()!=5)
        return;

    int trv = def[0].toInt();
    int v_1 = def[1].toInt();
    int v_2 = def[2].toInt();
    int elm = def[3].toInt();
    QString tbl = def[4];

    // faut il inserer une nouvelle ligne
    if(trv ==0)
    {
        msg = "insert into " + tbl + " (id, val, p) values(NULL,"
                +def[3]+","+ def[2]+");";

    }
    // Verifier si if faut supprimer la priorite
    if(v_1 == v_2)
    {
        msg = "delete from " + tbl + " " +
                "where (val="+def[3]+");";
        trv = 0;
    }

    // faut il une mise a jour ?
    if((v_1 != v_2)&& (trv!=0))
    {
        msg = "update " + tbl + " set p="+def[2]+" "+
                "where (val="+def[3]+");";

    }

    bool rep = query.exec(msg);

    if(!rep)
    {
        trv = false;
#ifndef QT_NO_DEBUG
        qDebug() << "select: " <<def[3]<<"->"<< query.lastError();
        qDebug() << "Bad code:\n"<<msg<<"\n-------";
#endif
    }
    else
    {
        trv = true;
#ifndef QT_NO_DEBUG
        qDebug() << "Fn :\n"<<msg<<"\n-------";
#endif

    }

    cmd->setChecked(true);
}

// Cette fonction retourne un pointeur sur un tableau de QStringList
// Ce tableau comporte 2 elements
// Element 0 liste des requetes construites
// Element 1 Liste des titres assosies a la requete
// En fonction de la zone a etudier les requetes sont adaptees
// pour integrer le nombre maxi de boules a prendre en compte
QStringList * LstCritereGroupement(int zn, stTiragesDef *pConf)
{
    QStringList *sl_filter = new QStringList [3];
    QString fields = "z"+QString::number(zn+1);

    int maxElems = pConf->limites[zn].max;
    int nbBoules = floor(maxElems/10)+1;


    // Parite & nb elment dans groupe
    sl_filter[0] <<fields+"%2=0"<<fields+"<"+QString::number(maxElems/2);
    sl_filter[1] << "P" << "G";
    sl_filter[2] << "Pair" << "< E/2";


    // Boule finissant par [0..9]
    for(int j=0;j<=9;j++)
    {
        sl_filter[0]<< fields+" like '%" + QString::number(j) + "'";
        sl_filter[1] << "F"+ QString::number(j);
        sl_filter[2] << "Finissant par: "+ QString::number(j);
    }

    // Nombre de 10zaine
    for(int j=0;j<nbBoules;j++)
    {
        sl_filter[0]<< fields+" >="+QString::number(10*j)+
                       " and "+fields+"<="+QString::number((10*j)+9);
        sl_filter[1] << "U"+ QString::number(j);
        sl_filter[2] << "Entre:"+ QString::number(j*10)+" et "+ QString::number(((j+1)*10)-1);
    }

    return sl_filter;
}


#if USE_repartition_bh
QGridLayout * SyntheseGenerale::MonLayout_SyntheseTotalGroupement(int fake)
{
    QGridLayout *lay_return = new QGridLayout;

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
    qtv_tmp->setStyleSheet("QTableView {selection-background-color: #939BFF;}");

    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtv_tmp->setAlternatingRowColors(true);
    qtv_tmp->setFixedSize(380,CHauteur1);

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
    lay_return->addLayout(vb_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);
    //disposition->addLayout(vb_tmp,2,0,Qt::AlignLeft|Qt::AlignTop);


    // simple click dans fenetre  pour selectionner boule
    connect( tbv_bloc2, SIGNAL(clicked(QModelIndex)) ,
             this, SLOT(slot_Select_G( QModelIndex) ) );

    // double click dans fenetre  pour afficher details boule
    connect( tbv_bloc2, SIGNAL(doubleClicked(QModelIndex)) ,
             this, SLOT(slot_MontreLesTirages( QModelIndex) ) );

    return (lay_return);

}
#else
QGridLayout * SyntheseGenerale::MonLayout_SyntheseTotalGroupement(int fake)
{
    QGridLayout *lay_return = new QGridLayout;

    int zone = 0;
    int maxElems = uneDemande.ref->limites[zone].max;
    //int nbBoules = floor(maxElems/10)+1;

    //QStringList *maRef[zone] = LstCritereGroupement(zone,uneDemande.ref);
    maRef[zone] = LstCritereGroupement(zone,uneDemande.ref);
    int nbCol = maRef[zone][0].size();
    int nbLgn = uneDemande.ref->nbElmZone[zone] + 1;

    QTableView *qtv_tmp = new QTableView;
    QString qtv_name = "";
    qtv_name = QString::fromLatin1(C_TBL_8) +
            QString::fromLatin1("_z")
            +QString::number(zone+1);
    qtv_tmp->setObjectName(qtv_name);

    //tbv_bloc2 = new QTableView;
    //qtv_tmp=tbv_bloc2;
    tbv_bloc2 = qtv_tmp;

    QStandardItemModel * tmpStdItem = NULL;
    QSqlQuery query(db_0) ;

    //Creer un tableau d'element standard
    if(nbCol)
    {
        tmpStdItem =  new QStandardItemModel(nbLgn,nbCol);
        qtv_tmp->setModel(tmpStdItem);

        QStringList tmp=maRef[zone][1];
        tmp.insert(0,"Nb");
        tmpStdItem->setHorizontalHeaderLabels(tmp);

        QStringList tooltips=maRef[zone][2];
        tooltips.insert(0,"Total");
        for(int pos=0;pos <=nbCol;pos++)
        {
            QStandardItem *item = tmpStdItem->horizontalHeaderItem(pos);
            item->setToolTip(tooltips.at(pos));
        }

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
        qtv_tmp->setSelectionMode(QAbstractItemView::MultiSelection);
        qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
        qtv_tmp->setStyleSheet("QTableView {selection-background-color: #939BFF;}");

        qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
        qtv_tmp->setAlternatingRowColors(true);
        qtv_tmp->setFixedSize(CLargeur1*1.8,CHauteur1);

        qtv_tmp->setSortingEnabled(true);
        qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
        qtv_tmp->verticalHeader()->hide();

        QHeaderView *htop = qtv_tmp->horizontalHeader();
        htop->setSectionResizeMode(QHeaderView::ResizeToContents);
        qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);


        QVBoxLayout *vb_tmp = new QVBoxLayout;
        QLabel * lab_tmp = new QLabel;
        lab_tmp->setText("Type Regroupement");
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
        QString msg1 = maRef[zone][0].at(i);
        QString sqlReq = "";
        sqlReq = sql_RegroupeSelonCritere(*(uneDemande.st_Ensemble_1),msg1);

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

    // simple click dans fenetre  pour selectionner boule

    qtv_tmp->setMouseTracking(true);
    connect(qtv_tmp,
            SIGNAL(entered(QModelIndex)),this,SLOT(slot_AideToolTip(QModelIndex)));

    connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
             this, SLOT(slot_ClicDeSelectionTableau( QModelIndex) ) );

    // double click dans fenetre  pour afficher details boule
    connect( qtv_tmp, SIGNAL(doubleClicked(QModelIndex)) ,
             this, SLOT(slot_MontreLesTirages( QModelIndex) ) );

    return (lay_return);

}
#endif
// ------------------------------
void SyntheseGenerale::slot_AideToolTip(const QModelIndex & index)
{
    QString msg="";
    const QAbstractItemModel * pModel = index.model();
    int col = index.column();

    QVariant vCol = pModel->headerData(col,Qt::Horizontal);
    QString headName = vCol.toString();

    if (col >=1)
    {
        QString s_nb = index.model()->index(index.row(),0).data().toString();
        QString s_va = index.model()->index(index.row(),col).data().toString();
        QString s_hd = headName;
        msg = msg + QString("%1 tirage(s) \nayant %2 boule(s)%3").arg(s_va).arg(s_nb).arg(s_hd);
    }

    if(msg.length())
        QToolTip::showText (QCursor::pos(), msg);
}

void SyntheseGenerale::DoBloc3(void)
{
#if 0
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
#endif
}

#if 0
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
#endif

void SyntheseGenerale::slot_ClicDeSelectionTableau(const QModelIndex &index)
{
    // L'onglet implique le tableau...
    //int origine = ptabComptage->currentIndex();
    int origine = 0;
    int totOngl = 0;
    int col = index.column();
    boolean noSelection = false;

    QTableView *view = qobject_cast<QTableView *>(sender());
    QStackedWidget *curOnglet = qobject_cast<QStackedWidget *>(view->parent()->parent());
    QItemSelectionModel *selectionModel = view->selectionModel();
    origine =curOnglet->currentIndex();
    totOngl = curOnglet->count();

    QString name1 = view->objectName();
    QString name2 = selectionModel->objectName();

    switch (origine) {
    case 0:
    case 1:
    case 3:
        if(col==0)
            noSelection = true;
        break;
    case 2:
        if(col<=1)
            noSelection = true;
        break;
    default:
        noSelection = true;
        break;
    }

    // verifier
    if(noSelection){
        // deselectionner l'element
        selectionModel->select(index, QItemSelectionModel::Deselect);

        return;
    }

    uneDemande.selection[origine] = selectionModel->selectedIndexes();

    // ne pas memoriser quand onglet des regroupements
    //if(origine<(ptabComptage->count()-1))
    if(origine < (totOngl-1))
        MemoriserChoixUtilisateur(index,origine,selectionModel,pMaConf,&uneDemande);

    ///Memoriser peut modifier contenu de uneDemande !!!!
    QString maselection = CreatreTitle(&uneDemande);
    selection->setText(maselection);


}

QString CreatreTitle(stCurDemande *pConf)
{
    QString titre = "";

    for (int i = 0; i< 4 ;i++)
    {
        QModelIndexList indexes = pConf->selection[i];
        if(indexes.size())
        {
            if (i<2)
                titre = titre + pConf->ref->nomZone[i];
            if(i==2)
                titre = titre + "c";
            if(i==3)
                titre = titre + "g";


            QModelIndex un_index;
            // Analyse de chaque indexe
            int compte = 0;
            foreach(un_index, indexes)
            {
                const QAbstractItemModel * pModel = un_index.model();
                int col = un_index.column();
                int lgn = un_index.row();
                int use = un_index.model()->index(lgn,0).data().toInt();
                int val = un_index.data().toInt();
                QVariant vCol = pModel->headerData(col,Qt::Horizontal);
                QString headName = vCol.toString();

                if(i==3)
                {
                    use = lgn;
                }

                compte++;
                if(compte == 1){
                    if(i<3){
                        titre = titre + " ("+headName+"):";
                    }
                }

                if(i<3){
                    titre = titre + QString::number(use)+",";
                }
                else
                {
                    titre = titre + " (" +headName+","+QString::number(use)+"),";
                }
            }
            titre.remove(titre.length()-1,1);
        }
        else
        {

#ifndef QT_NO_DEBUG
            qDebug()<< "Aucune selection active pour i="<<i;
#endif

        }
        titre = titre + " - ";
    }
    titre = titre.remove(titre.length()-3,3);

    return titre;
}

QString SyntheseGenerale::ActionElmZone(QString critere , QString operateur, int zone, stTiragesDef *pConf)
{
    QString ret_msg = "";

    // Operateur : or | and
    // critere : = | <>
    for(int i = 0; i<pConf->nbElmZone[zone];i++)
    {
        ret_msg = ret_msg +"tb2.B "+ critere +" tb1."
                + pConf->nomZone[zone]+QString::number(i+1)
                + " " + operateur+ " ";
    }
    int len_flag = operateur.length();
    ret_msg.remove(ret_msg.length()-len_flag-1, len_flag+1);

#ifndef QT_NO_DEBUG
    qDebug() << ret_msg;
#endif

    return ret_msg;
}

QString SyntheseGenerale::TrouverTirages(int col, QString str_nb, QString st_tirages, QString st_cri, int zn, stTiragesDef *pConf)
{
    QString st_tmp =  ActionElmZone("=","or",zn,pConf);
    QString st_return =
            "select tb1.*, count(tb2.B) as N"+QString::number(col)+ " "+
            "from (" + st_tirages.remove(";")+
            ") as tb1 "
            "left join "
            "("
            "select id as B from Bnrz where (z"+QString::number(zn+1)+
            " not null  and ("+st_cri+")) ) as tb2 " +
            "on "+
            "("
            +st_tmp+
            ") group by tb1.id";

#ifndef QT_NO_DEBUG
    qDebug() << st_return;
#endif
    st_return ="select * from("+
            st_return+
            ")as tb1 where(tb1.N"+QString::number(col)+ " in "+
            str_nb+");";

#ifndef QT_NO_DEBUG
    qDebug() << st_return;
#endif

    return(st_return);
}

void SyntheseGenerale::slot_MontreLesTirages(const QModelIndex & index)
{
    QTableView *view = qobject_cast<QTableView *>(sender());

    QSortFilterProxyModel *m = qobject_cast<QSortFilterProxyModel *>(view->model());
    QAbstractTableModel *sqm_tmp = NULL;

    if(view->objectName().contains("new")){
        sqm_tmp = qobject_cast<sqlqmDetails *>(m->sourceModel());
    }
    else{
        sqm_tmp = qobject_cast<QSqlQueryModel *>(m->sourceModel());
    }

    int nbcol = sqm_tmp->columnCount();
    int b_min = 0;
    int b_max = 0;
    for(int i = 0; i<nbcol;i++)
    {
        QString headName = sqm_tmp->headerData(i,Qt::Horizontal).toString();
        if(headName == "T")
        {
            b_min = i;
        }

        if(headName == "Ec")
        {
            b_max = i;
        }
    }

    int col = index.column();
    if(col <b_min || col >= b_max){
        return;
    }
    QStackedWidget *curOnglet = qobject_cast<QStackedWidget *>(view->parent()->parent());
    int nb_item = curOnglet->count();

    // Tableau de fonction permettant de creer la requete adapte a l'onglet
    QString (SyntheseGenerale::*ptrFunc[])(int,QString)=
    {
            &SyntheseGenerale::SqlCreateCodeBoule,
            &SyntheseGenerale::SqlCreateCodeBoule,
            &SyntheseGenerale::SqlCreateCodeCombi,
            &SyntheseGenerale::SqlCreateCodeGroupe,
            &SyntheseGenerale::SqlCreateCodeBary
};

    ///parcourir tous les onglets


    // Le simple click a construit la liste des boules
    //----------
    QString sqlReq = *(uneDemande.st_LDT_Depart);
    for(int onglet = 0; onglet<nb_item;onglet++)
    {
        sqlReq = (this->*ptrFunc[onglet])(onglet,sqlReq);
    }

    //----------
    stCurDemande *etude = new stCurDemande;
    *etude = uneDemande;


    etude->origine = Tableau2;
    etude->db_cnx = db_0.connectionName();

    etude->st_titre = CreatreTitle(&uneDemande);
    etude->st_TablePere = REF_BASE;
    etude->cur_dst = 0;
    etude->req_niv = 0;
    etude->st_Ensemble_1 = uneDemande.st_Ensemble_1;
    etude->ref = uneDemande.ref;
    etude->st_LDT_Filtre = new QString;
    etude->st_jourDef = new QString;
    *(etude->st_jourDef) = CompteJourTirage(db_0.connectionName());
    *(etude->st_LDT_Filtre) = sqlReq;

#ifndef QT_NO_DEBUG
    qDebug()<<etude->st_titre;
    qDebug()<<etude->st_TablePere;
    qDebug()<<*(etude->st_Ensemble_1);
    qDebug()<<*(etude->st_LDT_Filtre);
    qDebug()<<*(etude->st_jourDef);
    qDebug()<<"etude->st_jourDef";
#endif


    // Nouvelle de fenetre de detail de cette selection
    SyntheseDetails *unDetail = new SyntheseDetails(etude,pEcran,ptabTop);
    connect( ptabTop, SIGNAL(tabCloseRequested(int)) ,
             unDetail, SLOT(slot_FermeLaRecherche(int) ) );


}
QString SyntheseGenerale::SqlCreateCodeBary(int onglet, QString table)
{
    QString sqlReq ="";
    return sqlReq;
}

QString SyntheseGenerale::SqlCreateCodeBoule(int onglet, QString table)
{
    QString st_critere = "";
    QString sqlReq ="";

    QModelIndexList indexes =  uneDemande.selection[onglet];

    /// il y a t'il une selection
    sqlReq = table;
    if(indexes.size())
    {
        int max = uneDemande.ref->nbElmZone[onglet];
        QString champ = uneDemande.ref->nomZone[onglet];

        sqlReq = FiltreLaBaseSelonSelectionUtilisateur(indexes,onglet,max,champ,table);

#ifndef QT_NO_DEBUG
        qDebug() << sqlReq;
#endif
    }
    return sqlReq;
}
QString SyntheseGenerale::SqlCreateCodeEtoile(int onglet, QString table)
{
    QString st_critere = "";
    QString sqlReq ="";

    QModelIndexList indexes =  uneDemande.selection[onglet];

    /// il y a t'il une selection
    sqlReq = table;
    if(indexes.size())
    {
#ifndef QT_NO_DEBUG
        qDebug() << sqlReq;
#endif
    }
    return sqlReq;
}
QString SyntheseGenerale::SqlCreateCodeCombi(int onglet, QString table)
{
    QString st_critere = "";
    QString sqlReq ="";

    QModelIndexList indexes =  uneDemande.selection[onglet];

    /// il y a t'il une selection
    sqlReq = table;
    if(indexes.size())
    {
        int max = 1;
        QString champ = "pid";

        sqlReq = FiltreLaBaseSelonSelectionUtilisateur(indexes,onglet,max,champ,table);


#ifndef QT_NO_DEBUG
        qDebug() << sqlReq;
#endif
    }
    return sqlReq;
}

#ifdef EXEMPLE_SQL
-- Remarque il faut utiliser sqlite V3.26 au minimum

-- Selection tirages selon criteres
select t1.z1 as B,t2.* from Bnrz as t1, RefTirages as t2 where(t1.z1 in (t2.b1,t2.b2,t2.b3,T2.b4,t2.b5))

-- Calcul des ecarts dans la selection precendente pour chaque ligne
select t1.B as B,
ROW_NUMBER() OVER (PARTITION by t1.B order by t1.id) as LID,
lag(id,1,0) OVER (PARTITION by t1.B order by t1.id) as my_id,
(t1.id -(lag(id,1,0) OVER (PARTITION by t1.B order by t1.id))) as E,
t1.id,t1.J,t1.D,t1.C,t1.b1,t1.b2,t1.b3,t1.b4,t1.b5,t1.e1
from
(
        select t1.z1 as B,t2.* from Bnrz as t1, RefTirages as t2 where(t1.z1 in (t2.b1,t2.b2,t2.b3,T2.b4,t2.b5))
        ) as t1

-- Ecart Boules
select B,
count(*)  as T,
(select(min (Id)-1 ))as Ec,
max((CASE WHEN lid=2 then E END)) as Ep,
printf("%.1f",avg(E))as Em,
max(E) as M
FROM
(
        select t1.B as B,
        ROW_NUMBER() OVER (PARTITION by t1.B order by t1.id) as LID,
        lag(id,1,0) OVER (PARTITION by t1.B order by t1.id) as my_id,
        (t1.id -(lag(id,1,0) OVER (PARTITION by t1.B order by t1.id))) as E,
        t1.id,t1.J,t1.D,t1.C,t1.b1,t1.b2,t1.b3,t1.b4,t1.b5,t1.e1
        from
        (
            select t1.z1 as B,t2.* from Bnrz as t1, RefTirages as t2 where(t1.z1 in (t2.b1,t2.b2,t2.b3,T2.b4,t2.b5))
            ) as t1
        )as r1 group by b

-- Ecart Barycentre
select
B,
count(*)  as T,
(case WHEN
 (select(min (Id)-1 ))as Ec,
 max((CASE WHEN lid=2 then E END)) as Ep,
 printf("%.1f",avg(E))as Em,
 max(E) as M
 FROM
 (
     select ROW_NUMBER() OVER (PARTITION by t1.B order by t1.id) as LID,
     t1.B as B,
     lag(id,1,0) OVER (PARTITION by t1.B order by t1.id) as my_id,
     (t1.id -(lag(id,1,0) OVER (PARTITION by t1.B order by t1.id))) as E,
     t1.*
     from
     (
         select t1.bc as B,t2.* from r_RefTirages_0_brc_z1 as t1, analyses as t2 where(t1.bc in (t2.bc))
         ) as t1
     )as r1 GROUP by B order by T DESC

 -- Autre Selection tirages avec presence J,D,tip
 select t1.bc as B,t3.J,t3.D,t4.tip,t2.id
 from r_RefTirages_0_brc_z1 as t1,
 analyses as t2,
 RefTirages as t3,
 lstCombi_z1 as t4
 where(
     (t1.bc in (t2.bc))
     and
     (t2.id=t3.id)
     AND
     (t4.id=t2.fk_idCombi_z1)
     )

 -- Regroupement des barycentres de tous les tirages
 select ROW_NUMBER() OVER (PARTITION by t1.B order by t1.id) as LID,
 t1.B as B,
 lag(id,1,0) OVER (PARTITION by t1.B order by t1.id) as my_id,
 (t1.id -(lag(id,1,0) OVER (PARTITION by t1.B order by t1.id))) as E,
 t1.*
 from
 (
     select t1.bc as B,t2.* from r_RefTirages_0_brc_z1 as t1, analyses as t2 where(t1.bc in (t2.bc))
     ) as t1

 -- Version compacte des calculs...
 select B,
 count(*)  as T,
 count(CASE WHEN  J like 'lun%' then 1 end) as LUN,
 (select(min (Id)-1 ))as Ec,
 max((CASE WHEN lid=2 then E END)) as Ep,
 printf("%.1f",avg(E))as Em,
 max(E) as M
 FROM
 (
     select ROW_NUMBER() OVER (PARTITION by t1.B order by t1.id) as LID,
     t1.B as B,
     lag(id,1,0) OVER (PARTITION by t1.B order by t1.id) as my_id,
     (t1.id -(lag(id,1,0) OVER (PARTITION by t1.B order by t1.id))) as E,
     t1.*
     from
     (
         select t1.bc as B,t3.J,t3.D,t4.tip,t2.id
         from r_RefTirages_0_brc_z1 as t1,
         analyses as t2,
         RefTirages as t3,
         lstCombi_z1 as t4
         where(
             (t1.bc in (t2.bc))
             and
             (t2.id=t3.id)
             AND
             (t4.id=t2.fk_idCombi_z1)
             )) as t1
     )as r1 GROUP by B order by T DESC

 #endif


 QString SyntheseGenerale::Step_1_TrouverTirageSelonCriteres(int zn, QString tbl_reference, QString key, QString tbl_data)
{
     #if 0
     // exemple attendu
     select t1.z1 as B,
     t2.id,t2.J,t2.D,t2.C
     from Bnrz as t1,
     RefTirages as t2
     where
     (
     t1.z1 in (t2.b1,t2.b2,t2.b3,T2.b4,t2.b5)
     )
     #endif
     QString st_query = "";
     int len_zn = pMaConf->limites[zn].len;
     QString ref = "t2."+pMaConf->nomZone[zn]+"%1";
     QString st_critere = "(";

     for(int i=0;i<len_zn;i++){
         st_critere = st_critere + ref.arg(i+1);
         if(i<(len_zn-1)){
             st_critere=st_critere+QString(",");
         }
     }
     st_critere = st_critere+QString(")");

     QString dbg_more = "";
     #ifndef QT_NO_DEBUG
     dbg_more = ",t2.* ";
     #endif
     //key =z1

     st_query = "select t1."+key+" as B, "
     "t2.id,t2.J,t2.D,t2.C "
     +dbg_more
     +"from "+tbl_reference+" as t1, "
     +tbl_data+" as t2 "
     "where "
     "( "
     "t1."+key+" in "+st_critere+" "
     ") ";

     #ifndef QT_NO_DEBUG
     qDebug() << st_query;
     #endif

     bool isOk = true;
     QSqlQuery query(db_0);
     isOk = query.exec(st_query);

     return st_query;
 }

 QString SyntheseGenerale::Step_2_CalulerEcartDesReponses(QString str_reponses)
{
     #if 0
     select t1.B as B,
     ROW_NUMBER() OVER (PARTITION by t1.B order by t1.id) as LID,
     lag(id,1,0) OVER (PARTITION by t1.B order by t1.id) as my_id,
     (t1.id -(lag(id,1,0) OVER (PARTITION by t1.B order by t1.id))) as E,
     t1.id,t1.J,t1.D,t1.C,t1.b1,t1.b2,t1.b3,t1.b4,t1.b5,t1.e1
     from
     (
     select t1.z1 as B,
     t2.*
     from Bnrz as t1,
     RefTirages as t2
     where
     (
     t1.z1 in (t2.b1,t2.b2,t2.b3,T2.b4,t2.b5)
     )
     ) as t1

     #endif

     QString st_query = "select t1.B as B, "
     "ROW_NUMBER() OVER (PARTITION by t1.B order by t1.id) as LID, "
     "lag(id,1,0) OVER (PARTITION by t1.B order by t1.id) as my_id, "
     "(t1.id -(lag(id,1,0) OVER (PARTITION by t1.B order by t1.id))) as E, t1.* "
     "from "
     "( "
     +str_reponses+
     ") as t1 ";


     #ifndef QT_NO_DEBUG
     qDebug() << st_query;
     #endif

     bool isOk = true;
     QSqlQuery query(db_0);
     isOk = query.exec(st_query);

     if(!isOk){
         qDebug()<< query.lastError().text();
         qDebug()<< query.executedQuery();
     }

     return st_query;

 }

 QString SyntheseGenerale::Step_3_RegrouperEcartDesReponses(QString str_reponses)
{
     #if 0
     select B,
     (select NULL) as C,
     count(*)  as T,
     count(CASE WHEN  J like 'lun%' then 1 end) as LUN,
     (select(min (Id)-1 ))as Ec,
     max((CASE WHEN lid=2 then E END)) as Ep,
     printf("%.1f",avg(E))as Em,
     max(E) as M,
     (Select NULL) as Es,
     (select NULL) as P, (select NULL) as F
     FROM
     (
     select t1.B as B,
     ROW_NUMBER() OVER (PARTITION by t1.B order by t1.id) as LID,
     lag(id,1,0) OVER (PARTITION by t1.B order by t1.id) as my_id,
     (t1.id -(lag(id,1,0) OVER (PARTITION by t1.B order by t1.id))) as E,
     t1.id,t1.J,t1.D,t1.C,t1.b1,t1.b2,t1.b3,t1.b4,t1.b5,t1.e1
     from
     (
     select t1.z1 as B,
     t2.*
     from Bnrz as t1,
     RefTirages as t2
     where
     (
     t1.z1 in (t2.b1,t2.b2,t2.b3,T2.b4,t2.b5)
     )
     ) as t1
     )as r1 group by b

     #endif

     QString str_count_days = *st_JourTirageDef+"," ;
     QString st_query = "select "
     "(Select NULL) as Id,"
     "B,"
     "(select NULL) as C, "
     "count(*)  as T, "
     +str_count_days+
     "(select(min (Id)-1 ))as Ec, "
     "max((CASE WHEN lid=2 then E END)) as Ep, "
     "(printf(\"%.1f\",avg(E)))as Em, "
     "max(E) as M, "
     "(printf(\"%.1f\",sqrt(variance(E)))) as Es, "
     "(printf(\"%.1f\",median(E))) as Me, "
     "(select NULL) as P, (select NULL) as F "
     "FROM "
     "( "
     +str_reponses+
     ")as r1 group by b ";

     #ifndef QT_NO_DEBUG
     qDebug() << st_query;
     #endif

     bool isOk = true;
     QSqlQuery query(db_0);
     isOk = query.exec(st_query);

     if(!isOk){
         qDebug()<< query.lastError().text();
         qDebug()<< query.executedQuery();
     }

     return st_query;
 }

 bool SyntheseGenerale::Boules_Details(int zn, QString tbl_dst, QString ref, QString key, QString data){
     bool isOk = true;
     QSqlQuery query(db_0);

     QString st_requete = Step_1_TrouverTirageSelonCriteres(zn,  ref,  key,  data);

     st_requete = Step_2_CalulerEcartDesReponses(st_requete);
     st_requete = Step_3_RegrouperEcartDesReponses(st_requete);

     #if 0
     QSqlQueryModel *sqm_tmp = new QSqlQueryModel;

     QString sql_msgRef = PBAR_ReqComptage(pEtude, ReqTirages, curOng, ongPere);

     sqm_tmp->setQuery(sql_msgRef,db_0);

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

     #endif


     /// Lancement de la requete pour trouver le nom des colonnes
     if((isOk=query.exec(st_requete))){
         QString type = "";
         QString st_header = "(";
         int nbCol = query.record().count();
         for (int i = 0; i < nbCol; i++){
             QString name = query.record().fieldName(i);
             if(i == 0){
                 st_header = st_header + name + " integer primary key";
             }
             else{
                 if(i<nbCol){
                     st_header = st_header + ",";
                 }

                 if(i==11 || i==13 || i== 14){
                     type = " float";
                 }
                 else
                 {
                     type = " int";
                 }

                 st_header = st_header + name + type;
             }
         }
         st_header = st_header +")";
         #ifndef QT_NO_DEBUG
         qDebug() << st_header;
         #endif

         /// Creation de la table
         st_header = "create table if not exists "+tbl_dst+" " + st_header;
         if((isOk = query.exec(st_header))){
             /// mettre les donnees precedentes
             st_header = "insert into "+tbl_dst+" select * from ("+st_requete+")";
             #ifndef QT_NO_DEBUG
             qDebug() << st_header;
             #endif
             isOk = query.exec(st_header);
         }

     }

     if(!isOk){
         qDebug()<< query.lastError().text();
         qDebug()<< query.executedQuery();
     }
     return(isOk);
 }

 QString SyntheseGenerale::SqlCreateCodeGroupe(int onglet, QString table)
{
     QString st_critere = "";
     QString st_occure = "";
     QString sqlReq ="";

     QModelIndexList indexes =  uneDemande.selection[onglet];

     int nbChoix = maRef[0][0].size();

     /// il y a t'il une selection
     sqlReq = table;
     if(indexes.size())
     {
         QModelIndex un_index;
         int curCol = 0;
         int occure = 0;
         QString *Selection = new QString[nbChoix];

         /// Parcourir les selections
         foreach(un_index, indexes)
         {
             curCol = un_index.model()->index(un_index.row(), un_index.column()).column();
             occure = un_index.model()->index(un_index.row(), 0).data().toInt();

             if(Selection[curCol-1]==""){
                 Selection[curCol-1] = QString::number(occure);
             }
             else{
                 Selection[curCol-1] = Selection[curCol-1]+tr(",")+QString::number(occure);
             }
             #ifndef QT_NO_DEBUG
             qDebug() << Selection[curCol-1];
             #endif

         }
         ; ///Pause
         /// Parcourir les selections
         for(int col = 0; col<nbChoix; col++)
         {
             if(Selection[col]==""){
                 continue;
             }
             else{
                 st_critere = "("+maRef[0][0].at(col)+")";
                 st_occure = "("+Selection[col]+")";
                 sqlReq =TrouverTirages(col,st_occure,sqlReq,st_critere,0,uneDemande.ref);
             }
         }

         #ifndef QT_NO_DEBUG
         qDebug() << sqlReq;
         #endif

     }
     return sqlReq;
 }

 #if 0
 void SyntheseGenerale::FillRegroupement(int nbCol,stTiragesDef *conf)
{
     QSqlQuery query ;

     ///------------------------------
     bool status = true;
     int zn = 0;
     for(int j=0; (j< nbCol) && (status == true);j++)
     {
         // Creer Requete pour compter items
         QString msg1 = maRef[zn][0].at(j);
         QString sqlReq = "";
         QString db_data = ;
         sqlReq = ApplayFilters(db_data,msg1,zn,conf);
         //sqlReq = sql_RegroupeSelonCritere()

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

                 QStandardItem * item_1 = tmpStdItem->item(nb,j+1);
                 item_1->setData(tot,Qt::DisplayRole);
                 tmpStdItem->setItem(nb,j+1,item_1);
             }while(query.next() && status);
         }
     }
 }
 #endif

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
 from tirages as tb3, analyses as tb4, lstCombi_z1 as tb5
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
     (tb4.fk_idCombi_z1 = tb5.id)
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
        from tirages as tb3, analyses as tb4, lstCombi_z1 as tb5
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
            (tb4.fk_idCombi_z1 = tb5.id)
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

#if 1
QString CompteJourTirage(QString cnx_name)
{
    bool status = false;
    QString msg = "";

    QSqlDatabase use_db = QSqlDatabase::database(cnx_name);
    QSqlQuery query(use_db) ;

    QString st_tmp = "";
    QString st_table = "J";


    msg = "select distinct substr(tb1."+st_table+",1,3) as J from ("+
            REF_BASE+") as tb1 order by J asc;";

    if((status = query.exec(msg)))
    {
        status = query.first();
        if (query.isValid())
        {
            do
            {
                //count(CASE WHEN  J like 'lundi%' then 1 end) as LUN,
                st_tmp = st_tmp + "count(CASE WHEN  J like '"+
                        query.value(0).toString()+"%' then 1 end) as "+
                        query.value(0).toString()+",";
            }while((status = query.next()));

            //supprimer derniere ','
            st_tmp.remove(st_tmp.length()-1,1);
            st_tmp = st_tmp + " ";
        }
    }

#ifndef QT_NO_DEBUG
    qDebug() << "CreerCritereJours ->"<< query.lastError();
    qDebug() << "SQL 1:\n"<<msg<<"\n-------";
    qDebug() << "SQL 2:\n"<<st_tmp<<"\n-------";
#endif

    return st_tmp;
}

#else
QString CompteJourTirage(stTiragesDef *pMaConf)
{
    QString st_msg = "";

    int nb_tir = pMaConf->nb_tir_semaine;

    for(int i=0;i<nb_tir;i++)
    {
        st_msg = st_msg +
                "count(CASE WHEN  J like '"
                +pMaConf->jour_tir[i].left(2)+
                "%' then 1 end) as "
                +pMaConf->jour_tir[i].left(2)+
                ",";
    }

#ifndef QT_NO_DEBUG
    qDebug()<< st_msg;
#endif

    st_msg.remove(st_msg.length()-1,1);

    return st_msg;
}
#endif


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
            lstCombi_z1 as tb5
            where
            (
                tb4.id = tb3.id
            and
            tb5.id = tb4.fk_idCombi_z1
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
            "lstCombi_z1 as tb5 "
            "where "
            "( "
            "tb4.id = tb3.id "
            "and "
            "tb5.id = tb4.fk_idCombi_z1 "
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
