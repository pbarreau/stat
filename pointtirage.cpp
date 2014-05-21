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

PointTirage::PointTirage(NE_FDJ::E_typeJeux leJeu) :
  QGraphicsItem()
{
  tirRef = new tirages(leJeu);

  stTiragesDef mesdef;
  tirRef->getConfig(&mesdef);

  tirDef = mesdef;

#if 0
  QString tir_msg = tirRef->qs_zColBaseName(0);
  tir_msg = tir_msg + "," + tirRef->qs_zColBaseName(1);
  tir_msg = "select " + tir_msg + " from tirages "
            "where (tirages.id ="
            +QString::number(1)+");";
#endif
  //QString msg = tirRef->s_LibColAnalyse(&tirDef);
  //msg = tirRef->qs_zColBaseName(0);
  //setFlag(ItemIsMovable);
  //setFlag(ItemSendsGeometryChanges);
  setFlags(ItemIsSelectable);
  setCacheMode(DeviceCoordinateCache);
  setZValue(-1);
}

void PointTirage::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  bool status = false;
  QSqlQuery sql_1;
  QPointF pos_item = this->scenePos();

  int lgntir = pos_item.x()/C_COEF_X;
  static int prev_x = 0;

  QString msg = "";
  static QString msg_prev = "";

  if(prev_x != lgntir )
  {
    // Recuperer le tools tips dans la base
    msg = tirRef->s_LibColAnalyse(&tirDef);
    msg = "select "
          + msg + " from analyses "
          "where (analyses.id ="
          +QString::number(lgntir)+");";

    status = sql_1.exec(msg);
    if(status)
    {
      sql_1.first();
      if(sql_1.isValid())
      {
        msg ="";
        for(int i = 0; i<sql_1.record().count(); i++)
        {
          msg=msg +sql_1.value(i).toString()+",";
          if(i==(tirDef.limites[0].max/10))
          {
            msg = msg + "[";
          }
        }
        msg.remove(msg.length()-1,1);
        msg = msg + "]";
        // ---------
        // Recuperation du tirage
        QString tir_msg = tirRef->qs_zColBaseName(0);
        tir_msg = tir_msg + "," + tirRef->qs_zColBaseName(1);
        tir_msg = "select jour_tirage,date_tirage," + tir_msg + " from tirages "
                  "where (tirages.id ="
                  +QString::number(lgntir)+");";
        status = sql_1.exec(tir_msg);
        if(status)
        {
          sql_1.first();
          if(sql_1.isValid())
          {
            tir_msg ="";
            for(int i = 0; i<sql_1.record().count(); i++)
            {
              tir_msg=tir_msg +sql_1.value(i).toString()+",";
              if(i==1)
              {
                tir_msg.simplified();
                tir_msg.replace(","," ");
                tir_msg = tir_msg + "\r\n";
              }
              if(i==tirDef.nbElmZone[0]-1+2)
              {
                tir_msg = tir_msg + "[";
              }
            }
            tir_msg.remove(tir_msg.length()-1,1);
            tir_msg = tir_msg + "]";

          }
        }
        else
        {
          tir_msg = "err tir !";
        }
        msg = tir_msg +"\r\n" + msg;
        // -------------
      }

    }
    else
    {
      msg = "No data";
    }

    msg_prev = msg;
  }
  else
  {
    msg = msg_prev;
  }

  qDebug() << "Click m1:" << pos_item;
  qDebug() << "Abs:" << pos_item.x()/C_COEF_X;

  QPoint test1 = event->buttonDownScreenPos(Qt::LeftButton);
  QToolTip::showText(test1,msg,0);

  update();
  QGraphicsItem::mousePressEvent(event);
}

void PointTirage::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  qDebug() << "Click m2:" << event->pos();
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
