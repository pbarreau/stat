#include <QLabel>
#include "BFpm_2.h"
//http://www.qtcentre.org/threads/24267-QSortFilterProxyModel-setFilterRegExp-for-more-than-1-column
//https://stackoverflow.com/questions/39488901/change-qsortfilterproxymodel-behaviour-for-multiple-column-filtering

BFpm_2::BFpm_2(QLabel *pText, int value, QObject *parent) : QSortFilterProxyModel(parent)
{
    pTotal = pText;
    ligneVisibles = value;
}

void BFpm_2::setFilterKeyColumns(const QList<qint32> &filterColumns)
{
    m_columnPatterns.clear();

    foreach(qint32 column, filterColumns)
        m_columnPatterns.insert(column, QRegExp());
}

void BFpm_2::addFilterRegExp(const QRegExp &pattern)
{
    // pour chacune des colonne mettre le filtre
    invalidateFilter();
    for(QMap<qint32, QRegExp>::const_iterator iter = m_columnPatterns.constBegin();
        iter != m_columnPatterns.constEnd();
        ++iter)
    {
        qint32 col = iter.key();
        m_columnPatterns[col] = pattern;
    }
    filterChanged();

    // Nouveau resultat de lignes comment remonter info ???
    ligneVisibles = this->rowCount();
    QString msg = "Total : " + QString::number(ligneVisibles);
    pTotal->setText(msg);
}

bool BFpm_2::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    bool ret = false;

    if(m_columnPatterns.isEmpty())
        return true;

    for(QMap<qint32, QRegExp>::const_iterator iter = m_columnPatterns.constBegin();
        (iter != m_columnPatterns.constEnd())
        && !ret;
        ++iter)
    {
        int colId = iter.key();
        QModelIndex index = sourceModel()->index(sourceRow, colId, sourceParent);

        QString celVal = index.data().toString();

        QRegExp tt4 = iter.value();
        QString maVal = tt4.pattern();

        ret = celVal.contains(maVal,Qt::CaseInsensitive);

        //if(!ret)
        //    return ret;
    }

    return ret;
}

int BFpm_2::getFilterNbRow(void)
{
    return ligneVisibles;
}

