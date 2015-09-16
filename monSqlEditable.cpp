#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QtSql>
#include <QtGui>

#include "refresultat.h"

MaSqlRequeteEditable::MaSqlRequeteEditable(QObject *parent)
    : QSqlQueryModel(parent)
{
}

Qt::ItemFlags MaSqlRequeteEditable::flags(
        const QModelIndex &index) const
{
    Qt::ItemFlags flags = QSqlQueryModel::flags(index);
    if (index.column() == 1)
        flags |= Qt::ItemIsEditable;
    return flags;
}
