#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>

#include "pointtirage.h"

PointTirage::PointTirage() :
  QGraphicsItem()
{
  //setFlag(ItemIsMovable);
  //setFlag(ItemSendsGeometryChanges);
  setFlags(ItemIsSelectable);
  setCacheMode(DeviceCoordinateCache);
  setZValue(-1);
}

void PointTirage::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  qDebug() << "Click m1:" << event;

  if (event->button() == Qt::RightButton) {
    QPointF test = event->buttonDownScenePos(Qt::RightButton);
    qDebug() << test;
    setSelected(true);
  }

  update();
  QGraphicsItem::mousePressEvent(event);
}

void PointTirage::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  qDebug() << "Click m2:" << event->pos();
  update();
  QGraphicsItem::mouseReleaseEvent(event);
}

QRectF PointTirage::boundingRect() const
{

  qreal adjust = 0;
  return QRectF( -2 - adjust, -2 - adjust,
                 4 + adjust, 4 + adjust);

}

QPainterPath PointTirage::shape() const
{
  QPainterPath path;
  path.addEllipse(-2, -2, 4, 4);
  return path;
}

void PointTirage::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
  painter->setPen(Qt::NoPen);
  //painter->setBrush(Qt::darkGray);
  //painter->drawEllipse(-7, -7, 20, 20);

  QRadialGradient gradient(-3, -3, 10);
  if (option->state & QStyle::State_Sunken) {
    gradient.setCenter(0, 0);
    gradient.setFocalPoint(3, 3);
    gradient.setColorAt(1, QColor(Qt::yellow).light(120));
    gradient.setColorAt(0, QColor(Qt::darkYellow).light(120));
  } else {
    gradient.setColorAt(0, Qt::yellow);
    gradient.setColorAt(1, Qt::darkYellow);
  }
  painter->setBrush(gradient);

  painter->setPen(QPen(Qt::black, 0));
  painter->drawEllipse(-2, -2, 4, 4);
}
