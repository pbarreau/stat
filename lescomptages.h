#ifndef LESCOMPTAGES_H
#define LESCOMPTAGES_H

#include <QString>
#include <QGridLayout>
#include <QSqlDatabase>

#include "compter.h"
#include "db_tools.h"
#include "labelclickable.h"

/// informer de la prochaine definition de la classe
class cLesComptages;

/// -------DEFINE---------
#define CTXT_LABEL  "selection Z:aucun - C:aucun - G:aucun"

/// -------ENUM---------
/// Type de jeu possible d'etudier
typedef enum _eGame{
    eGameLoto, /// Loto
    eGameEuro  /// Euro million
}eGame;

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
    bool (cLesComptages::*pFuncInit)(QString tbName,QSqlQuery *query); /// fonction traitant la creation
}stCreateTable;

/// Renseignement sur les bornes de la zone a etudier
typedef struct _stParam_1{
    int min; /// plus petite valeur possible
    int max; /// plus gande valeur possible
    int len; /// nb d'elements pouvant etre choisi entre min et max
    int win; /// nb d'element dans len assurant le jackpot
}stParam_1;

/// Renseignement sur le nom de la zone a etudier
typedef struct _stParam_2
{
    QString abv;    /// abbreviation du nom de la zone
    QString std;    /// nom standard
}stParam_2;

/// Definition d'un jeu
typedef struct _stGameConf{
    int nbDef; /// nombre de zones a regarder
    stParam_1 *limites; /// bornes sur la zone
    stParam_2 *nom; /// nom de la zone
}stGameConf;


/// -------CLASS---------
class cLesComptages:public QGridLayout
{
    Q_OBJECT

    /// in : infos representant les tirages
public:
    cLesComptages(eGame game, eBddUse def);
    cLesComptages(eGame game, eBddUse def, QString stLesTirages);
    ~cLesComptages();

private:
    QString ListeDesJeux(int zn);
    bool ouvrirBase(eBddUse cible, eGame game);
    bool OPtimiseAccesBase(void);
    void effectuerTraitement(eGame game);

    bool creerTablesDeLaBase(void);
    void definirConstantesDuJeu(eGame game);
    bool f1(QString tbName,QSqlQuery *query);
    bool f2(QString tbName,QSqlQuery *query);
    bool f3(QString tbName,QSqlQuery *query);
    bool f4(QString tbName,QSqlQuery *query);
    bool TraitementCodeVueCombi(int zn);
    bool TraitementCodeTblCombi(QString tbName,int zn);
    bool TraitementCodeTblCombi_2(QString tbName, QString tbCnp, int zn);


    /// TBD
    bool chargerDonneesFdjeux(QString tbName);
    bool LireLesTirages(QString tblName, stFdjData *def);
    QString DateAnormer(QString input);
    QString JourFromDate(QString LaDate, QString verif, stErr2 *retErr);

    void efffectuerTraitement_2();


public slots:
    void slot_changerTitreZone(QString le_titre);
    void slot_AppliquerFiltres();

private:
    static int total;       /// compteur des objets de cette classe
    QSqlDatabase dbInUse;   /// base de donnees associee a cet objets
    QString dbUseName;      /// nom de la connection
    eGame curGame;          /// type de jeu
    stGameConf gameInfo;    /// parametres du jeu
    LabelClickable selection[3];
    QString titre[3];
    QString sql[3];
};

#endif // LESCOMPTAGES_H
