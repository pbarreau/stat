#ifndef GAME_H
#define GAME_H

#include <QString>
#include "properties.h"

/// Type de jeu possible d'etudier
typedef enum _eFdjType{
    eFdjNotSet, /// Jeu pas encore configure
    eFdjLoto,   /// Loto
    eFdjEuro,   /// Euro million
    eFdjEol     /// End of list
}eFdjType;
extern const QString gameLabel [eFdjEol];

typedef enum _eAnaType{
    eAnaFdj, /// le jeu e
    eAnaUsr
}eAnaType;

typedef struct _stParam_2 {
    QString std;    /// nom long de la zone
    QString abv;    /// nom abreg de la zone
    QString sel;    /// nom correspondant à la selection en cours
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
    eFdjType fdjGame; /// type du jeu
    eAnaType anaBase; /// origine
    int znCount; /// nombre de zones a regarder
    stParam_1 *limites; /// bornes sur la zone
    stParam_2 *names; /// nom de la zone
}BGame;

#endif // GAME_H
