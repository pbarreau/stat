#ifndef BTHREAD_1_H
#define BTHREAD_1_H

#include <QThread>

#include <QObject>
#include <QSqlDatabase>


#include "BcUpl.h"

typedef struct _tsk_1{
  const stGameConf *pGame;
  const QModelIndexList *my_indexes;
  etEns e_id;
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
  etLst c_id;
  etCal g_cl;
  int l_id;
  int z_id;
  int g_id;
  int g_lm;
  int o_id;
  int r_id;
}stTskProgress;

struct stParamInThread{
  etEns e_id; /// Ensemble des elements de depart
  int u_id;   /// indice de l'objet upl en cours
  int l_id;   /// Ref du top most onglet dans onglet uplet
  int g_lm;   /// indice de l'uplet en cours dans le groupe uplet principal
  int o_id;   /// indice de l'offset jour
  int r_id;   /// indice de l'onglet des reponses ( uplets )
  etLst c_id; /// Etape du calcul generant le code sql des tables reponses uplets

  /// codage rapide
  etCal g_cl;
  int zid;
  int gid;
  QModelIndexList my_indexes;
};

class BThread_1: public QWidget //: public QThread
{
  Q_OBJECT

 public:
  BThread_1(const stGameConf *pGame);
  BThread_1(stTsk1 *def);
  void start();
  void start(etStep eStep = eStep_T1);
  void setUserSelection(QString sel);

 public slots:
  void BSlot_IhmIsSet();
  void BSlot_StartUkScan(stParam_tsk *tsk_param);
  void BSlot_StartFullScan(BView *tbv);

 private:
  QString getSqlLstUplToScan(const stGameConf *pGame, BView *view);
  void run() ;//override;
  void creationTables(etStep eStep = eStep_T1);

  bool T1_Fill_Bdd(stParam_tsk *tsk_param);
  bool T1_Fill_Bdd(const stGameConf *pGame, BView *view, QString table, stParamInThread val);
  stParam_tsk *T1_Scan(stParam_tsk *tsk_param);
  QString getTablePrefixFromSelection_tsk(QString items, int zn=0, stUpdData *upl_data=nullptr);

  stParam_tsk *FillBdd_StartPoint(stParam_tsk *tsk_param);
  bool Uk_FillBdd_StartPoint(const stGameConf *pGame, BView *view, stParamInThread *val, const QSqlQuery &query);
  bool Mk_FullTbls(const stGameConf *pGame, QString table, stParamInThread *val);
  bool Uk_MakeTable_T1(const stGameConf *pGame, BView *view, QString table, stParamInThread val);
  bool Uk_MakeTable_T2(const stGameConf *pGame, BView *view, QString table, stParamInThread val);
  bool Uk_MakeTable_T3(const stGameConf *pGame, BView *view, QString table, stParamInThread val);
  void T2_Fill_Bdd(stParam_tsk *tsk_param);
  void T3_Fill_Bdd(stParam_tsk *tsk_param);
  void T4_Fill_Bdd(stParam_tsk *tsk_param);

  bool updateTracking(int v_key, etCal v_cal);


 private:
  QString getSqlTbv(const stGameConf *pGame, int zn, int tir_Id, int day_Delta, int upl_Grp, int r_id=-1, etLst target=E_LstCal, int sel_item=-1);
  QString sql_ElmFrmTir(const stGameConf *pGame, int zn, etLst sql_step, int tir_id,QString tabInOut[][3]);
  QString sql_ShowItems(const stGameConf *pGame, int zn, etLst sql_show, int cur_upl, QString cur_sql, int upl_sub=-1);
  void sql_upl_lev_1(const stGameConf *pGame, int z_id, int l_id, int g_id, int o_id, int r_id, int c_id, QString tabInOut[][3]);
  void sql_upl_lev_2(const stGameConf *pGame, int z_id, int l_id, int o_id, int g_id, int r_id,  QString tabInOut[][C_TOT_CAL][3]);

  QString sql_ElmNotFrmTir(const stGameConf *pGame, int zn, int  upl_ref_in, QString tabInOut[][3]);
  QString sql_NxtTirUpl(const stGameConf *pGame, int zn,int offset, QString tabInOut[][3]);
  QString sql_TirFrmUpl(const stGameConf *pGame, int zn,int  upl_ref_in, QString tabInOut[][3]);
  QString sql_TotFrmTir(const stGameConf *pGame, int zn, int g_id, int r_id, etLst c_id, QString tabInOut[][3]);
  QString sql_UplFrmElm(const stGameConf *pGame, int zn, int upl_ref_in, int upl_sub, etLst sql_step, QString tabInOut[][3]);

  QString getSqlTbv(const stGameConf *pGame, stParamInThread *val, int sel_item=-1);
  void sql_upl_lev_1(const stGameConf *pGame, stParamInThread *val, QString tabInOut[][3]);
  QString sql_ElmFrmTir(const stGameConf *pGame, stParamInThread *val,QString tabInOut[][3]);
  void sql_upl_lev_2(const stGameConf *pGame, stParamInThread *val,  QString tabInOut[][C_TOT_CAL][3]);

 signals:
  void BSig_DbTableSet(QString table, stParamInThread *val);
  void BSig_ShowUpletProgress(BView *view, const stParamInThread *val);
  void BSig_SkowUkScan(stParam_tsk *tsk_param);
  void BSig_Step(const stParam_tsk *tsk_param);
  void BSig_Animate(const stParam_tsk *tsk_param, BAnimateCell *a_tbv);
  void BSig_UserSelect(const stParam_tsk *tsk_param);


 private:
  stTsk1 *tsk_1;
  QString cnx;  // nom de la connection a la bdd
  QSqlDatabase db_tsk1;
  QString cur_sel;
};

#endif // BTHREAD_1_H
