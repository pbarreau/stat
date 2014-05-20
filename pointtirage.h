#ifndef POINTTIRAGE_H
#define POINTTIRAGE_H

#include <QGraphicsItem>

#include "tirages.h"

#define C_COEF_X  10
#define C_COEF_Y  1

class PointTirage : public QGraphicsItem
{
  //Q_OBJECT
public:
  //explicit PointTirage();
  PointTirage(tirages *pref=NULL);
  QRectF boundingRect() const;
  QPainterPath shape() const;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);

signals:
  
public slots:

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent *event);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
  QPointF curPos;
  tirages *tirRef;
  stTiragesDef tirDef;
};

#endif // POINTTIRAGE_H
