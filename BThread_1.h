#ifndef BTHREAD_1_H
#define BTHREAD_1_H

#include <QThread>

#include <QSqlDatabase>


#include "BcUpl.h"

typedef struct _tsk_1{
  const stGameConf *pGame;
  const QModelIndexList *my_indexes;
  eUpl_Ens e_id;
  int z_id;
  int obj_upl;
}stTsk1;

typedef enum _progress{
 eStep_T0, /// depart
 eStep_T1, /// Info dispos
 eStep_T2, /// Info dispos
 eStep_T3, /// Info dispos
 eStep_T4 /// Info dispos
}etStep;

typedef struct _tskProgress
{
  etStep e_id;
  QString t_on;
  QString t_rf;
  eUpl_Lst c_id;
  int l_id;
  int z_id;
  int g_id;
  int o_id;
  int r_id;
}stTskProgress;

class BThread_1 : public QThread
{
  Q_OBJECT

 public:
  BThread_1(stTsk1 *def);


 private:
  void run() override;
  void creationTables(etStep eStep = eStep_T1);

  bool T1_Fill_Bdd(stParam_tsk *tsk_param);
  stParam_tsk *T1_Scan(stParam_tsk *tsk_param);
  QString getTablePrefixFromSelection(QString items, int zn=0, stUpdData *upl_data=nullptr);

  stParam_tsk *FillBdd_StartPoint(stParam_tsk *tsk_param);
  void T2_Fill_Bdd(stParam_tsk *tsk_param);
  void T3_Fill_Bdd(stParam_tsk *tsk_param);
  void T4_Fill_Bdd(stParam_tsk *tsk_param);

  bool updateTracking(int v_key, eUpl_Cal v_cal);


 private:
  QString getSqlTbv(const stGameConf *pGame, int zn, int tir_Id, int day_Delta, int upl_Grp, int r_id=-1, eUpl_Lst target=ELstCal, int sel_item=-1);
  QString sql_ElmFrmTir(const stGameConf *pGame, int zn, eUpl_Lst sql_step, int tir_id,QString tabInOut[][3]);
  QString sql_ShowItems(const stGameConf *pGame, int zn, eUpl_Lst sql_show, int cur_upl, QString cur_sql, int upl_sub=-1);
  void sql_upl_lev_1(const stGameConf *pGame, int zn, int tirLgnId, int upl_ref_in, int offset, int upl_sub, int step, QString tabInOut[][3]);
  void sql_upl_lev_2(const stGameConf *pGame, int z_id, int l_id, int o_id, int g_id, int r_id,  QString tabInOut[][C_TOT_CAL][3]);

  QString sql_ElmNotFrmTir(const stGameConf *pGame, int zn, int  upl_ref_in, QString tabInOut[][3]);
  QString sql_NxtTirUpl(const stGameConf *pGame, int zn,int offset, QString tabInOut[][3]);
  QString sql_TirFrmUpl(const stGameConf *pGame, int zn,int  upl_ref_in, QString tabInOut[][3]);
  QString sql_TotFrmTir(const stGameConf *pGame, int zn, int upl_ref_in, int upl_sub, eUpl_Lst sql_step, QString tabInOut[][3]);
  QString sql_UplFrmElm(const stGameConf *pGame, int zn, int upl_ref_in, int upl_sub, eUpl_Lst sql_step, QString tabInOut[][3]);

 signals:
  void BSig_Step(const stTskProgress *step);

 private:
  stTsk1 *tsk_1;
  QString cnx;  // nom de la connection a la bdd pour ce process
  QSqlDatabase db_tsk1;
};

#endif // BTHREAD_1_H
