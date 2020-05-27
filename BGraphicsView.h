#ifndef BGRAPHICSVIEW_H
#define BGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QSqlDatabase>
#include <QWheelEvent>
#include <QGraphicsItemGroup>
#include <QMap>

#include "game.h"
#include "bstflt.h"


#define C_COEF_X  10
#define C_COEF_Y  1
#define C_COEF_C  5

class BGraphicsView : public QGraphicsView
{
public:
BGraphicsView(const stGameConf *pGame, QBrush coul_fond=Qt::lightGray);
void DessineCourbeSql(const stGameConf *pGame, int zn, int lgn_id, QColor pen_id=Qt::red, int sqlIdY =1, int scale_y=1, int delta_y=0);
QGraphicsItemGroup *getLine(int zn, int l_id);
QGraphicsScene *getScene();

protected:
virtual void wheelEvent(QWheelEvent* event);
//virtual void	mousePressEvent ( QMouseEvent * event );

private :
void draw_cmb(const stGameConf *pGame, int zn, int lgn_id, QColor pen_id=Qt::red);

private:
QSqlDatabase db_0;
QGraphicsScene *Scene;
const stGameConf *gme_conf;
etCount type;
QMap<int, QGraphicsItemGroup *> **dessin;

};

#endif // BGRAPHICSVIEW_H
