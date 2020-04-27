#ifndef BTBVIEW_H
#define BTBVIEW_H

#include <QTableView>
#include <QGroupBox>
#include <QSqlDatabase>
#include <QMenu>
#include <QPushButton>

#include "BFlt.h"
#include "BGpbMenu.h"
#include "game.h"

class BTbView : public QTableView, BFlt
{
 Q_OBJECT

 public :
 BTbView(const stGameConf *pGame, int in_zn, etCount in_typ, QTableView *parent=nullptr);
 QGroupBox * getScreen();
 BGpbMenu * getGpb();
 void updateTitle();
 void setUpLayout(QLayout *usr_up);
 void setUsrGameButton(QPushButton *usr_game);
 QPushButton *getUsrGameButton(void);

 ~BTbView();

 private slots:
 void slot_V2_AideToolTip(const QModelIndex & index);
 void slot_V2_ccmr_SetPriorityAndFilters(QPoint pos);

 public slots:
 void slot_usrCreateGame();

 private:
 QString mkTitle(int zn, etCount eCalcul, QTableView *view);
 void construireMenu(void);

 public:
 BFlt *lbflt;

 private:
 QSqlDatabase db_tbv;
 const stGameConf *cur_game;
 BGpbMenu *myGpb;
 QLayout *up;
 QMenu *menu;
 QPushButton *btn_usrGame;
};

#endif // BTBVIEW_H
