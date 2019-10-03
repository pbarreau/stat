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


class CBaryCentre:public BCount
{
 Q_OBJECT
 public:
 CBaryCentre(const stNeedsOfBary &param);
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
 static int total;
 QSqlDatabase db_1;
 QString tbl_src;
 QString tbl_ana;
 QString tbl_flt;
 stGameConf gameDef;
};

#endif // CBARYCENTRE_H
