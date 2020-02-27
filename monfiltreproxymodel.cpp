#include <QLabel>
#include "monfiltreproxymodel.h"
//http://www.qtcentre.org/threads/24267-QSortFilterProxyModel-setFilterRegExp-for-more-than-1-column
//https://stackoverflow.com/questions/39488901/change-qsortfilterproxymodel-behaviour-for-multiple-column-filtering

MonFiltreProxyModel::MonFiltreProxyModel(QLabel *pText, int value, QObject *parent) : QSortFilterProxyModel(parent)
{
    pTotal = pText;
    ligneVisibles = value;
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
        qint32 col = iter.key();
        m_columnPatterns[col] = pattern;
    }
    filterChanged();

    // Nouveau resultat de lignes comment remonter info ???
    ligneVisibles = this->rowCount();
    QString msg = "Total : " + QString::number(ligneVisibles);
    pTotal->setText(msg);
}

bool MonFiltreProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
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

int MonFiltreProxyModel::getFilterNbRow(void)
{
    return ligneVisibles;
}

//---------------
BUpletFilterProxyModel::BUpletFilterProxyModel(int uplet, QObject *parent): QSortFilterProxyModel(parent)
{
 col_uplets=uplet;
}

 bool BUpletFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
 bool ret = true;

 if(lst_usr.isEmpty())
  return ret;

for (int j = 0; (j< lst_usr.size()) && ret;j++) {
  int sel = lst_usr[j].toInt();

  bool lgn = false;

	for (int i= 0; (i < col_uplets) && !lgn; i++) {
	 QModelIndex cur_index = sourceModel()->index(sourceRow, i, sourceParent);
	 int boule = sourceModel()->data(cur_index).toInt();

	 if(boule==sel){
		lgn = true;
		break;
	 }
	} /// for colonnes
	ret = ret && lgn;

 }

 //int nb_lgn = sourceModel()->rowCount();
 return ret;
}

void BUpletFilterProxyModel::setUplets(const QString& lstBoules)
{
 if(lstBoules.size()){
  lst_usr = lstBoules.split(",");
 }
 else {
  lst_usr.clear();
 }
 invalidateFilter();
}
