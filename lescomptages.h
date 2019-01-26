#ifndef LESCOMPTAGES_H
#define LESCOMPTAGES_H

#include <QString>
#include <QGridLayout>
#include <QSqlDatabase>
#include <QLabel>

#include "ihm_tirages.h"
#include "cmpt_elem_details.h"
#include "cmpt_comb_details.h"
#include "cmpt_grou_details.h"

#include "compter.h"
#include "db_tools.h"
#include "labelclickable.h"

/// informer de la prochaine definition de la classe
class BPrevision;
//class BCountGroup;

/// -------DEFINE---------
#define CTXT_LABEL  "selection Z:aucun - C:aucun - G:aucun"

/// -------ENUM---------
/// Type de jeu possible d'etudier

/// Localisation de la base de donnees
typedef enum _eBddDest{
    eBddUseRam, /// en memoire
    eBddUseDisk   /// sur disque
}eBddUse;
/// -------STRUCT---------
typedef struct _stErr2
{
    bool status;
    QString msg;
}stErr2;

typedef struct _stZnDef
{
    int start;  /// offset de debut zone dans fichier
    int len;    /// taille dans la zone
    int min;    /// valeur mini possible
    int max;    /// valeur maxi possible
}stZnDef;

typedef struct _stConfFdjData
{
    bool wget;  /// A telecharger ?
    int ofdate; /// Offset dans fichier pour avoir la date
    int ofday;  /// Offset dans fichier pour avoir le jour
    int nbZone; /// Nb zone a lire
    stZnDef *pZn; /// Pointeur vers caracteristique de chacune des zones
}stConfFdjData;

/// Tirage file format
typedef struct _stFdjData
{
    QString fname;  /// fichier en cours de traitement
    int id;
    stConfFdjData param;
}stFdjData;

typedef struct
{
    QString tbDef; /// nom de la table
    bool (BPrevision::*pFuncInit)(QString tbName,QSqlQuery *query); /// fonction traitant la creation
}stCreateTable;

typedef struct
{
    QString src;
    BGame cnf;
    C_GrpDetails *grp;
    C_CmbDetails *cmb;
    QTabWidget *niv;
    int zn;
    int id;
}stUsePrm; /// Parametre a utiliser

/// -------CLASS---------
class BPrevision:public QGridLayout
{
    Q_OBJECT

    /// in : infos representant les tirages
public:
    BPrevision(eGame game, eFrom from, eBddUse def);
    ~BPrevision();

private:
    QString ListeDesJeux(int zn, int n, int p);
    bool ouvrirBase(eBddUse cible, eGame game);
    bool OPtimiseAccesBase(void);
    bool AuthoriseChargementExtension(void);
    void effectuerTraitement(eGame game);
    QStringList **PreparerCriteresAnalyse(void);


    bool creerTablesDeLaBase(void);
    BGame *definirConstantesDuJeu(eGame game);
    bool f1(QString tbName,QSqlQuery *query);
    bool f2(QString tbName,QSqlQuery *query);
    bool importerFdjDansTable(QString tbName,QSqlQuery *query);
    bool f4(QString tbName,QSqlQuery *query);
    bool f5(QString tbName,QSqlQuery *query);
    bool f6(QString tbName,QSqlQuery *query);
    bool TraitementCodeVueCombi(int zn);
    bool TraitementCodeTblCombi(QString tbName,int zn);
    bool TraitementCodeTblCombi_2(QString tbName, QString tbCnp, int zn);

    bool AnalyserEnsembleTirage(QString tblIn,QStringList **pCri,const BGame &onGame,int zn);

    bool analyserDonneesSource(QString source, QString resultat);

    bool FaireTableauSynthese(QString InputTable,const BGame &onGame, int zn);
    bool SupprimerVueIntermediaires(void);

    //QTableView *Visuel_1(QString source,const BGame &config);
    QWidget *partieDroite(QString source,const BGame &config);
    QWidget *ConstruireElementNiv_2(const stUsePrm &data);
    QWidget *ConstruireElementNiv_3(const stUsePrm &data);
    QWidget *FormElm(const stUsePrm &data);
    QWidget *FormCmb(const stUsePrm &data);
    QWidget *FormGrp(const stUsePrm &data);
    QWidget *FormStb(const stUsePrm &data);


    /// TBD
    bool chargerDonneesFdjeux(QString tbName);
    bool LireLesTirages(QString tblName, stFdjData *def);
    QString normaliserDate(QString input);
    QString JourFromDate(QString LaDate, QString verif, stErr2 *retErr);

    void showAll(QString source, const BGame &config);
    bool isTableCnpinDb(int n, int p);
    void creerJeuxUtilisateur(int n, int p);

Q_SIGNALS:
    void sig_isClickedOnBall(const QModelIndex &index);
private slots:
    void slot_emitThatClickedBall(const QModelIndex &index);
    void slot_SurligneEcartEtDetails(const QModelIndex &index);
    void slot_ccmrTirages(QPoint pos, QTableView *view);
    void slot_CalculSurTirage(const QModelIndex & index);

public slots:
    void slot_changerTitreZone(QString le_titre);
    void slot_makeUserGamesList();
    void slot_filterUserGamesList();

private:
    static int total;       /// compteur des objets de cette classe
    QSqlDatabase dbInUse;   /// base de donnees associee a cet objets
    QString dbUseName;      /// nom de la connection
    BGame onGame;           /// parametres du jeu pour statistique globale
    BGame monJeu;           /// parametres pour filtration
    QStringList **slFlt;    /// zn_filters
    IHM_Tirages * Etape_2;
    C_ElmDetails *c1;
    C_CmbDetails *c2;
    C_GrpDetails *c3;
    QString tblTirages;
    QSqlQueryModel *sqm_resu;
    LabelClickable selection[3];
    QLabel *lignes;
    QString titre[3];
    QString sql[3];
    stUsePrm stBdata;
    QList<QTableView *>qtvDetails;
    QList<QTableView *>qtvEcarts;
};


#endif // LESCOMPTAGES_H
