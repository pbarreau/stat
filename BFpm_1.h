#ifndef BFPM_1_H
#define BFPM_1_H

#include <QSortFilterProxyModel>


class BFpm_1 : public QSortFilterProxyModel
{
 Q_OBJECT
public:
explicit BFpm_1();

protected:
bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

};

#endif // BFPM_1_H
