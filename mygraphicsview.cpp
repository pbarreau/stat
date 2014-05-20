

//Qt includes
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QTextStream>
#include <QScrollBar>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>

#include <math.h>

#include "MyGraphicsView.h"
#include "pointtirage.h"

MyGraphicsView::MyGraphicsView(QWidget *parent): QGraphicsView(parent)
{
  setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

  //Set-up the scene
  QGraphicsScene* Scene = new QGraphicsScene(this);
  setScene(Scene);
  setBackgroundBrush(Qt::yellow);
  setCacheMode(CacheBackground);
  setViewportUpdateMode(BoundingRectViewportUpdate);
  setRenderHint(QPainter::Antialiasing);
  setTransformationAnchor(AnchorUnderMouse);
  setWindowTitle(tr("Elastic Nodes"));

  //--------------
  //Populate the scene
  QSqlQuery sql_1;
  bool status = false;
  QString msg_1="select max(analyses.id), max(lstcombi.poids) from analyses , lstcombi;";

  //parent->setWindowTitle("Graphique");
  parent->setMinimumWidth(390);
  parent->setMinimumHeight(390);

  status = sql_1.exec(msg_1);
  if(status)
  {
    sql_1.first();
    if(sql_1.isValid())
    {
      int mx = sql_1.value(0).toInt() + 1;
      int my = ceil(sql_1.value(1).toDouble()) + 1;

      QSqlQuery sql_2;
      QString msg_2 = "select analyses.id, lstcombi.poids from analyses inner join lstcombi on analyses.id_poids = lstcombi.id;";
      status = sql_2.exec(msg_2);
      if(status)
      {
        sql_2.first();
        if(sql_2.isValid())
        {
          int sx = 0;
          double sy = 0.0;

          do
          {
            int x = sql_2.value(0).toInt()*2;
            double y = sql_2.value(1).toDouble()*5;
            PointTirage *ptir = new PointTirage;
            ptir->setPos(x,y);
            Scene->addItem(ptir);
            Scene->addRect(x, y, 1, 1);
            if(sx==0)
            {
              sx=x;
              sy=y;
            }
            Scene->addLine(x,y,sx,sy);
            sx=x;
            sy=y;
          }while(sql_2.next());
        }
      }
      //Set-up the view
      setSceneRect(0, 0, 2000, 500);

    }
  }


  //Use ScrollHand Drag Mode to enable Panning
  setDragMode(ScrollHandDrag);
  //--------------
}
#ifdef USE_WORKING
MyGraphicsView::MyGraphicsView(QWidget *parent): QGraphicsView(parent)
{
  setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

  //Set-up the scene
  QGraphicsScene* Scene = new QGraphicsScene(this);
  setScene(Scene);
  setBackgroundBrush(Qt::yellow);

  //--------------
  //Populate the scene
  for(int x = 0; x < 1000; x = x + 25) {
    for(int y = 0; y < 1000; y = y + 25) {

      if(x % 100 == 0 && y % 100 == 0) {
        Scene->addRect(x, y, 2, 2);

        QString pointString;
        QTextStream stream(&pointString);
        stream << "(" << x << "," << y << ")";
        QGraphicsTextItem* item = Scene->addText(pointString);
        item->setPos(x, y);
      } else {
        Scene->addRect(x, y, 1, 1);
      }
    }
  }

  //Set-up the view
  setSceneRect(0, 0, 1000, 1000);

  //Use ScrollHand Drag Mode to enable Panning
  setDragMode(ScrollHandDrag);
  //--------------
}
#endif

void MyGraphicsView::wheelEvent(QWheelEvent* event) {

  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

  // Scale the view / do the zoom
  double scaleFactor = 1.15;
  if(event->delta() > 0) {
    // Zoom in
    scale(scaleFactor, scaleFactor);
  } else {
    // Zooming out
    scale(1.0 / scaleFactor, 1.0 / scaleFactor);
  }

  // Don't call superclass handler here
  // as wheel is normally used for moving scrollbars
}

#ifdef USE_CODE
void MyGraphicsView::mouseMoveEvent(QMouseEvent* ev)
{
#if 0
  QPoint p = ev->pos();

  // recuperer l'item a la position specifie
  QGraphicsItem *item = itemAt(p);

  // mettre un tooltips
  item->setToolTip("dd");
#endif
}

void MyGraphicsView::mousePressEvent(QMouseEvent *event)
{

  QGraphicsItem *item = itemAt(event->pos());

  if (item) {
    qDebug() << "You clicked on item" << item;
    //QPointF cord= item->mapFromItem();
    //qDebug() << "item" << cord;
  } else {
    qDebug() << "You didn't click on an item.";
  }

  //item->setToolTip("Un texte");

}

void MyGraphicsView::slot_itemSelected(QGraphicsItem *item)
{
  qDebug() << "Click item:" << item;
}
#endif

void MyGraphicsView::mousePressEvent(QMouseEvent *event)
{
  qDebug() << "Click 3:" << event;
  if(event->button() == Qt::RightButton)
  {
    //QPoint p1 = event->pos();
    QPointF p2 = event->pos();
    //qDebug() << "p1:" << p1;
    qDebug() << "p2:" << p2;
  }

  //QGraphicsScene::mousePressEvent(event);
  //QGraphicsScene  + QGraphicsSceneMouseEvent
  // http://stackoverflow.com/questions/10082571/mousepressevent-in-view-and-items-in-qt
}


#if 0
void MyGraphicsView::hoverEnterEvent ( QGraphicsSceneHoverEvent * event )
{
  QPointF ref = event->scenePos();
  qDebug() << "Click 1:" << ref;
}

void MyGraphicsView::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
  // do something

  QPointF ref = event->scenePos();
  qDebug() << "Click 2:" << ref;

  // call the parents's event
  //QGraphicsItem::mouseReleaseEvent(event);
}

#endif
