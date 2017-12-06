#include <QItemDelegate>
#include <QPainter>
#include <QModelIndex>

#include "delegate.h"


//---------------------------------------------------
void Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                     const QModelIndex &index) const
{
    //int col = index.column();
    int val = 0;
    QColor u[]={QColor(247,255,15,180),
                QColor(0,157,255,180),
                QColor(82,63,255,180),
                QColor(255,12,222,180),
                QColor(255,42,0,180),
                QColor(255,157,0,180),
                QColor(76,255,0,180),
                QColor(255,0,0,180)
               };

    if (index.data().canConvert(QMetaType::Int))
    {
        val =  index.data().toInt();
        val = (int)floor(val/10);

        if(val < 0 || val >6)
            val = 7;

        painter->fillRect(option.rect, u[val]);
    }
    // Remettre le texte initial sur la couleur
    QItemDelegate::paint(painter, option, index);
}
