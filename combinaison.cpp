#include <math.h>
#include "tirages.h"
#include <stdio.h>
#include <cstring>
#define MAX 1024
int NB;

static  void FGEN_RechercheCombi(int k, QStringList &l, const QString &s, QStringList &ret);
static void afficher(int *t, int l);
static void kcombinaison(int *t, int n, int p, void (*func)(int*,int));

void tirages::ConstruireListeCnp(int n, int p, QStringList &out)
{
    QStringList in;

    for(int i =0; i< p; i++)
    {
        in <<  QString::number(i+1);
    }

    if(n>15)
        return;

    FGEN_RechercheCombi(n, in, "" , out);
}

int tirages::Cnp_v2(int n, int p) {
    int *t = new int [n]; //Tableau de n éléments
    t[0] = 1;
    for (int i = 1; i <= n; i++) {
        t[i] = 1;
        for (int j = i - 1; j >= 1; j--) //On part de le fin pour ne pas écraser les valeurs.
            t[j] = t[j] + t[j - 1]; //On fait les calculs nécessaires.
    }
    return t[p]; //On renvoie la valeur recherchée.
}

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

/// https://openclassrooms.com/forum/sujet/algorithmique-calcul-de-combinaison-n-p-70883
/// Calcul de C(n,p) combinaison sans repetition de p elements pris parmis n
/// ie : comment faire des ensembles distincts de p elements
/// lorsque l'on prend ces elements dans l'ensemble n
int Cnp_v1 (int n, int p) {
    int m[n][n];
    int value = 0;
    if (p > n || p < 0)
        return -1 ; //erreur
    for (int i = 0 ; i < n ; i++) {
        m[i][0] = 1 ;
        m[i][i] = 1 ;
        for (int j = 1 ; j < i ; j++) {
            m[i][j] = m[i-1][j] + m[i-1][j-1] ;
            if (j == p && i == n){
                value = m[i][j] ;
                break;
                return value; /// A tester
            }
        }
    }
    return value;
}

int Cnp_v2(int n, int p) {
    if(n<0 || p<0 || p>n) return 0;
    int *t = new int [n]; //Tableau de n éléments
    t[0] = 1;
    for (int i = 1; i <= n; i++) {
        t[i] = 1;
        for (int j = i - 1; j >= 1; j--) //On part de le fin pour ne pas écraser les valeurs.
            t[j] = t[j] + t[j - 1]; //On fait les calculs nécessaires.
    }
    return t[p]; //On renvoie la valeur recherchée.
}

/// http://jm.davalan.org/mots/comb/comb/combalgo.html
#if 0
/// Un programme comb.c écrit en C calcule toutes les combinaisons dans un tableau d'entiers de taille Cnp × p
/// pour qu'on puisse en faire ce qu'on veut ensuite.
/// Toutefois, si on veut juste afficher les combinaisons
/// ou les envoyer à un autre programme au fur et à mesure de leur création,
/// le programme simplifié comb2.c suffira.
/// comb2.c
/* comb2.c
*
* Recherche de toutes les combinaisons de p Ã©lÃ©ments de
* l'ensemble {1, 2, 3, 4, 5, ..., n}
*
* 01/05/2005 Jean-Paul Davalan <jpdvl@wanadoo.fr>
*
* compilation :  gcc -O2 -o comb2 comb2.c
* usage : comb2 n p
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void combinaisons(int n, int p, int k, int *L, int *t, int r) {
    int i, j, j1, t2[n];
    if(r<p-k) return;
    if(k==p) {
        for(i=0;i<p;i++) printf("%d ", L[i] + 1);
        printf("\n");
        return;
    }
    for(i=0;i<r;i++) {
        L[k] = t[i];
        for(j=i+1, j1=0;j<r;j++, j1++) {
            t2[j1] = t[j];
        }
        combinaisons(n, p, k+1, L, t2, j1);
    }
}

void effectue(int n, int p) {
    int L[p], t[n], i;
    for(i=0;i<n;i++)
        t[i] = i;
    combinaisons(n, p, 0, L, t, n);
}

int main(int argc, char *argv[]) {
    int n, p;
    if(argc<3) {
        printf("usage : %s n p\n",argv[0]);
        exit(1);
    }
    n = atoi(argv[1]); // lecture des paramÃ¨tres
    p = atoi(argv[2]);
    if(n<0 || p<0 || p>n) return 0;
    effectue(n, p);

    return 0;
}
/// ------
#endif

/// https://www.dcode.fr/combinaisons#q1
#if READ
Quel est l'algorithme de dénombrement des combinaisons ?

// pseudo code
debut denombrement_combinaisons( k , n ) {
 si (k = n) retourner 1;
 si (k > n/2) k = n-k;
 res = n-k+1;
 pour i = 2 par 1 tant que i < = k
  res = res * (n-k+i)/i;
 fin pour
 retourner res;
fin
// langage C
double factorielle(double x) {
 double i;
 double result=1;
 if (x >= 0) {
  for(i=x;i>1;i--) {
   result = result*i;
  }
  return result;
 }
 return 0; // erreur
}
double compter_combinaisons(double x,double y) {
 double z = x-y;
 return factorielle(x)/(factorielle(y)*factorielle(z));
}
Quel est l'algorithme pour générer des combinaisons ?

// javascript
function combinaisons(a) { // a = new Array(1,2)
  var fn = function(n, source, en_cours, tout) {
    if (n == 0) {
      if (en_cours.length > 0) {
        tout[tout.length] = en_cours;
      }
      return;
    }
    for (var j = 0; j < source.length; j++) {
      fn(n - 1, source.slice(j + 1), en_cours.concat([source[j]]), tout);
    }
    return;
  }
  var tout = [];
  for (var i=0; i < a.length; i++) {
    fn(i, a, [], tout);
  }
  tout.push(a);
  return tout;
}
#endif
