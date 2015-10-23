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


#include "tirages.h"

namespace NE_Analyses{
typedef enum _les_tableaux
{
    bToutes,   /// toutes les boubles
    bFini    /// fin de la liste
}E_Syntese;
}



class SyntheseGenerale : public QObject
{
    Q_OBJECT

private:
    //stCurDemande *pLaDemande;
    GererBase *bdd;
    stTiragesDef *pMaConf;
    QMdiArea *pEcran;
    int curzn;
    stCurDemande uneDemande;
    QStringList lst_selection[3];
    QString *st_bdTirages;
    QString *st_JourTirageDef;

    QGridLayout *disposition;
    //QStandardItemModel *sim_bloc1;

    QTableView * tbv_LesTirages;
    QSqlQueryModel *sqm_LesTirages;

    QTableView * tbv_bloc1_1;
    QSqlQueryModel *sqm_bloc1_1;
    QTableView * tbv_bloc1_2;
    QSqlQueryModel *sqm_bloc1_2;
    QTableView * tbv_bloc1_3;
    QSqlQueryModel *sqm_bloc1_3;

    QTableView * tbv_bloc2;
    QSqlTableModel * sqtblm_bloc2;

    QTableView * tbv_bloc3;
    QSqlTableModel * sqtblm_bloc3;


public:
    SyntheseGenerale(GererBase *pLaBase, int zn, stTiragesDef *pConf, QMdiArea *visuel);
    QGridLayout *GetDisposition(void);
    QTableView *GetListeTirages(void);

    // penser au destructeur pour chaque pointeur

public slots:
    void slot_MontreLesTirages(const QModelIndex & index);
    void slot_SelectionneBoules(const QModelIndex & index);
    void slot_ChangementEnCours(const QItemSelection &selected,const QItemSelection &deselected);


private:
    //QString OrganiseChampsDesTirages(QString st_base_reference);
    //QString CompteJourTirage();
    void DoTirages(void);
    void DoComptageTotal(void);
    void DoBloc2(void);
    void DoBloc3(void);
    void MemoriserChoixUtilisateur(int zn, QTableView *ptbv, const QModelIndex & index);

    //QGridLayout * MonLayout_SyntheseTotalBoules(int dst);
    QGridLayout * MonLayout_SyntheseTotalEtoiles(int dst);
    QGridLayout * MonLayout_SyntheseTotalRepartitions(int dst);

    QGridLayout * Presente_SyntheseEcarts(void);
    QGridLayout * MonLayout_SyntheseEcarts(QStandardItemModel *G_sim_Ecarts);


    QString SD_Tb1(QStringList boules, QString sqlTblRef, int dst);

    //QString SD_Tb2(QStringList boules, QString sqlTblRef, int dst);

};
#endif // REFRESULTAT_H
