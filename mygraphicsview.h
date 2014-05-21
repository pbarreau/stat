#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsRectItem>

#include <QGraphicsSceneMouseEvent>
#include "tirages.h"

class MyGraphicsView: public QGraphicsView
{
public:
  MyGraphicsView(NE_FDJ::E_typeJeux leJeu=NE_FDJ::fdj_euro,QGraphicsView *ptr_view=NULL, QWidget* parent = NULL);

public slots:

protected:
  virtual void wheelEvent(QWheelEvent* event);

};

class UnConteneurDessin: public QGraphicsView
{
public:
  UnConteneurDessin(QWidget* parent = NULL);


protected:
  virtual void wheelEvent(QWheelEvent* event);

};

#endif // MYGRAPHICSVIEW_H
