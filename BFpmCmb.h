#ifndef BFPMCMB_H
#define BFPMCMB_H

#include <QSortFilterProxyModel>
#include "game.h"
#include "BView_1.h"

class BFpmCmb : public QSortFilterProxyModel
{
 Q_OBJECT

 public:
 explicit BFpmCmb();

 protected:
 bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent);
};

#endif // BFPMCMB_H
