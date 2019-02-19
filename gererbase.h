#ifndef GERERBASE_H
#define GERERBASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QTableView>

#include "tirages.h"
#include "labelclickable.h"
#include "cnp_SansRepetition.h"

class GererBase;
typedef struct
{
    QString tbDef; /// description de la table
    QString *tbData; /// donnees a mettre
    int nb_data; // nb donnees a mettre
    bool (GererBase::*pFuncInit)(QString, QString *); /// fonction traitant les donnees
}stTbToCreate;


class GererBase : public QWidget,tirages
{
    Q_OBJECT
public:
    //GererBase(QObject *parent = 0);
    GererBase(stParam *param, stErr *retErr, stTiragesDef *pConf);
    ~GererBase();


public:
    QString get_IdCnx(void);
    QVariant data(const QModelIndex &index, int role) const;

    bool CreerBasePourEtude(bool action, NE_FDJ::E_typeJeux type);
    QString mk_IdDsk(NE_FDJ::E_typeJeux type);
    QString mk_IdCnx(NE_FDJ::E_typeJeux type);

    bool CreationTablesDeLaBDD(tirages *pRref);
    bool CTB_Table1(QString nomTable, tirages *pRef);
    bool CreerTableDistriCombi(void);
    bool LireLesTirages(tiragesFileFormat *def,int file_id,stErr *retErr);
    bool NEW_AnalyseLesTirages(tirages *pRef);
    bool SupprimerBase();
    QSqlError lastError();
    void AfficherBase(stTiragesDef *pConf, QTableView *cibleview);
    void AfficherResultatCouverture(stTiragesDef *pConf, QTableView *cibleview);
    void DistributionSortieDeBoule(int boule, QStandardItemModel *modele);
    void RechercheVoisin(int boule, int zn, stTiragesDef *pConf, LabelClickable *l_nb, QStandardItemModel *fen);
    void RechercheAbsent(int boule, int zn, stTiragesDef *pConf, QLabel *l_nb, QStandardItemModel *fen);
    int TotalRechercheVoisinADistanceDe(int dist, int b_id, int zn, stTiragesDef *pConf, int voisin);
    bool RechercheAbsentADistanceDe(int dist, int b_id, int zn, stTiragesDef *pConf, int voisin);
    void CouvertureBase(QStandardItemModel *dest, stTiragesDef *pRef);
    void MontrerLaBoule(int boule, QTableView *fen);
    void MLB_DansCouverture(int boule, stTiragesDef *pConf,QTableView *fen);
    //void MontreMesPossibles(const QModelIndex & index, stTiragesDef *pConf, QStandardItemModel *fen);
    void MontreMesPossibles(const QModelIndex & index, stTiragesDef *pConf, QTableView *qfen);
    void MontreMesAbsents(const QModelIndex & index, stTiragesDef *pConf, QTableView *qfen);
    void MLB_MontreLesCommuns(stTiragesDef * pConf,QTableView *qfen);
    bool CreerColonneOrdreArrivee(int id, stTiragesDef *pConf);
    void MLB_DansLaQtTabView(int boule, QTableView *fen);
    void MLB_DansMesPossibles(int boule,QBrush couleur, QTableView *fen);
    int CouleurVersBid(QTableView *fen);
    void MLP_DansLaQtTabView(stTiragesDef *pConf, QString etude, QStandardItemModel *fen); // Montre la parite
    void PopulateCellMenu(int b_id, int v_id, int zone, stTiragesDef *pConf, QMenu *menu, QObject *receiver);
    void EffectuerTrieMesPossibles(int tri_id, int col_id, int b_id, stTiragesDef *pConf, QStandardItemModel * vue);
    void EffectuerTrieMesAbsents(int tri_id, int col_id, int b_id, stTiragesDef *pConf, QStandardItemModel * vue);
    void TotalApparitionBoule(int boule, stTiragesDef *pConf, int zone, QStandardItemModel *modele);
    void CouvMontrerProbable(int i, QStandardItemModel *dest);
    //void RechercheBaseTiragesPariteNbBoule(int nb, stTiragesDef *ref, QTableView *base);
    //void RepartitionUniteDizaine(int nb, stTiragesDef *ref, QTableView *base);
    void MLP_UniteDizaine(stTiragesDef *pConf, QStandardItemModel *fen);
    void RechercheCombinaison(stTiragesDef *ref, QTabWidget *onglets);
    bool TST_Requete(int &totCln, QString &sql_msg, int lgn, QString &col, QStandardItemModel *&qsim_ptr);
    void TST_RechercheVoisin(QStringList &boules, int zn, stTiragesDef *pConf, LabelClickable *l_nb, QStandardItemModel *modele);
    int TST_TotalRechercheVoisinADistanceDe(int zn, stTiragesDef *pConf, int dist, int v_id, QStringList &boules);
    QString TST_ConstruireWhereData(int zn, stTiragesDef *pConf,QStringList &boules);
    QString TST_ZoneRequete(stTiragesDef *pConf, int zone, QString operateur, int boule, QString critere);
    QString TST_GetIntervalCouverture(int etendue);
    void TST_LBcDistBr(int zn,stTiragesDef *pConf,int dist, int br,int bc);
    int TST_TotalRechercheADistance_F2(int dist, QString col, int bp_ref, int bp_look);

public slots:
    void slot_DetailsCombinaison(const QModelIndex & index) ;

private:
    void combirec(int k, QStringList &l, const QString &s, QStringList &ret);
    void RangerValeurResultat(int &totCol, int &lgn, QString &msg, int &val, QStandardItemModel *&qsim_ptr);
    void AfficherMaxOccurenceBoule(int boule, int zn, stTiragesDef *pConf, LabelClickable *l_nb);
    bool CreerTableVoisinsDeBoule(int b_id, int zone, stTiragesDef *pConf, int max_voisins);
    bool CreerTableAbsentDeBoule(int b_id, int zone, stTiragesDef *pConf, int max_voisins);
    void RechercherVoisinDeLaBoule(int b_id, int zone, stTiragesDef *pConf, int max_voisins);
    void RechercherAbsentDeLaBoule(int b_id, int zone, stTiragesDef *pConf, int max_absents);
    void MontrerResultatRechercheVoisins(QStandardItemModel *modele, int zone, stTiragesDef *pConf, int b_id);
    void MontrerResultatRechercheAbsent(QStandardItemModel *modele,int zone, stTiragesDef *pConf,int b_id);
    void MontrerDetailCombinaison(QString msg);
    void CreerTablePonderationAbsentDeBoule(int b_id, int zone, stTiragesDef *pConf);

