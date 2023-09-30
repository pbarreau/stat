#ifndef BUPLET_H
#define BUPLET_H

#include <QObject>

#include <QThreadPool>
#include <QMutex>

#include <QWidget>
#include <QSqlDatabase>
#include <QGroupBox>

#include <QLineEdit>
#include <QTableView>

#include <QTableView>
#include <QDialog>
#include <QMap>

#include "bvtabbar.h"
#include "game.h"
#include "BView.h"
#include "Bc.h"

#include "BAnimateCell.h"
#include "ns_upl.h"

//#include "BThread_1.h"

#define  C_MIN_UPL    1
#define  C_MAX_UPL    3
#define  C_NB_ONG     3

#define  C_NB_OFFSET  3
#define  C_NB_SUB_ONG 3
#define  C_NB_SUB_ONG_CAL C_NB_SUB_ONG - 1
#define  C_NB_TIR_LIR 2

#define  C_TBL_UPL "Upl_lst"

#define C_PGM_THREADED false
#define C_MAX_SCAN_ITEMS 20



typedef enum _eEnsemble /// Ensemble dans lequel chercher les uplets
{
 E_EnsNotSet, /// Ensemble non defini
 E_EnsFdj,    /// Liste des tirages de la fdj
 E_EnsUsr,    /// Selection de l'utilisateur
 E_EnsEnd     /// Fin enumeration
}etEns;
extern const QString TXT_UplSrcKey[E_EnsEnd];



typedef struct _dbUpdl{
  etEns e_id; /// type d'ensemble
  etCal id_cal;
  int id_db;
  int id_zn;
  bool isPresent;
}stUpdData;

/// --------------
struct stDataUpl{
  etEns   e_id; /// Origine
  etCal   i_cl; /// Etat de la recherche
  etLst   c_id; /// Calcul type id
  int     l_id; /// Ligne id (dans base ou user)
  int     z_id; /// Zone id
  int     g_id; /// Groupe id (Cnp)
  int     g_lm; /// Groupe element (indice element dans Groupe id)
  int     o_id; /// Offset from day value
  int     r_id; /// Response id
  QString t_rf; /// Nom de la table de reference
  QString t_on; /// nom de la table actuelle
};
/// --------------

typedef struct _stDays
{
  int delta;
  QString onglet;
  QString tbl;
}stDays;

typedef struct _tskProgress stTskProgress;
typedef struct _param_tsk
{
  const stGameConf *p_gm; /// ptr config du jeu
  int l_id;  /// Ligne id (dans base ou user)
  int z_id;  /// Zone id
  int g_id;  /// Groupe id (Cnp)
  int g_lm;  /// Groupe element (indice element dans Groupe id)
  int o_id;  /// Offset from day value
  int r_id;  /// Response id

  int upl_tot;  /// total de cette valeur
  bool clear;   /// Effacer resultat dans tbv

  QString t_rf; /// Nom de la table de reference
  QString t_on; /// nom de la table actuelle

  QString upl_txt; /// valeur du uplet
  QGroupBox *grb_target;

  etLst c_id; /// Calcul type id
  etEns e_id; /// type d'ensemble

  stUpdData glm_in;  /// index dans la base pour cet indice et +
  struct _tskProgress *tsk_step;
  BAnimateCell *a_tbv;
  BView *cupl;
  stDays const *ptrDelta;
  }stParam_tsk;

class BThread_1;

class BcUpl: public BCount
{
  Q_OBJECT

 public:
  static const stDays defDays[C_NB_ONG];


 public:
  explicit BcUpl(const stGameConf *pGame, QWidget *parent=nullptr, etEns eUpl=E_EnsFdj, int zn=0, const QItemSelectionModel *cur_sel=nullptr, QTabWidget *ptrUplRsp=nullptr);
  ~BcUpl();
  ///Test
  QGridLayout *getLayout(void);
  void setLayout(QGridLayout *lay);

 public slots:
  void BSlot_MkUsrUpletsShow(const QItemSelectionModel *cur_sel, const int zn);
  void BSlot_SkowUkScan(stParam_tsk *tsk_param);

