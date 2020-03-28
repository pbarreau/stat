#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QString>
#include <QStringList>
#include <QSqlDatabase>

#include "cnp_AvecRepetition.h"

BGnp::BGnp(int n, int p):BCnp((n+p-1),p)
{
}
BGnp::BGnp(int n, int p, QString cnx_bdd):BCnp((n+p-1),p, cnx_bdd,"")
{
}
BGnp::BGnp(int n, int p, QString cnx_bdd, QString tab="My"):BCnp((n+p-1),p, cnx_bdd,tab)
{
}
BGnp::~BGnp()
{
}

/// https://denishulo.developpez.com/tutoriels/access/combinatoire/#LIV-A

#if 0
QString ConstruireRequete(QString item, int n, QString name, int step);
QString LireTableau(int *t, int n);

BP_Gnp::BP_Gnp(int n, int p)
{
    gnp = Cardinal_np();
    pos = 0;
    tab = NULL;

    coefItem = GenereCombinaison(n,p);
}

BP_Gnp::~BP_Gnp()
{
    if(tab !=NULL){
        for(int i = 0; i< gnp;i++){
            delete(tab[i]);
        }
        delete(tab);
    }
}

bool BP_Gnp::CalculerGamma(void)
{
    bool isOK = false;

    if(tab==NULL){
        isOK = FaireTableauGamma();
    }

    return isOK;
}

bool BP_Gnp::FaireTableauGamma(void)
{
    bool isOk = false;
    tab = new int *[gnp]; /// tableau de pointeur d'entiers de Cnp lignes

    /// Allocation memoire OK ?
    if(tab != NULL){
        /// initialisation recursion
        int *L = new int [p];
        int *t = new int [n];

        if(t != NULL){

            for(int i =0; i<n;i++) t[i]=i;
        }

        /// demarrage
        if((L != NULL) && (t !=NULL)){
            CreerLigneTrianglePascal(0,L,t,n);
            isOk = true;
        }
    }

    return isOk;
}

double BP_Gnp::CalculerFactorielle(double *x)
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

double BP_Gnp::CalculerGnp(void)
{
    return(gnp);
}

double BP_Gnp::Cardinal_np()
{
    double v1 = n+p-1;
    double v2 = n-1;
    double v3 = p;

    double tmp = 0;

    tmp = CalculerFactorielle(&v1)/(CalculerFactorielle(&v3)*CalculerFactorielle(&v2));

    return tmp;

}

QStringList BP_Gnp::coef(void)
{
    return (coefItem);
}

QStringList BP_Gnp::GenereCombinaison(int n, int k)
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

QString BP_Gnp::MakeSqlFromGamma(stTiragesDef *pTirDef, int step, int k)
{
    int zone = 0;
    int n = pTirDef->nbElmZone[zone];
    QString name = pTirDef->nomZone[zone];
    QString tmp1 = "";
    QString tmp2 = "";
    BP_Gnp a(n,k);
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
#endif
