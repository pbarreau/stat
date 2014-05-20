#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsRectItem>

class MyGraphicsView: public QGraphicsView
{
public:
  MyGraphicsView(QWidget* parent = NULL);

public slots:
  void slot_itemSelected(QGraphicsItem *item);

protected:
    //Take over the interaction
    virtual void wheelEvent(QWheelEvent* event);
    virtual void mouseMoveEvent(QMouseEvent*);
    void mousePressEvent(QMouseEvent *event);
};

#endif // MYGRAPHICSVIEW_H
