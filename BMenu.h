#ifndef BMENU_H
#define BMENU_H


#include <QMenu>
#include <QPoint>
#include <QCheckBox>
#include <QTableView>
#include <QSqlDatabase>
#include <QModelIndex>

#include "compter.h"

class BMenu : public QMenu
{
 Q_OBJECT

 public:
 BMenu(const QPoint pos, QString cnx, const etCount eType, const QTableView *view);

 private:
 void construireMenu(void);
 bool getFiltre(stTbFiltres *ret, const etCount origine, const QTableView *view, const QModelIndex index);
 QMenu *mnu_Priority(stTbFiltres *ret, const etCount eSrc, const QTableView *view, const QModelIndex index);

 public slots:
 void slot_showMenu();

 private slots:
 void slot_Filtre(bool chk);
 void slot_SetAll(bool chk);

 private:
 QMenu *main_menu;
 QSqlDatabase db_1;
 etCount eCalcul;
 const QTableView *lview;
 QModelIndex  index;
 stTbFiltres val;
};

#endif // BMENU_H
