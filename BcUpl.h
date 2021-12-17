#ifndef BUPLET_H
#define BUPLET_H

#include <QObject>
#include <QWidget>
#include <QSqlDatabase>
#include <QGroupBox>

#include <QLineEdit>
#include <QTableView>

#include <QTableView>
#include <QDialog>

#include "bvtabbar.h"
#include "game.h"
#include "BView.h"
#include "Bc.h"

#define  C_MIN_UPL    1
#define  C_MAX_UPL    3
#define  C_NB_ONG     3
#define  C_NB_SUB_ONG 3
#define  C_NB_SUB_ONG_CAL C_NB_SUB_ONG - 1
#define  C_NB_TIR_LIR 2

#define  C_TBL_UPL "Upl_lst"

class BcUpl: public BCount
{
  Q_OBJECT

 public:
  /*
  typedef struct _stIn{
    int uplet;   /// Valeur du n-uplet
    QString cnx; /// Nom de la connexion
    int dst;     /// distance vis a vis uplet
  }st_In;
*/
  typedef enum _eEnsemble /// Ensemble dans lequel chercher les uplets
  {
   eEnsNotSet, /// Ensemble non defini
   eEnsFdj,    /// Liste des tirages de la fdj
   eEnsUsr,    /// Selection de l'utilisateur
   eEnsEnd     /// Fin enumeration
  }eUpl_Ens;
  static const QString Txt_eUpl_Ens[eEnsEnd];

  typedef enum _eCalcul
  {
   eCalNotSet,
   eCalTot, /// Calcul sur boule
   eCalCmb, /// Calcul sur Combinaison
   eCalBrc  /// Calcul sur barycentre
  }eUpl_Cal;

  typedef struct _param_tsk
  {
    const stGameConf *ptr_gmCf; /// ptr config du jeu
    eUpl_Ens eEns_id; /// type d'ensemble
    int z_id;  /// Zone id
    int l_id;  /// Ligne id (dans base ou user)
    int g_id;  /// Groupe id (Cnp)
    int g_lm;  /// Groupe element (indice element dans Groupe id)
    QString qstr_sql;
  }stParam_tsk;


 private:
  typedef enum _eCalTirages
  {
   ELstBle,  /// Liste des boules
   ELstUpl,	/// Liste des uplets apartir des boules
   ELstTirUpl,	/// Liste des tirages ayant ces uplets (J0)
   ELstUplTot,	/// Total de chacun des uplets
   ELstBleNot,	/// Ensemble complementaire des boules ( refa uplet 1, 2, 3)
   ELstTirUplNext, /// Liste des tirages apres les uplets initiaux
   ELstBleNext, /// Liste des boules jour  != 0
   ELstUplNot,	/// Uplet 1,2,3 cree apartir de l'ensemble complementaire
   ELstUplTotNot, /// Total de chacun des uplets
   ELstUplNext, /// Liste des uplets apartir des boules next day
   ELstUplTotNext, /// Total de chacun des uplets
   ELstCal,        /// Fin des calculs possible
   ELstShowCal,    /// Requete montrant les calculs
   ELstShowUnion,   /// Requete synthese de chacque boule
   ELstShowNotInUnion /// Requete ensemble complementaire
   ///ELstCalUsr,
   ///ELstCalFdj,
  }eUpl_Lst;

#ifndef QT_NO_DEBUG
  static const QString sqlStepText[ELstCal];
#endif

#define C_TOT_CAL ELstCal



 public:
  //BUplet(st_In const &param);
  //BUplet(st_In const &param, int index=0);
  //BUplet(st_In const &param, QString ensemble="");
  //BcUpl(st_In const &param, int index=0, eUpl_Cal eCal=eCalNotSet,const QModelIndex &ligne=QModelIndex(), const QString & data="", QWidget *parent=nullptr);
  explicit BcUpl(const stGameConf *pGame, eUpl_Ens eUpl=eEnsFdj, int zn=0, const QItemSelectionModel *cur_sel=nullptr, QTabWidget *ptrUplRsp=nullptr);
  ~BcUpl();
  //int getUpl(void);
  //QString sql_UsrSelectedTirages(const QModelIndex & index, int pos);
  QString findUplets(const stGameConf *pGame, const int zn =0, const int loop=2, const int key=-1, QString tb_def="tb6", const int ref_day=1, const int delta=-1);

 public slots:
  //void slot_Selection(const QString& lstBoules);
  //void slot_FindNewUplet(const QModelIndex & index);
  void BSlot_MkUsrUpletsShow(const QItemSelectionModel *cur_sel, const int zn);

 private slots:
  void BSlot_ShowTotal(const QString& lstBoules);
  void BSlot_clicked(const QModelIndex &index);
  //void BSlot_Tab(int);

 private:
  QSqlDatabase db_0;
  QModelIndexList my_indexes;
  QTabWidget *uplTirTab;
  int upl_zn;
  eUpl_Ens useData;

  //QString gpb_title;
  //st_In input;
  //QGroupBox *gpb_upl;
  //QTableView *qtv_upl;
  //QString ens_ref;
  //static int tot_upl;


 private:
  QString getTablePrefixFromSelection(QString items, int zn=0, bool *wasPresent=nullptr);
  QHBoxLayout *getBar_Rch(BView *qtv_tmp,int tab_id);

  // QGroupBox *gpbCreate(int index, eUpl_Cal eCal, const QModelIndex & ligne, const QString &data, QWidget *parent);
  //QTableView *doTabShowUplet(QString tbl_src, const QModelIndex &ligne);
  //int  getNbLines(QString tbl_src);
  //QString getUpletFromIndex(int nb_uplet, int index, QString tbl_src);
  //QString getBoulesTirage(int index);

