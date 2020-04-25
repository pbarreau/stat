#ifndef BFPMTIRAGEV1_H
#define BFPMTIRAGEV1_H

#include <QSortFilterProxyModel>

class BFpmTirageV1 : public QSortFilterProxyModel
{
public:
explicit BFpmTirageV1();

protected:
bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

public slots:
//void setUplets(const QString& lstBoules);

};

#endif // BFPMTIRAGEV1_H
