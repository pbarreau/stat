#ifndef GAME_H
#define GAME_H

/// Type de jeu possible d'etudier
typedef enum _eGame{
    eGameToSet, /// Jeu pas encore configure
    eGameLoto,  /// Loto
    eGameEuro   /// Euro million
}eGame;

typedef enum _eGoal{
    eFdj, /// le jeu e
    eUsr
}eFrom;

typedef struct _stNamesZones {
    QString std;    /// nom long de la zone
    QString abv;    /// nom abreg de la zone
    QString sel;    /// nom correspondant à la selection en cours
}stNamesZones;

#if 0
typedef struct _stParam_1{
    int len;    /// nombre d'elements composant la zone
    int min;    /// valeur mini possible pour un element
    int max;    /// valeur maxi possible pour un element
    int win;    /// nb elements a avoir pour gagner
}stParam_1;
#endif
typedef struct _stBornes
{
    int len;    /// nb element composant la zone
    int min;    /// val min de la zone
    int max;    /// val max de la zone
    int win;    /// nb element a avoir sur la zone pour gagner
}stBornes;

/// Definition d'un jeu
typedef struct _stGameConf{
    eGame type; /// type du jeu
    eFrom from; /// origine
    int znCount; /// nombre de zones a regarder
    stBornes *limites; /// bornes sur la zone
    stNamesZones *names; /// nom de la zone
}BGame;

#endif // GAME_H
