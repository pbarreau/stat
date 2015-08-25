#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsRectItem>

#include <QGraphicsSceneMouseEvent>
#include "tirages.h"

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
    MyGraphicsView(eGType gtype = eNoGraph, QGraphicsView *ptr_view=NULL, QString titre="Tbd", QColor coul_fond = Qt::yellow);
    void DessineCourbeSql(QString msg_2, NE_FDJ::E_typeJeux leJeu, QColor cpen, int sqlIdY =1, int scale_y=1, int delta_y=0);
     QGraphicsScene * GetScene();

public slots:

protected:
    virtual void wheelEvent(QWheelEvent* event);

private:
    QGraphicsScene *Scene;
    eGType scene_type;

};

class UnConteneurDessin: public QGraphicsView
{
public:
    UnConteneurDessin(QWidget* parent = NULL);


protected:
    virtual void wheelEvent(QWheelEvent* event);
    //void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void	mousePressEvent ( QMouseEvent * event );

};

#endif // MYGRAPHICSVIEW_H
