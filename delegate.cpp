#include <QItemDelegate>
#include <QPainter>
#include <QModelIndex>
#include <QStyleOptionViewItem>

#include "delegate.h"


//---------------------------------------------------
void Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                     const QModelIndex &index) const
{
    int val = 0;
#if 0
    QColor u[]={QColor(247,255,15,180),
                QColor(0,157,255,180),
                QColor(82,63,255,180),
                QColor(255,12,222,180),
                QColor(255,42,0,180),
                QColor(255,157,0,180),
                QColor(76,255,0,180),
                QColor(255,0,0,180)
               };
#endif
QColor u[]= {QColor(201,230,255,255), QColor(255,195,155,255), QColor(170,255,255,255),
            QColor(224,255,147,255),QColor(231,181,255,255),QColor(73,179,255,255),Qt::red};

    if (index.data().canConvert(QMetaType::Int))
    {
        val =  index.data().toInt();
        val = (int)floor(val/10);

        if(val < 0 || val >6)
            val = 7;

        painter->fillRect(option.rect, u[val]);
    }

    QStyleOptionViewItem maModif(option);
    maModif.palette.setColor(QPalette::WindowText, Qt::green);
    //maModif.palette.setColor(QPalette::Text,Qt::green);
    maModif.palette.setColor(QPalette::Text,Qt::red);
    QColor ItemForegroundColor = index.data(Qt::ForegroundRole).value<QColor>();

    // Remettre le texte initial sur la couleur
    QItemDelegate::paint(painter, maModif, index);
}
