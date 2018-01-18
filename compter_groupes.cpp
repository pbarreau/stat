#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QGridLayout>
#include <QTableView>
#include <QSqlQuery>
#include <QHeaderView>
#include <QToolTip>
#include <QStackedWidget>

#include "compter_groupes.h"
#include "db_tools.h"

int cCompterGroupes::total = 0;

cCompterGroupes::~cCompterGroupes()
{
    total --;
}

cCompterGroupes::cCompterGroupes(QString in):B_Comptage(&in)
{
    total++;
    QTabWidget *tab_Top = new QTabWidget(this);
    unNom = "'Compter Groupes'";
    demande = 0;


    int nb_zones = nbZone;
    maRef = new  QStringList* [nb_zones] ;
    p_qsim_3 = new QStandardItemModel *[nb_zones];

    QGridLayout *(cCompterGroupes::*ptrFunc[])(QString *, int) =
    {
            &cCompterGroupes::Compter,
            &cCompterGroupes::Compter

};

    for(int zn = 0; zn< nb_zones; zn++)
    {
        QString *name = &db_data;
        QWidget *tmpw = new QWidget;

        maRef[zn] = CreateFilterForData(zn);

        QString tblSynthese = "Montest";
        bool isOK = AnalyserEnsembleTirage(db_data,tblSynthese,zn);

        QGridLayout *calcul = (this->*ptrFunc[zn])(name, zn);
        tmpw->setLayout(calcul);
        tab_Top->addTab(tmpw,tr((*name).toUtf8()));
    }

#if 0
    QWidget * Resultats = new QWidget;
    QGridLayout *layout = new QGridLayout();
    layout->addWidget(tab_Top);
    Resultats->setLayout(layout);
    Resultats->setWindowTitle("Test1-"+QString::number(total));
    Resultats->show();
#endif
}

QGridLayout *cCompterGroupes::Compter(QString * pName, int zn)
{
    QGridLayout *lay_return = new QGridLayout;


    QTableView *qtv_tmp_1 = CompterLigne (pName, zn);
    QTableView *qtv_tmp_2 = CompterEnsemble (pName, zn);

    // positionner les tableaux
    lay_return->addWidget(qtv_tmp_1,0,0,Qt::AlignLeft|Qt::AlignTop);
    lay_return->addWidget(qtv_tmp_2,1,0,Qt::AlignLeft|Qt::AlignTop);


    return lay_return;
}

bool cCompterGroupes::AnalyserEnsembleTirage(QString InputTable, QString OutputTable, int zn)
{
    /// Verifier si des vues temporaires precedentes sont encore presentes
    /// Si oui les effacer
    /// Si non prendre la liste des criteres a appliquer sur l'ensemble
    /// puis faire tant qu'il existe un critere
    /// effectuer la selection comptage vers une nouvelle vu temporaire i
    /// quand on arrive a nombre de criteres total -1 la vue destination
    /// sera OutputTable.

    bool isOk = true;
    QString msg = "";
    QSqlQuery query;
    QString stDefBoules = TB_RZBN;
    QString st_OnDef = "";

    //int nbZone = nbZone;
    QString ref="(tbleft.%1%2=tbRight.B)";

    /// sur quel nom des elements de la zone
    st_OnDef=""; /// remettre a zero pour chacune des zones
    for(int j=0;j<limites[zn].len;j++)
    {
        st_OnDef = st_OnDef + ref.arg(names[zn].court).arg(j+1);
        if(j<(limites[zn].len)-1)
            st_OnDef = st_OnDef + " or ";
    }

#ifndef QT_NO_DEBUG
    qDebug() << "on definition:"<<st_OnDef;
#endif

    QStringList *slst=&maRef[zn][0];

    /// Verifier si des tables existent deja
    if(SupprimerVueIntermediaires())
    {
        /// les anciennes vues ne sont pas presentes
        ///  on peut faire les calculs
        int loop = 0;
        int nbTot = slst[0].size();
        QString curName = InputTable;
        QString curTarget = "view vt_0";
        do
        {
            msg = "create " + curTarget
                    +" as select tbLeft.*, count(tbRight.B) as "
                    + slst[1].at(loop)
                    +" from("+curName+")as tbLeft "
                    +"left join (select c1.id as B from "
                    +stDefBoules+" as c1 where (c1.z"
                    +QString::number(zn+1)+" not null and (c1."
                    +slst[0].at(loop)+"))) as tbRight on ("
                    +st_OnDef+") group by tbLeft.id";
            isOk = query.exec(msg);
#ifndef QT_NO_DEBUG
            qDebug() << "msg:"<<msg;
#endif
            loop++;
            curName = "vt_" +  QString::number(loop-1);
            if(loop <  nbTot-1)
            {
                curTarget = "view vt_"+QString::number(loop);
            }
            else
            {
                curTarget = "table vrz"+QString::number(zn+1)+"_"+OutputTable;
            }
        }while(loop < nbTot && isOk);


        /// supression tables intermediaires
        if(isOk)
            isOk = SupprimerVueIntermediaires();
    }

    if(!isOk)
    {
        QString ErrLoc = "AnalyserEnsembleTirage:";
        DB_Tools::DisplayError(ErrLoc,&query,msg);
    }
    return isOk;
}

