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

#define SCALE_X 10

class BPointTirage : public QGraphicsItem
{
 public:
 BPointTirage(const stGameConf *pGame, int zn, int lgn_id, int pos_x, double pos_y);
 int x(void);
 double y(void);
 int lgn(void);
 int zn(void);

 protected:
 QRectF boundingRect() const;
 QPainterPath shape() const;
 void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
 void mousePressEvent(QGraphicsSceneMouseEvent *event);
 void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

 private:
 int zn_id;
 int lgn_id;
 int v_x;
 double v_y;
 static QList<QGraphicsLineItem *> lst_lignes;
 QSqlDatabase db_0;

};

#endif // BPOINTTIRAGE_H
