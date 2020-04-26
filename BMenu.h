#ifndef BMENU_H
#define BMENU_H


#include <QMenu>
#include <QPoint>
#include <QCheckBox>
#include <QTableView>
#include <QSqlDatabase>
#include <QModelIndex>

#include "BTbView.h"
#include "compter.h"

class BMenu : public QMenu
{
 Q_OBJECT

 public:
 BMenu(const QPoint pos, QString cnx, const etCount eType, BTbView *view, QWidget *parent=nullptr);

 private:
 void construireMenu(void);
 QMenu *mnu_Priority(stTbFiltres *ret, const etCount eSrc, const BTbView *view, const QModelIndex index);
 bool getdbFlt(stTbFiltres *ret, const etCount origine, const BTbView *view, const QModelIndex index);
 bool chkShowMenu(void);

 public slots:
 void slot_showMenu();

 private slots:
 void slot_isWanted(bool chk);
 void slot_isChoosed(bool chk);
 void slot_isFiltred(bool chk);
 void slot_priorityForAll(bool chk);
 void slot_ChoosePriority(QAction *cmd);

 private:
 ///void initialiser_v1(const QPoint pos, const etCount eType, BTbView *view);
 void initialiser_v2(const QPoint pos, const etCount eType, BTbView *view);
 ///void gererMenu_v1();
 void gererMenu_v2();
 void presenterMenu();

 private:
 QMenu *main_menu;
 QSqlDatabase db_menu;
 etCount eCalcul;
 BTbView *lview;
 QModelIndex  index;
 stTbFiltres val;
};

#endif // BMENU_H
