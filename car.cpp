#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QString>
#include <QStringList>

#include "car.h"

QString ConstruireRequete(QString item, int n, QString name, int step);
QString LireTableau(int *t, int n);

GammaNk::GammaNk(int n, int k)
{
    nbItem = CompterCardinalGamma(n,k);
    coefItem = GenereCombinaison(n,k);
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

double GammaNk::cardinal(void)
{
    return(nbItem);
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

QStringList GammaNk::coef(void)
{
    return (coefItem);
}

QStringList GammaNk::GenereCombinaison(int n, int k)
{
    int tab[1024];
    //int ptrTab = 0;
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

    }
    ligne = LireTableau(tab, n);
    lst << ligne;

    return lst;
}

QString GammaNk::MakeSqlFromGamma(stTiragesDef *pTirDef, int step, int k)
{
    int zone = 0;
    int n = pTirDef->nbElmZone[zone];
    QString name = pTirDef->nomZone[zone];
    QString tmp1 = "";
    QString tmp2 = "";
    GammaNk a(n,k);
    QStringList lesCoefs = a.coef();

    for(int i = 0; i< lesCoefs.size(); i++)
    {
        tmp1 = ConstruireRequete(lesCoefs.at(i),n,name, step);
        tmp2 = tmp2 + "("+ tmp1 +")" + "or";
    }
    tmp2.remove(tmp2.length()-2,2);

#ifndef QT_NO_DEBUG
qDebug()<< tmp2;
#endif

    return tmp2;
}

QString ConstruireRequete(QString item, int n, QString name, int step)
{
    QStringList lst = item.split(";");
    QString tmp = "";

        for(int i=1;i<lst.size();i++)
        {
         tmp = tmp + "("+name+lst.at(i)+
                 "="+name+lst.at(0)+"+"+
                 QString::number(i*step)+")";
         tmp = tmp + "and";
        }
        tmp.remove(tmp.length()-3,3);

        return tmp;
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
