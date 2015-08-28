

//Qt includes
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QTextStream>
#include <QScrollBar>
#include <QMouseEvent>
#include <QWheelEvent>

#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>

#include <math.h>

#include "mygraphicsview.h"
#include "pointtirage.h"

QGraphicsScene * MyGraphicsView::GetScene(void)
{
    return Scene;
}

MyGraphicsView::MyGraphicsView(eGType gtype, QGraphicsView *ptr_view, QString titre, QColor coul_fond)
{

    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    //Set-up the scene
    Scene = new QGraphicsScene;
    scene_type = gtype;

    ptr_view->setScene(Scene);
    ptr_view->setBackgroundBrush(coul_fond);
    ptr_view->setCacheMode(CacheBackground);
    ptr_view->setViewportUpdateMode(FullViewportUpdate);
    ptr_view->setRenderHint(QPainter::Antialiasing);
    ptr_view->setTransformationAnchor(AnchorUnderMouse);
    ptr_view->setDragMode(ScrollHandDrag);
    ptr_view->setWindowTitle(tr(titre.toLocal8Bit()));
}


void MyGraphicsView::DessineCourbeSql(QString msg_2, NE_FDJ::E_typeJeux leJeu, QColor cpen, int sqlIdY, int scale_y, int delta_y)
{
    QSqlQuery sql_2;
    bool status = false;
#ifndef QT_NO_DEBUG
    qDebug() << msg_2;
#endif
    status = sql_2.exec(msg_2);
    if(status)
    {
        sql_2.first();
        if(sql_2.isValid())
        {
            int sx = 0;
            double sy = 0.0;
            //QColor cpen = Qt::black;

            do
            {
                int x = sql_2.value(0).toInt()* C_COEF_X;
                int y_sql = sql_2.value(sqlIdY).toDouble();
                double y = this->height();

               y = y -  y_sql * C_COEF_Y* scale_y;
               y -=delta_y;

                // Ajount d'une boule representant un tirage
                PointTirage *ptir = new PointTirage(leJeu,scene_type);

                ptir->setPos(x,y);

                Scene->addItem(ptir);

                if(sx==0)
                {
                    sx=x;
                    sy=y;
                }

                // Courbe
                QLineF L1(x,y,sx,sy);
                Scene->addLine(L1,QPen(cpen));
                sx=x;
                sy=y;
            }while(sql_2.next());
        }
    }
}

void MyGraphicsView::wheelEvent(QWheelEvent* event) {

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    //this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

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

////------------------------------------------------
UnConteneurDessin::UnConteneurDessin(QWidget *parent): QGraphicsView(parent)
{

}

void UnConteneurDessin::wheelEvent(QWheelEvent* event) {

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    //this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    // Scale the view / do the zoom
    double scaleFactor = 1.15;
    if(event->delta() > 0) {
        // Zoom in
        scale(scaleFactor, scaleFactor);
    } else {
        // Zooming out
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }

    //setSceneRect(0, 0, 2000, 500);
    // Don't call superclass handler here
    // as wheel is normally used for moving scrollbars
}

void UnConteneurDessin::mousePressEvent ( QMouseEvent * event )
{
    if(event->button() == Qt::RightButton)
    {
#ifndef QT_NO_DEBUG
        qDebug()<< "Mouse press";
#endif
        QGraphicsItem *lgn = NULL;
        QPoint pos = event->pos();

        lgn = this->itemAt(pos);
        if(lgn)
        {
            //this->scene()->removeItem(lgn);
        }
    }

    update();
    QGraphicsView::mousePressEvent(event);
}
