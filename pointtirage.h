#ifndef POINTTIRAGE_H
#define POINTTIRAGE_H

#include <QGraphicsItem>
#include <QList>

#include "tirages.h"
#include "mygraphicsview.h"

#define C_COEF_X  10
#define C_COEF_Y  1
#define C_COEF_C  5

class PointTirage : public QGraphicsItem
{
  //Q_OBJECT
public:
  //explicit PointTirage();
  PointTirage(QString db_cnx, NE_FDJ::E_typeJeux leJeu=NE_FDJ::fdj_euro, eGType sceneType = eNoGraph);
  QRectF boundingRect() const;
  QPainterPath shape() const;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);
  QString TST_GetTirageFromPoint(int x_val);
  QString TST_GetTiragePariteFromPoint(int x_val);
  QString TST_GetTirageOrdreFromPoint(int x_val);
  QString TST_GetTirageGroupeFromPoint(int x_val);
  void TST_ShowPointFromTirage();

signals:
  
public slots:

private:
  void TST_TracerLigne(QGraphicsSceneMouseEvent *event);
  void TST_ToolTipsInfotirage(QGraphicsSceneMouseEvent *event);

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent *event);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
  //QPointF curPos;
  static tirages *tirRef;
  static stTiragesDef tirDef;
  static QList<QGraphicsLineItem *> lst_lignes;
  QSqlDatabase db_ici;
  QString (PointTirage::*ptrFunc)(int val);
};

#endif // POINTTIRAGE_H