bool cCompterGroupes::SupprimerVueIntermediaires(void)
{
    bool isOk = true;
    QString msg = "";
    QSqlQuery query;
    QSqlQuery qDel;

    msg = "SELECT name FROM sqlite_master "
          "WHERE type='view' AND name like'vt_%';";
    isOk = query.exec(msg);

    if(isOk)
    {
        query.first();
        if(query.isValid())
        {
            /// il en existe donc les suprimer
            do
            {
                QString viewName = query.value("name").toString();
                msg = "drop view if exists "+viewName;
                isOk = qDel.exec(msg);
            }while(query.next()&& isOk);
        }
    }

    if(!isOk)
    {
        QString ErrLoc = "SupprimerVueIntermediaires:";
        DB_Tools::DisplayError(ErrLoc,&query,msg);
    }

    return isOk;
}

QTableView *cCompterGroupes::CompterLigne(QString * pName, int zn)
{
    QTableView *qtv_tmp = new QTableView;

    int nbCol = maRef[zn][0].size();
    QStandardItemModel * sqm_tmp =  new QStandardItemModel(1,nbCol);
    p_qsim_3[zn] = sqm_tmp;
    qtv_tmp->setModel(sqm_tmp);

    QStringList tmp=maRef[zn][1];
    sqm_tmp->setHorizontalHeaderLabels(tmp);

    for(int pos=0;pos<nbCol;pos++)
    {
        QStandardItem *item = new QStandardItem();
        sqm_tmp->setItem(0,pos,item);
        qtv_tmp->setColumnWidth(pos,LCELL);
    }

    qtv_tmp->setSortingEnabled(false);
    qtv_tmp->setAlternatingRowColors(true);
    qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Bloquer largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->hide();

    // Taille tableau
    int b = qtv_tmp->columnWidth(0);
    int n = sqm_tmp->columnCount();
    qtv_tmp->setFixedWidth((b*n)+5);

    b = HCELL;
    n = 1;
    qtv_tmp->setFixedHeight((b*n)+5);

    return qtv_tmp;
}

