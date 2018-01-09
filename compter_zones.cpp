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

#include <QActionGroup>
#include <QAction>
#include <QMenu>

#include "compter_zones.h"
int cTabZnCount::total = 0;

cTabZnCount::~cTabZnCount()
{
    total --;
}

cTabZnCount::cTabZnCount(QString in):B_Comptage(&in)
{
    total++;
    QTabWidget *tab_Top = new QTabWidget;

    int nb_zones = nbZone;
    lesSelections = new QModelIndexList [nb_zones];

    QTableView *(cTabZnCount::*ptrFunc[])(QString *, int) =
    {
            &cTabZnCount::Compter,
            &cTabZnCount::Compter

};

    for(int i = 0; i< nb_zones; i++)
    {
        QString *name = new QString;
        QTableView *calcul = (this->*ptrFunc[i])(name, i);
        tab_Top->addTab(calcul,tr((*name).toUtf8()));
    }

    QWidget * Resultats = new QWidget;
    QGridLayout *layout = new QGridLayout();
    layout->addWidget(tab_Top);
    Resultats->setLayout(layout);
    Resultats->setWindowTitle("Test2");
    Resultats->show();
}


void cTabZnCount::slot_ClicDeSelectionTableau(const QModelIndex &index)
{
    // L'onglet implique le tableau...
    int tab_index = 0;
    QTableView *view = qobject_cast<QTableView *>(sender());
    QStackedWidget *curOnglet = qobject_cast<QStackedWidget *>(view->parent());
    QItemSelectionModel *selectionModel = view->selectionModel();
    tab_index = curOnglet->currentIndex();
    lesSelections[tab_index]= selectionModel->selectedIndexes();
}

void cTabZnCount::slot_RequeteFromSelection(const QModelIndex &index)
{    QString st_titre = "";
     QVariant vCol;
     QString headName;
     const QAbstractItemModel * pModel = index.model();

     QString st_critere = "";
     QString sqlReq ="";
     QTableView *view = qobject_cast<QTableView *>(sender());
     QStackedWidget *curOnglet = qobject_cast<QStackedWidget *>(view->parent());

     /// il y a t'il une selection
     ///parcourir tous les onglets
     sqlReq = db_data;
     int nb_item = curOnglet->count();
     for(int onglet = 0; onglet<nb_item;onglet++)
     {
         QModelIndexList indexes =  lesSelections[onglet];

         if(indexes.size())
         {
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
                     vCol = pModel->headerData(curCol,Qt::Horizontal);
                     headName = vCol.toString();
                     st_titre = st_titre + "("+headName+"," + QString::number(occure) + "),";
                 }
             }
             st_titre.remove(st_titre.length()-1,1);

             // signaler que cet objet a construit la requete
             a.db_data = sqlReq;
             a.tb_data = "b"+QString::number(total)+":"+st_titre;
             emit sig_ComptageReady(a);

         }

     }
}

QString cTabZnCount::GEN_Where_3(int loop,
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

#ifndef QT_NO_DEBUG
        qDebug() << "GEN_Where_3\n";
        qDebug() << "SQL msg:\n"<<ret_msg<<"\n-------";
#endif

    return ret_msg;
}

QString cTabZnCount::PBAR_ReqComptage(QString ReqTirages, int zn,int distance)
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

QTableView *cTabZnCount::Compter(QString * pName, int zn)
{
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

#if 0
    for(int j=0;j<=sqm_tmp->columnCount();j++)
        qtv_tmp->setColumnWidth(j,LCELL);
    qtv_tmp->setFixedSize(CLargeur1,CHauteur1);
#endif

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

    return qtv_tmp;
}

void cTabZnCount::slot_ccmr_tbForBaseEcart(QPoint pos)
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

QMenu *cTabZnCount::ContruireMenu(QString tbl, int val)
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


void cTabZnCount::slot_ChoosePriority(QAction *cmd)
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
bool cTabZnCount::VerifierValeur(int item,int *lev, QString table)
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
