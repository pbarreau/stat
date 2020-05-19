#ifndef CCOMPTERCOMBINAISONS_H
#define CCOMPTERCOMBINAISONS_H

#include <QTabWidget>
#include <QTableView>
#include <QString>
#include <QStringList>
#include <QGridLayout>

#include "BCount.h"

#define HCELL       55

class BCountComb:public BCount
{
 Q_OBJECT
 /// in : infos representant les tirages
 public:
 BCountComb(const stGameConf *pDef);

 BCountComb(const stGameConf &pDef, const QString &in, QSqlDatabase fromDb);
 ~BCountComb();
 void marquerDerniers_cmb(const stGameConf *pGame, etCount eType, int zn);
 QString getFilteringData(int zn);

 public slots:
 void slot_ClicDeSelectionTableau(const QModelIndex &index);
 void slot_RequeteFromSelection(const QModelIndex &index);


 private slots:
 void BSlot_FilterCmb(const QString &flt_string);

 private:
 static int total;
 int hCommon;


 private:
 QGridLayout *Compter(QString * pName, int zn);
 QString RequetePourTrouverTotal_z1(QString st_baseUse, int zn, int dst);
 void LabelFromSelection(const QItemSelectionModel *selectionModel, int zn);
 void SqlFromSelection (const QItemSelectionModel *selectionModel, int zn);
 QString ConstruireCriteres(int zn);

 private:
 QWidget *fn_Count(const stGameConf *pGame, int zn);
 QString usr_doCount(const stGameConf *pGame, int zn);
 void marquerDerniers_tir(const stGameConf *pGame, etCount eType, int zn);
 QHBoxLayout *getBarFltTirages(int zn, BGTbView *qtv_tmp);

 private:
 //virtual QString getType();
 virtual  QTabWidget *startCount(const stGameConf *pGame, const etCount eCalcul);
 virtual bool usr_MkTbl(const stGameConf *pDef, const stMkLocal prm, const int zn);
 virtual void usr_TagLast(const stGameConf *pGame, BTbView *view, const etCount eType, const int zn);
 virtual QLayout * usr_UpperItems(int zn, BTbView *cur_tbv);


 private:
 BCountComb * addr;
 QSqlDatabase db_cmb;


};

#endif // CCOMPTERCOMBINAISONS_H