QTableView *cCompterGroupes::CompterEnsemble(QString * pName, int zn)
{
    QTableView *qtv_tmp = new QTableView;
    int nbLgn = limites[zn].len + 1;
    //(* pName) = names[zn].court;

    QStandardItemModel * sqm_tmp = NULL;
    QSqlQuery query ;

    int nbCol = maRef[zn][0].size();

    //Creer un tableau d'element standard
    if(nbCol)
    {
        sqm_tmp =  new QStandardItemModel(nbLgn,nbCol);
        qtv_tmp->setModel(sqm_tmp);

        QStringList tmp=maRef[zn][1];
        tmp.insert(0,"Nb");
        sqm_tmp->setHorizontalHeaderLabels(tmp);

        QStringList tooltips=maRef[zn][2];
        tooltips.insert(0,"Total");
        for(int pos=0;pos <=nbCol;pos++)
        {
            QStandardItem *item = sqm_tmp->horizontalHeaderItem(pos);
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
                sqm_tmp->setItem(lgn,pos,item);
                //qtv_tmp->setColumnWidth(pos,LCELL);
            }
        }
        // Gestion du QTableView
        qtv_tmp->setSelectionMode(QAbstractItemView::MultiSelection);
        qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
        qtv_tmp->setStyleSheet("QTableView {selection-background-color: #939BFF;}");

        qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
        qtv_tmp->setAlternatingRowColors(true);

        qtv_tmp->setSortingEnabled(true);
        qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
        qtv_tmp->verticalHeader()->hide();

        //largeur des colonnes
        qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

        int b = qtv_tmp->columnWidth(0);
        int n = sqm_tmp->columnCount();
        qtv_tmp->setFixedWidth((b*n)+5);

        //n = (qtv_tmp->horizontalHeader()->height()) * (nbLgn+1.2);
        b = qtv_tmp->rowHeight(0);
        n =nbLgn+1;
        qtv_tmp->setFixedHeight((b*n)+5);
        //qtv_tmp->setFixedSize(CLargeur1*1.8,CHauteur1);

        // simple click dans fenetre  pour selectionner boules
        connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
                 this, SLOT(slot_ClicDeSelectionTableau( QModelIndex) ) );

        // Double click dans fenetre  pour creer requete
        connect( qtv_tmp, SIGNAL(doubleClicked(QModelIndex)) ,
                 this, SLOT(slot_RequeteFromSelection( QModelIndex) ) );

        qtv_tmp->setMouseTracking(true);

        connect(qtv_tmp,
                SIGNAL(entered(QModelIndex)),this,SLOT(slot_AideToolTip(QModelIndex)));

        ///------------------------------
        RecalculGroupement(zn,nbCol,sqm_tmp);
    }

    return qtv_tmp;
}

void cCompterGroupes::RecalculGroupement(int zn,int nbCol,QStandardItemModel *sqm_tmp)
{
    bool status = true;
    QSqlQuery query ;

    for(int j=0; (j< nbCol) && (status == true);j++)
    {
        //Effacer calcul precedent
        for(int k=0; k<limites[zn].len+1;k++)
        {
            QStandardItem * item_1 = sqm_tmp->item(k,j+1);
            item_1->setData("",Qt::DisplayRole);
            sqm_tmp->setItem(k,j+1,item_1);
        }

        // Creer Requete pour compter items
        QString msg1 = maRef[zn][0].at(j);
        QString sqlReq = "";
        sqlReq = CriteresAppliquer(db_data,msg1,zn);

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

                QStandardItem * item_1 = sqm_tmp->item(nb,j+1);
                item_1->setData(tot,Qt::DisplayRole);
                sqm_tmp->setItem(nb,j+1,item_1);
            }while(query.next() && status);
        }
    }

}

QString cCompterGroupes::sql_ComptePourUnTirage(int id,QString st_tirages, QString st_cri, int zn)
{
#if 0
    /* Req_1 : pour compter le nombre de boules pair par tirages */
    select tb1.id as Tid, count(tb2.B) as Nb from
            (
                select * from tirages where id=1
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
            ) group by tb1.id; /* Fin Req_1 */
#endif

    QString st_tmp =  CriteresCreer("=","or",zn);
    QString st_return =
            "select tb1.id as Tid, count(tb2.B) as Nb from "
            "(("
            "select * from " + st_tirages.remove(";")
            + " where id = "
            +QString::number(id)
            +") as r1 "
             ") as tb1 "
             "left join "
             "("
             "select id as B from Bnrz where (z"+QString::number(zn+1)+
            " not null  and ("+st_cri+")) ) as tb2 " +
            "on "
            "("
            +st_tmp+
            ") group by tb1.id;";

    return(st_return);

}

