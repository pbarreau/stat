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
    cLesComptages(QString stLesTirages);
    ~cLesComptages();

private:
    QString ListeDesJeux(int zn);
    bool ouvrirBase(eBddUse cible, eGame game);
    void effectuerTraitement(eGame game);
    bool creerTablesDeLaBase(void);
    void definirConstantesDuJeu(eGame game);
    bool f1(QString tbName,QSqlQuery *query);

    /// TBD
    void efffectuerTraitement_2();


public slots:
    void slot_changerTitreZone(QString le_titre);
    void slot_AppliquerFiltres();

private:
    static int total;
    QSqlDatabase dbInUse;
    QString dbUseName;
    stGameConf gameInfo;
    LabelClickable selection[3];
    QString titre[3];
    QString sql[3];
};

#endif // LESCOMPTAGES_H
