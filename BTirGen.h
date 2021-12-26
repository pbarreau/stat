#ifndef BTirGen_H
#define BTirGen_H

#include <QWidget>
#include <QSqlDatabase>
#include <QGroupBox>
#include <QLabel>
#include <QHBoxLayout>

#include "BSqlQmTirages_3.h"
#include "bstflt.h"
#include "blineedit.h"
#include "BLstSelect.h"
#include "BTirAna.h"
#include "BTirages.h"
#include "game.h"

class BView_1;
class BTirGen : public BTirages
{
  Q_OBJECT
 public:
  explicit BTirGen(const stGameConf *pGame, BView_1 *bv1 = nullptr, etTir gme_tir = eTirGen, QWidget *parent = nullptr);
  ~BTirGen();
  stGameConf * getGameConf(void);
  static stGameConf *gameUsrNew(const stGameConf *pGame, QString gameId="");
  void ShowPreviousGames(stGameConf *pGame);
  //void setAna(BTirAna * in_ana);


 private:
  QString showOrderedSelection(QString sql_msg);
  bool isNewUsrGame(const stGameConf *pGame, QString *gameId, QString *data);
  bool getGameKey(const stGameConf *pGame, QString *key);
  bool isSufficient(const stGameConf *pGame, int tot);
  bool isAlreadyKnown(QString key, QString * gameId);
  bool createGame(const stGameConf *pGame, QString gameId, QString data);
  void mkGameWidget(stGameConf * current);
  QGroupBox *  LireBoule(stGameConf *pGame, QString tbl_cible);
  QGroupBox *  LireTable(stGameConf *pGame, QString tbl_cible);
  QHBoxLayout *getBarZoomTirages(BView *qtv_tmp);
  QHBoxLayout *getBarFltTirages(int chk_nb_col, BView *qtv_tmp);

  ///void deletePreviousResults(const stGameConf *pGame);

 signals:

 public slots:
  void BSlot_ShowTotal(const QString& lstBoules);

 private slots:
  void BSlot_MouseOverLabel(QLabel *l);
  void BSlot_Clicked_Gen();
  void BSlot_Clicked_Gen(const QModelIndex &index);
  void BSlot_CheckBox(const QPersistentModelIndex &target, const Qt::CheckState &chk);
  void BSlot_ShowBtnId(int btn_id);
  //void slot_ShowChk(void);
  //void slot_ShowNhk(void);

 private:
  stGameConf *gameDef;
  QSqlDatabase db_gme;
  QWidget *resu;
  BView_1 *cible;
  QLabel *lb_Big;
  //BLineEdit *ble_rch;
};

#endif // BTirGen_H
