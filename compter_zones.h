#ifndef CTABZNCOUNT_H
#define CTABZNCOUNT_H

#include <QTabWidget>
#include <QTableView>
#include <QString>
#include <QStringList>
#include <QMenu>
#include <QActionGroup>
#include <QWidget>
#include <QObject>
#include <QSortFilterProxyModel>
#include <QSqlQueryModel>

#include "game.h"
#include "compter.h"
#include "labelclickable.h"

class BCountElem:public BCount//, public cFdjData
{
 Q_OBJECT
 /// in : infos representant les tirages
 public:
 BCountElem(const stGameConf *pDef);

 BCountElem(const stGameConf &pDef, const QString &in, QSqlDatabase fromDb, QWidget *LeParent);
 ~BCountElem();
 int getCounter(void);
 QString getFilteringData(int zn);
 LabelClickable *getLabPriority(void);

 public slots:
 void slot_ClicDeSelectionTableau(const QModelIndex &index);
 void slot_RequeteFromSelection(const QModelIndex &index);

 public:
 static int total;
 int hCommon; // taille des tableaux

 private:
 QGridLayout *Compter(QString * pName, int zn);
 void marquerDerniers_tir(const stGameConf *pGame, etCount eType, int zn);
 QStringList * CreateFilterForData(int zn);
 QString CriteresAppliquer(QString st_tirages, QString st_cri,int zn);
 QString TrouverTirages(int col, int nb, QString st_tirages, QString st_cri, int zn);
 QString PBAR_ReqComptage(QString ReqTirages, int zn,int distance);
 void SqlFromSelection (const QItemSelectionModel *selectionModel, int zn);

 private:
 QWidget *fn_Count(const stGameConf *pGame, int zn);
 QString sql_MkCountItems(const stGameConf *pGame, int zn);

 private:
 //virtual QString getType();
 virtual  QTabWidget *creationTables(const stGameConf *pGame);


 private:
 BCountElem * addr;
 QSqlDatabase db_1;


 Q_SIGNALS:
 void sig_isClickedOnBall(const QModelIndex &index);

};

#endif // CTABZNCOUNT_H
