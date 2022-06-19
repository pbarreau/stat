#ifndef BTHREAD_1_H
#define BTHREAD_1_H

#include <QThread>

#include <QObject>
#include <QSqlDatabase>
#include <QMap>


#include "BcUpl.h"

struct stThreadParam{
  int u_id;
  eUpl_Ens e_id;
  stTskParam_1 *data;
};

#if 0
typedef struct _stBViewPath{
  QTabWidget *tab;
  int pos;
}stBViewPath;

typedef struct _stUplBViewPos{
    BView *view;
    stBViewPath ong_data[3];
}stUplBViewPos;
#endif

typedef struct _tsk_1{
  const stGameConf *pGame;
  const QModelIndexList *my_indexes;
  eUpl_Ens e_id;
  int z_id;
  int obj_upl;
  const QMap<QString, stUplBViewPos> *lst_view;
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
  eUpl_Cal g_cl;
  int l_id;
  int z_id;
  int g_id;
  int g_lm;
  int o_id;
  int r_id;
}stTskProgress;


class BThread_1: public QWidget //: public QThread
{
  Q_OBJECT

 public:
  BThread_1(const stGameConf *pGame);
  BThread_1(stTsk1 *def);
  void start();
  void start(etStep eStep = eStep_T1);
  void setUserSelection(QString sel);
  void setBview_1(BView **** tbv);

 public slots:
  void BSlot_UplCal(const stGameConf *pGame, const eUpl_Ens e_id, stTskParam_1 *tsk_param);

 private:
  void run() ;//override;
  void creationTables(etStep eStep = eStep_T1);

  bool T1_Fill_Bdd(const stGameConf *pGame, const stThreadParam *tsk_param, QString *tblName);
  bool T1_Fill_Bdd(stParam_tsk *tsk_param);
  stParam_tsk *T1_Scan(stParam_tsk *tsk_param);
  QString getTablePrefixFromSelection_tsk(QString items, int zn=0, stUpdData *upl_data=nullptr);

  stParam_tsk *FillBdd_StartPoint(stParam_tsk *tsk_param);
  void T2_Fill_Bdd(stParam_tsk *tsk_param);
  void T3_Fill_Bdd(stParam_tsk *tsk_param);
  void T4_Fill_Bdd(stParam_tsk *tsk_param);

  bool updateTracking(int v_key, eUpl_Cal v_cal);


 private:
  bool isSelectedKnown(etTir uplType, QString cur_sel, int zn, int *key);
  QString getCommaSeparatedTirage(const stGameConf *pGame, int zn, int tir_id);
  //QString getSqlTbv(const stGameConf *pGame, const eUpl_Ens e_id, int zn, int tir_Id, int day_Delta, int upl_Grp, int r_id=-1, eUpl_Lst target=ELstCal, int sel_item=-1);
  QString getSqlTbv(const stGameConf *pGame, const stThreadParam *tsk_param, eUpl_Lst target=ELstCal, int sel_item=-1);
  QString sql_ElmFrmTir(const stGameConf *pGame, const stThreadParam *tsk_param, eUpl_Lst sql_step, QString tabInOut[][3]);
  QString sql_ShowItems(const stGameConf *pGame, const stThreadParam *tsk_param, eUpl_Lst sql_show, QString cur_sql, int upl_sub=-1);
  void sql_upl_lev_1(const stGameConf *pGame, const stThreadParam *tsk_param, int step, QString tabInOut[][3]);
  void sql_upl_lev_2(const stGameConf *pGame, const stThreadParam *tsk_param,  QString tabInOut[][C_TOT_CAL][3]);

  QString sql_ElmNotFrmTir(const stGameConf *pGame, int zn, int  upl_ref_in, QString tabInOut[][3]);
  QString sql_NxtTirUpl(const stGameConf *pGame, int zn,int offset, QString tabInOut[][3]);
  QString sql_TirFrmUpl(const stGameConf *pGame, int zn,int  upl_ref_in, QString tabInOut[][3]);
  QString sql_TotFrmTir(const stGameConf *pGame, int zn, int upl_ref_in, int upl_sub, eUpl_Lst sql_step, QString tabInOut[][3]);
  QString sql_UplFrmElm(const stGameConf *pGame, int zn, int upl_ref_in, int upl_sub, eUpl_Lst sql_step, QString tabInOut[][3]);

 signals:
  void BSig_UplReadyStep1(const QString tblName, stTskParam_1 *tsk_param);
  void BSig_Step(const stParam_tsk *tsk_param);
  void BSig_Animate(const stParam_tsk *tsk_param, BAnimateCell *a_tbv);
  void BSig_UserSelect(const stParam_tsk *tsk_param);


 private:
  stTsk1 *tsk_1;
  QString cnx;  // nom de la connection a la bdd
  QSqlDatabase db_tsk1;
  QString cur_sel;
  BView ****uplThread_Bview_1;
  //QMap<QString, stUplBViewPos> *lst_view;
};

#endif // BTHREAD_1_H
