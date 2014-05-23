

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

#include "mygraphicsview.h"
#include "pointtirage.h"

MyGraphicsView::MyGraphicsView(NE_FDJ::E_typeJeux leJeu, QGraphicsView *ptr_view, QWidget *parent)//: QGraphicsView(parent)
{
  setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

  //Set-up the scene
  QGraphicsScene* Scene = new QGraphicsScene;
  ptr_view->setScene(Scene);
  ptr_view->setBackgroundBrush(Qt::yellow);
  ptr_view->setCacheMode(CacheBackground);
  ///setViewportUpdateMode(BoundingRectViewportUpdate);
  ptr_view->setViewportUpdateMode(FullViewportUpdate);
  ptr_view->setRenderHint(QPainter::Antialiasing);
  ptr_view->setTransformationAnchor(AnchorUnderMouse);
  ptr_view->setDragMode(ScrollHandDrag);
  ptr_view->setWindowTitle(tr("Elastic Nodes"));


  //--------------
  //Populate the scene
  QSqlQuery sql_1;
  bool status = false;
  QString msg_1="select max(analyses.id), max(lstcombi.poids) from analyses , lstcombi;";

  parent->setWindowTitle("Graphique");
  parent->setMinimumWidth(390);
  parent->setMinimumHeight(390);

  status = sql_1.exec(msg_1);
  if(status)
  {
    sql_1.first();
    if(sql_1.isValid())
    {
      //int mx = sql_1.value(0).toInt() + 1;
      //int my = ceil(sql_1.value(1).toDouble()) + 1;

      QSqlQuery sql_2;
      QString msg_2 = "select analyses.id, lstcombi.poids, lstcombi.pos from analyses inner join lstcombi on analyses.id_poids = lstcombi.id;";
      status = sql_2.exec(msg_2);
      if(status)
      {
        sql_2.first();
        if(sql_2.isValid())
        {
          int sx = 0;
          double sy = 0.0;
          int mem_crenau = 0;
          QColor cpen = Qt::black;

          do
          {
            int x = sql_2.value(0).toInt()* C_COEF_X;
            double y = sql_2.value(1).toDouble()*C_COEF_Y;
            int crenau = sql_2.value(2).toInt()*C_COEF_C;

            switch(crenau/C_COEF_C)
            {
              case 0:
                cpen = Qt::red;
                break;
#if 0
              case 1:
                cpen = Qt::green;
                break;
              case 2:
                cpen = Qt::gray;
                break;
              case 3:
                cpen = Qt::magenta;
                break;
              case 4:
                cpen = Qt::blue;
                break;
#endif
              default:
                cpen = Qt::black;
            }

            // Ajount d'une boule representant un tirage
            PointTirage *ptir = new PointTirage(leJeu);
            ptir->setPos(x,y);
            Scene->addItem(ptir);

            if(sx==0)
            {
              sx=x;
              sy=y;
              mem_crenau = crenau;
            }

            // Courbe des combis
            //QLineF L1(sx,mem_crenau,x,mem_crenau);
            //QGraphicsLineItem *L2 = new QGraphicsLineItem(x,crenau,x,crenau);

            //Scene->addLine(L1,QPen(cpen));
            //Scene->addLine(x,crenau,x,crenau);
            //mem_crenau = crenau;

            // Courbe  des tirages
            Scene->addLine(x,y,sx,sy);
            sx=x;
            sy=y;
          }while(sql_2.next());
        }
      }
      //Set-up the view
      //setSceneRect(0, 0, 5000, 5000);

    }
  }

}

void MyGraphicsView::wheelEvent(QWheelEvent* event) {

  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
  //this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

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

////------------------------------------------------
UnConteneurDessin::UnConteneurDessin(QWidget *parent): QGraphicsView(parent)
{

}

void UnConteneurDessin::wheelEvent(QWheelEvent* event) {

  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
  //this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

  // Scale the view / do the zoom
  double scaleFactor = 1.15;
  if(event->delta() > 0) {
    // Zoom in
    scale(scaleFactor, scaleFactor);
  } else {
    // Zooming out
    scale(1.0 / scaleFactor, 1.0 / scaleFactor);
  }

  //setSceneRect(0, 0, 2000, 500);
  // Don't call superclass handler here
  // as wheel is normally used for moving scrollbars
}
