#include <QPainter>

#include "BColorIndex_v2.h"
#include "colors.h"

BColorIndex_v2::BColorIndex_v2(Bp::E_Col c_Tot, Bp::E_Col c_Ec):colEc(c_Ec),colTotal(c_Tot)
{

}

void BColorIndex_v2::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
 QStyleOptionViewItem maModif(option);
 initStyleOption(&maModif, index);

 int col = index.column();

 /// Colonnes fond bleu
 if(col == colTotal || col == colEc){
  painter->fillRect(option.rect, COULEUR_FOND_TOTAL);
 }

 QStyledItemDelegate::paint(painter, option, index);

}