 private slots:
#if C_PGM_THREADED
  void BSlot_tsk_started();
  void BSlot_tsk_finished();
  void BSlot_scan_finished();
  void BSlot_tsk_progress(const stParam_tsk *tsk_param);
  void BSlot_UserSelect(const stParam_tsk *tsk_param);
  void BSlot_Animate(const stParam_tsk *tsk_param, BAnimateCell *a_tbv);
#endif
  void BSlot_ShowTotal(const QString& lstBoules);
  void BSlot_UVL1_Click_Fn_0(const QModelIndex &index);
  void BSlot_UVL1_Click_Fn_1(const QModelIndex &index);
  void BSlot_UVL1_CM1(QPoint pos);
  void BSlot_UVL1_Click_Fn_2(const QModelIndex &index);
  void BSlot_UVL1_Click_Fn_3(bool chk);
  void BSlot_UVL2_Click_Fn_1(const QModelIndex &index);
  void BSlot_TGid_Click(int index);
  void BSlot_over(const QModelIndex &index);
  void BSlot_Repaint(const BView * tbv);
  void BSlot_UVL1_Cmr_Fn_1(QPoint pos);
  void BSlot_UplSel(const QModelIndex & index);
  void BSlot_UplScanAll(void);
  void BSlot_UplScanSelected(void);

 Q_SIGNALS:
  void BSig_StartUkScan(stParam_tsk *tsk_param);
  //void BSig_UplCal(const stGameConf *pGame, const etEns e_id, stTskParam_1 *tsk_param);
  void BSig_UplFdjShow(const QString items, int zn);
  //void BSig_Animate(const stParam_tsk *tsk_param, BAnimateCell *a_tbv);

 private:
  static int nb_max_recherche;
  QMutex mutex;
  static QThreadPool *pool;
  static int obj_upl;
  QSqlDatabase db_0;
  QModelIndexList my_indexes;
  QTabWidget *uplTirTab;
  QGridLayout *lay_Uplets;
  bool isScanRuning;
  etEns e_id;

  int upl_zn;
  QString upl_current;
  BView ****upl_Bview_0;   /// Bilan d'un uplet
  BView ****upl_Bview_1;   /// Selection d'un uplet
  BView ******upl_Bview_2; /// Resultat pour la selection
  BView ******upl_Bview_3; /// Bilan de resultat
  BView ******upl_Bview_4; /// complementaire de resultat

  BAnimateCell ****tbv_Anim;   /// Selection d'un uplet
  BThread_1 *producteur;
  //QMap<QString, stUplBViewPos> *mapView_1;
  QWidget *ana_parent;


 private:
  bool isSelectedKnown(const QItemSelectionModel *cur_sel, int zn, int *key);
  QString getTablePrefixFromSelection_upl(QString items, int zn=0, stUpdData *upl_data=nullptr);
  QHBoxLayout *getBar_Rch(BView *qtv_tmp,int tab_id);

  QString sql_ShowItems(const stGameConf *pGame, int zn, etLst sql_show, int cur_upl, QString cur_sql, int upl_sub=-1);

 private:
  virtual  QTabWidget *startCount(const stGameConf *pGame, const etCount E_Calcul);
  virtual bool usr_MkTbl(const stGameConf *pDef, const stMkLocal prm, const int zn);
  virtual void usr_TagLast(const stGameConf *pGame, BView_1 *view, const etCount eType, const int zn);
  //virtual QLayout * usr_UpperItems(int zn, BTbView *cur_tbv);
  QGridLayout *Compter(QString * pName, int zn);

 private:
  QString getSqlTbv(const stGameConf *pGame, int zn, int tir_Id, int day_Delta, int upl_Grp, int upl_Sub=-1, etLst target=E_LstCal, int sel_item=-1);
  QWidget *showUplFromRef(const stGameConf *pGame, int z_id, int l_id, int g_id);
  QWidget *getUplDetails(const stGameConf *pGame, int zn, int tirLgnId, int src_upl, int relativeDay, int nb_recherche);
  void sql_upl_lev_1(const stGameConf *pGame, int zn, int tirLgnId, int upl_ref_in, int offset, int upl_sub, int step, QString tabInOut[][3]);
  void sql_upl_lev_2(const stGameConf *pGame, int zn, int tirLgnId, int offset, int upl_ref_in, int upl_sub, QString tabInOut[][C_TOT_CAL][3]);

