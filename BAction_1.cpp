#include "BAction_1.h"

BAction_1::BAction_1(const QString &label, const QTableView *view, QPoint pos):
  QAction(label),tbv(view),point(pos)
{
 connect(this, SIGNAL(triggered()), this, SLOT(BSlot_onTriggered()));
}

void BAction_1::BSlot_onTriggered()
{
 QModelIndex index = tbv->indexAt(point);
 emit BSig_ActionAt(index);
}