void cCompterGroupes::slot_DecodeTirage(const QModelIndex & index)
{
    static int sortir = 0;

    // recuperer la ligne de la table
    int lgn = index.model()->index(index.row(),0).data().toInt();

    if(sortir != lgn)
    {
        sortir = lgn;
    }
    else
    {
        return;
    }

    QSqlQuery query;

    for(int zn = 0; zn < nbZone;zn ++)
    {
        QStandardItemModel *sqm_tmp = p_qsim_3[zn];
        int nbCol = maRef[zn][0].size();
        bool status = true;
        for(int j=0; (j< nbCol) && (status == true);j++)
        {
            // Creer Requete pour compter items
            QString msg1 = maRef[zn][0].at(j);
            QString sqlReq = "";
            sqlReq = sql_ComptePourUnTirage(lgn,db_data,msg1,zn);

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
                    int tot = query.value(1).toInt();

                    QStandardItem * item_1 = sqm_tmp->item(0,j);
                    item_1->setData(tot,Qt::DisplayRole);
                    sqm_tmp->setItem(0,j,item_1);
                }while(query.next() && status);
            }
        }
    }
}

// Cette fonction retourne un pointeur sur un tableau de QStringList
// Ce tableau comporte 2 elements
// Element 0 liste des requetes construites
// Element 1 Liste des titres assosies a la requete
// En fonction de la zone a etudier les requetes sont adaptees
// pour integrer le nombre maxi de boules a prendre en compte
QStringList * cCompterGroupes::CreateFilterForData(int zn)
{
    QStringList *sl_filter = new QStringList [3];
    QString fields = "z"+QString::number(zn+1);

    //int maxElems = pConf->limites[zn].max;
    int maxElems = limites[zn].max;
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
QString cCompterGroupes::TrouverTirages(int col, int nb, QString st_tirages, QString st_cri, int zn)
{

    QString st_tmp =  CriteresCreer("=","or",zn);
    QString st_return =
            "/*S"+QString::number(demande)+"a '"+st_cri+"'*/"+
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
            ") group by tb1.id"+
            "/*S"+QString::number(demande)+"a*/";


    st_return =
            "/*S"+QString::number(demande)+"b*/"+
            "select * from("+
            st_return+
            ")as tb1 where(tb1.N"+QString::number(col)+ "="+
            QString::number(nb)+")/*S"+QString::number(demande)+"b*/;";

    demande++;

#ifndef QT_NO_DEBUG
    qDebug() << st_return;
#endif

    return(st_return);
}

QString cCompterGroupes::CriteresAppliquer(QString st_tirages, QString st_cri, int zn)
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
    ////---data, range, name, filter
#endif

    QString st_tmp =  CriteresCreer("=","or",zn);
    QString st_return =
            "select Nb, count(Nb) as Tp from "
            "("
            "select tb1.id as Tid, count(tb2.B) as Nb from "
            "("
            "select * from (" + st_tirages.remove(";")+") as r1 "
                                                       ") as tb1 "+
            "left join "
            "("
            "select id as B from Bnrz where (z"+QString::number(zn+1)+
            " not null  and ("+st_cri+")) ) as tb2 " +
            "on "+
            "("
            +st_tmp+
            ") group by tb1.id order by Nb desc "
            ")"
            "group by Nb;";

#ifndef QT_NO_DEBUG
    qDebug() << st_return;
#endif

    return(st_return);
}

void cCompterGroupes::slot_ClicDeSelectionTableau(const QModelIndex &index)
{
    // L'onglet implique le tableau...
    int tab_index = 0;
    QTableView *view = qobject_cast<QTableView *>(sender());
    QStackedWidget *curOnglet = qobject_cast<QStackedWidget *>(view->parent()->parent());
    QItemSelectionModel *selectionModel = view->selectionModel();
    tab_index = curOnglet->currentIndex();

    // Colonne courante
    int col = index.column();

    /// click sur la colonne boule id ?
    if(!col)
    {
        /// oui alors deselectionner l'element
        selectionModel->select(index, QItemSelectionModel::Deselect);
        return;
    }

    lesSelections[tab_index]=selectionModel->selectedIndexes();
    LabelFromSelection(selectionModel,tab_index);
}

