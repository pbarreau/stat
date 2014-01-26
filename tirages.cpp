#include <QFileDialog>

#include "tirages.h"

stTiragesDef tirages::conf;
NE_FDJ::E_typeJeux tirages::choixJeu;

tirages::tirages(NE_FDJ::E_typeJeux jeu)
{
    int zone;

    choixJeu = jeu;
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

QString tirages::LabelColonnePourBase(stTiragesDef *ref)
{
    int zone, elem, j;
    int nbZn = ref->nb_zone ;
    int *zn_conf=ref->nbElmZone;
    QString *tab = ref->nomZone;
    QString msg1 = "";


    // creation du message pour les colonnes
    for(zone=0;zone<nbZn;zone++)
    {
        elem = zn_conf[zone];
        for(j=0;j<elem;j++)
        {
            msg1 = msg1 + tab[zone]+QString::number(j+1) +",";
        }
    }

    if(msg1.length() != 0){
        msg1.remove(msg1.size()-1,1);
        msg1 = "jour, " + msg1;
    }
    return msg1;
}

QString tirages::SelectSource(bool load)
{
    QString fileName_2 ="";
    QString msg = "";

    // Choix du fichier contenant les tirages
    switch(choixJeu)
    {
    case NE_FDJ::fdj_euro:
        msg = "Fichier pour Euro million";
        fileName_2 = "euromillions_2.csv";
        break;
    case NE_FDJ::fdj_loto:
        msg="Fichier pour Loto";
        fileName_2="nouveau_loto.csv";
        break;
    default:
        break;
    }

    // Selection d'un fichier et non utilisation du defaut
    if(!load)
    {
        // Menu selection fichier avec chemin
        fileName_2=QFileDialog::getOpenFileName(0,
                                                qPrintable(msg), QDir::currentPath(), "Fdj (*.csv);;Perso (*.*)");

    }

     return     fileName_2;
}
