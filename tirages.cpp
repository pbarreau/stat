#include <QFileDialog>

#include <math.h>
#include "tirages.h"

static  void FGEN_RechercheCombi(int k, QStringList &l, const QString &s, QStringList &ret);

// Variables static de la classe
stTiragesDef tirages::conf;
stMyLinkedList *tirages::arrive;
int *tirages::total;
QString *tirages::lib_col;

tirages::tirages(NE_FDJ::E_typeJeux jeu)
{
    int zone, j;

    conf.choixJeu = jeu;
    conf.pTir = this;
    switch(jeu)
    {
    case NE_FDJ::fdj_none:
    {
        conf.nb_zone = 0;
        conf.nb_tir_semaine = 0;
        conf.nbElmZone = NULL;
        conf.limites = NULL;
        conf.jour_tir=NULL;
        conf.nomZone = NULL;
        conf.offsetFichier = NULL;
    }
        break;

    case NE_FDJ::fdj_slot:
    {
        conf.nb_zone = 2;
        conf.nb_tir_semaine = 4;
        conf.nbElmZone = new int [conf.nb_zone];
        conf.nbElmZone[0]=5;
        conf.nbElmZone[1]=1;
        conf.limites = new stBornes [conf.nb_zone];
        conf.limites[0].min = 1;
        conf.limites[0].max = 49;
        conf.limites[1].min = 1;
        conf.limites[1].max = 10;
        conf.jour_tir=new QString[conf.nb_tir_semaine];
        conf.jour_tir[0]="LUNDI";
        conf.jour_tir[1]="MERCREDI";
        conf.jour_tir[2]="SAMEDI";
        conf.jour_tir[3]="VENDREDI";
        conf.nomZone = new QString [conf.nb_zone];
        conf.nomZone[0]="b";
        conf.nomZone[1]="e";
        conf.FullNameZone = new QString [conf.nb_zone];
        conf.FullNameZone[0]="Boules";
        conf.FullNameZone[1]="Etoiles";
        conf.offsetFichier = new int [conf.nb_zone];
        conf.offsetFichier[0]=4;
        conf.offsetFichier[1]=9;
    }
        break;

    case NE_FDJ::fdj_loto:
    {
        conf.nb_zone = 2;
        conf.nb_tir_semaine = 3;
        conf.nbElmZone = new int [conf.nb_zone];
        conf.nbElmZone[0]=5;
        conf.nbElmZone[1]=1;
        conf.limites = new stBornes [conf.nb_zone];
        conf.limites[0].min = 1;
        conf.limites[0].max = 49;
        conf.limites[1].min = 1;
        conf.limites[1].max = 10;
        conf.jour_tir=new QString[conf.nb_tir_semaine];
        conf.jour_tir[0]="LUNDI";
        conf.jour_tir[1]="MERCREDI";
        conf.jour_tir[2]="SAMEDI";
        conf.nomZone = new QString [conf.nb_zone];
        conf.nomZone[0]="b";
        conf.nomZone[1]="e";
        conf.FullNameZone = new QString [conf.nb_zone];
        conf.FullNameZone[0]="Boules";
        conf.FullNameZone[1]="Etoiles";
        conf.offsetFichier = new int [conf.nb_zone];
        conf.offsetFichier[0]=4;
        conf.offsetFichier[1]=9;
    }
        break;

    case NE_FDJ::fdj_euro:
    {
        conf.nb_zone = 2;
        conf.nb_tir_semaine = 2;
        conf.nbElmZone = new int [conf.nb_zone];
        conf.nbElmZone[0]=5;
        conf.nbElmZone[1]=2;
        conf.limites = new stBornes [conf.nb_zone];
        conf.limites[0].min = 1;
        conf.limites[0].max = 50;
        conf.limites[1].min = 1;
        conf.limites[1].max = 12;
        conf.jour_tir=new QString[conf.nb_tir_semaine];
        conf.jour_tir[0]="MARDI";
        conf.jour_tir[1]="VENDREDI";
        conf.nomZone = new QString [conf.nb_zone];
        conf.nomZone[0]="b";
        conf.nomZone[1]="e";
        conf.FullNameZone = new QString [conf.nb_zone];
        conf.FullNameZone[0]="Boules";
        conf.FullNameZone[1]="Etoiles";
        conf.offsetFichier = new int [conf.nb_zone];
        conf.offsetFichier[0]=4;
        conf.offsetFichier[1]=9;
    }
        break;

    default:
        break;
    }


    if(conf.nb_zone)
    {
        int nbelment = conf.limites[0].max;
        arrive = new stMyLinkedList[nbelment];
        memset(arrive,0,sizeof(stMyLinkedList)*nbelment);

        total = new int [nbelment+1];
        for(int i =1; i<=nbelment;i++ )
        {
            if (i< nbelment)
            {
                arrive[i].n = i+1;
            }
            else
            {
                arrive[i].n=0;
            }
            arrive[i].p = i-1;
        }
        lib_col = new QString [conf.nb_zone];
        value.valBoules = new int *[conf.nb_zone];

    }
    for(zone = 0; zone < conf.nb_zone; zone++)
    {
        lib_col[zone] = this->qs_zColBaseName(zone);
        value.valBoules[zone] =  new int [conf.nbElmZone[zone]];
        for(j=0;j<conf.nbElmZone[zone];j++)
        {
            value.valBoules[zone][j]=0;
        }
    }
}

