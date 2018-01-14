#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QTableView>
#include <QHeaderView>
#include <QString>
#include <QStringList>
#include <QGridLayout>
#include <QSortFilterProxyModel>
#include <QFormLayout>
#include <QStackedWidget>

#include "delegate.h"
#include "filtrecombinaisons.h"
#include "compter_combinaisons.h"
#include "cnp.h"
#include "db_tools.h"

int cCompterCombinaisons::total = 0;

cCompterCombinaisons::~cCompterCombinaisons()
{
    total --;
}

cCompterCombinaisons::cCompterCombinaisons(QString in):B_Comptage(&in)
{
    total++;
    QTabWidget *tab_Top = new QTabWidget(this);
    unNom = "'Compter Combinaisons'";

    QGridLayout *(cCompterCombinaisons::*ptrFunc[])(QString *, int) =
    {
            &cCompterCombinaisons::Compter,
            &cCompterCombinaisons::Compter_euro

};

    for(int i = 0; i< 2; i++)
    {
        QString *name = new QString;
        QWidget *tmpw = new QWidget;
        QGridLayout *calcul = (this->*ptrFunc[i])(name, i);
        tmpw->setLayout(calcul);
        tab_Top->addTab(tmpw,tr((*name).toUtf8()));
    }

#if 0
    QWidget * Resultats = new QWidget;
    QGridLayout *layout = new QGridLayout();
    layout->addWidget(tab_Top);
    Resultats->setLayout(layout);
    Resultats->setWindowTitle("Test3-"+QString::number(total));
    Resultats->show();
#endif
}

void cCompterCombinaisons::slot_ClicDeSelectionTableau(const QModelIndex &index)
{
    // L'onglet implique le tableau...
    int tab_index = 0;
    QTableView *view = qobject_cast<QTableView *>(sender());
    QStackedWidget *curOnglet = qobject_cast<QStackedWidget *>(view->parent()->parent());
    QItemSelectionModel *selectionModel = view->selectionModel();
    tab_index = curOnglet->currentIndex();

    // Colonne courante
    int col = index.column();

    /// click sur une colonne a ne pas regarder ?
    if(col <=2 )
    {
        /// oui alors deselectionner l'element
        selectionModel->select(index, QItemSelectionModel::Deselect);
        return;
    }
    lesSelections[tab_index]=selectionModel->selectedIndexes();

    LabelFromSelection(selectionModel,tab_index);
    SqlFromSelection(selectionModel,tab_index);
}

void cCompterCombinaisons::LabelFromSelection(const QItemSelectionModel *selectionModel, int zn)
{
    QString str_titre =  "c[";


    QModelIndexList indexes = selectionModel->selectedIndexes();
    int nb_items = indexes.size();
    if(nb_items)
    {
        QModelIndex un_index;
        int curCol = 0;
        int occure = 0;

        /// Parcourir les selections
        foreach(un_index, indexes)
        {
            const QAbstractItemModel * pModel = un_index.model();
            curCol = pModel->index(un_index.row(), un_index.column()).column();
            occure = pModel->index(un_index.row(), 1).data().toInt();

            // si on n'est pas sur la premiere colonne
            if(curCol)
            {
                QVariant vCol;
                QString headName;

                vCol = pModel->headerData(curCol,Qt::Horizontal);
                headName = vCol.toString();
                str_titre = str_titre + "("+headName+"," + QString::number(occure) + "),";
            }
        }

        // supression derniere ','
        str_titre.remove(str_titre.length()-1,1);
    }
    // on marque la fin
    str_titre = str_titre +"]";

    // informer disponibilité
    names[zn].selection = str_titre;
    emit sig_TitleReady(str_titre);
}

void cCompterCombinaisons::SqlFromSelection (const QItemSelectionModel *selectionModel, int zn)
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
            occure = pModel->index(un_index.row(), 1).data().toInt();

            // si on n'est pas sur colonne interdite
            if(curCol>2)
            {
                vCol = pModel->headerData(curCol,Qt::Horizontal);
                headName = vCol.toString();

                // Construire la liste des boules
                lstBoules << QString::number(occure);
            }
        }

        // Creation du critere de filtre
        QString tab = "tbz.pid";
        QString scritere = DB_Tools::GEN_Where_3(1,tab,false,"=",lstBoules,false,"or");
        if(headName != "T" and headName !="")
        {
            scritere = scritere + " and (J like '%" + headName +"%')";
        }
        sqlSelection[zn] = scritere;
    }
}

