#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QtGui>
#include <QHeaderView>
#include <QLineEdit>
#include <QTableView>
#include <QSqlQueryModel>

#include "monQview.h"
#include "SyntheseDetails.h"

QString SD_Tb2(QStringList boules, int lgn, int dst);

MonQtViewDelegate::MonQtViewDelegate(QLineEdit *pDist, QStringList &lstChoix, QTabWidget *memo,QObject *parent)
    : QStyledItemDelegate(parent)
{
    pereOnglet = memo;
    lesBoules = lstChoix;
    distancePerso = pDist;
}

QWidget *MonQtViewDelegate::createEditor(QWidget *parent,
                                         const QStyleOptionViewItem &/* option */,
                                         const QModelIndex & index ) const
{
    QTableView *editor = new QTableView(parent);
    if(index.column() == 1)
    {

        QSqlQueryModel *sqm_tmp = new QSqlQueryModel;

        int ref = pereOnglet->currentIndex();
        int distance[4]={0,-1,1,-2};
        QStringList lst_boules = lesBoules;
        int ligne = index.row()+1;

#ifndef QT_NO_DEBUG
        qDebug() << ref;
#endif

        if(ref ==3)
        {
            distance[ref]= distancePerso->text().toInt()*-1;
        }
        QString sql_msgRef = SD_Tb2(lst_boules,ligne,distance[ref]);

        editor->setSortingEnabled(true);
        editor->sortByColumn(0,Qt::AscendingOrder);
        editor->setAlternatingRowColors(true);


        //qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
        editor->setSelectionMode(QAbstractItemView::NoSelection);
        editor->setSelectionBehavior(QAbstractItemView::SelectItems);
        editor->setEditTriggers(QAbstractItemView::NoEditTriggers);

        QSortFilterProxyModel *m=new QSortFilterProxyModel();
        m->setDynamicSortFilter(true);

        sqm_tmp->setQuery(sql_msgRef);
        m->setSourceModel(sqm_tmp);
        //editor->setModel(sqm_tmp);

        editor->setModel(m);
        for(int j=0;j<5;j++)
            editor->setColumnWidth(j,40);
        // Ne pas modifier largeur des colonnes
        editor->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        editor->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    }
    return editor;
}

void MonQtViewDelegate::setEditorData(QWidget *editor,
                                      const QModelIndex &index) const
{

}

void MonQtViewDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                     const QModelIndex &index) const
{

}

void MonQtViewDelegate::updateEditorGeometry(QWidget *editor,
                                             const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

QString SD_Tb2(QStringList boules, int lgn, int dst)
{
#if 0

    --etoiles
            --debut requete tb3
            select tb3.id as Tid,
            tb3.jour_tirage as J,
            substr(tb3.date_tirage,-2,2)||'/'||substr(tb3.date_tirage,6,2)||'/'||substr(tb3.date_tirage,1,4) as D,
            tb5.tip as C,
            tb3.b1 as b1, tb3.b2 as b2,tb3.b3 as b3,tb3.b4 as b4,tb3.b5 as b5,
            tb3.e1 as e1,
            tb3.bp as P,
            tb3.bg as G
            from tirages as tb3, analyses as tb4, lstcombi as tb5
            inner join
            (
                select *  from tirages as tb1
                ) as tb2
            on (
                (tb3.id = tb2.id + 0)
                and
                (tb4.id = tb3.id)
                and
                (tb4.id_poids = tb5.id)
                )
            --Fin requete tb3


            -- Requete comptage du resultat precedent
            select tbleft.boule as B, count(tbright.Tid) as T,
            count(CASE WHEN  J like 'lundi%' then 1 end) as LUN, count(CASE WHEN  J like 'mercredi%' then 1 end) as MER, count(CASE WHEN  J like 'same%' then 1 end) as SAM
            from
            (
                select id as boule from Bnrz where (z2 not null )
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
                from tirages as tb3, analyses as tb4, lstcombi as tb5
                inner join
                (
                    select *  from tirages as tb1
                    ) as tb2
                on (
                    (tb3.id = tb2.id + 0)
                    and
                    (tb4.id = tb3.id)
                    and
                    (tb4.id_poids = tb5.id)
                    )
                --Fin requete tb3
                ) as tbright
            on
            (
                (
                    tbleft.boule = tbright.e1
            )
                ) group by tbleft.boule;

#endif

    QString st_msg = "";
    QStringList lst_tmp = boules;

    lst_tmp << QString::number(lgn);

    QString st_cri1 = GEN_Where_3(5,"tb1.b",true,"=",lst_tmp,false,"or");
    st_msg =
            "select tbleft.boule as B, count(tbright.Tid) as T,"
            "count(CASE WHEN  J like 'lundi%' then 1 end) as LUN,"
            "count(CASE WHEN  J like 'mercredi%' then 1 end) as MER,"
            "count(CASE WHEN  J like 'same%' then 1 end) as SAM "
            "from"
            "("
            "select id as boule from Bnrz where (z2 not null )"
            ") as tbleft "
            "left join"
            "("
            "select tb3.id as Tid,"
            "tb3.jour_tirage as J,"
            "substr(tb3.date_tirage,-2,2)||'/'||substr(tb3.date_tirage,6,2)||'/'||substr(tb3.date_tirage,1,4) as D,"
            "tb5.tip as C,"
            "tb3.b1 as b1, tb3.b2 as b2,tb3.b3 as b3,tb3.b4 as b4,tb3.b5 as b5,"
            "tb3.e1 as e1,"
            "tb3.bp as P,"
            "tb3.bg as G "
            "from tirages as tb3, analyses as tb4, lstcombi as tb5 "
            "inner join"
            "("
            "select *  from tirages as tb1 "
            "where"
            "("
            +st_cri1+
            ")"
            ") as tb2 "
            "on ("
            "(tb3.id = tb2.id + "
            + QString::number(dst)+
            ") "
            "and"
            "(tb4.id = tb3.id)"
            "and"
            "(tb4.id_poids = tb5.id)"
            ")"
            ") as tbright "
            "on"
            "("
            "("
            "tbleft.boule = tbright.e1 "
            ")"
            ") group by tbleft.boule;"
            ;

#ifndef QT_NO_DEBUG
    qDebug() << st_msg;
#endif

    return (st_msg);
}
