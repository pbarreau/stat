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
//QList<QPersistentModelIndex> indexes;

/*
void setType(etCount cur_type);
QModelIndexList indexes();
void setZoneId(int cur_zn);
QItemSelectionModel * selected();
*/

private:
///void addSelection(etCount cur_type, int cur_zn, QItemSelection cur_sel);

private:
QItemSelectionModel *sel;
/*
etCount in_type;
int in_zn;
*/
};

typedef QList<QList<BLstSelect *>*> B2LstSel;
typedef QList<QPersistentModelIndex> QPersistentModelIndexList;

#endif // BLSTSELECT_H
