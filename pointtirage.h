#ifndef POINTTIRAGE_H
#define POINTTIRAGE_H

#include <QGraphicsItem>

class PointTirage : public QGraphicsItem
{
  //Q_OBJECT
public:
  //explicit PointTirage();
  PointTirage();
  QRectF boundingRect() const;
  QPainterPath shape() const;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

signals:
  
public slots:

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent *event);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
  QPointF curPos;
};

#endif // POINTTIRAGE_H
