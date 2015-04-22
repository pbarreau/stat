

//Qt includes
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QTextStream>
#include <QScrollBar>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>

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

void MyGraphicsView::DessineCourbeSql(QString msg_2, NE_FDJ::E_typeJeux leJeu, QColor cpen, int scale_y, int delta_y)
{
    QSqlQuery sql_2;
    bool status = false;


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
                double y = sql_2.value(1).toDouble()*C_COEF_Y *scale_y;
                y+=delta_y;

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


#ifdef USE_OLD_CODE
MyGraphicsView::MyGraphicsView(stTiragesDef *pConf, QGraphicsView *ptr_view)
{
    NE_FDJ::E_typeJeux leJeu = pConf->choixJeu;

    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    //Set-up the scene
    QGraphicsScene* Scene = new QGraphicsScene;
    ptr_view->setScene(Scene);
    ptr_view->setBackgroundBrush(Qt::yellow);
    ptr_view->setCacheMode(CacheBackground);
    ///setViewportUpdateMode(BoundingRectViewportUpdate);
    ptr_view->setViewportUpdateMode(FullViewportUpdate);
    ptr_view->setRenderHint(QPainter::Antialiasing);
    ptr_view->setTransformationAnchor(AnchorUnderMouse);
    ptr_view->setDragMode(ScrollHandDrag);
    ptr_view->setWindowTitle(tr("Elastic Nodes"));


    //--------------
    //Populate the scene
    QSqlQuery sql_1;
    bool status = false;
    QString msg_1="select max(analyses.id), max(lstcombi.poids) from analyses , lstcombi;";
    int zn=1;

    //parent->setWindowTitle("Graphique");
    //parent->setMinimumWidth(390);
    //parent->setMinimumHeight(390);

    status = sql_1.exec(msg_1);
    if(status)
    {
        sql_1.first();
        if(sql_1.isValid())
        {
            //int mx = sql_1.value(0).toInt() + 1;
            //int my = ceil(sql_1.value(1).toDouble()) + 1;

            QSqlQuery sql_2;
            QString msg_2 = "select analyses.id, lstcombi.poids, lstcombi.pos from analyses inner join lstcombi on analyses.id_poids = lstcombi.id;";
            status = sql_2.exec(msg_2);
            if(status)
            {
                QSqlQuery sql_3;
                // select tirages.id, comb_e.poids from tirages inner join comb_e on comb_e.e1=tirages.e1 and comb_e.e2 = tirages.e2
                QString msg_3 = "select tirages.id, comb_e.poids from tirages inner join comb_e on ";
                QString msg_4 = "";
                for(int i =0; i<pConf->nbElmZone[zn];i++)
                {
                    msg_4 = msg_4 + TB_COMBI "_" + pConf->nomZone[zn] + "." + pConf->nomZone[zn] + QString::number(i+1)
                            +"=" TB_BASE "." + pConf->nomZone[zn] + QString::number(i+1) + " and ";
                }
                msg_4.remove(msg_4.length()-5,5);
                msg_3 = msg_3 + msg_4 + ";";
                status = sql_3.exec(msg_3);
                sql_2.first();
                sql_3.first();
                if(sql_2.isValid() && sql_3.isValid())
                {
                    int sx = 0;
                    double sy = 0.0;
                    double sy_e = 0.0;
                    //         int mem_crenau = 0;
                    QColor cpen = Qt::black;

                    do
                    {
                        int x = sql_2.value(0).toInt()* C_COEF_X;
                        double y = sql_2.value(1).toDouble()*C_COEF_Y;
                        //            int crenau = sql_2.value(2).toInt()*C_COEF_C;

                        double y_e = sql_3.value(1).toDouble()*C_COEF_Y;

#if 0
                        switch(crenau/C_COEF_C)
                        {
                        case 0:
                            cpen = Qt::red;
                            break;
                        case 1:
                            cpen = Qt::green;
                            break;
                        case 2:
                            cpen = Qt::gray;
                            break;
                        case 3:
                            cpen = Qt::magenta;
                            break;
                        case 4:
                            cpen = Qt::blue;
                            break;
                        default:
                            cpen = Qt::black;
                        }
#endif

                        // Ajount d'une boule representant un tirage
                        PointTirage *ptir = new PointTirage(leJeu);
                        PointTirage *petl = new PointTirage(leJeu);

                        ptir->setPos(x,y);
                        petl->setPos(x,y_e);

                        Scene->addItem(ptir);
                        Scene->addItem(petl);

                        if(sx==0)
                        {
                            sx=x;
                            sy=y;
                            sy_e=y_e;
                            //              mem_crenau = crenau;
                        }

                        // Courbe des combis
                        QLineF L1(x,y_e,sx,sy_e);
                        //QGraphicsLineItem *L2 = new QGraphicsLineItem(x,crenau,x,crenau);

                        Scene->addLine(L1,QPen(Qt::red));
                        //Scene->addLine(x,crenau,x,crenau);
                        //mem_crenau = crenau;

                        // Courbe  des tirages
                        Scene->addLine(x,y,sx,sy);
                        //Scene->addLine(x,y_e,sx,sy_e);
                        sx=x;
                        sy=y;
                        sy_e = y_e;
                    }while(sql_2.next()&& sql_3.next());
                }
            }
            //Set-up the view
            //setSceneRect(0, 0, 5000, 5000);

        }
    }

}
#endif

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
