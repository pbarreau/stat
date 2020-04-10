#ifndef CBARYCENTRE_H
#define CBARYCENTRE_H

#include <QWidget>
#include <QSqlDatabase>
#include <QGridLayout>
#include <QObject>

#include "compter.h"
#include "game.h"


class BCount;

typedef struct _prmbary{
 QString ncx; /// nom de la connection
 QSqlDatabase db; /// base en cours
 QString tbl_in; /// Nom de la table avec les infos
 QString tbl_ana;
 QString tbl_flt;
 stGameConf pDef;
 QObject *origine;
}stNeedsOfBary;


class BCountBrc:public BCount
{
 Q_OBJECT
 public:
 BCountBrc(const stGameConf *pDef);

 BCountBrc(const stNeedsOfBary &param);
 QString getFilteringData(int zn);

 private:
 void marquerDerniers_bar(int zn);
 void hc_RechercheBarycentre(QString tbl_in);
 bool isTableTotalBoulleReady(QString tbl_total);
 bool mettreBarycentre(QString tbl_dst, QString src_data);
 bool repereDernier(QString tbl_bary);
 QGridLayout *AssocierTableau(QString tbl_src);
 QGridLayout *Compter(QString * pName, int zn);

 private:
 QWidget *fn_Count(const stGameConf *pGame, int zn);
 QString sql_MkCountItems(const stGameConf *pGame, int zn);

 private:
 virtual QString getType();
 virtual  QTabWidget *creationTables(const stGameConf *pGame);


 private:
 BCountBrc * addr;
 QSqlDatabase db_1;

 private:
 static int total;
 QString tbl_src;
 QString tbl_ana;
 QString tbl_flt;
 stGameConf gameDef;
};

#endif // CBARYCENTRE_H
