#ifndef CAR_H
#define CAR_H

#include <QstringList>

//http://dictionnaire.sensagent.leparisien.fr/Combinaison%20avec%20r%C3%A9p%C3%A9tition/fr-fr/

// combinaison avec répétition
class GammaNk
{
private:
    int cardinal;
    QStringList coef;

public:
   GammaNk(int n, int k);

private:
   double CompterCardinalGamma(int n, int k);
   double CalculerFactorielle(double *x);
   QStringList GenereCombinaison(int n, int k);

};

#endif // CAR_H
