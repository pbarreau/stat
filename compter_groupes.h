#ifndef CCOMPTERGROUPES_H
#define CCOMPTERGROUPES_H

#include <QTabWidget>
#include <QTableView>
#include <QString>
#include <QStringList>
#include <QGridLayout>
#include <QStandardItemModel>
#include <QPoint>

#include "compter.h"


class BCountGroup:public BCount
{
 Q_OBJECT
 /// in : infos representant les tirages
 /// tb : nom de la table decrivant les zones
 public:
 BCountGroup(const stGameConf *pDef, QStringList** lstCri=nullptr);

 BCountGroup(const stGameConf &pDef, const QString &in, QStringList** lstCri, QSqlDatabase fromDb);
 ~BCountGroup();
 bool AnalyserEnsembleTirage(QString InputTable, QString OutputTable, int zn);
 bool SupprimerVueIntermediaires(void);
 QString getFilteringData(int zn);
 bool marquerDerniers_grp(const stGameConf *pGame, etCount eType, int zn);



 public slots:
 void slot_ClicDeSelectionTableau(const QModelIndex &index);
 void slot_RequeteFromSelection(const QModelIndex &index);
 void slot_DecodeTirage(const QModelIndex & index);
 void BSlot_AnaLgn(const int & l_id);
 void BSlot_RazSelection(void);


 private slots:
 //void slot_ccmr_SetPriorityAndFilters(QPoint pos);
 //void slot_wdaFilter(bool isChecked);



 private:
 static int total;
 int demande;
 QPoint save_pos;
 QTableView * save_view;
 QStringList **maRef; //zn_filter
 QStandardItemModel ** p_qsim_3;


 private:
 QGridLayout *Compter(QString * pName, int zn);
 QTableView *CompterLigne(QString * pName, int zn);
 QTableView *CompterEnsemble(QString * pName, int zn);
 QStringList * CreateFilterForData(int zn);
 QString CriteresAppliquer(QString st_tirages, QString st_cri,int zn);
 QString TrouverTirages(int col, int nb, QString st_tirages, QString st_cri, int zn);
 void RecalculGroupement(int zn,int nbCol,QStandardItemModel *sqm_tmp);
 QString sql_ComptePourUnTirage(int id,QString st_tirages, QString st_cri, int zn);
 void SqlFromSelection (const QItemSelectionModel *selectionModel, int zn);
 bool updateOrInsertGrpSelection(int d_cell_id, bool isPresent,bool isChecked, int zn);
 bool updateGrpTable(int d_lgn, int d_col, bool isChecked, int zn);


 private:
 QWidget *fn_Count(const stGameConf *pGame, int zn);
 void verticalResizeTableViewToContents(QTableView *tableView);
 bool db_MkTblItems(const stGameConf *pGame, int zn, QString dstTbl, QSqlQuery * query, QString * msg);

 private:
 //virtual QString getType();
 virtual  QTabWidget *startCount(const stGameConf *pGame, const etCount eCalcul);
 virtual bool usr_MkTbl(const stGameConf *pDef, const stMkLocal prm, const int zn);
 void marquerDerniers_tir(const stGameConf *pGame, etCount eType, int zn);
 virtual void usr_TagLast(const stGameConf *pGame, BTbView *view, const etCount eType, const int zn);
 //virtual QLayout * usr_UpperItems(int zn, BTbView *cur_tbv);
 QWidget *mainIhmGrp(const stGameConf *pGame, const etCount eCalcul, const ptrFn_tbl fn, const int zn);
 QWidget *usr_GrpTb1(int zn);
 QString getSqlForLine(int l_id, int zn);
 void showLineDetails(int l_id, QString sql_msg);

 private:
 QSqlDatabase db_grp;
 QStringList **slFlt;
 BGTbView *tbvAnaLgn;
 int total_cells;

};

#endif // CCOMPTERGROUPES_H
