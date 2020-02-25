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

#include "cnp_SansRepetition.h"

/// informer de la prochaine definition de la classe
class BPrevision;

/// -------DEFINE---------
#define CTXT_LABEL  "selection Z:aucun - C:aucun - G:aucun"

/// -------ENUM---------
/// Type de jeu possible d'etudier


typedef enum _eBddUsage{
 eDbForFdj=0,/// Base Fdj
 eDbForCnp,  /// Base Cnp
 eDbForEol   /// Fin de liste
}etDbUsage;

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
  etDbPlace bddStore;

	QString tblFdj_ana;	/// analyse des tirages provenant de fdj
	QString tblFdj_brc;		 /// Table de la base ayant les barycentres calcules depuis fdj
	QString tblFdj_dta;	/// liste des tirages provenant de fdj
	QString tblUsr_ana;	/// analyse des tirages provenant de usr
	QString tblUsr_dta;	/// liste des tirages provenant de usr

  stGameConf gameInfo;
 }stPrmPrevision;

#if 0
    typedef struct _stPrmOnGame{
     QString usr_source; /// nom de la table ayant la liste des tirages utilisateur
     QString usr_analys; /// table regroupant l'analyse des repartitions
     QString fdj_dta;		 /// Nom de la table ayant les tirages de la fdj
     QString fdj_brc;		 /// Table de la base ayant les barycentres calcules depuis fdj
     BGame def;
    }stPrmOnGame;
#endif

 /// in : infos representant les tirages
 public:
 BPrevision(stPrmPrevision prm);
 ~BPrevision();

 private:
 bool AnalyserEnsembleTirage(QString InputTable,const stGameConf &onGame, int zn);
 bool chargerDonneesFdjeux(QString tbName);
 bool creerTablesDeLaBase(void);
 bool f1(QString tbName,QSqlQuery *query);
 bool f1_TbFiltre(QString tbName,QSqlQuery *query);
 bool f2(QString tbName,QSqlQuery *query);
 bool f3(QString tbName,QSqlQuery *query);
 bool f4(QString tbName,QSqlQuery *query);
 bool f5(QString tbName,QSqlQuery *query);
 bool f6(QString tbName,QSqlQuery *query);
 bool FaireTableauSynthese(QString InputTable,const stGameConf &onGame, int zn);
 bool isPreviousDestroyed(void);
 bool isTableCnpinDb(int n, int p);
 bool LireLesTirages(QString tblName, stFdjData *def);
 bool OPtimiseAccesBase(void);
 bool ouvrirBase(etDbPlace cible, etFdjType game);
 bool SupprimerVueIntermediaires(void);
 bool TraitementCodeTblCombi(QString tbName,int zn);
 bool TraitementCodeTblCombi_2(QString tbName, QString tbCnp, int zn);
 bool TraitementCodeVueCombi(int zn);
 QString DateAnormer(QString input);
 QString JourFromDate(QString LaDate, QString verif, stErr2 *retErr);
 QString ListeDesJeux(int zn, int n, int p);
 QString mk_IdCnx(etFdjType type, etDbUsage eDbUsage);
 QString mk_IdDsk(etFdjType type, etDbUsage eDbUsage);
 QStringList * CreateFilterForData(int zn);
 stGameConf *definirConstantesDuJeu(etFdjType game);

 void analyserTirages(stPrmPrevision calcul, QString source, const stGameConf &config);
 bool do_SqlCnpPrepare(void);
 bool do_SqlCnpCount(void);

 QString sql_CnpMkUplet(int nb, QString col, QString ensemble="B_elm");
 QString sql_CnpCountUplet(int nb, QString tbl_cnp, QString ensemble="B_fdj");
 QString sql_CnpCountFromId(int tir_id, int uplet);

 QString CreateSqlFrom(QString tbl, int val_p);
 QString FN2_getFieldsFromZone(int zn, QString alias="");

 void ContinuerCreation(QString tbl_cible, QString tbl_cible_ana);
 void creerJeuxUtilisateur(int n, int p);
 void effectuerTraitement(etFdjType game);

 Q_SIGNALS:
 void sig_isClickedOnBall(const QModelIndex &index);

 private slots:
 void slot_emitThatClickedBall(const QModelIndex &index);
 void slot_CnpEnd(const BCnp::Status eStatus, const int val_n, const int val_p);

 public slots:
 void slot_changerTitreZone(QString le_titre);
 void slot_UGL_Create();
 void slot_UGL_SetFilters();
 void slot_UGL_ClrFilters();

 private:
 BCountComb *c2;
 BCountElem *c1;
 BCountGroup *c3;
 CBaryCentre *c;
 int cur_item;
 QLabel *lignes;
 QSqlDatabase db_1;      /// base de donnees associee a cet objets
 QSqlQueryModel *sqm_resu;
 QString cnx_db_1;       /// nom de la connection
 QString sql[3];
 QString tblTirages;
 QString titre[3];
 QStringList **slFlt;    /// zn_filters
 static int total_items;
 stGameConf onGame;           /// parametres du jeu pour statistique globale
 stPrmPrevision conf;
 stPrmPrevision monJeu;     /// parametres pour filtration
};


#endif // LESCOMPTAGES_H
