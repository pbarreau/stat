#ifndef REFRESULTAT_H
#define REFRESULTAT_H

#include <QtGui>

#include <QFormLayout>
#include <QStandardItemModel>
#include <QTableView>
#include <QSqlTableModel>
#include <QStyledItemDelegate>
#include <QTabWidget>
#include <QComboBox>
#include <QAbstractItemModel>
#include <QAction>
#include <QObject>
#include <QModelIndex>

#include "filtrecombinaisons.h"
#include "SyntheseDetails.h"


#include "refetude.h"
#include "properties.h"

namespace NE_Analyses{
 typedef enum _les_tableaux
 {
  bToutes,   /// toutes les boubles
  bFini    /// fin de la liste
 }E_Syntese;
}

#define CTXT_SELECTION  "selection b:aucun - e:aucun - c:aucun - g:aucun - ba:aucun"

class B_ActFrMdlIndex:public QAction //Barreau_ActionFromModelIndex
{
  Q_OBJECT
 public:
  B_ActFrMdlIndex(const QModelIndex &index,const QString &label,QObject * parent =0,...)
   :QAction(label,parent), m_index(index)
  {connect(this, SIGNAL(triggered()), this, SLOT(onTriggered()));}

 protected Q_SLOTS:
  void onTriggered()
  {
   emit sig_SelectionTirage(m_index,0);
  }

 Q_SIGNALS:
  void sig_SelectionTirage(const QModelIndex &my_index, int val);

 private:
  QModelIndex m_index;
};

class SyntheseGenerale : public QObject
{
  Q_OBJECT

 private:
  int mon_brc_tmp;
  struct stSelInfo{
    QTableView * qtv;        /// id de la Qtable
    QModelIndexList* lstSel; /// index selection
  };

  typedef QString (SyntheseGenerale::*pMy_DigOne)(int zn, int lgn, QStringList **pList, int i);

  struct stDigAll{
    pMy_DigOne fnDig;
    int zn;
    int lgn;
    QStringList **pList;
    QString target;
    int i;
  };

  bool sql_DigAll();
  QList < QTabWidget *> **id_tab_tmp;// 2 zones
  QList <QPair<int,stSelInfo*>*> **qlSel;
  QList<QTableView *> *tbInfo;
  QList<QTableView *> *tbv;
  QWidget **parentWidget;

  //stCurDemande *pLaDemande;
  B_ActFrMdlIndex *MonTraitement;
  GererBase *origine;
  QSqlDatabase db_0;
  stTiragesDef *pMaConf;
  QMdiArea *pEcran;
  QTabWidget *ptabTop;
  QTabWidget *ptabComptage;
  RefEtude *tabEcarts;
  //int curzn;
  stCurDemande uneDemande;
  //QStringList lst_selection[3];
  QStringList **maRef; //zn_filter
  QString *st_bdTirages;
  QString *st_JourTirageDef;

  QGridLayout *disposition;
  //QStandardItemModel *sim_bloc1;

  QTableView * tbv_LesTirages;
  //QSqlQueryModel *sqm_LesTirages;

  QTableView * tbv_LesEcarts;

  LabelClickable *selection;
  QTableView * tbv_bloc1_1;
  QSqlQueryModel *sqm_bloc1_1;
  QTableView * tbv_bloc1_2;
  QSqlQueryModel *sqm_bloc1_2;
  QTableView * tbv_bloc1_3;
  QSqlQueryModel *sqm_bloc1_3;

  QSortFilterProxyModel * mysortModel;

  QTableView * tbv_bloc2;
  //QSqlTableModel * sqtblm_bloc2;

  //QTableView * tbv_bloc3;
  //QSqlTableModel * sqtblm_bloc3;

  //QStandardItemModel *gsim_AnalyseUnTirage;

 public:
  SyntheseGenerale(GererBase *pLaBase, QTabWidget *ptabSynt, int zn, stTiragesDef *pConf, QMdiArea *visuel);
  QGridLayout *GetDisposition(void);
  QTableView *GetListeTirages(void);
  RefEtude *GetTabEcarts(void);
  void GetInfoTableau(int onglet, QTableView **pTbl, QSqlQueryModel **pSqm, QSortFilterProxyModel **pSfpm);
#ifdef USE_SG_CODE
  void MemoriserProgression(QString table, stMyHeadedList *h, stMyLinkedList *l, int start, int y, int cid, int tid);
  void MettreCouleur(int start, int cur);
  void PresenterResultat(int cid, int tid);
#endif
  // penser au destructeur pour chaque pointeur