void cCompterCombinaisons::slot_RequeteFromSelection(const QModelIndex &index)
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

QString cCompterCombinaisons::RequetePourTrouverTotal_z1(QString st_baseUse,QString st_cr1, int dst)
{
    QString st_msg1 =
            "select count(CASE when tb2.id = 1 then 1 end) as last, tb1.id as Id, tb1.tip as Repartition, count(tb2.id) as T, "
            + db_jours +
            " "
            "from  "
            "("
            "select id,tip from lstcombi"
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
    return    st_msg1 ;
}

QString cCompterCombinaisons::RequetePourTrouverTotal_z2(QString st_baseUse,int zn)
{
    QString st_criteres = ConstruireCriteres(zn);
    QString st_msg1 =
            "select count(CASE when tb2.id = 1 then 1 end) as last, tb1.*,count(tb2.id) as T, "
            + db_jours +
            " "
            "from  "
            "("
            +"MyCnp_"+QString::number(limites[zn].max)+"_"+QString::number(limites[zn].len)+
            ") as tb1 "
            "left join "
            "("
            "select tb2.* from "
            "("
            +st_baseUse+
            " as tb2"
            ")"
            " )as tb2 "
            "on"
            "("
            + st_criteres +
            ")group by tb1.id order by T desc";

#ifndef QT_NO_DEBUG
    qDebug()<< st_msg1;
#endif
    return    st_msg1 ;
}

QString cCompterCombinaisons::ConstruireCriteres(int zn)
{
    /// critere a construire
    QString msg = "";

    /// recuperer le nombre de boules constituant la zone
    int lenZn = limites[zn].len;

    /// caculer le nombre de maniere distincte
    /// de prendre 1 dans l'ensemble
    BP_Cnp *b = new BP_Cnp(lenZn,1);
    int items = b->BP_count();


    /// prendre chaque possibilite
    for(int i = 0; i< items;i++)
    {
        /// recuperer la ligne donnant le coefficient
        int * ligne = b->BP_getPascalLine(i);

        /// prendre chaque coefficient de la ligne
        /// ICI pas la peine car la ligne contient 1 seul element
        int value = ligne[0];

        ///contruire le nom du champ de la table
        //QString tab1 = "tb1."+names[zn].court+QString::number(value);
        QString tab1 = "tb1.c"+QString::number(value);

        /// construire la requete sur ce champs
        int loop = lenZn;
        QStringList lstChamps;
        lstChamps << "tb2."+names[zn].court;
        msg = msg + DB_Tools::GEN_Where_3(loop,tab1,false,"=",lstChamps,true,"or");
        if(i<lenZn-1)
            msg = msg + "and";
    }
#ifndef QT_NO_DEBUG
    qDebug()<< msg;
#endif
    return msg;
}

QGridLayout *cCompterCombinaisons::Compter(QString * pName, int zn)
{
    QGridLayout *lay_return = new QGridLayout;
    (* pName) = names[zn].court;

    QTableView *qtv_tmp = new QTableView;

    QString qtv_name = QString::fromLatin1(TB_SC) + "_z"+QString::number(zn+1);
    qtv_tmp->setObjectName(qtv_name);

    QSqlQueryModel *sqm_tmp = new QSqlQueryModel;

    //tbv_bloc1_3 =qtv_tmp;

    //sqm_bloc1_3 = new QSqlQueryModel;
    //sqm_tmp=sqm_bloc1_3;

    QString st_baseUse = db_data;
    QString st_cr1 = "tb1.id=tb2.pid";
    QString st_msg1 = RequetePourTrouverTotal_z1(db_data,st_cr1,0);

    sqm_tmp->setQuery(st_msg1);
    int nbcol = sqm_tmp->columnCount();

    qtv_tmp->setSortingEnabled(true);
    qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
    qtv_tmp->setAlternatingRowColors(true);
    qtv_tmp->setStyleSheet("QTableView {selection-background-color: #939BFF;}");

    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);

    qtv_tmp->setFixedSize(250,CHauteur1);

    QSortFilterProxyModel *m=new QSortFilterProxyModel();
    m->setDynamicSortFilter(true);
    m->setSourceModel(sqm_tmp);
    qtv_tmp->setModel(m);
    qtv_tmp->setItemDelegate(new Dlgt_Combi);

    qtv_tmp->verticalHeader()->hide();
    qtv_tmp->hideColumn(0);

    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // simple click dans fenetre  pour selectionner boules
    connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
             this, SLOT(slot_ClicDeSelectionTableau( QModelIndex) ) );

    // Double click dans fenetre  pour creer requete
    connect( qtv_tmp, SIGNAL(doubleClicked(QModelIndex)) ,
             this, SLOT(slot_RequeteFromSelection( QModelIndex) ) );

