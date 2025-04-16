#ifndef BLSTSELECT_H
#define BLSTSELECT_H

#include <QItemSelectionModel>
#include <QItemSelection>


#include "bstflt.h"

class BLstSelect
{
 public:
 BLstSelect();
 BLstSelect(etCount cur_type, int cur_zn, QItemSelectionModel *cur_sel, int cur_upl=-1);
 void clearSelection();
 etCount type;
 int zn;
 int upl;
 QModelIndexList  indexes;

 private:
 ///void addSelection(etCount cur_type, int cur_zn, QItemSelection cur_sel);

 private:
 QItemSelectionModel *sel;
};

typedef QList<QList<BLstSelect *>*> B2LstSel;
typedef QList<QPersistentModelIndex> QPersistentModelIndexList;

#endif // BLSTSELECT_H
