#include "BLstSelect.h"

BLstSelect::BLstSelect():type(eCountToSet),zn(-1),sel(nullptr)
{

}

BLstSelect::BLstSelect(etCount cur_type, int cur_zn, QItemSelectionModel *cur_sel)
    :type(cur_type),zn(cur_zn),sel(cur_sel)
{
 indexes = sel->selectedIndexes();
}

void BLstSelect::clearSelection()
{
 sel->clear();
}

/*
void BLstSelect::setType(etCount cur_type)
{
 in_type = cur_type;
}

QModelIndexList BLstSelect::indexes()
{
 return sel->selectedIndexes();
}

void BLstSelect::setZoneId(int cur_zn)
{
 in_zn = cur_zn;
}

QItemSelectionModel * BLstSelect::selected()
{
 return sel;
}
*/
