#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QFile>
#include <QString>
#include <QTextStream>

#include <QMessageBox>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

#include "tirages.h"

QString NormaliseJour(QString input);

bool GererBase::LireLesTirages(QString fileName_2, tirages *pRef)
{
  QFile fichier(fileName_2);
  QString msg = "";
  QString clef_1= "";
  QString clef_2= "";
  bool ret = false;

  // On ouvre notre fichier en lecture seule et on vérifie l'ouverture
  if (!fichier.open(QIODevice::ReadOnly | QIODevice::Text))
  {
	QMessageBox msgBox;
	msg = "Auto chargement : " + fileName_2 + "\nEchec!!";
	msgBox.setText(msg);
	msgBox.exec();
	return false;
  }


  QTextStream flux(&fichier);
  QString ligne = "";
  QStringList list1;
  stTiragesDef ref;
  pRef->getConfig(&ref);

  QString str_1 = "";
  QString str_2 = "";
  //QString cln_tirages = pRef->s_LibColBase(&ref);
  //QString LesColonnes = cln_tirages;

  QSqlQuery sql_1(db);
  QSqlQuery sql_2(db);

  int nbPair = 0;
  int nbE1 = 0;

  // Variable pour garder valeur des lignes
  QString date_tirage = "";
  QString jour_tirage = "";
  int zone = 0;
  int ElmZone = 0;
  int max_zone = ref.nb_zone;
  int maxElmZone = 0;
  int val1 = 0;
  int val2 = 0;

  // Table des tirages
  str_1 = pRef->s_LibColBase(&ref);
  str_2 = str_1;
  str_1.replace(QRegExp("\\s+"),""); // suppression des espaces
  str_1.replace(",",", :");
  str_1 = "INSERT INTO tirages (id," + str_2 + ")VALUES (:id, :" + str_1 + ")";
  sql_1.prepare(str_1);

  // Table des analyses
  str_1 = pRef->s_LibColAnalyse(&ref);
  str_2 = str_1;
  str_1.replace(QRegExp("\\s+"),""); // suppression des espaces
  str_1.replace(",",", :");
  str_1 = "INSERT INTO analyses (id," + str_2 + ")VALUES (:id, :" + str_1 + ")";
  sql_2.prepare(str_1);

  // Construction d'une variable en fonction du max /10
  int **pRZone = new int *[max_zone]; // Pointeur de repartition des zones
  for(zone=0;zone< max_zone;zone++)
  {
	pRZone[zone]=new int[(ref.limites[zone].max/10)+1];
  }

  // Passer la premiere ligne
  ligne = flux.readLine();


  // Analyse des suivantes
  int nb_lignes=0;
  while(! flux.atEnd())
  {
	ligne = flux.readLine();
	nb_lignes++;

	//traitement de la ligne
	list1 = ligne.split(";");

	// recuperation du jour de la semaine
	jour_tirage = list1.at(1);
	sql_1.bindValue(":jour_tirage", jour_tirage);

	// Recuperation du date_tirage
	date_tirage= NormaliseJour(list1.at(2));
	sql_1.bindValue(":date_tirage", date_tirage);

	// Recuperation des boules
	for(zone=0;zone< max_zone;zone++)
	{
	  for(int j = 0; j < (ref.limites[zone].max/10)+1; j++)
		pRZone[zone][j]=0;

	  maxElmZone = ref.nbElmZone[zone];

	  for(ElmZone=0;ElmZone < maxElmZone;ElmZone++)
	  {
		// Recuperation de la valeur
		val1 = list1.at(ref.offsetFichier[zone]+ElmZone).toInt();
		pRef->value.valBoules[zone][ElmZone]= val1;

		// Preparation pour affectation variable sql (tirages)
		clef_1 = ":"+ref.nomZone[zone]+QString::number(ElmZone+1);
		clef_1.replace(QRegExp("\\s+"),"");
		sql_1.bindValue(clef_1,val1);

		// incrementation du compteur unite/dizaine
		pRZone[zone][val1/10]++;
	  }

	  for(int j = 0; j < (ref.limites[zone].max/10)+1; j++)
	  {
		val2 = pRZone[zone][j];
		// Preparation pour affectation variable sql (analyses)
		clef_2 = ":"+ref.nomZone[zone]+"d"+QString::number(j);
		clef_2.replace(QRegExp("\\s+"),"");
		sql_2.bindValue(clef_2,val2);
	  }

	  // Calcul perso a mettre dans la base
	  // Automatisation possible ?????
	  nbPair = pRef->RechercheNbBoulesPairs(zone);
	  clef_1 = " :" + ref.nomZone[zone]+ CL_PAIR;
	  sql_1.bindValue(clef_1.replace(QRegExp("\\s+"),""),nbPair);

	  nbE1 = pRef->RechercheNbBoulesDansGrp1(zone);
	  clef_1 = " :" + ref.nomZone[zone]+ CL_SGRP ;
	  sql_1.bindValue(clef_1.replace(QRegExp("\\s+"),""),nbE1);

	}
	// Mettre dans la base
	ret = sql_1.exec();
	ret = sql_2.exec();
  }
  return true;
}

QString NormaliseJour(QString input)
{
  // La fonction doit retourner une date au format  AAAA-MM-JJ
  // http://fr.wikipedia.org/wiki/ISO_8601
  QString ladate = "";

  // regarder la taille de la date
  if (input.size()==10){
	// fdj euro million v2 -> JJ/MM/AAAA
	QStringList tmp = input.split("/");
	ladate = tmp.at(2) + "-"
			 + tmp.at(1)+ "-"
			 + tmp.at(0);

  }
  else
  {
	// fdj euro million v1 -> AAAAMMJJ
	ladate =input.left(4) + "-"
			+ input.mid(4,2)+ "-"
			+ input.right(2);
  }
  return ladate;
}


