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

#if 0
    // simple click dans fenetre  pour selectionner boule
    connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
             this, SLOT(slot_ClicDeSelectionTableau( QModelIndex) ) );

    connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
             pEcran->parent(), SLOT(slot_PresenteLaBoule( QModelIndex) ) );


    // double click dans fenetre  pour afficher details boule
    connect( qtv_tmp, SIGNAL(doubleClicked(QModelIndex)) ,
             this, SLOT(slot_detailsDetails( QModelIndex) ) );
#endif
    return qtv_tmp;
}
