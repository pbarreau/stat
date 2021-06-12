#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QSqlQuery>
#include <QToolTip>

#include "BFpmFdj.h"
#include "BGraphicsView.h"
#include "BPointTirage.h"
#include "pointtirage.h"

BGraphicsView::BGraphicsView(const stGameConf *pGame, BView *lesTirages, QBrush coul_fond )
    :gme_conf(pGame), tirages(lesTirages)
{
 db_0 = QSqlDatabase::database(pGame->db_ref->cnx);

 int nb_zone = pGame->znCount;
 dessin = new QMap<int,QGraphicsItemGroup *> *[nb_zone] ;
 for (int i=0;i<nb_zone;i++) {
  dessin[i]=new QMap<int,QGraphicsItemGroup *>;
 }

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

QGraphicsItemGroup *BGraphicsView::getLine(int zn,int l_id)
{
 QGraphicsItemGroup *ret_ligne;

 ret_ligne = dessin[zn]->value(l_id);

 return ret_ligne;
}

QGraphicsScene *BGraphicsView::getScene()
{
 return Scene;
}

void BGraphicsView::DessineCourbeSql(const stGameConf *pGame, int zn, int lgn_id, QColor pen_id, int sqlIdY, int scale_y, int delta_y)
{
 draw_cmb(pGame,zn,lgn_id);
}

void BGraphicsView::draw_cmb(const stGameConf *pGame, int zn, int lgn_id, QColor pen_id)
{
 QString msg = "";
 bool b_retVal = false;
 QSqlQuery query(db_0);
 QGraphicsItemGroup* gr = new QGraphicsItemGroup;

 QStringList keys = pGame->slFlt[zn][Bp::colDefTitres];
 QString key = keys[lgn_id];
 if(key.contains(",")){
  key = key.split(",").at(0);
 }

 int nb_items_z0 = pGame->limites[0].len;
 int nb_items_z1 = pGame->limites[1].len;

 int on_lgn = lgn_id;

 QColor cur_pen_id = Qt::red;
 if(zn==1){
  cur_pen_id = Qt::blue;
  int delta = pGame->slFlt[0][0].size() - pGame->slFlt[1][0].size();
  if(lgn_id > delta ){
   on_lgn = lgn_id + delta;
  }
 }
 //msg = "select min(t1.id) as min_x, max(t1.id) as max_x, min(t2.idcomb) as min_y, max (t2.idComb) as max_y from B_fdj as t1, B_ana_z1 as t2 where(t1.id=t2.id)";
 msg = "";
 msg = msg + "select min(t1.id) as min_x, max(t1.id) as max_x,\n";
 msg = msg + "min(t2."+key+") as min_y, max (t2."+key+") as max_y\n";
 msg = msg + "from B_fdj as t1, B_ana_z"+QString::number(zn+1)+" as t2 where(t1.id=t2.id)";

 b_retVal = query.exec(msg);
 if(b_retVal && (b_retVal=query.first())){
  int Max_x = query.value(1).toInt();
  int Max_y = query.value(3).toInt();

	//scene()->setSceneRect(0,-800,Max_x*10,800);
	if(Max_x == 0) Max_x = 1;
	if(Max_y == 0) Max_y = 1;

  qreal kx = 10;
  qreal ky = 10;


	//msg = "select t1.id as X, t2.idComb as Y from B_fdj as t1, B_ana_z1 as t2 where(t1.id=t2.id)";
	msg = "";
	msg = msg + "select t1.id as X,\n";
	msg = msg + "t2."+key+" as Y\n";
	msg = msg + "from B_fdj as t1, B_ana_z"+QString::number(zn+1)+" as t2 where(t1.id=t2.id)";
	b_retVal = query.exec(msg);
	if(b_retVal && (b_retVal=query.first())){
	 qreal sx = -1;
	 qreal sy = -1;
	 bool start_line = false;
	 do{
		int pos_x = query.value(0).toInt();
		int v_x = pos_x*10;
		//qreal p_x = v_x * kx;
		int hauteur = this->height();
		//double v_y = hauteur - ((query.value(1).toInt()*ky))-(lgn_id*10);
		int val = query.value(1).toInt();
		int ref = pGame->limites[zn].len;
		if(on_lgn > keys.size() -2){
		 val = ref * val / Max_y;
		 //val = log(val);
		}
		double v_y = hauteur - ((val*ky) - (nb_items_z0+1)*4*zn) + (10 * on_lgn*((nb_items_z0+1)+nb_items_z1+1));

		BPointTirage *un_tirage = new BPointTirage(pGame,zn,lgn_id,pos_x,v_y);
		scene()->addItem(un_tirage);
		gr->addToGroup(un_tirage);

		if(start_line){
		 QLineF L1(v_x,v_y,sx,sy);
		 QGraphicsLineItem *une_ligne = Scene->addLine(L1,QPen(cur_pen_id));
		 une_ligne->setZValue(lgn_id);
		 gr->addToGroup(une_ligne);
		}
		sx = v_x;
		sy = v_y;
		start_line = true;
	 }while(query.next());
	}
 }
 dessin[zn]->insert(lgn_id,gr);
 //scene()->addItem(gr);

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

void BGraphicsView::mousePressEvent(QMouseEvent *event)
{
 QGraphicsItem *un_item = nullptr;
 BPointTirage *un_tirage = nullptr;

#if 0
 QGraphicsScene *monDessin = scene();
 QGraphicsItem * mon_item = monDessin->mouseGrabberItem();
#endif

 QList<QGraphicsItem *> lst_items;
 int nbr_items = 0;

 QPoint click_pos = event->pos();

 lst_items = items(click_pos);
 nbr_items = lst_items.size();

 if(nbr_items <= 1){
  QGraphicsView::mousePressEvent(event);
  return;
 }

 /// On se positionne sur notre classe BPointTirage
 un_item = lst_items.at(nbr_items-2);

 //this->centerOn(un_item);
 //un_item->setFocus();
 QPointF value = un_item->scenePos();

 un_tirage = static_cast<BPointTirage *>(un_item);
 static int prev_x = -1;

 int cur_x = un_tirage->x();
 double cur_y = un_tirage->y();
 int cur_zn = un_tirage->zn();
 int cur_lgn = un_tirage->lgn();

 Qt::MouseButtons la_souris = event->button();

 if(event->button() == Qt::LeftButton)
 {
  if(prev_x == cur_x){
   return;
  }
  else
  {
   prev_x = cur_x;
  }

  BView * ptr_qtv = tirages;


  //ptr_qtv->setSelectionBehavior(QAbstractItemView::SelectItems);

  BFpmFdj * fpm_tmp = qobject_cast<BFpmFdj *>( ptr_qtv->model());
  fpm_tmp->sort(0);
  ptr_qtv->scrollTo(fpm_tmp->index(cur_x-1,0));

  QAbstractItemView::SelectionBehavior prevBehav = ptr_qtv->selectionBehavior();
  ptr_qtv->setStyleSheet("QTableView {selection-background-color: red;}");
  ptr_qtv->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ptr_qtv->setSelectionMode(QAbstractItemView::SingleSelection);
  ptr_qtv->setSelectionBehavior(QAbstractItemView::SelectRows);
  ptr_qtv->selectRow(cur_x-1);
  ptr_qtv->setSelectionBehavior(prevBehav);
#if 0
  //QAbstractItemModel *mon_model = ptr_qtv->model();
  //QModelIndex item1 = mon_model->index(cur_x,0, QModelIndex());

  ptr_qtv->setAutoScroll(true);
  ptr_qtv->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);

  ptr_qtv->setCurrentIndex(item1);
  ptr_qtv->scrollTo(item1);
  ptr_qtv->selectRow(cur_x);
#endif

 }

 if(event->button() == Qt::RightButton)
 {
  QGraphicsLineItem *lgn_tmp=NULL;
  if(un_item->cursor()==Qt::SizeHorCursor){
  scene()->removeItem(un_item);
  }
  else
  {
   foreach (QGraphicsItem *piece, lst_items) {
    int un_type=piece->type();

    if(piece->zValue() == 30){
     scene()->removeItem(piece);
     return;
    }
   }

   int value = un_item->type();
   int taille = scene()->width();
   QPen crayon = QPen(Qt::yellow);
   crayon.setStyle(Qt::DashLine);
   lgn_tmp = scene()->addLine(0,cur_y,taille,cur_y,crayon);
   value = lgn_tmp->type();
   lgn_tmp->setCursor(Qt::SizeHorCursor);
   lgn_tmp->setZValue(30);
  }
 }

 if(event->button() == Qt::MiddleButton)
 {
 }
 update();
 QGraphicsView::mousePressEvent(event);
}

void BGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
 //static bool hide = false;
 static QGraphicsLineItem *lgn_tmp=NULL;

 static BPointTirage *prv_item = nullptr;
 BPointTirage *un_item = nullptr;

 QList<QGraphicsItem *> lst_items;
 int nbr_items = 0;


 QPoint mouse_pos = event->pos();
 lst_items = items(mouse_pos);
 nbr_items = lst_items.size();

 if(nbr_items <= 1){
  QToolTip::hideText();
  if(lgn_tmp){
   scene()->removeItem(lgn_tmp);
   lgn_tmp = nullptr;
  }
  QGraphicsView::mouseMoveEvent(event);
  return;
 }

 if(lst_items.at(nbr_items-2)->acceptHoverEvents() == false){
  QToolTip::hideText();
  if(lgn_tmp){
   scene()->removeItem(lgn_tmp);
   lgn_tmp = nullptr;
  }
  QGraphicsView::mouseMoveEvent(event);
  return;
 }

 /// On se positionne sur notre classe BPointTirage
 un_item = static_cast<BPointTirage *>(lst_items.at(nbr_items-2));

#if 0
 /// On trace une droite
 QPen crayon = QPen(Qt::magenta);
 crayon.setStyle(Qt::DotLine);
 int hauteur = scene()->height();
 lgn_tmp = scene()->addLine(un_item->x()*10,0,un_item->x()*10,hauteur,crayon);
#endif

#if 0
 if(un_item==prv_item){
  QGraphicsView::mouseMoveEvent(event);
  return;
 }else
 {
  prv_item=un_item;
  //QToolTip::hideText();
  QToolTip::showText (QCursor::pos(), "msgOK");
 }
#endif

 int zn = un_item->zn();
 int id_col = un_item->lgn();
 int id_lgn = un_item->x();
 QStringList keys = gme_conf->slFlt[zn][Bp::colDefTitres];
 int mx_col = keys.size();
 QString key = keys[id_col];
 if(key.contains(",")){
  key = key.split(",").at(0);
 }

 bool b_retVal = false;
 QSqlQuery query(db_0);
 QString msg = "";

 if(id_col == mx_col-1){
  msg = "select tip from b_cmb_z"+QString::number(zn+1)
        + " as t1, b_ana_z"+QString::number(zn+1)
        +" as t2 where (t2.id="
        +QString::number(id_lgn)+" and t1.id=t2."+key+")";
 }
 else
 {
  msg = "select " + key
        + " from b_ana_z"+QString::number(zn+1)
        +" as t2 where (t2.id="
        +QString::number(id_lgn)+")";
 }

 b_retVal = query.exec(msg);
 if(b_retVal && (b_retVal=query.first())){
  msg = gme_conf->names[zn].std+ ", "+ key + " : " + query.value(0).toString();
 }
 QToolTip::showText (QCursor::pos(), msg);
 QGraphicsView::mouseMoveEvent(event);
}
