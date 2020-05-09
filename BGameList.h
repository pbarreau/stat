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
#include "BAnalyserTirages.h"

class BGameLst : public QWidget
{
 Q_OBJECT
 public:
 explicit BGameLst(const stGameConf *pGame, QWidget *parent = nullptr);
 ~BGameLst();
 stGameConf * getGameConf(void);
 QString getGameLabel(void);
 static stGameConf *gameUsrNew(const stGameConf *pGame, QString gameId="");
 void ShowPreviousGames(stGameConf *pGame);


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
 BGameAna *doLittleAna(const stGameConf *pGame, QString msg);
 QWidget *ana_fltSelection(QWidget **J);


 signals:
 void BSig_AnaLgn(const int &lgn_id, const int &prx_id);

 public slots:
 void BSlot_ShowTotal(const QString& lstBoules);
 void BSlot_FilterRequest(const Bp::E_Ana ana, const B2LstSel *sel);

 private slots:
 void BSlot_MouseOverLabel(QLabel *l);
 void BSlot_Clicked();
 void BSlot_Clicked(const QModelIndex &index);
 void BSlot_CheckBox(const QPersistentModelIndex &target, const Qt::CheckState &chk);
 void BSlot_ShowBtnId(int btn_id);
 void slot_ShowChk(void);
 void slot_ShowNhk(void);

 private:
 static int gme_counter;
 int gme_id;
 int sub_id;
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