  void sql_FillTabArgs(const stGameConf *pGame, int zn,int upl_ref_in, QString *tab_arg, QString tabInOut[][3]);

  QString sql_ElmFrmTir(const stGameConf *pGame, int zn, etLst sql_step, int tir_id, QString tabInOut[][3]);
  QString sql_ElmNotFrmTir(const stGameConf *pGame, int zn, int upl_ref_in, QString tabInOut[][3]);

  QString sql_UplFrmElm(const stGameConf *pGame, int zn, int upl_ref_in, int upl_sub, etLst sql_step, QString tabInOut[][3]);
  QString sql_TirFrmUpl(const stGameConf *pGame, int zn, int upl_ref_in, QString tabInOut[][3]);
  QString sql_TotFrmTir(const stGameConf *pGame, int zn, int upl_ref_in, int upl_sub, etLst sql_step, QString tabInOut[][3]);

  QString sql_NxtTirUpl(const stGameConf *pGame, int zn,int offset, QString tabInOut[][3]);

  void sql_RepartitionVoisin(QString tabInOut[][3], int jour, int upl, int ref_id, int delta=0);

  QWidget *Bview_init(const stGameConf *pGame, int zn, int tirLgnId, int offset, int src_upl, int relativeDay, int dst_upl);
  BView * Bview_3_fill_1(BView *qtv_tmp, QString sql_msg);
  BView * Bview_4_fill_1(BView *qtv_tmp, QString sql_msg);

  //QWidget *fill_Bview_1(const stGameConf *pGame, int zn, int tirLgnId, int i);
  QWidget *MkMainUplet(stParam_tsk *tsk_param);

  int Bview_UpdateAndCount(etLst id, BView *qtv_tmp, QString sql_msg);
  //int Bview_3_fill_2(BView *qtv_tmp, QString sql_msg);
  //int Bview_4_fill_2(BView *qtv_tmp, QString sql_msg);
  //bool effectueRecherche(etEns upl_type, QString upl_sql, int upl_id,int zn_id, int nb_items);
  //bool tsk_upl_1 (const stGameConf *pGame, const stParam_tsk *param);
  //void rechercheUplet(QString tbl_prefix, const stGameConf *pGame, const stParam_tsk *param, int fake_sel);
  //void tsk_upl_2(QString cnx, QString tbl, QString sql);
  //void tsk_upl_0(stParam_tsk *tsk_param);

  //bool updateTracking(int v_key, etCal v_cal);

  //stParam_tsk *FillBdd_StartPoint(stParam_tsk *tsk_param);
  //bool T1_Fill_Bdd(stParam_tsk *tsk_param);
  //stParam_tsk *T1_Scan(stParam_tsk *tsk_param);
  //void T2_Fill_Bdd(stParam_tsk *tsk_param);
  //void T3_Fill_Bdd(stParam_tsk *tsk_param);
  //void T4_Fill_Bdd(stParam_tsk *tsk_param);

  void FillTbv_StartPoint(stParam_tsk *tsk_param);

  BView *FillTbv_BView_1(stParam_tsk *tsk_param);
  void startAnimation(const stParam_tsk *tsk_param, BAnimateCell *a_tbv);
  void T1_setTitle(BView *qtv_tmp, const stTskProgress *step);

  void FillTbv_BView_2(stParam_tsk *tsk_param);
  void FillTbv_BView_3(stParam_tsk *tsk_param);
  void FillTbv_BView_4(stParam_tsk *tsk_param);

  int getFromView_Lid(const BView *view);
  QString getFromIndex_CurUpl(const QModelIndex &index, int upl_GrpId, QGroupBox **grb = nullptr);
  //void BSlot_clicked_old(const QModelIndex &index);
  bool updateTracking_upl(int v_key, etCal v_cal);

};


#endif // BUPLET_H
