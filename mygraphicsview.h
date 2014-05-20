#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsRectItem>

#include <QGraphicsSceneMouseEvent>

class MyGraphicsView: public QGraphicsView
{
public:
  MyGraphicsView(QWidget* parent = NULL);

public slots:
  void slot_itemSelected(QGraphicsItem *item);

//public:

protected:
  //Take over the interaction
  virtual void wheelEvent(QWheelEvent* event);
  //virtual void mouseMoveEvent(QMouseEvent*);
  void mousePressEvent(QMouseEvent *event);
  //void mousePressEvent(QGraphicsSceneMouseEvent *event);
  //virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
  //virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
  //virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

};

#endif // MYGRAPHICSVIEW_H
