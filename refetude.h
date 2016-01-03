#ifndef REFETUDE_H
#define REFETUDE_H

#include <QWidget>
#include <QString>
#include <QGridLayout>
#include <QTabWidget>
#include "tirages.h"

class sCouv
{
    public:
    sCouv(int zn,stTiragesDef *pDef);
    ~sCouv();

    public:
    stTiragesDef *p_conf;
    int p_deb;
    int p_fin;
    int **p_val;
    int *p_connu;
};

class RefEtude:private sCouv
{
public:
    RefEtude(QString stFiltreTirages, int zn, stTiragesDef *pDef);

private:
    QWidget *CreationOnglets();
    QGridLayout *MonLayout_TabTirages();
    QGridLayout *MonLayout_TabCouvertures();
    QGridLayout *MonLayout_TabEcarts();

    bool RechercheCouverture(int zn);
    bool AnalysePourCouverture(QSqlRecord unTirage, int *bIdStart, int zn, sCouv *memo);

private:
    QString p_stRefTirages;
    stTiragesDef *p_conf;
};

#endif // REFETUDE_H
