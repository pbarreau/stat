#ifndef LESCOMPTAGES_H
#define LESCOMPTAGES_H

#include <QString>
#include <QGridLayout>
#include <QSqlDatabase>
#include <QLabel>

#include "compter_zones.h"
#include "compter_combinaisons.h"
#include "compter_groupes.h"
#include "cbarycentre.h"

#include "compter.h"
#include "db_tools.h"

/// informer de la prochaine definition de la classe
class BPrevision;

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


/// -------CLASS---------
class BPrevision:public QGridLayout
{
    Q_OBJECT

		public:
		typedef struct _stPrmPrevision{
		 BGame gameInfo;
		 QString tirages_fdj;	/// liste des tirages provenant de fdj
		 QString tirages_usr;	/// liste des tirages provenant de usr
		 eFdjType fdjType;
		 eAnaType anaType;
		 eBddUse def;
		}stPrmPrevision;

		typedef struct _stPrmOnGame{
		 QString usr_source; /// nom de la table ayant la liste des tirages utilisateur
		 QString usr_analys; /// table regroupant l'analyse des repartitions
		 QString fdj_dta;		 /// Nom de la table ayant les tirages de la fdj
		 QString fdj_brc;		 /// Table de la base ayant les barycentres calcules depuis fdj
		 BGame def;
		}stPrmOnGame;

		/// in : infos representant les tirages
public:
    BPrevision(stPrmPrevision prm);
    ~BPrevision();

private:
    QString ListeDesJeux(int zn, int n, int p);
    bool ouvrirBase(eBddUse cible, eFdjType game);
    QString mk_IdDsk(eFdjType type);
    QString mk_IdCnx(eFdjType type);
    bool OPtimiseAccesBase(void);
    void effectuerTraitement(eFdjType game);

    bool creerTablesDeLaBase(void);
    BGame *definirConstantesDuJeu(eFdjType game);
    bool f1(QString tbName,QSqlQuery *query);
    bool f2(QString tbName,QSqlQuery *query);
    bool f3(QString tbName,QSqlQuery *query);
    bool f4(QString tbName,QSqlQuery *query);
    bool f5(QString tbName,QSqlQuery *query);
    bool f6(QString tbName,QSqlQuery *query);
    bool TraitementCodeVueCombi(int zn);
    bool TraitementCodeTblCombi(QString tbName,int zn);
    bool TraitementCodeTblCombi_2(QString tbName, QString tbCnp, int zn);

    bool AnalyserEnsembleTirage(QString InputTable,const BGame &onGame, int zn);
    bool FaireTableauSynthese(QString InputTable,const BGame &onGame, int zn);
    bool SupprimerVueIntermediaires(void);
    QStringList * CreateFilterForData(int zn);

    /// TBD
    bool chargerDonneesFdjeux(QString tbName);
    bool LireLesTirages(QString tblName, stFdjData *def);
    QString DateAnormer(QString input);
    QString JourFromDate(QString LaDate, QString verif, stErr2 *retErr);

    void analyserTirages(QString source, const BGame &config);
    bool isTableCnpinDb(int n, int p);
    void creerJeuxUtilisateur(int n, int p);
    void ContinuerCreation(QString tbl_cible, QString tbl_cible_ana);
    bool isPreviousDestroyed(void);

Q_SIGNALS:
    void sig_isClickedOnBall(const QModelIndex &index);
private slots:
    void slot_emitThatClickedBall(const QModelIndex &index);

public slots:
    void slot_changerTitreZone(QString le_titre);
    void slot_UGL_Create();
    void slot_UGL_SetFilters();
    void slot_UGL_ClrFilters();

private:
    static int total_items;
    int cur_item;

    stPrmPrevision conf;

    /// compteur des objets de cette classe
    QSqlDatabase db_1;      /// base de donnees associee a cet objets
    QString cnx_db_1;       /// nom de la connection
    BGame onGame;           /// parametres du jeu pour statistique globale
    stPrmOnGame monJeu;     /// parametres pour filtration
    QStringList **slFlt;    /// zn_filters
    BCountElem *c1;
    BCountComb *c2;
    BCountGroup *c3;
    CBaryCentre *c;
    QString tblTirages;
    QSqlQueryModel *sqm_resu;
    QLabel *lignes;
    QString titre[3];
    QString sql[3];
};


#endif // LESCOMPTAGES_H
