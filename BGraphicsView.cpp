#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QSqlQuery>

#include "BGraphicsView.h"
#include "BPointTirage.h"

BGraphicsView::BGraphicsView(const stGameConf *pGame, etCount in_type, QBrush coul_fond )
    :gme_conf(pGame), type(in_type)
{
 db_0 = QSqlDatabase::database(pGame->db_ref->cnx);

 //Set-up the scene
 Scene = new QGraphicsScene;
 Scene->setSceneRect(0,0,800,600);
 this->setScene(Scene);

 this->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
 this->setRenderHint(QPainter::Antialiasing);

#if 0
 // a gradient background
 QLinearGradient gradient(QPointF(0, 0), QPointF(800, 600));
 gradient.setColorAt(0, Qt::lightGray);
 gradient.setColorAt(1, Qt::yellow);
 this->setBackgroundBrush(gradient);
#endif

 this->setBackgroundBrush(coul_fond);
 this->setCacheMode(CacheBackground);

 this->setViewportUpdateMode(FullViewportUpdate);
 this->setTransformationAnchor(AnchorUnderMouse);
 this->setDragMode(ScrollHandDrag);
}

void BGraphicsView::DessineCourbeSql(const stGameConf *pGame, etCount in_type, QColor cpen, int sqlIdY, int scale_y, int delta_y)
{
 switch (in_type) {
  case eCountCmb:
   draw_cmb(pGame);
   break;
  default:
      ;
 }
}

void BGraphicsView::draw_cmb(const stGameConf *pGame)
{
 QString msg = "";
 bool b_retVal = false;
 QSqlQuery query(db_0);

 BPointTirage *un_tirage = new BPointTirage(pGame);
 un_tirage->setPos(4,4);
 scene()->addItem(un_tirage);

 un_tirage = new BPointTirage(pGame);
 un_tirage->setPos(4,596);
 scene()->addItem(un_tirage);

 un_tirage = new BPointTirage(pGame);
 un_tirage->setPos(796,596);
 scene()->addItem(un_tirage);

 un_tirage = new BPointTirage(pGame);
 un_tirage->setPos(796,4);
 scene()->addItem(un_tirage);

 this->ensureVisible(this->rect());
}

void BGraphicsView::wheelEvent(QWheelEvent* event) {

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