  //QString getJourTirage(int index);
  //QString getCmbTirage(int index);
  //QString getBrcTirage(int index);

  //QTabWidget * getTabUplRsp(void);
  //bool DoCreateTblUplet(QString tbl);
  //QString FN2_getFieldsFromZone(int zn, QString alias);

  //bool do_SqlCnpCount(int uplet_id);
  //QString sql_CnpMkUplet(int nb, QString col, QString tbl_in="B_elm");
  //QString sql_CnpCountUplet(int nb, QString tbl_cnp, QString tbl_in="B_fdj");
  //QString sql_UsrCountUplet(int nb, QString tbl_cnp, QString tbl_in="B_fdj");
  QString sql_ShowItems(const stGameConf *pGame, int zn, eUpl_Lst sql_show, int cur_upl, QString cur_sql, int upl_sub=-1);

 private:
  virtual  QTabWidget *startCount(const stGameConf *pGame, const etCount eUpl_Cal);
  virtual bool usr_MkTbl(const stGameConf *pDef, const stMkLocal prm, const int zn);
  virtual void usr_TagLast(const stGameConf *pGame, BView_1 *view, const etCount eType, const int zn);
  //virtual QLayout * usr_UpperItems(int zn, BTbView *cur_tbv);
  QGridLayout *Compter(QString * pName, int zn);

 private:
  QString getSqlTbv(const stGameConf *pGame, int zn, int tir_Id, int day_Delta, int upl_Grp, int upl_Sub=-1, eUpl_Lst target=ELstCal, int sel_item=-1);
  QWidget *showUplFromRef(const stGameConf *pGame, int zn, int tirLgnId, int upl_ref);
  QWidget *getUplDetails(const stGameConf *pGame, int zn, int tirLgnId, int src_upl, int relativeDay, int nb_recherche);
  void sql_upl_lev_1(const stGameConf *pGame, int zn, int tirLgnId, int upl_ref_in, int offset, int upl_sub, int step, QString tabInOut[][3]);
  void sql_upl_lev_2(const stGameConf *pGame, int zn, int tirLgnId, int offset, int upl_ref_in, int upl_sub, QString tabInOut[][C_TOT_CAL][3]);

  void sql_FillTabArgs(const stGameConf *pGame, int zn,int upl_ref_in, QString *tab_arg, QString tabInOut[][3]);

  QString sql_ElmFrmTir(const stGameConf *pGame, int zn, eUpl_Lst sql_step, int tir_id, QString tabInOut[][3]);
  QString sql_ElmNotFrmTir(const stGameConf *pGame, int zn, int upl_ref_in, QString tabInOut[][3]);

  QString sql_UplFrmElm(const stGameConf *pGame, int zn, int upl_ref_in, int upl_sub, eUpl_Lst sql_step, QString tabInOut[][3]);
  QString sql_TirFrmUpl(const stGameConf *pGame, int zn, int upl_ref_in, QString tabInOut[][3]);
  QString sql_TotFrmTir(const stGameConf *pGame, int zn, int upl_ref_in, int upl_sub, eUpl_Lst sql_step, QString tabInOut[][3]);

  QString sql_NxtTirUpl(const stGameConf *pGame, int zn,int offset, QString tabInOut[][3]);

  void sql_RepartitionVoisin(QString tabInOut[][3], int jour, int upl, int ref_id, int delta=0);

  //void tryUneFn();
  QWidget *Bview_init(const stGameConf *pGame, int zn, int tirLgnId, int offset, int src_upl, int relativeDay, int dst_upl);
  BView * Bview_3_fill_1(BView *qtv_tmp, QString sql_msg);
  BView * Bview_4_fill_1(BView *qtv_tmp, QString sql_msg);
  QWidget *fill_Bview_1(const stGameConf *pGame, int zn, int tirLgnId, int i);
  int Bview_UpdateAndCount(eUpl_Lst id, BView *qtv_tmp, QString sql_msg);
  int Bview_3_fill_2(BView *qtv_tmp, QString sql_msg);
  int Bview_4_fill_2(BView *qtv_tmp, QString sql_msg);
  bool effectueRecherche(eUpl_Ens upl_type, QString upl_sql, int upl_id,int zn_id, int nb_items);
  bool tsk_upl_1 (const stGameConf *pGame, const stParam_tsk *param);
  void rechercheUplet(QString tbl_prefix, const stGameConf *pGame, const stParam_tsk *param, int fake_sel);
  void tsk_upl_2(QString cnx, QString tbl, QString sql);
  void tsk_upl_0(stParam_tsk *tsk_param);
  void FillBdd(QString tbl, stParam_tsk *tsk_param);
  void FillTbv(QString tbl, stParam_tsk *tsk_param);
  int getFromView_Lid(const BView *view);
  QString getFromIndex_CurUpl(const QModelIndex &index, int upl_GrpId);
  void BSlot_clicked_old(const QModelIndex &index);


};


#endif // BUPLET_H

#if 0
class BUplWidget: public QWidget
{

 public:
  //BUplWidget(QString cnx, QWidget *parent=0);
  //BUplWidget(QString cnx, int index, QWidget *parent=0);
  //BUplWidget(QString cnx, QString usr_ens, QWidget *parent=0);
  BUplWidget(QString cnx, int index=0, const QModelIndex & ligne=QModelIndex(), const QString & data="", BcUpl *origine=0, QWidget *parent=0);

 private:
  QString sql_lstTirCmb(int ligne, int dst);
  QString sql_lstTirBrc(int ligne, int dst);

};

#endif
