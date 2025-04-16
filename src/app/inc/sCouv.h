#ifndef SCOUV_H
#define SCOUV_H

#include <QList>
#include "tirages.h"

class sCouv
{
public:
    sCouv(int zn,stTiragesDef *pDef);
    ~sCouv();

private:
    int zoneEtudie;

public:
    stTiragesDef *p_conf;
    int **p_TotalMois;
    QList<bool> *p_trackingBoule;
    int p_deb;
    int p_fin;
    int **p_val; // Colonne (A)rrivee), (B)oule), (T)otal
};


#endif // SCOUV_H
