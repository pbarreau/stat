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

#include "filtrecombinaisons.h"
#include "SyntheseDetails.h"
#include "refetude.h"

#include "tirages.h"


namespace NE_Analyses{
typedef enum _les_tableaux
{
    bToutes,   /// toutes les boubles
    bFini    /// fin de la liste
}E_Syntese;
}

#define CTXT_SELECTION  "selection b:aucun - e:aucun - c:aucun - g:aucun"

class SyntheseGenerale : public QObject
{
    Q_OBJECT

private:
    //stCurDemande *pLaDemande;
    GererBase *bdd;
    stTiragesDef *pMaConf;
    QMdiArea *pEcran;
    QTabWidget *ptabTop;
    QTabWidget *ptabComptage;
    RefEtude *tabEcarts;
    int curzn;
    stCurDemande uneDemande;
    QStringList lst_selection[3];
    QString *st_bdTirages;
    QString *st_JourTirageDef;

    QGridLayout *disposition;
    //QStandardItemModel *sim_bloc1;

    QTableView * tbv_LesTirages;
    QSqlQueryModel *sqm_LesTirages;

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
    QSqlTableModel * sqtblm_bloc2;

    QTableView * tbv_bloc3;
    QSqlTableModel * sqtblm_bloc3;

    QStandardItemModel *gsim_AnalyseUnTirage;


public:
    SyntheseGenerale(GererBase *pLaBase, QTabWidget *ptabSynt, int zn, stTiragesDef *pConf, QMdiArea *visuel);
    QGridLayout *GetDisposition(void);
    QTableView *GetListeTirages(void);
    RefEtude *GetTabEcarts(void);
    void GetInfoTableau(int onglet, QTableView **pTbl, QSqlQueryModel **pSqm, QSortFilterProxyModel **pSfpm);


    // penser au destructeur pour chaque pointeur

public slots:
    void slot_ClicDeSelectionTableau(const QModelIndex & index);
    void slot_MontreLesTirages(const QModelIndex & index);
    void slot_ShowTotalBoule(const QModelIndex & index);
    void slot_ShowBoule(const QModelIndex & index);
    //void slot_Select_G(const QModelIndex & index);
    //void slot_Select_C(const QModelIndex & index);
    //void slot_Select_E(const QModelIndex & index);
    //void slot_Select_B(const QModelIndex & index);

    void slot_ChangementEnCours(const QItemSelection &selected,const QItemSelection &deselected);
    void slot_RazSelection(QString);

private:
    //QString OrganiseChampsDesTirages(QString st_base_reference);
    //QString CompteJourTirage();
    void DoTirages(void);
    void DoComptageTotal(void);
    void DoBloc3(void);

    QGridLayout * MonLayout_SyntheseTotalGroupement(int fake);
    QGridLayout * MonLayout_SyntheseTotalBoules(int dst);
    QGridLayout * MonLayout_SyntheseTotalEtoiles(int dst);
    QGridLayout * MonLayout_SyntheseTotalRepartitions(int dst);


    QString SD_Tb1(QStringList boules, QString sqlTblRef, int dst);

    //QString SD_Tb2(QStringList boules, QString sqlTblRef, int dst);

};
#endif // REFRESULTAT_H
