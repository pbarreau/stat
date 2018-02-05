#include <QItemDelegate>
#include <QPainter>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QLineF>

#include <QSqlQueryModel>

#include "delegate.h"


//---------------------------------------------------
void BDelegateStepper::paint(QPainter *painter, const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    QStyleOptionViewItem maModif(option);
    QLineF angleline;

    int val = 0;
    int bg = 0;

    QColor u[]= {QColor(201,230,255,255), QColor(255,195,155,255), QColor(170,255,255,255),
                 QColor(224,255,147,255),QColor(231,181,255,255),QColor(73,179,255,255),
                 Qt::red,Qt::white};
    QColor p[]= {Qt::gray,Qt::red,Qt::black,Qt::black};
    QPalette t(u[1]);

    int cx = maModif.rect.width()/4;
    int cy = maModif.rect.height()/2;
    int refx = maModif.rect.topLeft().x();
    QPoint c(refx +cx*3,maModif.rect.topLeft().y()+cy);

    /// Mettre une couleur en fonction du groupe u,dizaine,v,...
    if(index.model()->index(index.row(),0).data().canConvert(QMetaType::Int))
    {
        val =  index.model()->index(index.row(),0).data().toInt();
        val = (int)floor(val/10);

        if(val < 0 || val >6)
            val = (sizeof(u)/sizeof(QColor))-2;

        // Faut il changer le fond de la case
        if(index.model()->index(index.row(),2).data().canConvert(QMetaType::Int))
        {
            // Ok info de background
            bg = index.model()->index(index.row(),2).data().toInt();
            if(bg !=0)
            {
                val = (sizeof(u)/sizeof(QColor))-1;
            }
        }


        painter->fillRect(option.rect, u[val]);
    }


    if (option.state & QStyle::State_Selected)
    {
        painter->fillRect(option.rect, t.highlight());

        /// test
        /*        gradient.setCenter(0, 0);
        gradient.setFocalPoint(3, 3);
        gradient.setColorAt(1, QColor(Qt::red).light(120));
        gradient.setColorAt(0, QColor(Qt::darkYellow).light(120));
*/
        //QRadialGradient gradient(cx, cy, maModif.rect.bottomRight().rx());

        //painter->setBrush(gradient);
        //painter->setPen(QPen(Qt::black, 0));
        painter->save();
        painter->setBrush(Qt::red);
        painter->drawEllipse(c,cx/2,cy/2);
        painter->restore();
    }

    // A t on un chiffre (boule)?



    if(index.model()->index(index.row(),1).data().canConvert(QMetaType::Int))
    {

        int pen = index.model()->index(index.row(),1).data().toInt();
        if (pen >=0 && pen < 4)
        {
            switch(pen)
            {

            case 1:
                //test diagonale
                angleline.setPoints(maModif.rect.topLeft(), maModif.rect.bottomRight());
                painter->drawLine(angleline);
                painter->save();
                painter->setBrush(Qt::green);
                painter->drawEllipse(c,cx/2,cy/2);
                painter->restore();


            case 2:
            {

                QString lab = index.model()->data(index,Qt::DisplayRole).toString();
                QTextDocument doc;
                doc.setHtml(QString("<html><strong>%1</strong></html>").arg(lab));
                //doc.setTextWidth(maModif.rect.width());

                painter->save();
                painter->translate(maModif.rect.left(), maModif.rect.top());
                QRect clip(0, 0, maModif.rect.width(), maModif.rect.height());
                painter->setClipRect(clip);

                QAbstractTextDocumentLayout::PaintContext ctx;
                ctx.palette.setColor(QPalette::Text, p[pen]);
                ctx.clip = clip;
                doc.documentLayout()->draw(painter, ctx);

                painter->setBrush(Qt::yellow);
                painter->drawEllipse(c,cx/2,cy/2);

                painter->restore();
            }
                break;

            case 3:
                //test diagonale
                angleline.setPoints(maModif.rect.topLeft(), maModif.rect.bottomRight());
                painter->drawLine(angleline);
                maModif.palette.setColor(QPalette::Text,p[pen]);
                QItemDelegate::paint(painter, maModif, index);
                painter->save();
                painter->setBrush(Qt::magenta);
                painter->drawEllipse(c,cx/2,cy/2);
                painter->restore();

                break;

            default:
                //test diagonale
                angleline.setPoints(maModif.rect.topRight(), maModif.rect.bottomLeft());
                //angleline.setLine();
                painter->drawLine(angleline);

                maModif.palette.setColor(QPalette::Text,p[pen]);
                QItemDelegate::paint(painter, maModif, index);

                painter->save();
                painter->setBrush(Qt::blue);
                painter->drawEllipse(c,cx/2,cy/2);
                painter->restore();

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

void BDelegateElmOrCmb::paint(QPainter *painter, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    int col = index.column();
    int row = index.row();
    int nbCol = index.model()->columnCount();
    int val = 0;
    QStyleOptionViewItem maModif(option);
    QColor u[]= {QColor(201,230,255,255),QColor(200,170,100,140)};


    /// Regarder la valeur de la derniere colonne
    /// Elle indique que mettre comme couleur
    if(index.model()->index(index.row(),nbCol-1).data().canConvert(QMetaType::Int))
    {
        val =  index.model()->index(index.row(),nbCol-1).data().toInt();
    }


    switch(col)
    {
    case 0: /// Filtre active sur le cas
    {
        if (val & 0x2)
        {
            painter->fillRect(option.rect, u[0]);
        }
    }
        break;
    case 1: /// C'est le dernier tirage
    {
        if (val & 0x1)
        {
            painter->fillRect(option.rect, u[1]);
        }

    }
        break;
    default:
        break;
    }


    QItemDelegate::paint(painter, maModif, index);
}


void BDelegateFilterGrp::paint(QPainter *painter, const QStyleOptionViewItem &option,
                               const QModelIndex &index) const
{
    int col = index.column();
    int row = index.row();
    int nbCol = index.model()->columnCount();
    int val = 0;
    QStyleOptionViewItem maModif(option);
    QColor u[]= {QColor(201,230,255,255),QColor(200,170,100,140)};


    /// Regarder la valeur de la derniere colonne
    /// Elle indique que mettre comme couleur
    if(index.model()->index(index.row(),nbCol-1).data().canConvert(QMetaType::Int))
    {
        val =  index.model()->index(index.row(),nbCol-1).data().toInt();
    }


    if(col>0 && col<(nbCol-1)){
        /// bit actif
        if(val & (1<<col-1)){
            painter->fillRect(option.rect, u[0]);
        }
    }
    QItemDelegate::paint(painter, maModif, index);
}

BSqmColorizePriority::BSqmColorizePriority(QObject *parent):QSqlQueryModel(parent)
{

}

QVariant BSqmColorizePriority::data(const QModelIndex &index, int role) const
{
    QColor u[]= {
        Qt::black,
        Qt::red,
        Qt::green,
        QColor(255,216,0,255),
        QColor(255,106,0,255),
        QColor(178,0,255,255)};

    if(index.column()== 0 )
    {
        int nbCol=index.model()->columnCount();

        /// recuperation de l'info donnant la couleur
        QModelIndex priority = index.sibling(index.row(),nbCol-2);


        /// Choix de la couleur a appliquer
        if(priority.data().canConvert(QMetaType::Int)){
            int val = priority.data().toInt();
            if (role == Qt::TextColorRole){
                return (u[val]);
            }
        }
    }
    return QSqlQueryModel::data(index,role);
}

