#ifndef BMENU_H
#define BMENU_H


#include <QMenu>
#include <QPoint>
#include <QCheckBox>
#include <QTableView>
#include <QSqlDatabase>
#include <QModelIndex>

#include "BFlt.h"
#include "BView_1.h"
#include "Bc.h"

class BMenu : public QMenu,BFlt
{
 Q_OBJECT

 public:
 BMenu(const QPoint pos, const BFlt *conf, BView_1 *view);

 private:
 void construireMenu(void);
 QMenu *mnu_Priority(stTbFiltres *ret, const etCount eSrc, const BView_1 *view, const QModelIndex index);
 ///bool getdbFlt(stTbFiltres *ret, const etCount origine, const BTbView *view, const QModelIndex index);
 ///bool chkShowMenu(void);

 public slots:
 void BSlot_Menu_1();

 private slots:
 void slot_isUplets(bool chk);
 void slot_isWanted(bool chk);
 void slot_isChoosed(bool chk);
 void slot_isFiltred(bool chk);
 ///void slot_priorityForAll(bool chk);
 void slot_ChoosePriority(QAction *cmd);

 private:
 ///void initialiser_v1(const QPoint pos, const etCount eType, BTbView *view);
 ///void initialiser_v2(const QPoint pos, const etCount eType, BTbView *view);
 ///void gererMenu_v1();
 void gererMenu_v2();
 void presenterMenu();

 Q_SIGNALS:
 void BSig_MkUsrUplets_L1(const QItemSelectionModel *cur_sel);

 public:
 BMenu *addr;

 private:
 QMenu *main_menu;
 QSqlDatabase db_menu;
 //etCount eCalcul;
 //BTbView *lview;
 //QModelIndex  index;
 //stTbFiltres val;
};

#endif // BMENU_H