 public slots:
#ifdef USE_SG_CODE
  void slot_MaFonctionDeCalcul(const QModelIndex &my_index, int cid);
#endif
  void slot_ccmr_TbvLesTirages(QPoint pos); /// custom context menu request
  void slot_ClicDeSelectionTableau(const QModelIndex & index);
  void slot_MontreLesTirages(const QModelIndex & index);
  void slot_ShowTotalBoule(const QModelIndex & index);
  void slot_ShowBoule(const QModelIndex & index);
  void slot_AnalyseLigne(const QModelIndex & index);
  //void slot_Select_G(const QModelIndex & index);
  //void slot_Select_C(const QModelIndex & index);
  //void slot_Select_E(const QModelIndex & index);
  //void slot_Select_B(const QModelIndex & index);

  //void slot_ChangementEnCours(const QItemSelection &selected,const QItemSelection &deselected);
  void slot_RazSelection(QString);
  void slot_AideToolTip(const QModelIndex & index);

  void slot_ccmr_tbForBaseEcart(QPoint pos);
  QMenu *ContruireMenu(QString tbl, int val);
  void slot_ChoosePriority(QAction *cmd);
  void slot_wdaFilter(int val);


 private slots:
  void slot_ShowBouleForNewDesign(const QModelIndex & index);
  void slot_saveNewSelection(const QItemSelection &selected, const QItemSelection &deselected);

 private:
  bool do_CmbRef(void);
  bool TraitementCodeVueCombi(int zn);
  bool TraitementCodeTblCombi(QString tbName,int zn);
  bool TraitementCodeTblCombi_2(QString tbName, QString tbCnp, int zn);

  QString sql_DigOne(int zn, int lgn, QStringList **pList, int i);
  bool sql_DigAll(stDigAll prm);

  QString CreatreTitle(stCurDemande *pConf);
  void Surligne(int *path,int val);
  int incValue(int *val);
  bool SimplifieSelection(QTableView *view);
  int * getPathToView(QTableView *view, QList<QTabWidget *> **id_tab, QTableView **sel_view);
  void saveSelection(int zn, int calc, QTableView *view, QTableView *ptrSel);
  void mettreInfoSelection(int calc, QList<QPair<int, stSelInfo *> *> *a, QTableView *view, QTableView *ptrSel);

  QString A1_0_TrouverLignes(int zn, QString tb_src, QString tb_ref, QString key);
  QString A1_1_CalculerEcart(QString str_reponses);
  QString A1_2_RegrouperEcart(QString str_reponses);
  bool tot_Contruire_Tbl(int zn, QString tbl_dst, QString ref, QString key, QString data);

  QString A4_0_TrouverLignes(int zn, QString tb_src, QString tb_ref, QString key);
  QString A4_1_CalculerEcart(QString str_reponses);
  QString A4_2_RegrouperEcart(QString str_reponses);

  bool MarquerDerniers(int zn, QString tb_src, QString tb_ref, QString key, QString tbl_dst);
  bool MarquerDerniers_brc(int zn, QString tb_src, QString tb_ref, QString key, QString tbl_dst);


  bool Contruire_Executer(QString tbl_dst, QString st_requete);

  //QString OrganiseChampsDesTirages(QString st_base_reference);
  //QString CompteJourTirage();
  void DoTirages(void);
  void DoComptageTotal(void);
  void do_LinesCheck(void);
  void do_SetKcmb(void);
  void DoBloc3(void);

  QString createSelection(void);
  QString ChercherSelection(int zn, QModelIndexList sel_wko, QTableView * tbv_wko, QString tb_src);
  QString TrouverTirages(int col, QString str_nb, QString st_tirages, QString st_cri, int zn, stTiragesDef *pConf);

  QString ActionElmZone(QString critere , QString operateur, int zone, stTiragesDef *pConf);
  QString tot_SqlCreateZn(int zn, QList<QPair<int, stSelInfo *> *> *a);
  QString cmb_SqlCreateZ1(int onglet, QString table);
  QString grp_SqlCreateZ1(int onglet, QString table);
  QString brc_SqlCreateZn(int zn, QList<QPair<int, stSelInfo *> *> *a);
  QString stb_SqlCreate(int onglet, QString table);

  QString SqlCreateCodeEtoile(int onglet, QString table);

#if TRY_CODE_NEW
 private:
  enum typeCalc{tot,brc,cmb,grp,endCalc,nop};
  enum typeTab{detail,synthese,endTab};

