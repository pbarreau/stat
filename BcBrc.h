#ifndef CBARYCENTRE_H
#define CBARYCENTRE_H

#include <QWidget>
#include <QSqlDatabase>
#include <QGridLayout>
#include <QObject>

#include "Bc.h"
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


class BcBrc:public BCount
{
 Q_OBJECT
 public:
 BcBrc(const stGameConf *pDef);

 BcBrc(const stNeedsOfBary &param);
 QString getFilteringData(int zn);

 private:
 void marquerDerniers_bar(const stGameConf *pGame, etCount eType, int zn);
 void hc_RechercheBarycentre(QString tbl_in);
 bool isTableTotalBoulleReady(QString tbl_total);
 bool mettreBarycentre(QString tbl_dst, QString src_data);
 bool repereDernier(QString tbl_bary);
 QGridLayout *AssocierTableau(QString tbl_src);
 QGridLayout *Compter(QString * pName, int zn);

 private:
 QWidget *fn_Count(const stGameConf *pGame, int zn);
 QString getSqlMsg(const stGameConf *pGame, int zn);

 private:
 //virtual QString getType();
 virtual  QTabWidget *startCount(const stGameConf *pGame, const etCount E_Calcul);
 virtual bool usr_MkTbl(const stGameConf *pDef, const stMkLocal prm, const int zn);
 void marquerDerniers_tir(const stGameConf *pGame, etCount eType, int zn);
 virtual void usr_TagLast(const stGameConf *pGame, BView_1 *view, const etCount eType, const int zn);


 private:
 BcBrc * addr;
 QSqlDatabase db_brc;

 private:
 static int total;
 QString tbl_src;
 QString tbl_ana;
 QString tbl_flt;
 stGameConf gameDef;
};

#endif // CBARYCENTRE_H
