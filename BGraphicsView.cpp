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
 //Scene->setSceneRect(0,0,800,600);
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

 msg = "select min(t1.id) as min_x, max(t1.id) as max_x, min(t2.idcomb) as min_y, max (t2.idComb) as max_y from B_fdj as t1, B_ana_z1 as t2 where(t1.id=t2.id)";
 b_retVal = query.exec(msg);
 if(b_retVal && (b_retVal=query.first())){
  int mx = query.value(1).toInt();
  qreal kx = (800/mx)+4;
  int my = query.value(3).toInt();
  qreal ky = ((300/my)+4);

	msg = "select t1.id as X, t2.idComb as Y from B_fdj as t1, B_ana_z1 as t2 where(t1.id=t2.id)";
	b_retVal = query.exec(msg);
	if(b_retVal && (b_retVal=query.first())){
	 qreal sx = -1;
	 qreal sy = -1;
	 bool start_line = false;
	 do{
		int v_x = query.value(0).toInt()*15;
		//qreal p_x = v_x * kx;
		double v_y = this->height() - ((query.value(1).toInt()*0.5));
		//qreal p_y = 600 - (v_y * ky);

		BPointTirage *un_tirage = new BPointTirage(pGame);
		un_tirage->setPos(v_x,v_y);
		scene()->addItem(un_tirage);

		if(start_line){
		 QLineF L1(v_x,v_y,sx,sy);
		 Scene->addLine(L1,QPen(Qt::red));
		}
		sx = v_x;
		sy = v_y;
		start_line = true;
	 }while(query.next());
	}
 }

/*
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
*/
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

