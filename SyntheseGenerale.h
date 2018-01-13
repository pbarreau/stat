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

#define CTXT_SELECTION  "selection b:aucun - e:aucun - c:aucun - g:aucun"

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
    GererBase *bdd;
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




private:
    //QString OrganiseChampsDesTirages(QString st_base_reference);
    //QString CompteJourTirage();
    void DoTirages(void);
    void DoComptageTotal(void);
    void DoBloc3(void);
    QString TrouverTirages(int col, int nb, QString st_tirages, QString st_cri,int zn, stTiragesDef *pConf);
    QString ActionElmZone(QString critere , QString operateur, int zone, stTiragesDef *pConf);
    QString SqlCreateCodeBoule(int onglet, QString table);
    QString SqlCreateCodeEtoile(int onglet, QString table);
    QString SqlCreateCodeCombi(int onglet, QString table);
    QString SqlCreateCodeGroupe(int onglet, QString table);

    QGridLayout * MonLayout_SyntheseTotalGroupement(int fake);
    QGridLayout * MonLayout_SyntheseTotalBoules(int dst);
    QGridLayout * MonLayout_SyntheseTotalEtoiles(int dst);
    QGridLayout * MonLayout_SyntheseTotalRepartitions(int dst);

    QTableView *tbForPrincipeAuguste(int nbcol, int nblgn);
    QGridLayout *MonLayout_TabAuguste(int col, int lgn);

    QString SD_Tb1(QStringList boules, QString sqlTblRef, int dst);
    void CompleteMenu(QMenu *LeMenu,QString tbl, int clef);

    //QString SD_Tb2(QStringList boules, QString sqlTblRef, int dst);

};
#endif // REFRESULTAT_H
