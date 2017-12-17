#include <QItemDelegate>
#include <QPainter>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

#include "delegate.h"


//---------------------------------------------------
void Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                     const QModelIndex &index) const
{
    int val = 0;
    QColor u[]= {QColor(201,230,255,255), QColor(255,195,155,255), QColor(170,255,255,255),
                 QColor(224,255,147,255),QColor(231,181,255,255),QColor(73,179,255,255),Qt::red};
    QColor p[]= {Qt::gray,Qt::red,Qt::black,Qt::black};

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
            switch(pen)
            {
            case 1:
            case 2:
            {
                QString lab = index.model()->data(index,Qt::DisplayRole).toString();
                QTextDocument doc;
                doc.setHtml(QString("<html><strong>%1</strong></html>").arg(lab));
                doc.setTextWidth(maModif.rect.width());

                painter->save();
                painter->translate(maModif.rect.left(), maModif.rect.top());
                QRect clip(0, 0, maModif.rect.width(), maModif.rect.height());
                painter->setClipRect(clip);

                QAbstractTextDocumentLayout::PaintContext ctx;
                ctx.palette.setColor(QPalette::Text, p[pen]);
                ctx.clip = clip;
                doc.documentLayout()->draw(painter, ctx);
                painter->restore();
            }
                break;
            default:
                maModif.palette.setColor(QPalette::Text,p[pen]);
                QItemDelegate::paint(painter, maModif, index);
                break;
            }

        }
    }

    //maModif.text = "";
    //maModif.widget->style()->drawControl(QStyle::CE_ItemViewItem, &maModif, painter);

    //painter->translate(maModif.rect.left(), maModif.rect.top());
    //QRect clip(0, 0, maModif.rect.width(), maModif.rect.height());
    //doc.drawContents(painter, clip);
    //painter->restore();
    //QColor ItemForegroundColor = index.data(Qt::ForegroundRole).value<QColor>();

}
