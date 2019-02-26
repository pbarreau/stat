#ifndef GAME_H
#define GAME_H

#include <QString>
#include "properties.h"

/// Type de jeu possible d'etudier
typedef enum _eGame{
    eGameToSet, /// Jeu pas encore configure
    eGameLoto,  /// Loto
    eGameEuro,   /// Euro million
    eGameDefEnd
}eGame;
extern const QString gameLabel [eGameDefEnd];

typedef enum _eGoal{
    eFdj, /// le jeu e
    eUsr
}eFrom;

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
}stParam_1;

/// Definition d'un jeu
typedef struct _stGameConf{
    eGame type; /// type du jeu
    eFrom from; /// origine
    int znCount; /// nombre de zones a regarder
    stParam_1 *limites; /// bornes sur la zone
    stParam_2 *names; /// nom de la zone
}BGame;

#endif // GAME_H
