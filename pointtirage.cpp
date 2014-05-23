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
  int poids = pos_item.y()/C_COEF_Y;
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

void PointTirage::TST_ToolTipsInfotirage(QGraphicsSceneMouseEvent *event)
{
  bool status = false;
  QSqlQuery sql_1;
  QPointF pos_item = this->scenePos();
  QPoint pos_item_screen = event->buttonDownScreenPos(Qt::LeftButton);


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
        QString tir_msg = "";
        QString nbParite = "";

        for(int i = 0; i<sql_1.record().count(); i++)
        {
          msg=msg +sql_1.value(i).toString()+",";
          if(i==(tirDef.limites[0].max/10))
          {
            msg = msg + "[";
          }
        }
        msg.remove(msg.length()-1,1);
        msg = "R:" +msg + "]";
        // ---------
        // Recuperation du tirage
        tir_msg = tirRef->qs_zColBaseName(0);
        tir_msg = tir_msg + "," + tirRef->qs_zColBaseName(1);
        tir_msg = "select jour_tirage,date_tirage,"
                  + tir_msg
                  + ", bp, ep "
                  + " from tirages "
                  "where (tirages.id ="
                  +QString::number(lgntir)+");";
        status = sql_1.exec(tir_msg);
        if(status)
        {
          sql_1.first();
          if(sql_1.isValid())
          {
            tir_msg ="";
            QString st_item ="";
            int nb_item = sql_1.record().count();

            for(int i = 0; i<nb_item; i++)
            {
              st_item = sql_1.value(i).toString();

              if(i<nb_item -2)
              {
                tir_msg=tir_msg +st_item+",";

                if(i==1)
                {
                  tir_msg.simplified();
                  tir_msg.replace(","," ");
                  tir_msg = tir_msg + "\r\nT:";
                }

                if(i==tirDef.nbElmZone[0]-1+2)
                {
                  tir_msg = tir_msg + "[";
                }

              }

              else
              {
                // Recuperation du nombre de boules pairs
                nbParite = nbParite + st_item + ",[";
              }
            }
            tir_msg.remove(tir_msg.length()-1,1);
            tir_msg = tir_msg + "]";

            nbParite.remove(nbParite.length()-2,2);
            nbParite = "P:"+nbParite+"]\r\n";
          }
        }
        else
        {
          tir_msg = "err tir !";
        }
        msg = tir_msg +"\r\n" + nbParite + msg;
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
#ifndef QT_NO_DEBUG
  qDebug() << "Click m1:" << pos_item;
  qDebug() << "Abs:" << pos_item.x()/C_COEF_X;
  qDebug() << "Ord:" << pos_item.y()/C_COEF_Y;
#endif
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
