#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsRectItem>

#include <QGraphicsSceneMouseEvent>
#include "tirages.h"

class MyGraphicsView: public QGraphicsView
{
public:
  MyGraphicsView(QWidget* parent = NULL,tirages *pref=NULL);

public slots:

protected:
  virtual void wheelEvent(QWheelEvent* event);

};

#endif // MYGRAPHICSVIEW_H
