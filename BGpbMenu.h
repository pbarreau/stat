#ifndef BGPBMENU_H
#define BGPBMENU_H

#include <QSqlDatabase>

#include <QGroupBox>
#include <QMouseEvent>
#include <QPointF>
#include <QMenu>

#include "BFlt.h"

class BView_1;

class BGpbMenu : public QGroupBox, BFlt
{
 Q_OBJECT
public:
explicit BGpbMenu(const BFlt *conf, BView_1 *in_parent=nullptr);

protected:
void mousePressEvent ( QMouseEvent * event ) ;

signals:
void sig_ShowMenu(const QGroupBox *cible, const QPoint &p);

public slots:
void slot_ShowMenu(const QGroupBox *cible, const QPoint &p);

private slots:
void slot_ManageFlts(QAction *all_cmd);

private:
void displayTbvMenu_gpb(void);
void setNewFlt(Bp::F_Flts flt_def);

private:
BView_1 *parent;
QSqlDatabase db_gbm; /// db group box menu
QString tb_flt;
QString tb_tirages;
int zn;
etCount typ;
QMenu *menu;
const QGroupBox *use_gpb;
};

#endif // BGPBMENU_H
