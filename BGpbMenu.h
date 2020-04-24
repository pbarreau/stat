#ifndef BGPBMENU_H
#define BGPBMENU_H

#include <QSqlDatabase>

#include <QGroupBox>
#include <QMouseEvent>
#include <QPointF>
#include <QMenu>


class BGpbMenu : public QGroupBox
{
 Q_OBJECT
public:
explicit BGpbMenu(QString cnx, QGroupBox *parent=nullptr);

protected:
void mousePressEvent ( QMouseEvent * event ) ;

signals:
void sig_ShowMenu(const QGroupBox *cible, const QPoint &p);

public slots:
void slot_ShowMenu(const QGroupBox *cible, const QPoint &p);

private:
void gbm_Menu(void);

private:
QSqlDatabase db_gbm; /// db group box menu
QMenu *menu;
};

#endif // BGPBMENU_H
