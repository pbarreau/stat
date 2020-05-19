#ifndef BCOLORINDEX_V2_H
#define BCOLORINDEX_V2_H

#include <QStyledItemDelegate>
#include "bstflt.h"

class BColorIndex_v2 : public QStyledItemDelegate
{
public:
BColorIndex_v2(Bp::E_Col c_Tot = Bp::colTotalv1, Bp::E_Col c_Ec=Bp::noCol);
void paint(QPainter *painter, const QStyleOptionViewItem &option,
           const QModelIndex &index) const;

private:
Bp::E_Col colEc;
Bp::E_Col colTotal;
};

#endif // BCOLORINDEX_V2_H
