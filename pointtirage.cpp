#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QToolTip>

#include <QSqlQuery>
#include <QSqlRecord>

#include "pointtirage.h"
tirages *PointTirage::tirRef;
stTiragesDef PointTirage::tirDef;
QList<QGraphicsLineItem *> PointTirage::lst_lignes;


PointTirage::PointTirage(NE_FDJ::E_typeJeux leJeu, eGType sceneType) :
  QGraphicsItem()
{
  tirRef = new tirages(leJeu);

  stTiragesDef mesdef;
  tirRef->getConfig(&mesdef);

  tirDef = mesdef;

  switch(sceneType)
  {
    case eRepartition:
      ptrFunc = &PointTirage::TST_GetTirageOrdreFromPoint;
      break;
    case eParite:
      ptrFunc = &PointTirage::TST_GetTiragePariteFromPoint;
      break;
    case eGroupe:
      ptrFunc = &PointTirage::TST_GetTirageGroupeFromPoint;
      break;
    default:
      ptrFunc = NULL;
      break;
  }

  //setFlag(ItemIsMovable);
  //setFlag(ItemSendsGeometryChanges);
  setFlags(ItemIsSelectable);
  setCacheMode(DeviceCoordinateCache);
  setZValue(-1);
}

void PointTirage::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  if(event->button() == Qt::LeftButton)
  {
    TST_ToolTipsInfotirage(event);
  }

  if(event->button() == Qt::RightButton)
  {
    TST_TracerLigne(event);
  }

  if(event->button() == Qt::MiddleButton)
  {
    QGraphicsLineItem *lgn_tmp=NULL;
    // Effacer les traits si present
    while(!lst_lignes.isEmpty()) {
      lgn_tmp = lst_lignes.at(0);
      this->scene()->removeItem(lgn_tmp);
      //delete (lst_lignes.at(0));
      lst_lignes.removeAt(0);
    }
  }
  update();
  QGraphicsItem::mousePressEvent(event);

}

void PointTirage::TST_TracerLigne(QGraphicsSceneMouseEvent *event)
{
  QGraphicsLineItem *lgn_tmp=NULL;

  QPointF pos_item = scenePos();
  QPoint pos_item_screen = event->buttonDownScreenPos(Qt::RightButton);
  //int poids = pos_item.y()/C_COEF_Y;
  int taille = this->scene()->sceneRect().width();
  int coord = pos_item.y()*C_COEF_Y;

  lgn_tmp = this->scene()->addLine(0,coord,taille,coord,QPen(Qt::red));
  lst_lignes.append(lgn_tmp);

#ifndef QT_NO_DEBUG
  qDebug() << "Click :" << pos_item_screen;
  qDebug() << "Abs:" << pos_item.x()/C_COEF_X;
  qDebug() << "Ord_point:" << pos_item_screen.y()/C_COEF_Y;
  qDebug() << "Ord:" << pos_item_screen.y()/C_COEF_Y;
#endif
  QToolTip::showText(pos_item_screen,"QString::number(poids",0);
}

// Montre un point a partir d'un tirage
void PointTirage::TST_ShowPointFromTirage()
{

}

QString PointTirage::TST_GetTirageFromPoint(int x_val)
{
  bool status = false;
  QSqlQuery sql_1;
  QString msg = "";
  QString req = "";

  req = tirRef->qs_zColBaseName(0);
  req = req + "," + tirRef->qs_zColBaseName(1);
  req = "select jour_tirage,date_tirage,"
        + req
        + " from tirages "
        "where (tirages.id ="
        +QString::number(x_val)+");";

  status = sql_1.exec(req);
  if(status)
  {
    sql_1.first();

    req ="";
    QString st_item ="";
    int nb_item = sql_1.record().count();

    for(int i = 0; i<nb_item; i++)
    {
      st_item = sql_1.value(i).toString();

      req=req +st_item+",";

      if(i==1)
      {
        req.simplified();
        req.replace(","," ");
        req = req + "\r\nT:";
      }

      if(i==tirDef.nbElmZone[0]+1)
      {
        req = req + "[";
      }
    }
    req.remove(req.length()-1,1);
    req = req + "]";
    msg = req;

  }
  else
  {
    msg = "Erreur requete sur tirage !!";
  }
  return msg;
}

QString PointTirage::TST_GetTiragePariteFromPoint(int x_val)
{
  bool status = false;
  QSqlQuery sql_1;
  QString msg = "";

  msg = "select bp,ep from tirages where (tirages.id ="
        +QString::number(x_val)+");";

  status = sql_1.exec(msg);
  if(status)
  {
    sql_1.first();
    msg = "P:" + sql_1.value(0).toString() +
          "[" + sql_1.value(1).toString() + "]";
  }
  else
  {
    msg = "Erreur Req Parite";
  }
  return msg;
}

