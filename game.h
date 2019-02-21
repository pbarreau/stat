#ifndef GAME_H
#define GAME_H

#include <QString>

/// Type de jeu possible d'etudier
typedef enum _eGames{
    eGameToSet, /// Jeu pas encore configure
    eGameLoto,  /// Loto
    eGameEuro,  /// Euro million
    eGameLstEnd /// Fin de cette liste
}eGames;

typedef enum _eGoal{
    eGoalOff,
    eFdj,       /// le jeu e
    eUsr
}eFrom;

typedef struct _stZnNames {
    QString std;    /// nom long de la zone
    QString abv;    /// nom abreg de la zone
    QString sel;    /// nom correspondant à la selection en cours
}stZnNames;

typedef struct _stBornes
{
    int len;    /// nb element composant la zone
    int min;    /// val min de la zone
    int max;    /// val max de la zone
    int win;    /// nb element a avoir sur la zone pour gagner
    int sel;    /// nb element selectionne par utilisateur
}stZnLimites;

/// Definition d'un jeu
typedef struct _stGameConf{
    eGames type; /// type du jeu
    eFrom from; /// origine
    int prevision_id;
    int znCount; /// nombre de zones a regarder
    stZnLimites *limites; /// bornes sur la zone
    stZnNames *names; /// nom de la zone
}B_Game;

#endif // GAME_H
