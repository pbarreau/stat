#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QGridLayout>

#include <QStackedWidget>

#include <QSqlQuery>
#include <QSortFilterProxyModel>

#include <QTableView>
#include <QHeaderView>
#include <QToolTip>
#include <QMessageBox>

#include <QActionGroup>
#include <QAction>
#include <QMenu>

#include "compter_zones.h"
int cCompterZoneElmts::total = 0;

cCompterZoneElmts::~cCompterZoneElmts()
{
    total --;
}

cCompterZoneElmts::cCompterZoneElmts(QString in, QWidget *LeParent):B_Comptage(&in,LeParent)
{
    total++;
    QTabWidget *tab_Top = new QTabWidget(this);
    unNom = "'Compter Zones'";

    int nb_zones = nbZone;

    QGridLayout *(cCompterZoneElmts::*ptrFunc[])(QString *, int) =
    {
            &cCompterZoneElmts::Compter,
            &cCompterZoneElmts::Compter

};

    for(int i = 0; i< nb_zones; i++)
    {
        QString *name = new QString;
        QWidget *tmpw = new QWidget;
        QGridLayout *calcul = (this->*ptrFunc[i])(name, i);
        tmpw->setLayout(calcul);
        tab_Top->addTab(tmpw,tr((*name).toUtf8()));
    }

    tab_Top->setWindowTitle("Test2-"+QString::number(total));
#if 0
    QWidget * Resultats = new QWidget;
    QGridLayout *layout = new QGridLayout();
    layout->addWidget(tab_Top);
    Resultats->setLayout(layout);
    Resultats->setWindowTitle("Test2-"+QString::number(total));
    Resultats->show();
#endif
}


void cCompterZoneElmts::slot_ClicDeSelectionTableau(const QModelIndex &index)
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

    // Aucune colonne active ?
    if(memo[tab_index]==-1)
    {
        // alors memoriser la colonne active
        memo[tab_index] =col;
    }
    else
    {
        /// une colonne a deja ete selectionne
        /// combien d'elements deja selectionne ?
        int tot_items = selectionModel->selectedIndexes().size();

        if(tot_items >1)
        {
            /// plus d'un elements
            /// verifier si le nouvel element choisi
            /// est aussi sur la meme colonne
            if(col != memo[tab_index])
            {
                /// non alors deselectionner l'element
                selectionModel->select(index, QItemSelectionModel::Deselect);
                return;
            }
        }
        else
        {
            /// c'est un changement de colonne
            /// ou une deselection d'element
            if(!tot_items)
            {
                /// c'est une deselection
                /// prochain coup on peut prendre
                /// ou l'on veut
                memo[tab_index]=-1;
            }
            else
            {
                /// on a changer de colonne
                memo[tab_index]=col;
            }
        }
    }


    //  choix Maxi atteind ?
    int nb_items = selectionModel->selectedIndexes().size();
    if(nb_items > limites[tab_index].len)
    {
        //un message d'information
        QMessageBox::warning(0, names[tab_index].complet, "Attention, maximum element atteind !",QMessageBox::Yes);

        // deselectionner l'element
        selectionModel->select(index, QItemSelectionModel::Deselect);
        return;
    }

    //lesSelections[tab_index]= selectionModel->selectedIndexes();
    LabelFromSelection(selectionModel,tab_index);
    SqlFromSelection(selectionModel,tab_index);
}

void cCompterZoneElmts::SqlFromSelection (const QItemSelectionModel *selectionModel, int zn)
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
        QString scritere = GEN_Where_3(loop,tab,true,"=",lstBoules,false,"or");
        if(headName != "T" and headName !="")
        {
            scritere = scritere + " and (J like '%" + headName +"%')";
        }
        sqlSelection[zn] = scritere;
    }
}

void cCompterZoneElmts::slot_RequeteFromSelection(const QModelIndex &index)
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


/// Requete permettant de remplir le tableau
///
QString cCompterZoneElmts::PBAR_ReqComptage(QString ReqTirages, int zn,int distance)
{
    QString msg = "";

    QString st_cri_all = "";
    QStringList boules;

    if(lesSelections[zn].size())
    {
        if(distance == 0 ){
            QModelIndex une_selection;
            foreach (une_selection, lesSelections[zn]) {
                int la_boule = une_selection.model()->index(une_selection.row(),0).data().toInt();
                boules<<QString::number(la_boule);
            }
            // Mettre une exception pour ne pas compter le cas
            st_cri_all= GEN_Where_3(1,"tbleft.boule",false,"!=",boules,false,"and");
            boules.clear();
            st_cri_all= st_cri_all + " and ";
        }
    }

    boules<< "tbright."+names[zn].court;
    int loop = limites[zn].len;
    st_cri_all= st_cri_all +GEN_Where_3(loop,"tbleft.boule",false,"=",boules,true,"or");
    boules.clear();

    msg =
            "select tbleft.boule as B, count(tbright.id) as T, "
            +db_jours
            + " "
              "from "
              "( "
              "select id as boule from Bnrz where (z"
            +QString::number(zn+1)
            +" not null ) "
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
    qDebug() << "PBAR_ReqComptage\n";
    qDebug() << "SQL 1:\n"<<st_cri_all<<"\n-------";
    qDebug() << "SQL 2:\n"<<db_jours<<"\n-------";
    qDebug() << "SQL 3:\n"<<ReqTirages<<"\n-------";
    qDebug() << "SQL msg:\n"<<msg<<"\n-------";
#endif


    return msg;
}

