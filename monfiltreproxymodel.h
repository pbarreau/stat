#ifndef MONFILTREPROXYMODEL_H
#define MONFILTREPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QLabel>

class MonFiltreProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit MonFiltreProxyModel(QLabel *pText, int value, QObject *parent = 0);
    void setFilterKeyColumns(const QList<qint32> &filterColumns);
    void addFilterRegExp(const QRegExp &pattern);
    int getFilterNbRow(void);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
    QMap<qint32, QRegExp> m_columnPatterns;
    int ligneVisibles;
    QLabel *pTotal;

};

#endif // MONFILTREPROXYMODEL_H