void tirages::getConfigFor(stTiragesDef *priv_conf)
{

    int *nbElmZone = NULL;
    int *offsetFichier = NULL;
    QString *jour_tir = NULL;
    QString *nomZone = NULL;
    QString *Fullzn = NULL;
    stBornes *limites = NULL;

    priv_conf->choixJeu = conf.choixJeu;
    priv_conf->nb_zone = conf.nb_zone;
    nbElmZone = new int [conf.nb_zone];
    limites = new stBornes [conf.nb_zone];
    nomZone = new QString [conf.nb_zone];
    Fullzn = new QString [conf.nb_zone];
    offsetFichier = new int [conf.nb_zone];

    for(int i = 0; i< conf.nb_zone; i++)
    {
        nbElmZone[i]=conf.nbElmZone[i];
        limites[i]=conf.limites[i];
        nomZone[i]=conf.nomZone[i];
        Fullzn[i]=conf.FullNameZone[i];
        offsetFichier[i]=conf.offsetFichier[i];
    }
    priv_conf->limites = limites;
    priv_conf->nbElmZone = nbElmZone;
    priv_conf->nomZone = nomZone;
    priv_conf->FullNameZone = Fullzn;
    priv_conf->offsetFichier = offsetFichier;

    priv_conf->nb_tir_semaine = conf.nb_tir_semaine;
    jour_tir=new QString[conf.nb_tir_semaine];

    for(int i = 0; i< conf.nb_tir_semaine; i++)
    {
        jour_tir[i]=conf.jour_tir[i];
    }
    priv_conf->jour_tir = jour_tir;
}

QString tirages::qs_zColBaseName(int zone)
{
    int elem = 0;
    QString msg1 = "";

    int items = conf.nbElmZone[zone];

    for(elem = 0; elem < items;elem++)
    {
        msg1 = msg1 + conf.nomZone[zone] + QString::number(elem +1) +",";
    }

#if 0
    for(elem = 0; elem < (this->conf.nbElmZone[zone]);elem++)
    {
        msg1 = msg1 + this->conf.nomZone[zone] + QString::number(elem +1) +",";
    }
#endif

    // Suppression de la derniere virgule
    if(msg1.length() != 0){
        msg1.remove(msg1.size()-1,1);
    }

    return msg1;
}

QString tirages::s_LibColAnalyse(stTiragesDef *pRef)
{
    QString msg2 = "";
    stTiragesDef ref = *pRef;

    for(int zone=0;zone<ref.nb_zone ;zone++)
    {
        int val = ref.limites[zone].max/10;

        val++;

        for(int j=0;j<val;j++)
        {
            //int val = j%10;
            msg2 = msg2 + ref.nomZone[zone] + "d"+QString::number(j)+",";
        }
    }

    if(msg2.length() != 0){
        msg2.remove(msg2.size()-1,1);
    }
    return msg2;
}

QString tirages::s_LibColBase(stTiragesDef *ref)
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

    // Colonnes pour parité
    for(zone=0;zone<nbZn;zone++)
    {
        msg1 = msg1 + tab[zone] + CL_PAIR + ",";
    }

    // Colonnes pour appartenance sous groupe
    for(zone=0;zone<nbZn;zone++)
    {
        msg1 = msg1 + tab[zone] + CL_SGRP +",";
    }

    // jour de la semaine du tirage
    msg1 = msg1 + "jour_tirage,";

    if(msg1.length() != 0){
        msg1.remove(msg1.size()-1,1);
        msg1 = "date_tirage, " + msg1;
    }
    return msg1;
}

QString tirages::SelectSource(bool load)
{
    QString fileName_2 ="";
    QString msg = "";

    // Choix du fichier contenant les tirages
    switch(this->conf.choixJeu)
    {
    case NE_FDJ::fdj_euro:
        msg = "Fichier pour Euro million";
        fileName_2 = "euromillions_4.csv";
        break;
    case NE_FDJ::fdj_loto:
        msg="Fichier pour Loto";
        fileName_2="loto2017.csv";
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


int tirages::RechercheNbBoulesLimite(int zone, int min, int max)
{
    int i;
    int ret = 0;
    for(i=0;i<(conf.nbElmZone[zone]);i++)
    {
        if((value.valBoules[zone][i] >= min) && (value.valBoules[zone][i]<max))
            ret++;
    }
    return ret;
}

int tirages::RechercheNbBoulesPairs(int zone)
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

int tirages::RechercheNbBoulesDansGrp1(int zone)
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

#if 0
void Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                     const QModelIndex &index) const
{
#if 0
    QString val;
    if (qVariantCanConvert<QString>(index.data()))
        val = qVariantValue<QString>(index.data());
    if (val == "1")
    {
        painter->fillRect(option.rect, option.palette.highlight());
    }
    else
        QItemDelegate::paint(painter, option, index);
#endif
}
#endif
