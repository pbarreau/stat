#ifndef BCOUV_H
#define BCOUV_H

#include<QString>
#include<QSqlDatabase>
#include <QTableView>
#include <QList>

#include "game.h"

typedef struct _track{
    int ane[12];/// Repartition sur l'annee
    int pos;    /// ordre arrive
    int num;    /// id de la boule
    int tot;    /// nb de fois vue
}stTrack;

typedef struct _infoCouv
{
    int tDeb;   /// numero tirage depart
    int tEnd;   /// numero tirage assurant couverture
    int pDeb;   /// position debut dans tirage
    int pEnd;   /// position fin dans tirage
    int *a;     /// Liste des arrivees
    int *t;     /// total de chaque arrivee
}BPstInfoCouv;

class BCouv2
{
public:
    BCouv2(const QString &in, const B_Game &pDef, QSqlDatabase fromDb);

private:
    void rechercherCouv(int zn);
    QTableView * TablePourLstcouv(int zn);

private:
    bool *isKnown;
    QList<BPstInfoCouv*> *couv;
    QString dataIn;
    B_Game   gameIn;
    QSqlDatabase dbIn;
};

#endif // BCOUV_H
