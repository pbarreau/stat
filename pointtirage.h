#ifndef POINTTIRAGE_H
#define POINTTIRAGE_H

#include <QGraphicsItem>

#define C_COEF_X  2
#define C_COEF_Y  5

class PointTirage : public QGraphicsItem
{
  //Q_OBJECT
public:
  //explicit PointTirage();
  PointTirage();
  QRectF boundingRect() const;
  QPainterPath shape() const;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *item);

signals:
  
public slots:

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent *event);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
  QPointF curPos;
};

#endif // POINTTIRAGE_H