QGridLayout *cCompterZoneElmts::Compter(QString * pName, int zn)
{
    QGridLayout *lay_return = new QGridLayout;

    QTableView *qtv_tmp = new QTableView;
    (* pName) = names[zn].court;

    QString qtv_name = QString::fromLatin1(TB_SE) + "_z"+QString::number(zn+1);
    qtv_tmp->setObjectName(qtv_name);

    QSqlQueryModel *sqm_tmp = new QSqlQueryModel;

    QString ReqTirages = db_data;
    QString sql_msgRef = PBAR_ReqComptage(ReqTirages, zn, 0);

    sqm_tmp->setQuery(sql_msgRef);

    qtv_tmp->setSortingEnabled(true);
    qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
    qtv_tmp->setAlternatingRowColors(true);
    //qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
    //qtv_tmp->setSelectionMode(QAbstractItemView::NoSelection);
    qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QSortFilterProxyModel *m=new QSortFilterProxyModel();
    m->setDynamicSortFilter(true);
    m->setSourceModel(sqm_tmp);

#if 0
    // Memorisation des pointeurs
    if(ongPere == 3)
    {
        dist->keepPtr(curOng,sqm_tmp,qtv_tmp,m);
    }
#endif

    qtv_tmp->setModel(m);
    qtv_tmp->verticalHeader()->hide();

    //largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    int b = qtv_tmp->columnWidth(1);
    int n = sqm_tmp->columnCount();
    qtv_tmp->setFixedWidth((n+0.85)*b);

    // positionner le tableau
    lay_return->addWidget(qtv_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);


    // simple click dans fenetre  pour selectionner boules
    connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
             this, SLOT(slot_ClicDeSelectionTableau( QModelIndex) ) );

    // Double click dans fenetre  pour creer requete
    connect( qtv_tmp, SIGNAL(doubleClicked(QModelIndex)) ,
             this, SLOT(slot_RequeteFromSelection( QModelIndex) ) );

    qtv_tmp->setMouseTracking(true);
    connect(qtv_tmp,
            SIGNAL(entered(QModelIndex)),this,SLOT(slot_AideToolTip(QModelIndex)));

    qtv_tmp->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(qtv_tmp, SIGNAL(customContextMenuRequested(QPoint)),this,
            SLOT(slot_ccmr_tbForBaseEcart(QPoint)));

    return lay_return;
}

void cCompterZoneElmts::slot_ccmr_tbForBaseEcart(QPoint pos)
{
    /// http://www.qtcentre.org/threads/7388-Checkboxes-in-menu-items
    /// https://stackoverflow.com/questions/2050462/prevent-a-qmenu-from-closing-when-one-of-its-qaction-is-triggered

    QTableView *view = qobject_cast<QTableView *>(sender());
    QModelIndex index = view->indexAt(pos);
    int col = view->columnAt(pos.x());

    if(col == 0)
    {
        QString tbl = view->objectName();

        int val = 0;
        if(index.model()->index(index.row(),0).data().canConvert(QMetaType::Int))
        {
            val =  index.model()->index(index.row(),0).data().toInt();
        }

        QMenu *MonMenu = new QMenu(this);
        QMenu *subMenu= ContruireMenu(tbl,val);
        MonMenu->addMenu(subMenu);
        MonMenu->exec(view->viewport()->mapToGlobal(pos));
    }
}

QMenu *cCompterZoneElmts::ContruireMenu(QString tbl, int val)
{
    QString msg2 = "Priorite";
    QMenu *menu =new QMenu(msg2, this);
    QActionGroup *grpPri = new  QActionGroup(menu);

    int niveau = 0;
    bool existe = false;
    existe = VerifierValeur(val,&niveau, tbl);



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


void cCompterZoneElmts::slot_ChoosePriority(QAction *cmd)
{
    QSqlQuery query;
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


/// Cette fonction cherche dans la table designée si une valeur est presente
/// auquel cas le champs priorité est aussi retourné
/// item : valeur a rechercher
/// *lev : valeur de priorité trouvé
/// table : nom de la table dans laquelle il faut chercher
bool cCompterZoneElmts::VerifierValeur(int item,int *lev, QString table)
{
    bool ret = false;
    QSqlQuery query ;
    QString msg = "";

    msg = "select * from " + table + " " +
            "where (val = "+QString::number(item)+");";
    ret =  query.exec(msg);

    if(!ret)
    {
#ifndef QT_NO_DEBUG
        qDebug() << "select: " <<table<<"->"<< query.lastError();
        qDebug() << "Bad code:\n"<<msg<<"\n-------";
#endif
    }
    else
    {
#ifndef QT_NO_DEBUG
        qDebug() << "Fn VerifierValeur:\n"<<msg<<"\n-------";
#endif

        // A t on un resultat
        ret = query.first();
        if(query.isValid())
        {
            int val = query.value(2).toInt();

            if(val >0 && val <=5)
            {
                *lev = val;
            }

        }
    }

    return ret;
}
