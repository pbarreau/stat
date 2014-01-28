#include <QFileDialog>

#include "tirages.h"

// Variables static de la classe
stTiragesDef tirages::conf;
NE_FDJ::E_typeJeux tirages::choixJeu;
int **tirages::couverture;

tirages::tirages(NE_FDJ::E_typeJeux jeu)
{
    int zone, j;

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
        conf.offsetFichier = new int [conf.nb_zone];
        conf.offsetFichier[0]=4;
        conf.offsetFichier[1]=9;
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
        conf.offsetFichier = new int [conf.nb_zone];
        conf.offsetFichier[0]=4;
        conf.offsetFichier[1]=9;
    }
        break;

    default:
        break;
    }

    couverture = new int *[conf.nb_zone];
    value.valBoules = new int *[conf.nb_zone];
    for(zone = 0; zone < conf.nb_zone; zone++)
    {
        value.valBoules[zone] =  new int [conf.nbElmZone[zone]];
        for(j=0;j<conf.nbElmZone[zone];j++)
        {
            value.valBoules[zone][j]=0;
        }

        couverture[zone]=new int[conf.limites[zone].max];
        for(j=0;j<conf.limites[zone].max;j++)
        {
            couverture[zone][j]=0;
        }
    }
}

void tirages::getConfig(stTiragesDef *priv_conf)
{
    priv_conf->nb_zone = conf.nb_zone;
    priv_conf->nbElmZone = conf.nbElmZone;
    priv_conf->limites = conf.limites;
    priv_conf->nomZone = conf.nomZone;
    priv_conf->offsetFichier = conf.offsetFichier;
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

    // Creation des colonnes pour criteres
    for(zone=0;zone<nbZn;zone++)
    {
        msg1 = msg1 + tab[zone] +"_pair," + tab[zone] +"_E1,";
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

int tirages::NbPairs(int zone)
{
    int i;
    int ret = 0;
    for(i=0;i<(conf.nbElmZone[zone]);i++)
    {
        if((value.valBoules[zone][i]%2) == 0)
            ret++;
    }
    return ret;
}

int tirages::NbDansE1(int zone)
{
    int i;
    int ret = 0;
    for(i=0;i<(conf.nbElmZone[zone]);i++)
    {
        if((value.valBoules[zone][i]) < (conf.limites[zone].max /2))
            ret++;
    }
    return ret;

}

#if 0
int toto()
{
    // calcul de l'ecart d'une boule
    boule_connue = 0;
    ecart_boule = 0;
    nb_ecart = 0;


    // boule connue
    // non alors commencer a compter les ecarts
    // Memoriser le nb d'ecart pour cette boule

    // on trouve de nouveau la boule donc ecart correspond au tirage avant de voir la boule
    // un ecart de plus
}
#endif
