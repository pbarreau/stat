#include <QString>
#include <QStringList>

#include "car.h"

QString LireTableau(int *t, int n);

GammaNk::GammaNk(int n, int k)
{
    cardinal = CompterCardinalGamma(n,k);
    coef = GenereCombinaison(n,k);
}

double GammaNk::CalculerFactorielle(double *x)
{
    double i;
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

double GammaNk::CompterCardinalGamma(int n, int k)
{
    double v1 = n+k-1;
    double v2 = n-1;
    double v3 = k;

    double tmp = 0;

    tmp = CalculerFactorielle(&v1)/(CalculerFactorielle(&v3)*CalculerFactorielle(&v2));

    return tmp;

}

QStringList GammaNk::GenereCombinaison(int n, int k)
{
    int tab[1024];
    int ptrTab = 0;
    int ptrCur = 0;
    int tmp =0;

    tab[0]=k;
    memset(tab+1,0,sizeof(tab)-1);

    QString ligne = "";
    QStringList lst;
    while(tab[n-1] != k)
    {
        ligne = LireTableau(tab, n);
        lst << ligne;
        tmp = tab[n-1];
        tab[n-1] =0;
        ptrCur=n-1;
        while(tab[ptrCur]==0) ptrCur--;
        tab[ptrCur]--;
        tab[ptrCur+1]=tmp+1;

#if 0
        if(tab[n] !=0)
        {
            tmp = tab[n];
            tab[n] = 0;
            ptrTab--;
            ptrCur =  ptrTab;
            tab[ptrCur] = tab[ptrCur] -1;
            tab[ptrCur+1] = tmp + 1;
        }
        else
        {
            ptrCur =  ptrTab;
            tab[ptrCur] = tab[ptrCur] -1;
            tab[ptrCur+1] = 1;
            ptrTab++;
        }
#endif
    }
    ligne = LireTableau(tab, n);
    lst << ligne;

    return lst;
}

QString LireTableau(int *t, int n)
{
    QString tmp = "";

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < t[i]; ++j) {
            tmp = tmp + QString::number(i+1)+";";
        }
    }
    tmp.remove(tmp.length()-1,1);
    return tmp;
}
