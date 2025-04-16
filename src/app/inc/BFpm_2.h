#ifndef MONFILTREPROXYMODEL_H
#define MONFILTREPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QLabel>

class BFpm_2 : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit BFpm_2(QLabel *pText, int value, QObject *parent = 0);
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
