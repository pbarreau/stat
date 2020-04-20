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
 QMenu *mnu_Priority(stTbFiltres *ret, const etCount eSrc, const QTableView *view, const QModelIndex index);
 bool getdbFlt(stTbFiltres *ret, const etCount origine, const QTableView *view, const QModelIndex index);
 bool setdbFlt(stTbFiltres in);

 public slots:
 void slot_showMenu();

 private slots:
 void slot_isWanted(bool chk);
 void slot_isChoosed(bool chk);
 void slot_isFiltred(bool chk);
 void slot_priorityForAll(bool chk);
 void slot_ChoosePriority(QAction *cmd);

 private:
 QMenu *main_menu;
 QSqlDatabase db_1;
 etCount eCalcul;
 const QTableView *lview;
 QModelIndex  index;
 stTbFiltres val;
};

#endif // BMENU_H
