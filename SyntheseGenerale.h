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

  private:
    QString A1_0_TrouverLignes(int zn, QString tb_src, QString tb_ref, QString key);
    QString A1_1_CalculerEcart(QString str_reponses);
    QString A1_2_RegrouperEcart(QString str_reponses);
    bool Contruire_Tbl_tot(int zn, QString tbl_dst, QString ref, QString key, QString data);

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
    void DoBloc3(void);
    QString TrouverTirages(int col, QString str_nb, QString st_tirages, QString st_cri, int zn, stTiragesDef *pConf);
    QString ActionElmZone(QString critere , QString operateur, int zone, stTiragesDef *pConf);
    QString SqlCreateCodeBoule(int onglet, QString table);
    QString SqlCreateCodeEtoile(int onglet, QString table);
    QString SqlCreateCodeCombi(int onglet, QString table);
    QString SqlCreateCodeGroupe(int onglet, QString table);
    QString SqlCreateCodeBary(int onglet, QString table);

    QGridLayout * MonLayout_R3_grp_z1(int fake);
    QGridLayout * MonLayout_R1_tot_zn(int dst);
    QGridLayout * MonLayout_R1_tot_z2(int dst);
    QGridLayout * MonLayout_R2_cmb_z1(int dst);
    QGridLayout * MonLayout_R4_brc_z1(int dst);

    QTableView * TbvAnalyse_tot(int zn, QString source, QString definition, QString key);
    QTableView * TbvResume_tot(int zn, QString tb_read);
    void mettreEnConformiteVisuel(QTableView *qtv_tmp, QString tb_total);

    QGridLayout* Vbox_Analyse(int zn, QString tb_src, QString tb_ref, QString key);
    QGridLayout* Vbox_Resume(int zn, QString tb_src, QString tb_ref, QString key);

    QTableView * TbvAnalyse_brc(int zn, QString source, QString definition, QString key);
    QTableView * TbvResume_brc(int zn, QString tb_in);
    void FaireResume(QTableView * qtv_tmp, QString tb_source, QString tb_write, QString st_requete, QString tb_total);

    //bool isTableTotalBoulleReady(QString tbl_total);
    bool mettreBarycentre(QString tbl_dst, QString src_data);
    bool Contruire_Tbl_brc(int zn, QString tbl_poids_boules, QString tb_ref, QString key, QString tbl_out);
    bool A4_0_CalculerBarycentre(QString tbl_dest, QString tbl_poids_boules);
    bool RajouterCalculBarycentreDansAnalyses(int zn,
                                              QString tb_src ,
                                              QString tb_ana,
                                              QString tb_ref ,
                                              QString ky_brc);



    QTableView *tbForPrincipeAuguste(int nbcol, int nblgn);
    QGridLayout *MonLayout_TabAuguste(int col, int lgn);

    QString SD_Tb1(QStringList boules, QString sqlTblRef, int dst);
    void CompleteMenu(QMenu *LeMenu,QString tbl, int clef);

    //QString SD_Tb2(QStringList boules, QString sqlTblRef, int dst);

};
#endif // REFRESULTAT_H