#if 0
    qtv_tmp->setColumnWidth(1,30);
    qtv_tmp->setColumnWidth(2,70);

    for(int j=2;j<=sqm_tmp->columnCount();j++)
        qtv_tmp->setColumnWidth(j,LCELL);
    // Ne pas modifier largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#endif



    // Filtre
    QFormLayout *FiltreLayout = new QFormLayout;
    FiltreCombinaisons *fltComb_tmp = new FiltreCombinaisons();
    QList<qint32> colid;
    colid << 2;
    fltComb_tmp->setFiltreConfig(sqm_tmp,qtv_tmp,colid);

    FiltreLayout->addRow("&Filtre Repartition", fltComb_tmp);

    lay_return->addLayout(FiltreLayout,0,0,Qt::AlignLeft|Qt::AlignTop);
    lay_return->addWidget(qtv_tmp,1,0,Qt::AlignLeft|Qt::AlignTop);

    return lay_return;
}

QGridLayout *cCompterCombinaisons::Compter_euro(QString * pName, int zn)
{
    QGridLayout *lay_return = new QGridLayout;
    (* pName) = names[zn].court;

    QTableView *qtv_tmp = new QTableView;

    QString qtv_name = QString::fromLatin1(TB_SC) + "_z"+QString::number(zn+1);
    qtv_tmp->setObjectName(qtv_name);

    QSqlQueryModel *sqm_tmp = new QSqlQueryModel;


    QString st_baseUse = db_data;
    QString st_cr1 = "tb1.id=tb2.pid";
    QString st_msg1 = RequetePourTrouverTotal_z2(db_data,zn);

    sqm_tmp->setQuery(st_msg1);
    int nbcol = sqm_tmp->columnCount();

    qtv_tmp->setSortingEnabled(true);
    qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
    qtv_tmp->setAlternatingRowColors(true);
    qtv_tmp->setStyleSheet("QTableView {selection-background-color: #939BFF;}");

    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);

    qtv_tmp->setFixedSize(250,CHauteur1);

    QSortFilterProxyModel *m=new QSortFilterProxyModel();
    m->setDynamicSortFilter(true);
    m->setSourceModel(sqm_tmp);
    qtv_tmp->setModel(m);
    qtv_tmp->setItemDelegate(new Dlgt_Combi);

    qtv_tmp->verticalHeader()->hide();
    qtv_tmp->hideColumn(0);

    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // simple click dans fenetre  pour selectionner boules
    connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
             this, SLOT(slot_ClicDeSelectionTableau( QModelIndex) ) );

    // Double click dans fenetre  pour creer requete
    connect( qtv_tmp, SIGNAL(doubleClicked(QModelIndex)) ,
             this, SLOT(slot_RequeteFromSelection( QModelIndex) ) );



    // Filtre
    QFormLayout *FiltreLayout = new QFormLayout;
    FiltreCombinaisons *fltComb_tmp = new FiltreCombinaisons();
    QList<qint32> colid;
    for(int i=0; i<limites[zn].len; i++)
    {
        colid << i+2;
    }
    fltComb_tmp->setFiltreConfig(sqm_tmp,qtv_tmp,colid);

    FiltreLayout->addRow("&Filtre Repartition", fltComb_tmp);

    lay_return->addLayout(FiltreLayout,0,0,Qt::AlignLeft|Qt::AlignTop);
    lay_return->addWidget(qtv_tmp,1,0,Qt::AlignLeft|Qt::AlignTop);

    return lay_return;
}
