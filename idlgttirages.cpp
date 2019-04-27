#include <QPainter>

#include "idlgttirages.h"


void idlgtTirages::paint(QPainter *painter, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const
{
    int val = 0;

    QColor u[]= {QColor(201,230,255,255), QColor(255,195,155,255), QColor(170,255,255,255),
                 QColor(224,255,147,255),QColor(231,181,255,255),QColor(73,179,255,255),
                 Qt::red,Qt::white};

    QStyleOptionViewItem maModif(option);

    /// Mettre une couleur en fonction du groupe u,dizaine,v,...
    if(index.column()>4 && index.column() <=9)
    {
        val =  index.data().toInt();
        val = val/10;

        if(val < 0 || val >6){
            val = (sizeof(u)/sizeof(QColor))-2;
        }

        painter->fillRect(option.rect, u[val]);
    }

    QItemDelegate::paint(painter, maModif, index);
}
