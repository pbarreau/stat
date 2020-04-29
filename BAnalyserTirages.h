#ifndef BANALYSERTIRAGES_H
#define BANALYSERTIRAGES_H

#include <QString>
#include <QSqlDatabase>
#include <QMap>

#include "game.h"
#include "bstflt.h"

class BAnalyserTirages : public QWidget
{
 Q_OBJECT
 public:
 explicit BAnalyserTirages(stGameConf *pGame, QWidget *parent=nullptr);
 BAnalyserTirages *self();
 QWidget *getVisual();
 static int getCounter(void);

 private:
 bool isPresentUsefullTables(stGameConf *pGame, QString tbl_tirages, QString cnx);
 void startAnalyse(stGameConf *pGame, QString tbl_tirages);

 bool mkTblLstElm(stGameConf *pGame, QString tbName, QSqlQuery *query);
 bool mkTblLstCmb(stGameConf *pGame, QString tbName, QSqlQuery *query);
 bool mkTblGmeDef(stGameConf *pGame, QString tbName,QSqlQuery *query);
 bool mkTblFiltre(stGameConf *pGame, QString tbName,QSqlQuery *query);

 QStringList* CreateFilterForData(stGameConf *pGame, QString tbl_tirages, int zn);
 QString sqlMkAnaBrc(stGameConf *pGame, QString tbl_tirages, int zn);
 QString sqlMkAnaCmb(stGameConf *pGame, QString tbl_ana_tmp, int zn);

 bool AnalyserEnsembleTirage(stGameConf *pGame, QStringList ** info, int zn, QString tbName);
 bool SupprimerVueIntermediaires(void);
 void PresenterResultats(stGameConf *pGame, QStringList ** info, QString tbName);

 bool usrFn_X1(const stGameConf *pGame, QString curName, QString curTarget, int zn_in);
 QString getFieldsFromZone(const stGameConf *pGame, int zn, QString alias);

 typedef bool(BAnalyserTirages::*ptrFnUsr)(const stGameConf *, QString, QString, int );

 signals:
 void bsg_clicked(const QModelIndex & index, const int &zn, const etCount &eTyp);

 public slots:
 void bsl_clicked(const QModelIndex & index, const int &zn, const etCount &eTyp);

 private:
 static int total_analyses;
 BAnalyserTirages *addr;
 //QWidget *show_results;
 QSqlDatabase db_1;
 QMap<QString,ptrFnUsr> map_UsrFn;

};

#endif // BANALYSERTIRAGES_H