void cCompterGroupes::SqlFromSelection (const QItemSelectionModel *selectionModel, int zn)
{
    QModelIndexList indexes = selectionModel->selectedIndexes();

    int nb_items = indexes.size();
    if(nb_items)
    {
        QModelIndex un_index;
        QStringList lstBoules;

        QVariant vCol;
        QString headName;
        int curCol = 0;
        int occure = 0;


        /// Parcourir les selections
        foreach(un_index, indexes)
        {
            const QAbstractItemModel * pModel = un_index.model();
            curCol = pModel->index(un_index.row(), un_index.column()).column();
            occure = pModel->index(un_index.row(), 0).data().toInt();

            // si on n'est pas sur la premiere colonne
            if(curCol)
            {
                vCol = pModel->headerData(curCol,Qt::Horizontal);
                headName = vCol.toString();

                // Construire la liste des boules
                lstBoules << QString::number(occure);
            }
        }

        // Creation du critere de filtre
        int loop = limites[zn].len;
        QString tab = "tbz."+names[zn].court;
        QString scritere = DB_Tools::GEN_Where_3(loop,tab,true,"=",lstBoules,false,"or");
        if(headName != "T" and headName !="")
        {
            scritere = scritere + " and (J like '%" + headName +"%')";
        }
        sqlSelection[zn] = scritere;
    }
}

#if 0
void cCompterGroupes::slot_RequeteFromSelection(const QModelIndex &index)
{
    QString st_critere = "";
    QString sqlReq ="";
    QString st_titre ="";
    QTableView *view = qobject_cast<QTableView *>(sender());
    QStackedWidget *curOnglet = qobject_cast<QStackedWidget *>(view->parent()->parent());

    ///parcourir tous les onglets
    sqlReq = db_data;
    int nb_item = curOnglet->count();
    for(int onglet = 0; onglet<nb_item;onglet++)
    {
        if(sqlSelection[onglet]!=""){
            st_critere = st_critere + "(/* DEBUT CRITERE z_"+
                    QString::number(onglet+1)+ "*/" +
                    sqlSelection[onglet]+ "/* FIN CRITERE z_"+
                    QString::number(onglet+1)+ "*/)and";
        }
        st_titre = st_titre + names[onglet].selection;
    }

    /// suppression du dernier 'and'
    st_critere.remove(st_critere.length()-3,3);

    sqlReq = "/* CAS "+unNom+" */select tbz.* from ("
            + sqlReq + ") as tbz where ("
            + st_critere +"); /* FIN CAS "+unNom+" */";


    // signaler que cet objet a construit la requete
    a.db_data = sqlReq;
    a.tb_data = st_titre;
    emit sig_ComptageReady(a);
}
#endif

void cCompterGroupes::slot_RequeteFromSelection(const QModelIndex &index)
{
    QString st_titre = "";

    QString st_critere = "";
    QString sqlReq ="";
    QTableView *view = qobject_cast<QTableView *>(sender());
    QStackedWidget *curOnglet = qobject_cast<QStackedWidget *>(view->parent()->parent());

    /// il y a t'il une selection
    ///parcourir tous les onglets
    sqlReq = db_data;
    int nb_item = curOnglet->count();
    for(int onglet = 0; onglet<nb_item;onglet++)
    {
        QModelIndexList indexes =  lesSelections[onglet];

        if(indexes.size())
        {
            st_titre = st_titre + names[onglet].selection + "-";

            QModelIndex un_index;
            int curCol = 0;
            int occure = 0;

            /// Parcourir les selections
            foreach(un_index, indexes)
            {
                curCol = un_index.model()->index(un_index.row(), un_index.column()).column();
                occure = un_index.model()->index(un_index.row(), 0).data().toInt();
                if(curCol)
                {
                    st_critere = "("+maRef[onglet][0].at(curCol-1)+")";
                    sqlReq =TrouverTirages(curCol,occure,sqlReq,st_critere,onglet);
                }
            }
        }
    }

    /// on informe !!!
    if(st_titre!="")
    {
        st_titre.remove(st_titre.length()-1,1);
        // signaler que cet objet a construit la requete
        sqlReq = "/*CAS "+unNom+" */"
                + sqlReq +
                "/*FIN CAS "+unNom+" */";

        a.db_data = sqlReq;
        a.tb_data = "g"+QString::number(total)+":"+st_titre;
        emit sig_ComptageReady(a);
    }

}
