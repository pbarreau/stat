#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QWidget>
#include <QSqlDatabase>

#include "properties.h"

/// Localisation de la base de donnees
typedef enum _eBddType{
 eDbSetOnRam, /// en memoire
 eDbSetOnDsk   /// sur disque
}etDbPlace;

/// Type de jeu possible d'etudier
typedef enum _eFdjType{
 eFdjNotSet, /// Jeu pas encore configure
 eFdjLoto,   /// Loto
 eFdjEuro,   /// Euro million
 eFdjEol     /// End of list
}etFdjType;
extern const QString gameLabel [eFdjEol];

typedef enum _eAnaType{
 eTirNotSet, /// Lst tirages pas encore etablie
 eTirFdj,    /// Lst tirages de la Fdj
 eTirGen,    /// Lst tirages autogeneree
 eTirUsr     /// Lst tirages choix utilisateur
}etTirType;

typedef struct _stParam_3 {
 QString cnx;    /// nom connexion a la base
 QString fdj;    /// nom Table liste des tirages
}stParam_3;

typedef struct _stParam_2 {
  QString std;    /// nom long de la zone
  QString abv;    /// nom abreg de la zone
  QString sel;    /// nom correspondant a  la selection en cours
}stParam_2;

typedef struct _stParam_1{
  int len;    /// nombre d'elements composant la zone
  int min;    /// valeur mini possible pour un element
  int max;    /// valeur maxi possible pour un element
  int win;    /// nb elements a avoir pour jackpot
  int usr;    /// nb elements choisi par utilisateur
}stParam_1;

/// Definition d'un jeu
typedef struct _stGameConf{
  int id;
  bool bUseMadeBdd;
  etFdjType eFdjType; /// type du jeu
  etTirType eTirType; /// origine
  int znCount; /// nombre de zones a regarder
  stParam_1 *limites; /// bornes sur la zone
  stParam_2 *names; /// nom de la zone
  stParam_3 *db_ref;
}stGameConf;

extern const stParam_1 loto_prm1_zn[];
extern const stParam_1 euro_prm1_zn[];

extern const stParam_2 loto_prm2_zn[];
extern const stParam_2 euro_prm2_zn[];

#endif // GAME_H
