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
#define CNP_N_MAX   7
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


extern QString OrganiseChampsDesTirages(QString st_base_reference, stTiragesDef *pMaConf);
extern QString req_msg(QString base, int zone, int boule, stTiragesDef *ref);
extern QStringList * LstCritereGroupement(int zn, stTiragesDef *pConf);
extern QString sql_RegroupeSelonCritere(QString st_tirages, QString st_cri);
extern QString sql_ComptePourUnTirage(int id,QString st_tirages, QString st_cri);
extern QString CompteJourTirage(stTiragesDef *pMaConf);
extern bool VerifierValeur(int item, QString table,int idColValue,int *lev);


#endif // TIRAGES_H
