#ifndef BGAMELIST_H
#define BGAMELIST_H

#include <QWidget>
#include <QSqlDatabase>
#include <QGroupBox>
#include <QLabel>

#include "game.h"
#include "BSqlQmTirages_3.h"

class BGameList : public QWidget
{
 Q_OBJECT
 public:
 explicit BGameList(const stGameConf *pGame, QWidget *parent = nullptr);
 ~BGameList();
 stGameConf * getGameConf(void);

 private:
 bool isNewUsrGame(const stGameConf *pGame, QString *gameId, QString *data);
 bool getGameKey(const stGameConf *pGame, QString *key);
 bool isSufficient(const stGameConf *pGame, int tot);
 bool isAlreadyKnown(QString key, QString * gameId);
 bool createGame(const stGameConf *pGame, QString gameId, QString data);
 void mkGameWidget(stGameConf * current);
 QGroupBox *  LireBoule(stGameConf *pGame, QString tbl_cible);
 QGroupBox *  LireTable(stGameConf *pGame, QString tbl_cible);
 stGameConf *newGameConf(const stGameConf *pGame, QString gameId);

 signals:

 public slots:
 void slot_ShowNewTotal(const QString& lstBoules);

 private slots:
 void slot_Colorize(QLabel *l);
 void slot_btnClicked();
 void slot_tbvClicked(const QModelIndex &index);
 void slot_UsrChk(const QPersistentModelIndex &target, const Qt::CheckState &chk);

 private:
 static int gme_counter;
 //BGameList *addr;
 stGameConf *gameDef;
 QSqlDatabase db_gme;
 //QString key;

 private:
 BSqlQmTirages_3 *sqm_resu;
 QGroupBox *gpb_Tirages;
 QLabel *lb_Big;



};

#endif // BGAMELIST_H
