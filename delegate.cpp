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
    QColor u[]= {QColor(201,230,255,255), QColor(255,195,155,255), QColor(170,255,255,255),
                 QColor(224,255,147,255),QColor(231,181,255,255),QColor(73,179,255,255),Qt::red};
    QColor p[]= {Qt::black,Qt::red,Qt::green,QColor(0,148,255,255)};

    //    if (index.data().canConvert(QMetaType::Int))
    if(index.model()->index(index.row(),0).data().canConvert(QMetaType::Int))
    {
        val =  index.model()->index(index.row(),0).data().toInt();
        val = (int)floor(val/10);

        if(val < 0 || val >6)
            val = 7;

        painter->fillRect(option.rect, u[val]);
    }

    QStyleOptionViewItem maModif(option);
    if(index.model()->index(index.row(),1).data().canConvert(QMetaType::Int))
    {
        int pen = index.model()->index(index.row(),1).data().toInt();
        if (pen >=0 && pen < 4)
        {
            maModif.palette.setColor(QPalette::Text,p[pen]);
        }

    }
    //QColor ItemForegroundColor = index.data(Qt::ForegroundRole).value<QColor>();

    // Remettre le texte initial sur la couleur
    QItemDelegate::paint(painter, maModif, index);
}
