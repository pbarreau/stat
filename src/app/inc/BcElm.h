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
#include "Bc.h"
#include "labelclickable.h"

class BcElm:public BCount//, public cFdjData
{
  Q_OBJECT
  /// in : infos representant les tirages
 public:
  BcElm(const stGameConf *pDef);

  BcElm(const stGameConf &pDef, const QString &in, QSqlDatabase fromDb, QWidget *LeParent);
  ~BcElm();
  int getCounter(void);
  QString getFilteringData(int zn);
  LabelClickable *getLabPriority(void);

 public slots:
  void slot_ClicDeSelectionTableau(const QModelIndex &index);
  void slot_RequeteFromSelection(const QModelIndex &index);
  void BSlot_MkUsrUplets_L3(const QItemSelectionModel *cur_sel);

 public:
  static int tot_elm;
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
  QString getSqlMsg(const stGameConf *pGame, int z_id);
  QString getSqlMsg(const stGameConf *pGame, const stMkLocal prm);

 private:
  void marquerProcheVoisin(const stGameConf *pGame, const int zn, stTbFiltres *a);
  virtual  QTabWidget *startCount(const stGameConf *pGame, const etCount E_Calcul);
  virtual bool usr_MkTbl(const stGameConf *pDef, const stMkLocal prm, const int zn);
  virtual void usr_TagLast(const stGameConf *pGame, BView_1 *view, const etCount eType, const int zn);
  virtual QLayout * usr_UpperItems(int zn, BView_1 *cur_tbv);
  //virtual QList<BLstSelect *> *getSelection(void);


 private slots:
  void BSlot_textChanged(const QString cur_txt);
  //void BSlot_TbvClick(const QModelIndex &index);

 private:
  QSqlDatabase db_elm;


 Q_SIGNALS:
  void sig_isClickedOnBall(const QModelIndex &index);
  void BSig_MkUsrUplets_L4(const QItemSelectionModel *cur_sel, const int zn_id);
  void BSig_ElmFdjShow(const QString items, int zn);

};

#endif // CTABZNCOUNT_H
