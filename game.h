#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QWidget>
#include <QSqlDatabase>

#include "properties.h"

/// Localisation de la base de donnees
typedef enum _eBddType{
 eDbRam, /// en memoire
 eDbDsk   /// sur disque
}etDb;

/// Type de jeu possible d'etudier
typedef enum _eFdjType{
 eFdjNone, /// Jeu pas encore configure
 eFdjLoto,   /// Loto
 eFdjEuro,   /// Euro million
 eFdjEol     /// End of list
}etFdj;
extern const QString gameLabel [eFdjEol];

typedef enum _eFdjCname{
 eCnameLoto,
 eCnameSuperLoto,
 eCnameGrandLotoNoel,
 eCnameGrandLoto,
 eFdjEndCnames_1,
 eCnameEuroMillionsMyMillion,
 eCnameEuroMillion,
 eFdjEndCnames_2
}eFCname;
extern const QString FdjCnames_1 [eFdjEndCnames_1];
extern const QString FdjCnames_2 [eFdjEndCnames_2 - eFdjEndCnames_1 -1];

typedef struct _stSrcHistoJeux{
  eFCname type;
  QString memo;
  QString http;
  QString file;
}stSrcHistoJeux;
extern const stSrcHistoJeux HistoLoto[11];

typedef enum _etTir{
 eTirNotSet, /// Lst tirages pas encore etablie
 eTirFdj,    /// Lst tirages de la Fdj
 eTirGen,    /// Lst tirages autogeneree
 eTirUsr     /// Lst tirages choix utilisateur
}etTir;

typedef  struct _stFdj{
 etFdj typeJeu;
 etDb db_type;
 bool use_odb;			/// utilisation ancien fichier *.sqlite
 bool fdj_new;			/// Recharger info base de la fdj
}stFdj;

typedef struct _stParam_3 {
 stFdj *ihm;
 QString cnx;    /// nom connexion a la base
 QString sql;    /// requete permettant liste des tirages a traiter
 QString src;    /// nom Table liste des tirages a traiter
 QString fdj;    /// nom de la tables des tirages fdj
 QString dad;    /// nom de la table pere de src
 QString jrs;    /// jours trouves dans les tirages a traiter
 QString flt;    /// nom de la tables des filtres des tirages
}stParam_3;

typedef struct _stParam_2 {
  QString std;    /// nom long de la zone
  QString abv;    /// nom abreg de la zone
  QString sel;    /// nom correspondant a  la selection en cours
}stParam_2;

typedef struct _stParam_1{
  int pos;    /// offset de debut zone dans fichier
  int len;    /// nombre d'elements composant la zone
  int min;    /// valeur mini possible pour un element
  int max;    /// valeur maxi possible pour un element
  int win;    /// nb elements a avoir pour jackpot
  int usr;    /// nb elements choisi par utilisateur
}stParam_1;
extern const stParam_1 defParam_1[];

/// Definition d'un jeu
#if 1
class stGameConf{
 public:
 stGameConf();
 stGameConf(stGameConf * conf_in);
 stGameConf* operator=(stGameConf * a);

 public:
 int id;
 bool bUseMadeBdd;
 etFdj eFdjType; /// type du jeu
 etTir eTirType; /// origine
 int znCount; /// nombre de zones a regarder
 int nbInDay; /// nombre de tirage sur un jour
 stParam_1 *limites; /// bornes sur la zone
 stParam_2 *names; /// nom de la zone
 stParam_3 *db_ref;
 QStringList** slFlt;

 private:
 static int gmeConf_counter;

};
#else
typedef struct _stGameConf{
  int id;
  bool bUseMadeBdd;
  etFdj eFdjType; /// type du jeu
  etTir eTirType; /// origine
  int znCount; /// nombre de zones a regarder
  stParam_1 *limites; /// bornes sur la zone
  stParam_2 *names; /// nom de la zone
  stParam_3 *db_ref;
  QStringList** slFlt;
}stGameConf;
#endif

extern const stParam_1 loto_prm1_zn[];
extern const stParam_1 euro_prm1_zn[];

extern const stParam_2 loto_prm2_zn[];
extern const stParam_2 euro_prm2_zn[];

#endif // GAME_H