QString PointTirage::TST_GetTirageOrdreFromPoint(int x_val)
{
  bool status = false;
  QSqlQuery sql_1;
  QString msg = "";

  msg = tirRef->s_LibColAnalyse(&tirDef);
  msg = "select "
        + msg + " from analyses "
        "where (analyses.id ="
        +QString::number(x_val)+");";

  status = sql_1.exec(msg);
  if(status)
  {
    sql_1.first();
    if(sql_1.isValid())
    {

      msg ="";
      int nb_item = sql_1.record().count();

      for(int i = 0; i<nb_item; i++)
      {
        msg=msg +sql_1.value(i).toString()+",";
        if(i==(tirDef.limites[0].max/10))
        {
          msg = msg + "[";
        }
      }
      msg.remove(msg.length()-1,1);
      msg = "R:" +msg + "]";
    }
    else
    {
      msg = "Req ordre elem 1 error !!";
    }

  }
  else
  {
    msg = "Erreur requete sur ordre !!";
  }

  return msg;
}

QString PointTirage::TST_GetTirageGroupeFromPoint(int x_val)
{
  bool status = false;
  QSqlQuery sql_1;
  QString msg = "";

  msg = "select bg,eg from tirages where (tirages.id ="
        +QString::number(x_val)+");";

  status = sql_1.exec(msg);
  if(status)
  {
    sql_1.first();
    msg = "G:" + sql_1.value(0).toString() +
          "[" + sql_1.value(1).toString() + "]";
  }
  else
  {
    msg = "Erreur requete sur Groupe !!";
  }

  return msg;
}

void PointTirage::TST_ToolTipsInfotirage(QGraphicsSceneMouseEvent *event)
{
  QPointF pos_item = this->scenePos();
  QPoint pos_item_screen = event->buttonDownScreenPos(Qt::LeftButton);


  int lgntir = pos_item.x()/C_COEF_X;
  static int prev_x = -1;

  QString msg = "";
  static QString msg_prev = "";

  if(prev_x != lgntir )
  {
    pos_item_screen = event->screenPos();
    QString st_CR = "\r\n";
    QString st_resu_func = "";

    // recup du tirage
    QString st_tir = TST_GetTirageFromPoint(lgntir);

    // Appeler la fonction adaptee a la fenetre graphique
    if(ptrFunc !=NULL){
      st_resu_func = (this->*ptrFunc)(lgntir);
    }
    msg = st_tir + st_CR + st_resu_func;

#if 0
    // recup parite
    QString st_tir_pa = TST_GetTiragePariteFromPoint(lgntir);

    // recup repartition
    QString st_tir_ordre = TST_GetTirageOrdreFromPoint(lgntir);

    // recup groupe
    QString st_tir_gp = TST_GetTirageGroupeFromPoint(lgntir);

    msg = st_tir + st_CR +
          st_tir_pa + st_CR +
          st_tir_ordre + st_CR +
          st_tir_gp;
#endif
    msg_prev = msg;
  }
  else
  {
    msg = msg_prev;
  }

  QToolTip::showText(pos_item_screen,msg,0);
}


void PointTirage::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
#ifndef QT_NO_DEBUG
  qDebug() << "Click m2:" << event->pos();
#endif
  update();
  QGraphicsItem::mouseReleaseEvent(event);
}

QRectF PointTirage::boundingRect() const
{

  qreal adjust = 0;
  return QRectF( -2 - adjust, -2 - adjust,
                 4 + adjust, 4 + adjust);

}

QPainterPath PointTirage::shape() const
{
  QPainterPath path;
  path.addEllipse(-2, -2, 4, 4);
  return path;
}

void PointTirage::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
  painter->setPen(Qt::NoPen);
  //painter->setBrush(Qt::darkGray);
  //painter->drawEllipse(-7, -7, 20, 20);

  QRadialGradient gradient(-3, -3, 10);
  if (option->state & QStyle::State_Sunken) {
    //setToolTip("TOTO");
    gradient.setCenter(0, 0);
    gradient.setFocalPoint(3, 3);
    gradient.setColorAt(1, QColor(Qt::red).light(120));
    gradient.setColorAt(0, QColor(Qt::darkYellow).light(120));
  } else {
    gradient.setColorAt(0, Qt::yellow);
    gradient.setColorAt(1, Qt::darkYellow);
  }
  painter->setBrush(gradient);

  painter->setPen(QPen(Qt::black, 0));
  painter->drawEllipse(-2, -2, 4, 4);
}
