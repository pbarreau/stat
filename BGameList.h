#ifndef BGAMELIST_H
#define BGAMELIST_H

#include <QWidget>
#include <QSqlDatabase>
#include <QGroupBox>
#include <QLabel>

#include "BSqlQmTirages_3.h"
#include "bstflt.h"
#include "blineedit.h"
#include "BLstSelect.h"

class BGameLst : public QWidget
{
 Q_OBJECT
 public:
 explicit BGameLst(const stGameConf *pGame, QWidget *parent = nullptr);
 ~BGameLst();
 stGameConf * getGameConf(void);
 QString getGameLabel(void);
 static stGameConf *gameUsrNew(const stGameConf *pGame, QString gameId="");

 private:
 bool isNewUsrGame(const stGameConf *pGame, QString *gameId, QString *data);
 bool getGameKey(const stGameConf *pGame, QString *key);
 bool isSufficient(const stGameConf *pGame, int tot);
 bool isAlreadyKnown(QString key, QString * gameId);
 bool createGame(const stGameConf *pGame, QString gameId, QString data);
 void mkGameWidget(stGameConf * current);
 QGroupBox *  LireBoule(stGameConf *pGame, QString tbl_cible);
 QGroupBox *  LireTable(stGameConf *pGame, QString tbl_cible);
 void updateTbv(QString msg);
 QString sqlVisualTable(QString tbl_src);
 QString makeSqlFromSelection(const B2LstSel * sel, QString *tbl_lst);
 QString makeSqlForNextLine(const B2LstSel * sel);
 QString select_elm(const QModelIndexList &indexes, int zn);
 QString select_cmb(const QModelIndexList &indexes, int zn, int tbl_id);
 QString select_brc(const QModelIndexList &indexes, int zn, int tbl_id);
 QString select_grp(const QModelIndexList &indexes, int zn, int tbl_id);
 QString elmSel_1(const QModelIndexList &indexes, int zn);
 QString elmSel_2(const QModelIndexList &indexes, int zn);

 signals:

 public slots:
 void slot_ShowNewTotal(const QString& lstBoules);
 void slot_RequestFromAnalyse(const Bp::E_Ana ana, const B2LstSel *sel);

 private slots:
 void slot_Colorize(QLabel *l);
 void slot_btnClicked();
 void slot_tbvClicked(const QModelIndex &index);
 void slot_UsrChk(const QPersistentModelIndex &target, const Qt::CheckState &chk);
 void slot_ShowAll(int btn_id);
 void slot_ShowChk(void);
 void slot_ShowNhk(void);

 private:
 static int gme_counter;
 stGameConf *gameDef;
 QSqlDatabase db_gme;
 QString game_lab;

 private:
 BSqlQmTirages_3 *sqm_resu;
 QGroupBox *gpb_Tirages;
 QLabel *lb_Big;
 BLineEdit *le_chk;



};

#endif // BGAMELIST_H