    bool OPtimiseAccesBase(void);
    bool CreationTablesDeLaBDD_v2();
    bool RajouterTable(stTbToCreate des);
    bool CreerTableCnp(QString tb, QString *data);
    bool CreerTableGnp(QString tb, QString *data);

    bool f1(QString tb, QString *data);
    bool f1_1(QString tb, QString *data);
    bool f1_2(QString tb, QString *data);
    bool f2(QString tb, QString *data);
    bool f2_2(QString tb, QString *data);
    bool f3(QString tb, QString *data);

    bool f4(QString tb, QString *data);
    bool TraitementCodeVueCombi(int zn);
    bool TraitementCodeTblCombi(int zn);

    bool SauverCombiVersTable (QStringList &combi);
    bool MettrePonderationCombi(int delta);
    bool LireFichiersDesTirages(bool autoLoad,stErr *retErr);
    bool AffectePoidsATirage_v2();
    bool ReorganiserLesTirages();
    bool GrouperCombi(int zn);


private:
    static int total_items;
    int cur_item;
    QSqlDatabase db;
    stTiragesDef conf;
    tirages *typeTirages;
    QSqlTableModel *tbl_model;
    QSqlTableModel *tbl_couverture;
    int iAffichageVoisinEnCoursDeLaBoule[2];

#if USE_CNP_SLOT_LINE
private:
    int curZone;
public slots:
    void slot_UseCnpLine(const sigData &d, const QString &p);
#endif

};

#endif // GERERBASE_H
