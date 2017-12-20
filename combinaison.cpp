#include <math.h>
#include "tirages.h"
#include <stdio.h>
#include <cstring>
#define MAX 1024
int NB;

static  void FGEN_RechercheCombi(int k, QStringList &l, const QString &s, QStringList &ret);
static void afficher(int *t, int l);
static void kcombinaison(int *t, int n, int p, void (*func)(int*,int));

void tirages::ListeCombinaison(stTiragesDef *ref)
{
  int nbBoules = floor(ref->limites[0].max/10)+1;
  for(int i = 1; i<=nbBoules;i++)
  {
      conf.sl_Lev0 << QString::number(i);
  }

  // Recuperation des combinaison C(1,5), C(2,5), C(3,5), C(4,5), C(5,5)
  for (int i = 0; i< 5; i++)
  {
      FGEN_RechercheCombi(i+1, conf.sl_Lev0, "" , conf.sl_Lev1[i]);

  }


  ////// TEST
  int tab[MAX];
  int n = 5, p = 3; // 4 objets pris parmi 3
  NB = 0;
  kcombinaison(tab,n,p,afficher);
  printf("Le NOMBRE de facon de prendre %d objets parmi %d sans ordre et avec repetition est: \n C(%d,%d) = %d\n", p, n, n+p-1, p, NB);


}

// Recherche des combinaison possible
void FGEN_RechercheCombi(int k, QStringList &l, const QString &s, QStringList &ret)
{
    QStringList tmp = l;

    tmp.removeAt(0);

    if (k==0) {
        ret << s ;
        return;
    }
    if (l.isEmpty()) return;

    if (s.isEmpty())
    {
        FGEN_RechercheCombi(k-1, tmp, l.at(0),ret);
    }
    else
    {
        FGEN_RechercheCombi(k-1, tmp, s+","+l.at(0),ret);
    }

    FGEN_RechercheCombi(k, tmp, s,ret);
}

// http://forum.hardware.fr/hfr/Programmation/C-2/resolu-renvoyer-combinaison-sujet_23393_1.htm
// C : http://www.dcode.fr/generer-calculer-combinaisons
// https://www.dcode.fr/combinaisons
double factorielle(double *x)
{ double i;
  double result=1;
  if(*x >= 1)
  {
    for(i=*x;i>1;i--)
    {
      result = result*i;
    }
    return result;
  }
  else
    if(*x == 0)
      return 1;
  return 0;
}

double compter_combinaisons(double x,double y)
{
  double z = x-y;
  return factorielle(&x)/(factorielle(&y)*factorielle(&z));
}


//----------------------------------
/// http://dictionnaire.sensagent.leparisien.fr/Combinaison%20avec%20r%C3%A9p%C3%A9tition/fr-fr/

void afficher(int *t, int l) {
    int i,j;
    for (i = 0; i < l; ++i) printf("%d ",t[i]);
    printf("[ ");
    for (i = 0; i < l; ++i) {
        for (j = 0; j < t[i]; ++j) {
            printf("%d ", i);
        }
    }
    printf("]\n");
    ++NB;
}

void kcombinaison(int *t, int n, int p, void (*func)(int*,int)) {
    int tmp,i;
    t[0] = p;
    memset(t+1,0, sizeof *t * (n - 1));
    while(t[n-1] != p) {
        func(t,n);

        tmp = t[n-1];
        t[n-1] = 0;
        i = n-1;
        while(t[i] == 0) --i;

        --t[i];
        t[i+1] = tmp + 1;
    }
    func(t,n);
}

#if 0
int main() {
    int tab[MAX];
    int n = 5, p = 3; // 4 objets pris parmi 3
    NB = 0;
    kcombinaison(tab,n,p,afficher);
    printf("Le NOMBRE de facon de prendre %d objets parmi %d sans ordre et avec repetition est: \n C(%d,%d) = %d\n", p, n, n+p-1, p, NB);
    return 0;
}
#endif
