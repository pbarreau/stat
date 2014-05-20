#ifndef QT_NO_DEBUG
#include <QDebug>
#endif


#include "pointtirage.h"

PointTirage::PointTirage() :
  QGraphicsItem()
{
  setZValue(-1);
  setFlags(QGraphicsItem::ItemIsSelectable);
}

void PointTirage::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  qDebug() << "Click m1:" << event;
    update();
    QGraphicsItem::mousePressEvent(event);
}

void PointTirage::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  qDebug() << "Click m2:" << event;
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}
