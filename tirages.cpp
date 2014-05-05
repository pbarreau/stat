#include <QFileDialog>

#include "tirages.h"

// Variables static de la classe
stTiragesDef tirages::conf;
QString *tirages::lib_col;
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
	  conf.limites[1].max = 11;
	  conf.jour_tir=new QString[conf.nb_tir_semaine];
	  conf.jour_tir[0]="MARDI";
	  conf.jour_tir[1]="VENDREDI";
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

  lib_col = new QString [conf.nb_zone];
  couverture = new int *[conf.nb_zone];
  value.valBoules = new int *[conf.nb_zone];
  for(zone = 0; zone < conf.nb_zone; zone++)
  {
	lib_col[zone] = this->qs_zColBaseName(zone);
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

QString tirages::qs_zColBaseName(int zone)
{
  int elem = 0;
  QString msg1 = "";

  for(elem = 0; elem < (this->conf.nbElmZone[zone]);elem++)
  {
	msg1 = msg1 + this->conf.nomZone[zone] + QString::number(elem +1) +",";
  }

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
  switch(choixJeu)
  {
	case NE_FDJ::fdj_euro:
	  msg = "Fichier pour Euro million";
	  fileName_2 = "euromillions_3.csv";
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
