#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QGraphicsScene>

#include "BPointTirage.h"

QList<QGraphicsLineItem *> BPointTirage::lst_lignes;

BPointTirage::BPointTirage(const stGameConf *pGame)
{
 db_0 = QSqlDatabase::database(pGame->db_ref->cnx);

 setFlags(ItemIsSelectable);
 setCacheMode(DeviceCoordinateCache);
 setZValue(-1);

}

void BPointTirage::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
 if(event->button() == Qt::LeftButton)
 {
  //TST_ToolTipsInfotirage(event);
 }

 if(event->button() == Qt::RightButton)
 {
  //TST_TracerLigne(event);
 }

 if(event->button() == Qt::MiddleButton)
 {
  QGraphicsLineItem *lgn_tmp=nullptr;
  // Effacer les traits si present
  while(!lst_lignes.isEmpty()) {
   lgn_tmp = lst_lignes.at(0);
   this->scene()->removeItem(lgn_tmp);
   //delete (lst_lignes.at(0));
   lst_lignes.removeAt(0);
  }
 }
 update();
 QGraphicsItem::mousePressEvent(event);

}

void BPointTirage::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
#ifndef QT_NO_DEBUG
 //qDebug() << "Click m2:" << event->pos();
#endif
 update();
 QGraphicsItem::mouseReleaseEvent(event);
}

QRectF BPointTirage::boundingRect() const
{

 qreal adjust = 0;
 return QRectF( -2 - adjust, -2 - adjust,
               4 + adjust, 4 + adjust);

}

QPainterPath BPointTirage::shape() const
{
 QPainterPath path;
 path.addEllipse(-2, -2, 4, 4);
 return path;
}

void BPointTirage::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
 painter->setPen(Qt::NoPen);

 QRadialGradient gradient(-3, -3, 10);
 if (option->state & QStyle::State_Sunken) {
  gradient.setCenter(0, 0);
  gradient.setFocalPoint(3, 3);
  gradient.setColorAt(1, QColor(Qt::red).light(120));
  gradient.setColorAt(0, QColor(Qt::darkYellow).light(120));
 } else {
  gradient.setColorAt(0, Qt::yellow);
  gradient.setColorAt(1, Qt::darkYellow);
 }
 painter->setBrush(gradient);

 painter->setPen(QPen(Qt::black, 0));
 painter->drawEllipse(-2, -2, 4, 4);
}
