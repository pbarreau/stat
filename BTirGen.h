#ifndef BTirGen_H
#define BTirGen_H

#include <QWidget>
#include <QSqlDatabase>
#include <QGroupBox>
#include <QLabel>

#include "BSqlQmTirages_3.h"
#include "bstflt.h"
#include "blineedit.h"
#include "BLstSelect.h"
#include "BTirAna.h"
#include "BTirages.h"
#include "game.h"

class BTirGen : public BTirages
{
 Q_OBJECT
 public:
 explicit BTirGen(const stGameConf *pGame, etTir gme_tir = eTirGen, QWidget *parent = nullptr);
 ~BTirGen();
 stGameConf * getGameConf(void);
 static stGameConf *gameUsrNew(const stGameConf *pGame, QString gameId="");
 void ShowPreviousGames(stGameConf *pGame);
 void setAna(BTirAna * in_ana);


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
 //QString getTiragesList(QString tbl_src);
 /*
 QString makeSqlFromSelection(const B2LstSel * sel, QString *tbl_lst);
 QString makeSqlForNextLine(const B2LstSel * sel);
 QString select_elm(const QModelIndexList &indexes, int zn);
 QString select_cmb(const QModelIndexList &indexes, int zn, int tbl_id);
 QString select_brc(const QModelIndexList &indexes, int zn, int tbl_id);
 QString select_grp(const QModelIndexList &indexes, int zn, int tbl_id);
 QString elmSel_1(const QModelIndexList &indexes, int zn);
 QString elmSel_2(const QModelIndexList &indexes, int zn);
 BTirAna *doLittleAna(const stGameConf *pGame, QString msg);
 QWidget *ana_fltSelection(QWidget **J);
*/
 void deletePreviousResults(const stGameConf *pGame);

 signals:
 //void BSig_AnaLgn(const int &lgn_id, const int &prx_id);

 public slots:
 void BSlot_ShowTotal(const QString& lstBoules);
 void BSlot_FilterRequest(const Bp::E_Ana ana, const B2LstSel *sel);

 private slots:
 void BSlot_MouseOverLabel(QLabel *l);
 void BSlot_Clicked_Gen();
 void BSlot_Clicked_Gen(const QModelIndex &index);
 void BSlot_CheckBox(const QPersistentModelIndex &target, const Qt::CheckState &chk);
 void BSlot_ShowBtnId(int btn_id);
 void slot_ShowChk(void);
 void slot_ShowNhk(void);
 void BSlot_closeTab(int index);

 private:
 static int gme_counter;
 //int gme_id;
 //int sub_id;
 stGameConf *gameDef;
 QSqlDatabase db_gme;
 BTirAna *cur_ana;
 QWidget *resu;
 //QTabWidget * tab_resu;

 private:
 BSqlQmTirages_3 *sqm_resu;
 QGroupBox *gpb_Tirages;
 QLabel *lb_Big;
 BLineEdit *le_chk;



};

#endif // BTirGen_H
