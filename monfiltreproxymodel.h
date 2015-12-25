#ifndef MONFILTREPROXYMODEL_H
#define MONFILTREPROXYMODEL_H

#include <QSortFilterProxyModel>

class MonFiltreProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit MonFiltreProxyModel(QObject *parent = 0);
    void setFilterKeyColumns(const QList<qint32> &filterColumns);
    void addFilterRegExp(const QRegExp &pattern);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
    QMap<qint32, QRegExp> m_columnPatterns;

};

#endif // MONFILTREPROXYMODEL_H
