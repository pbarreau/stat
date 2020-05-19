#ifndef BGRAPHICSVIEW_H
#define BGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QSqlDatabase>
#include <QWheelEvent>

#include "game.h"

#define C_COEF_X  10
#define C_COEF_Y  1
#define C_COEF_C  5

class BGraphicsView : public QGraphicsView
{
public:
BGraphicsView(stGameConf *pGame, QGraphicsView *ptr_view=nullptr, QString titre="Tbd", QColor coul_fond = Qt::yellow);
void DessineCourbeSql(stGameConf *pGame, QString msg_2, QColor cpen, int sqlIdY =1, int scale_y=1, int delta_y=0);

protected:
virtual void wheelEvent(QWheelEvent* event);

private:
QSqlDatabase db_0;
QGraphicsScene *Scene;

};

#endif // BGRAPHICSVIEW_H
