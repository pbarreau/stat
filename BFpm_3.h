#ifndef BFPM_3_H
#define BFPM_3_H

#include <QSortFilterProxyModel>

// https://stackoverflow.com/questions/39488901/change-qsortfilterproxymodel-behaviour-for-multiple-column-filtering
class BFpm_3:public QSortFilterProxyModel
{
 Q_OBJECT
 public:
 explicit BFpm_3(int uplet, int start=0, QObject *parent = nullptr);

 protected:
 bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

 public slots:
 void BSlot_MakeUplets(const QString& lstBoules);

 private:
 int col_deb;
 int col_tot;
 QStringList lst_usr;
};

#endif // BFPM_3_H