  struct ongDef{
    QString *names;
    int max;
  };
  struct keyHelp{
    QString key;
    QString tbl;
  };
  struct param_1{
    QTabWidget *tab_Top;
    QString tb_src;
    keyHelp hlp[2];
    ongDef onglet[3];
    QString **namesNiv;
    QString curName;
    int niv;
    int maxNiv;
    int zn;
    int *l_max;
    int *path;
  };
  struct param_2{
    param_1 prm_1;
    int zn; /// zone
    int dst;/// distance
    QString tb_wrt;///Write
  };


  typedef QGridLayout * (SyntheseGenerale::**tptrFnsCalc[2])(param_2 a);
  typedef QGridLayout * (SyntheseGenerale::*tptrFns[])(param_2 a); /// tableau de pointeur de fonctions
  typedef QGridLayout * (SyntheseGenerale::**pVtptrFns)(param_2 a); /// pointeur vers un tableau de pointeur de fonctions
  typedef QGridLayout * (SyntheseGenerale::*ptrFn)(param_2 a); /// pointeur de fonction

  struct DefFn{
    QString *ong;
    pVtptrFns lst;
    int tot;
  };

  struct CnfFnCalc{
    typeCalc calc; /// type de calcul
    //DefFn *pTabFn; /// pointeur vers le tableau des calculs
    pVtptrFns *pTabFn;
    //int l_max[3];
  };

  typedef struct{
    QString name;
    QStringList *head;
    int size;
  }stDesigConf;
#define TAILLE(a,b)  (sizeof(a)/sizeof(b))

  QWidget * CreerOnglets(param_1 prm, CnfFnCalc **conf);
  void specialDesign(int niv, QGridLayout *grid, QWidget *resu);
  QTableView *doTabLgnSelection(stDesigConf conf);
  QTableView *doTabLgnTirage(stDesigConf conf);

  QWidget *VbInfoDepart (param_1 a,CnfFnCalc *b[2]);
  QWidget *tot_zn (param_1 a,CnfFnCalc *b);
  QGridLayout *VbInfo_nop(param_2);
  QGridLayout *VbResu_nop(param_2);


#else
  struct prmLay{
    int dst;
    int zn;
  };
  QGridLayout * MonLayout_R1_tot_zn(prmLay prm);
  QGridLayout * MonLayout_R1_tot_z2(prmLay prm);
  QGridLayout * MonLayout_R2_cmb_z1(prmLay prm);
  QGridLayout * MonLayout_R3_grp_z1(prmLay prm);
  QGridLayout * MonLayout_R4_brc_z1(prmLay prm);
#endif
  QTableView * tot_TbvAnalyse(int zn, QString source, QString definition, QString key);
  QTableView * tot_TbvResume(int zn, QString tb_read);
  void mettreEnConformiteVisuel(QTableView *qtv_tmp, QString tb_total);

  /*
    struct prmVana{
        QWidget *parent;
        int zn;
        QString tb_src;
        QString tb_ref;
        QString key;
    };
    */
  QGridLayout* tot_VbInfo(param_2 prm);
  QGridLayout* tot_VbResu(param_2 prm);

  QGridLayout* brc_VbInfo(param_2 prm);
  QGridLayout* brc_VbResu(param_2 prm);

  QTableView * brc_TbvAnalyse(int zn, QString source, QString definition, QString key);
  QTableView * brc_TbvResume(int zn, QString tb_in);
  void FaireResume(QTableView * qtv_tmp, QString tb_source, QString tb_write, QString st_requete, QString tb_total);

  //bool isTableTotalBoulleReady(QString tbl_total);
  bool mettreBarycentre(QString tbl_dst, QString src_data);
  bool brc_Contruire_Tbl(int zn, QString tbl_poids_boules, QString tb_ref, QString key, QString tbl_out);
  bool A4_0_CalculerBarycentre(QString tbl_dest, QString tbl_poids_boules);
  bool RajouterCalculBarycentreDansAnalyses(int zn,
                                            QString tb_src ,
                                            QString tb_ana,
                                            QString tb_ref ,
                                            QString ky_brc);

  QString getFieldsFromZone(int zn, QString alias);

  QTableView *tbForPrincipeAuguste(int nbcol, int nblgn);
  QGridLayout *MonLayout_TabAuguste(int col, int lgn);

  QString SD_Tb1(QStringList boules, QString sqlTblRef, int dst);
  void CompleteMenu(QMenu *LeMenu,QString tbl, int clef);

  //QString SD_Tb2(QStringList boules, QString sqlTblRef, int dst);

};

#endif // REFRESULTAT_H
