#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QtGui>
#include <QHeaderView>
#include "refresultat.h"

MonQtViewDelegate::MonQtViewDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QWidget *MonQtViewDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex & index ) const
{
    if(index.column() == 1)
    {
        QTableView *editor = new QTableView(parent);
        QSqlQueryModel *sqm_tmp = new QSqlQueryModel;
        QString sql_msgRef = SD_Tb2();



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

        return editor;
    }
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
