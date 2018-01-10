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

#include "delegate.h"
#include "filtrecombinaisons.h"
#include "compter_combinaisons.h"

int cCompterCombinaisons::total = 0;

cCompterCombinaisons::~cCompterCombinaisons()
{
    total --;
}

cCompterCombinaisons::cCompterCombinaisons(QString in):B_Comptage(&in)
{
    total++;
    QTabWidget *tab_Top = new QTabWidget;

    QGridLayout *(cCompterCombinaisons::*ptrFunc[])(QString *, int) =
    {
            &cCompterCombinaisons::Compter,
            &cCompterCombinaisons::Compter

};

    for(int i = 0; i< 1; i++)
    {
        QString *name = new QString;
        QWidget *tmpw = new QWidget;
        QGridLayout *calcul = (this->*ptrFunc[i])(name, i);
        tmpw->setLayout(calcul);
        tab_Top->addTab(tmpw,tr((*name).toUtf8()));
    }

    QWidget * Resultats = new QWidget;
    QGridLayout *layout = new QGridLayout();
    layout->addWidget(tab_Top);
    Resultats->setLayout(layout);
    Resultats->setWindowTitle("Test3");
    Resultats->show();
}

void cCompterCombinaisons::slot_ClicDeSelectionTableau(const QModelIndex &index)
{
}

void cCompterCombinaisons::slot_RequeteFromSelection(const QModelIndex &index)
{

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

QGridLayout *cCompterCombinaisons::Compter(QString * pName, int zn)
{
    QGridLayout *lay_return = new QGridLayout;

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
