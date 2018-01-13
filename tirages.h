#ifndef TIRAGES_H
#define TIRAGES_H

#include <QObject>
#include <QString>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QTableView>
#include <QLabel>
#include <QStandardItemModel>
#include <QItemDelegate>
#include <QPainter>
#include <QTabWidget>
#include <QMdiArea>

#include "cnp.h"
#include "labelclickable.h"

#define CHauteur1   225
#define CLargeur1   200
#define XLenTir     520
#define YLenTir     250
#define LCELL       30
#define CHauteur2   200

#define C_EUR_NB_ZN 2       /// Constante jeu euro nb de zone 2
#define C_LTO_NB_ZN 2
#define CL_PAIR     "p"     /// Constante Label pour parite
#define CL_SGRP     "g"     /// Constante Label pour sous groupe dans zone

#define CL_TCOUV    "cz"    /// Nom table couverture de zone
#define CL_TOARR    "oaz"   /// Nom table ordre arrivee zone
#define CL_CCOUV    "c"     /// Nom colonne couverture
#define TB_COMBI    "comb"
#define TB_BASE     "tirages"
#define REF_BASE    "RefTirages"
#define TB_RZ       "Ref_znName"    /// Table Reference des noms des Zones
#define TB_RZVA     "Ref_znLimits"  /// limites sur Zones
#define TB_RZBN     "Bnrz"          /// Table Boules Names des Reference Zone
#define TB_SE       "SelElemt"  /// Choix de boules dans zone
#define TB_SC       "SelComb"  /// Choix de combinaison dans zone
#define TB_SG       "SelGrp"  /// Choix de criteres groupement dans zone

#define CL_IHM_TOT_0    8
#define CL_IHM_TOT_2    7
#define CL_IHM_TOT_1    6

#define BMAX(a,b) (((a)>(b))?(a):(b))
#define BMIN(a,b) (((a)<(b))?(a):(b))
#define CNP_N_MAX   20
#define CNP_P_MAX   5

namespace NE_FDJ{
typedef enum _les_jeux_a_tirages
{
    fdj_none,   /// aucun type defini
    fdj_loto,   /// jeu : loto
    fdj_sper,  /// superloto
    fdj_slot,   /// Loto + super loto
    fdj_euro,   /// jeu : euromillion
    fdj_fini    /// fin de la liste des jeux possibles
}E_typeJeux;

typedef enum _critere_recherche
{
    critere_boule,
    critere_parite,
    critere_enemble
}E_typeCritere;
}

typedef struct _myhead
{
    int depart;
    int total;
}stMyHeadedList;

typedef struct _ordreArrivee
{
    int x;  /// ordre arrivee
    int y;  /// nb de fois
    int n;  /// suivant
    int p;  /// precedent
}stMyLinkedList;

typedef struct _val_max_min
{
    int len;
    int min;
    int max;
}stBornes;

typedef struct _gererBaseParam
{
    bool destination;
    bool typeChargement;
    NE_FDJ::E_typeJeux typeJeu;

}stParam;

typedef struct _MyErrors
{
    bool status;
    QString msg;
}stErr;

typedef struct _tirages_def
{
    class tirages *pTir;
    NE_FDJ::E_typeJeux choixJeu;
    int *nbElmZone;
    int *offsetFichier;
    QString *jour_tir;
    QString *nomZone;
    QString *FullNameZone;
    stBornes *limites;
    unsigned char nb_tir_semaine;
    unsigned char nb_zone;
    QStringList sl_Lev0;
    QStringList sl_Lev1[5];
}stTiragesDef;

typedef struct _un_tirage
{
    QString date;
    int **valBoules;
}stUnTirage;


