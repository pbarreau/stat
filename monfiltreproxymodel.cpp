#include "monfiltreproxymodel.h"

MonFiltreProxyModel::MonFiltreProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
}

void MonFiltreProxyModel::setFilterKeyColumns(const QList<qint32> &filterColumns)
{
    m_columnPatterns.clear();

    foreach(qint32 column, filterColumns)
        m_columnPatterns.insert(column, QRegExp());
}

void MonFiltreProxyModel::addFilterRegExp(const QRegExp &pattern)
{
    // pour chacune des colonne mettre le filtre
    invalidateFilter();
    for(QMap<qint32, QRegExp>::const_iterator iter = m_columnPatterns.constBegin();
        iter != m_columnPatterns.constEnd();
        ++iter)
    {
        QString Sval = pattern.pattern();
        qint32 col = iter.key();
        m_columnPatterns[col] = pattern;
    }
    filterChanged();
}

bool MonFiltreProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    bool ret = false;

    if(m_columnPatterns.isEmpty())
        return true;

    for(QMap<qint32, QRegExp>::const_iterator iter = m_columnPatterns.constBegin();
        iter != m_columnPatterns.constEnd();
        ++iter)
    {
        int colId = iter.key();
        QModelIndex index = sourceModel()->index(sourceRow, colId, sourceParent);

        QString celVal = index.data().toString();

        QRegExp tt4 = iter.value();
        QString maVal = tt4.pattern();

        ret = celVal.contains(maVal,Qt::CaseInsensitive);

        if(!ret)
            return ret;
    }

    return ret;
}
