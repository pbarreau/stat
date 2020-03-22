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

// https://stackoverflow.com/questions/39488901/change-qsortfilterproxymodel-behaviour-for-multiple-column-filtering
class BUpletFilterProxyModel:public QSortFilterProxyModel
{
 Q_OBJECT
 public:
 explicit BUpletFilterProxyModel(int uplet, int start=0, QObject *parent = nullptr);

 protected:
 bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

 public slots:
 void setUplets(const QString& lstBoules);

 private:
 int col_deb;
 int col_tot;
 QStringList lst_usr;
};

#endif // MONFILTREPROXYMODEL_H
