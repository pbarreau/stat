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

#include <math.h>

#include "tirages.h"

QString NormaliseJour(QString input);

// pour trier la fusion superloto + loto :
// sort -o mysort.txt -t ";" -r -n -k3.9 -k3.4 -k3 nouveau_loto.csv
// http://www.cyberciti.biz/faq/linux-unix-sort-date-data-using-sortcommand/

bool GererBase::LireLesTirages(QString fileName_2, tirages *pRef)
{
    QFile fichier(fileName_2);
    QString msg = "";
     bool ret = true;

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
    QString date_tirage = "";
    QString jour_tirage = "";
    QStringList list1;

    stTiragesDef ref;
    pRef->getConfig(&ref);

    QString st_sqrq= "";
    QString st_bind= "";
    QString st_conf= "";

    QSqlQuery sql_1;
    QSqlQuery sql_2;

    int zone = 0;
    int max_zone = ref.nb_zone;

    // traitement Table 1
    st_conf= GEN_FieldsForTables(&ref);
    st_conf.remove("int");
    st_conf.replace(QRegExp("\\s+"),"");
    st_bind=st_conf;
    st_bind.replace(",",", :");
    st_sqrq = "INSERT INTO tirages (id,jour_tirage,date_tirage," +
            st_conf + ")VALUES (:id, :jour, :date, :" + st_bind + ")";
    sql_1.prepare(st_sqrq);

#ifndef QT_NO_DEBUG
        qDebug() << st_sqrq;
#endif

    // traitement Table 2
    st_conf= GEN_FieldsForTables(&ref,"t");
    st_conf.remove("int");
    st_conf.replace(QRegExp("\\s+"),"");
    st_sqrq = "INSERT INTO tirages_trie (id," +
            st_conf + ")VALUES (:id, :" + st_bind + ")";
    sql_2.prepare(st_sqrq);

#ifndef QT_NO_DEBUG
        qDebug() << st_sqrq;
#endif


    // Construction d'une variable en fonction du max /10
    int **pRZone = new int *[max_zone]; // Pointeur de repartition des zones
    for(zone=0;zone< max_zone;zone++)
    {
      pRZone[zone]=new int[(ref.limites[zone].max/10)+1];
    }


    // Passer la premiere ligne
    ligne = flux.readLine();

    // Analyse des suivantes
    QStringList lstConf[2];
    int nbBoules = floor(ref.limites[0].max/10);
    GEN_ListForAnalyse(lstConf,nbBoules,&ref);

    int nb_lignes=0;
    while(! flux.atEnd() && ret)
    {
      ligne = flux.readLine();
      nb_lignes++;

      //traitement de la ligne
      list1 = ligne.split(";");

      // recuperation du jour de la semaine
      jour_tirage = list1.at(1).simplified();
      sql_1.bindValue(":jour", jour_tirage);

      // Recuperation du date_tirage
      date_tirage= NormaliseJour(list1.at(2).simplified());
      sql_1.bindValue(":date", date_tirage);

      // Recuperation des boules

      int posTrie = ref.offsetFichier[1]+ref.nbElmZone[1];

      QStringList lst_trie ;
      switch(ref.choixJeu)
      {
      case NE_FDJ::fdj_euro:
          lst_trie <<  list1.at(posTrie);
          lst_trie <<  list1.at(posTrie+1);
          break;

      case NE_FDJ::fdj_loto:
          lst_trie = list1.at(posTrie).split("+");
          break;

      default:
          return false;
          break;
      }

      for(zone=0;zone< max_zone;zone++)
      {
          for(int j = 0; j < (ref.limites[zone].max/10)+1; j++)
            pRZone[zone][j]=0;

          int maxElmZone = ref.nbElmZone[zone];
          for(int ElmZone=0;ElmZone < maxElmZone;ElmZone++)
          {
              // Recuperation de la valeur
              int boule = list1.at(ref.offsetFichier[zone]+ElmZone).toInt();
              pRef->value.valBoules[zone][ElmZone]= boule;

              // Preparation pour affectation variable sql (tirages)
              QString clef_1 = ":"+ref.nomZone[zone]+QString::number(ElmZone+1);
              clef_1.replace(QRegExp("\\s+"),"");
              sql_1.bindValue(clef_1,boule);

              // Table 2
              if(lst_trie[zone].startsWith("-"))
                  lst_trie[zone].remove(0,1);

              boule=lst_trie[zone].split("-").at(ElmZone).toInt();
              sql_2.bindValue(clef_1,boule);
          }

      }

      // Mettre dans la table Tirages
      ret = sql_1.exec();
      if (ret)
      {
          // Mettre dans table ordre croissant
          ret = sql_2.exec();

          // Analyser ce tirage
          if (ret)
          {
          ret = AnalyserCeTirage(nb_lignes, lstConf, &ref);
          }

      }

    }

    return ret;
}

bool GererBase::AnalyserCeTirage(int tirId, QStringList pList[], stTiragesDef *pRef)
{
    bool status = true;

    //nouvelle ligne dans analyse
    QSqlQuery sql_1;
    QString sql_msg = "insert into analyses (id) values (null);";

    status = sql_1.exec(sql_msg);

    int total_cri = pList[0].size();
    for(int cri_id = 0 ; (cri_id < total_cri) && status; cri_id ++)
    {
     int val = RechercheInfoTirages(tirId, cri_id,pRef);
     sql_msg = "update analyses set "
             +pList[0].at(cri_id)+
             "="
             +QString::number(val)+
             " "
             "where id ="
             +QString::number(tirId)+
             ";";
#ifndef QT_NO_DEBUG
        qDebug() << sql_msg;
#endif

     status = sql_1.exec(sql_msg);
    }

    return status;
}

bool GererBase::OLD_LireLesTirages(QString fileName_2, tirages *pRef)
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

#ifndef QT_NO_DEBUG
        qDebug() << str_1;
        qDebug() << str_2;
#endif

	// Mettre dans la base
	ret = sql_1.exec();
	ret = sql_2.exec();
  }
  return ret;
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


