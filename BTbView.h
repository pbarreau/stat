#ifndef BTBVIEW_H
#define BTBVIEW_H

#include <QTableView>
#include <QGroupBox>
#include <QSqlDatabase>
#include <QMenu>
#include <QPushButton>
#include <QTabWidget>
#include <QGridLayout>

#include "BFlt.h"
#include "BGpbMenu.h"
#include "BTirGen.h"
#include "BTirAna.h"

#include "BGTbView.h"
#include "BTirages.h"

class BTbView : public BGTbView, BFlt
{
 Q_OBJECT

 public :
 BTbView(const stGameConf *pGame, int in_zn, etCount in_typ);
 //QGroupBox * getScreen();
 //BGpbMenu * getGpb();
 void updateTitle();
 void setUpLayout(QLayout *usr_up);
 void setUsrGameButton(QPushButton *usr_game);
 QPushButton *getUsrGameButton(void);
 stTbFiltres * getFlt(void);
 bool isOnUsrGame(void);
 static void agencerResultats(BTirages *lst, BTirAna * ana);
 static void activateTargetTab(QString id);
 static void addSubFlt(int id, QWidget *tab);
 static void addSpacer(int id, QSpacerItem *space);
 void setRowModelCount(int nb);
 void setRowSourceModelCount(int nb);

 ~BTbView();

 Q_SIGNALS:
 void bsg_clicked(const QModelIndex & index, const int &zn, const etCount &eTyp);

 private slots:
 void BSlot_MousePressed(const QModelIndex &index);
 void BSlot_TrackSelection(const QItemSelection &cur, const QItemSelection &last);

 private slots:
 void BSlot_ShowToolTip(const QModelIndex & index);
 void slot_V2_ccmr_SetPriorityAndFilters(QPoint pos);

 public slots:
 void BSlot_MakeCustomGame();

 private:
 QString mkTitle(int zn, etCount eCalcul, QTableView *view);
 void construireMenu(void);
 void showUsrGame(QWidget *une_selection, QString name);
 ///void mouseMoveEvent( QMouseEvent * inEvent );
 //void mousePressEvent(QMouseEvent *event);

 public:
 BFlt *lbflt;

 private:
 static QTabWidget * tbw_calculs;
 static QWidget * wdg_reponses;
 static QGridLayout *gdl_all;
 static QList<QGridLayout *> *gdl_list;

 QSqlDatabase db_tbv;
 const stGameConf *cur_game;
 //BGpbMenu *square;
 QLayout *up;
 QMenu *menu;
 QPushButton *btn_usrGame;
 int rowModelCount;
 int rowSourceModelCount;
};

#endif // BTBVIEW_H
