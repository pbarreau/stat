#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QSqlDatabase>

#include <QGraphicsSceneMouseEvent>
//#include "tirages.h"
#include "game.h"

typedef enum _graph
{
    eNoGraph,
    eTirage,
    eParite,
    eRepartition,
    eGroupe
}eGType;

class MyGraphicsView: public QGraphicsView
{
public:
    MyGraphicsView(QString db_cnx, eGType gtype = eNoGraph, QGraphicsView *ptr_view=nullptr, QString titre="Tbd", QColor coul_fond = Qt::yellow);
    void DessineCourbeSql(QString msg_2, etFdj leJeu, QColor cpen, int sqlIdY =1, int scale_y=1, int delta_y=0);
    QGraphicsScene * GetScene();

public slots:

protected:
    virtual void wheelEvent(QWheelEvent* event);

private:
    QSqlDatabase db_0;
    QGraphicsScene *Scene;
    eGType scene_type;

};

class UnConteneurDessin: public QGraphicsView
{
public:
    UnConteneurDessin(QWidget* parent = nullptr);


protected:
    virtual void wheelEvent(QWheelEvent* event);
    //void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void	mousePressEvent ( QMouseEvent * event );

};

#endif // MYGRAPHICSVIEW_H
