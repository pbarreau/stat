#ifndef BGAMELIST_H
#define BGAMELIST_H

#include <QWidget>
#include <QSqlDatabase>
#include <QGroupBox>
#include <QLabel>

#include "BSqlQmTirages_3.h"
#include "bstflt.h"

class BGameList : public QWidget
{
 Q_OBJECT
 public:
 explicit BGameList(const stGameConf *pGame, QWidget *parent = nullptr);
 ~BGameList();
 stGameConf * getGameConf(void);
 QString getGameId(void);

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
 void updateTbv(QString msg);

 signals:

 public slots:
 void slot_ShowNewTotal(const QString& lstBoules);
 void slot_RequestFromAnalyse(const QModelIndex & index, const int &zn, const etCount &eTyp);

 private slots:
 void slot_Colorize(QLabel *l);
 void slot_btnClicked();
 void slot_tbvClicked(const QModelIndex &index);
 void slot_UsrChk(const QPersistentModelIndex &target, const Qt::CheckState &chk);

 private:
 static int gme_counter;
 stGameConf *gameDef;
 QSqlDatabase db_gme;
 QString cur_game;

 private:
 BSqlQmTirages_3 *sqm_resu;
 QGroupBox *gpb_Tirages;
 QLabel *lb_Big;



};

#endif // BGAMELIST_H
