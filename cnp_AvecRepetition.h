#ifndef CAR_H
#define CAR_H
#include <QObject>

#include "cnp_SansRepetition.h"

// combinaison avec répétition
class BGnp:public BCnp
{
public:
    BGnp(int n, int p);
    BGnp(int n, int p, QString destBdd);
    BGnp(int n, int p, QString destBdd, QString tab);
    ~BGnp();
};

#if 0
//http://dictionnaire.sensagent.leparisien.fr/Combinaison%20avec%20r%C3%A9p%C3%A9tition/fr-fr/
class BP_Gnp
{
private:
    int n;      /// le n de Gamma np
    int p;      /// le p de Gamma np
    int gnp; /// nombre de combinaisons avec repetitions
    int pos;    /// varie de 0 au début à Gamma np-1 à la fin
    int **tab;  /// tableau de Gamma np lignes contenant chacune une combinaison
                /// sous la forme de p entiers (de 1 au moins à n au plus)

    QStringList coefItem;

public:
    BP_Gnp(int n, int p);
    ~BP_Gnp();

private:
    double Cardinal_np();
    double CalculerFactorielle(double *x);
    QStringList GenereCombinaison(int n, int k);
    double CalculerGnp(void);
    QStringList coef(void);
    static QString MakeSqlFromGamma(stTiragesDef *pTirDef, int step, int k);

};
#endif
#endif // CAR_H
