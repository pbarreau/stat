#include "tirages.h"

stTiragesDef tirages::conf;

tirages::tirages(NE_FDJ::E_typeJeux jeu)
{
    int zone;

    switch(jeu)
    {
    case NE_FDJ::fdj_loto:
    {
        conf.nb_zone = 2;
        conf.nbElmZone = new int [conf.nb_zone];
        conf.nbElmZone[0]=5;
        conf.nbElmZone[1]=1;
        conf.limites = new stBornes [conf.nb_zone];
        conf.limites[0].min = 1;
        conf.limites[0].max = 49;
        conf.limites[1].min = 1;
        conf.limites[1].max = 10;
        conf.nomZone = new QString [conf.nb_zone];
        conf.nomZone[0]="b";
        conf.nomZone[1]="e";
    }
        break;

    case NE_FDJ::fdj_euro:
    {
        conf.nb_zone = 2;
        conf.nbElmZone = new int [conf.nb_zone];
        conf.nbElmZone[0]=5;
        conf.nbElmZone[1]=2;
        conf.limites = new stBornes [conf.nb_zone];
        conf.limites[0].min = 1;
        conf.limites[0].max = 50;
        conf.limites[1].min = 1;
        conf.limites[1].max = 11;
        conf.nomZone = new QString [conf.nb_zone];
        conf.nomZone[0]="b";
        conf.nomZone[1]="e";
    }
        break;

    default:
        break;
    }

    value.valBoules = new int *[conf.nb_zone];
    for(zone = 0; zone < conf.nb_zone; zone++)
    {
        value.valBoules[zone] =  new int [conf.nbElmZone[zone]];
    }
}

void tirages::getConfig(stTiragesDef *priv_conf)
{
   priv_conf->nb_zone = conf.nb_zone;
   priv_conf->nbElmZone = conf.nbElmZone;
   priv_conf->limites = conf.limites;
   priv_conf->nomZone = conf.nomZone;
}
