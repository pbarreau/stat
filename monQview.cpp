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


MonQtViewDelegate::MonQtViewDelegate(stObjDetail *pDef, QObject *parent)
    : QStyledItemDelegate(parent)
{
    pParent = pDef;
    pereOnglet = pDef->pOnglet;
    pLaConfig = pDef->pParamObj;
    distancePerso = pDef->pDist;
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
        QStringList lst_boules = pLaConfig->lst_boules;
        int ligne = index.row()+1;

#ifndef QT_NO_DEBUG
        qDebug() << "Reference Onglet ->" + QString::number(ref);
#endif

        // selon le type de demande choisir une reference de requete
        // Fonction Pour La requete de base (obtenir les tirages)
        QString (MonQtViewDelegate::*ptrFuncN2_2[3])(QStringList &, int,int)const=
        {&MonQtViewDelegate::SD_Tb2_1,
                &MonQtViewDelegate::SD_Tb2_2,
                &MonQtViewDelegate::SD_Tb2_2};


        if(ref ==3)
        {
            distance[ref]= distancePerso->text().toInt()*-1;
        }

        int origine = pParent->pParamObj->origine;
        origine--;
        QString sql_msgRef = (this->*ptrFuncN2_2[origine])(lst_boules,ligne,distance[ref]);

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

QString MonQtViewDelegate::SD_Tb2_1(QStringList &boules, int lgn, int dst)const
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

QString MonQtViewDelegate::SD_Tb2_2(QStringList &boules, int lgn, int dst) const
{
#if 0
    -- Requete comptage du resultat precedent
    select tbleft.boule as B, count(tbright.Tid1) as T,
    count(CASE WHEN J like 'lundi%' then 1 end) as LUN, count(CASE WHEN J like 'mercredi%' then 1 end) as MER, count(CASE WHEN J like 'same%' then 1 end) as SAM
    from
    (
    select id as boule from Bnrz where (z2 not null )
    ) as tbleft
    left join
    (
   select tb3.id as Tid1, tb5.id as Pid,tb3.jour_tirage as J,substr(tb3.date_tirage,-2,2)||'/'||substr(tb3.date_tirage,6,2)||'/'||substr(tb3.date_tirage,1,4) as D,tb5.tip as C,tb3.b1 as b1, tb3.b2 as b2,tb3.b3 as b3,tb3.b4 as b4,tb3.b5 as b5,tb3.e1 as e1,tb3.bp as P,tb3.bg as G from tirages as tb3, analyses as tb4, lstcombi as tb5 inner join(select * from (select tb1.*, count(tb2.B) as N from tirages as tb1 left join(select id as B from Bnrz where (z1 not null and (z1 like '%1')))as tb2 on(tb1.b1 = tb2.B or tb1.b2 = tb2.B or tb1.b3 = tb2.B or tb1.b4 = tb2.B or tb1.b5 = tb2.B)group by tb1.id) as ensemble_1 where(ensemble_1.N =1)) as tb2 on ((tb3.id = tb2.id + 0) and(tb4.id = tb3.id)and(tb4.id_poids = tb5.id))
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
    )
    where
    (
    tbright.b1 = 1 or
    tbright.b2 = 1 or
    tbright.b3 = 1 or
    tbright.b4 = 1 or
    tbright.b5 = 1
    )group by tbleft.boule ;

    ---------------------------
    --- regarder la repartition des etoiles
    select tbleft.id as eId, count(tbright.Tid1) as T,
    count(CASE WHEN  J like 'lundi%' then 1 end) as LUN, count(CASE WHEN  J like 'mercredi%' then 1 end) as MER, count(CASE WHEN  J like 'same%' then 1 end) as SAM
     from (select id from Bnrz where (z2 is not null))as tbleft
    left join
    (
    ---- Debut
     ---- prendre ceux ou il y a une boule = 1
     select tb3.id as Tid1, tb5.id as Pid,tb3.jour_tirage as J,
    substr(tb3.date_tirage,-2,2)||'/'||substr(tb3.date_tirage,6,2)||'/'||substr(tb3.date_tirage,1,4) as D,
    tb5.tip as C,tb3.b1 as b1, tb3.b2 as b2,tb3.b3 as b3,tb3.b4 as b4,tb3.b5 as b5,tb3.e1 as e1,tb3.bp as P,tb3.bg as G
    from tirages as tb3, analyses as tb4, lstcombi as tb5
    inner join
    (
    -- nb boule dans un tirage se terminant par un 1
    select *  from
    (select tb1.*, count(tb2.B) as N from tirages as tb1 left join(select id as B from Bnrz where (z1 not null and (z1 like '%1'))
    )as tb2
    on
    (
    tb1.b1 = tb2.B or tb1.b2 = tb2.B or tb1.b3 = tb2.B or tb1.b4 = tb2.B or tb1.b5 = tb2.B
    )group by tb1.id
    -- fin nb boule dans un tirage se terminant par un 1
    ) as ensemble_1
    where(ensemble_1.N =2)
    ) as tb2
    on ((tb3.id = tb2.id + 0) and(tb4.id = tb3.id)and(tb4.id_poids = tb5.id))
    where
    (
      tb2.b1=2 or
      tb2.b2=2 or
      tb2.b3=2 or
      tb2.b4=2 or
      tb2.b5=2
    )
    ---- Fin
    ) as tbright
    on
    (
     tbright.e1 = tbleft.id
    ) group by tbleft.id;

#endif

    QString st_msg = "";
    QString st_ref = "";

    st_ref = pParent->pObjet->DoSqlMsgRef_Tb2(boules,dst);
    st_ref= st_ref.remove(st_ref.length()-1,1);

    st_msg =
            "select tbleft.id as eId, count(tbright.Tid1) as T, "
            "count(CASE WHEN  J like 'lundi%' then 1 end) as LUN, count(CASE WHEN  J like 'mercredi%' then 1 end) as MER, count(CASE WHEN  J like 'same%' then 1 end) as SAM "
            " from (select id from Bnrz where (z2 is not null))as tbleft   "
            "left join "
            "( "
           +st_ref+
             "where "
            "( "
            "  tb2.b1="+QString::number(lgn)+" or "
            "  tb2.b2="+QString::number(lgn)+" or "
            "  tb2.b3="+QString::number(lgn)+" or "
            "  tb2.b4="+QString::number(lgn)+" or "
            "  tb2.b5="+QString::number(lgn)+"  "
            ") "
             ") as tbright "
            "on "
            "( "
            " tbright.e1 = tbleft.id "
            ") group by tbleft.id;";

#ifndef QT_NO_DEBUG
    qDebug() << st_ref;
#endif


#ifndef QT_NO_DEBUG
    qDebug() << st_msg;
#endif

    return (st_msg);
}
