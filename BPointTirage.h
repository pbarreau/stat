#ifndef BPOINTTIRAGE_H
#define BPOINTTIRAGE_H

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QRectF>
#include <QPainterPath>
#include <QPainter>
#include <QStyleOptionGraphicsItem>


#include <QList>
#include "game.h"

class BPointTirage : public QGraphicsItem
{
 public:
 BPointTirage(const stGameConf *pGame);

 protected:
 QRectF boundingRect() const;
 QPainterPath shape() const;
 void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
 void mousePressEvent(QGraphicsSceneMouseEvent *event);
 void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

 private:
 //QPointF curPos;
 static QList<QGraphicsLineItem *> lst_lignes;
 QSqlDatabase db_0;

};

#endif // BPOINTTIRAGE_H