class tirages
{
protected:
    static stTiragesDef conf;
    static stMyLinkedList *arrive;
    static int *total;

public:
    static QString *lib_col;
    stUnTirage value;

public:
    tirages(NE_FDJ::E_typeJeux jeu = NE_FDJ::fdj_none);
    void getConfigFor(stTiragesDef *priv_conf);
    QString SelectSource(bool load);
    QString s_LibColBase(stTiragesDef *ref);
    QString s_LibColAnalyse(stTiragesDef *pRef);
    QString qs_zColBaseName(int zone);
    int RechercheNbBoulesPairs(int zone); // Nombre de nombre pair dans la zone
    int RechercheNbBoulesDansGrp1(int zone); // Nombre de nombre de la zone appartenant a E1;
    //int RechercheNbBoulesLimite(int zone, int min, int max);
    void ListeCombinaison(stTiragesDef *ref);
    void ConstruireListeCnp(int n, int p, QStringList &out);
    int Cnp_v2(int n, int p);


};

typedef struct _znfDef
{
    int start;  /// offset de debut zone dans fichier
    int len;    /// taille dans la zone
    int min;    /// valeur mini possible
    int max;    /// valeur maxi possible
}stFzn;

typedef struct _keyForFile
{
    bool wget;  /// A telecharger ?
    int ofdate; /// Offset dans fichier pour avoir la date
    int ofday;  /// Offset dans fichier pour avoir le jour
    int nbZone; /// Nb zone a lire
    stFzn *pZn; /// Pointeur vers caracteristique de chacune des zones
}stKey;

typedef struct _tiragesFileFormat
{
    QString fname;              /// file name
    NE_FDJ::E_typeJeux type;    /// type de jeux
    stKey param;
}tiragesFileFormat;

class DelegationDeCouleur : public QItemDelegate
{
    Q_OBJECT
public:
    DelegationDeCouleur(QWidget *parent = 0) : QItemDelegate(parent) {}
    //DelegationDeCouleur(const QModelIndex *index=0) : QItemDelegate(index) {}

public:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        drawBackground(painter, option, index);
        QItemDelegate::paint(painter, option, index);
    }

protected:
    virtual void drawBackground(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        Q_UNUSED(index);
        painter->fillRect(option.rect, QColor(qrand()%255, qrand()%255, qrand()%255));
    }
};

class GererBase : public QObject,tirages
{
    Q_OBJECT
public:
    //GererBase(QObject *parent = 0);
    GererBase(stParam *param, stErr *retErr, stTiragesDef *pConf);
    ~GererBase();


public:
    QVariant data(const QModelIndex &index, int role) const;
    bool CreerBasePourEtude(bool action, NE_FDJ::E_typeJeux type);
    bool OPtimiseAccesBase(void);
    bool CreerTableCnp();

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
    void slot_UseCnpLine(const sigData &d, const QString &p);

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

    bool CreationTablesDeLaBDD_v2();
    bool f1();
    bool f1_1();
    bool f1_2();
    bool f2();
    bool f2_2();
    bool f3();
    bool f4();
    bool SauverCombiVersTable (QStringList &combi);
    bool MettrePonderationCombi(int delta);
    bool LireFichiersDesTirages(bool autoLoad,stErr *retErr);
    bool AffectePoidsATirage_v2();
    bool ReorganiserLesTirages();
    bool GrouperCombi(int zn);

private:
    QSqlDatabase db;
    stTiragesDef conf;
    int curZone;
    tirages *typeTirages;
    QSqlTableModel *tbl_model;
    QSqlTableModel *tbl_couverture;
    int iAffichageVoisinEnCoursDeLaBoule[2];
};


extern QString OrganiseChampsDesTirages(QString st_base_reference, stTiragesDef *pMaConf);
extern QString req_msg(QString base, int zone, int boule, stTiragesDef *ref);
extern QStringList * LstCritereGroupement(int zn, stTiragesDef *pConf);
extern QString sql_RegroupeSelonCritere(QString st_tirages, QString st_cri);
extern QString sql_ComptePourUnTirage(int id,QString st_tirages, QString st_cri);
extern QString CompteJourTirage(stTiragesDef *pMaConf);
extern bool VerifierValeur(int item, QString table,int idColValue,int *lev);


#endif // TIRAGES_H
