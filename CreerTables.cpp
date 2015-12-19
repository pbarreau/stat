#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "gererbase.h"

bool GererBase::CreerTableDistriCombi(void)
{
   bool ret = true;
   QString st_sqlMsg = "";
   QSqlQuery query;

   st_sqlMsg =  "create table DistriCombi "
                "(id INTEGER PRIMARY KEY,"
                "id_com int, tip text, s1 int, s2 int, p1 int, p2 int);";

           ret = query.exec(st_sqlMsg);
           query.finish();
   return ret;
}

bool GererBase::CreationTablesDeLaBDD(tirages *pRef)
{
  QSqlQuery query;
  QString msg1;
  stTiragesDef ref;
  bool ret = true;
  int zone = 0;
  int j = 0;
  int max_boules = 0;

  // Recuperation de la config jeu
  pRef->getConfig(&ref);


  // Creation Table Reference Boules des Zones
  ret = CTB_Table1(TB_BNRZ,pRef);

  // Creation table pour la couverture
  for(zone=0;(zone<ref.nb_zone && ret == true);zone++)
  {
	msg1 =  "create table " + QString::fromLocal8Bit(CL_TCOUV) + ref.nomZone[zone] +
			" (id INTEGER PRIMARY KEY, depart int, fin int, taille int);";
	ret = query.exec(msg1);
	query.finish();
  }

  // Creation table pour ordre arrivee des boules pour la couverture donne
  for(zone=0;(zone<ref.nb_zone) && (ret == true);zone++)
  {
	//    msg1 =  "create table " + CL_TCOUV + "%1 (id INTEGER PRIMARY KEY)";
	//    msg1 = msg1.arg(zone+1);
	msg1 =  "create table " + QString::fromLocal8Bit(CL_TOARR) + ref.nomZone[zone] +
			" (id INTEGER PRIMARY KEY, boule int);";
	ret = query.exec(msg1);

	// Preparer les boules de la zone
	if(ret){
	  msg1 = "insert into " + QString::fromLocal8Bit(CL_TOARR) + ref.nomZone[zone] +
			 " (id, boule) values (:id, :boule)";
	  ret = query.prepare(msg1);

	  for(j=0;(j<ref.limites->max)&& ret;j++)
	  {
		query.bindValue(":boule", j+1);
		ret = query.exec();
	  }
	  query.finish();
	}
  }

  // creation de la table analyse des boules (lors du chargement de la base)
  msg1 = pRef->s_LibColAnalyse(&ref);
  if((msg1.length() != 0) && ret)
  {
	msg1.replace(",", " int,");
	msg1 = "create table analyses (id INTEGER PRIMARY KEY,"+
           msg1 + " int, id_poids int);";

	ret = query.exec(msg1);
	query.finish();
  }

  // Creer une chaine pour stocker le nom des champs de la table des tirages
  msg1 = pRef->s_LibColBase(&ref);
  if((msg1.length() != 0) && ret)
  {
	// Retirer le premier element
	msg1.remove("date_tirage, ");
	msg1.replace(",", " int,");
	msg1 =  "create table tirages (id integer primary key,date_tirage TEXT," +
			msg1 + " int);";

	ret = query.exec(msg1);
	query.finish();
  }

  return ret;
}

// CTB_T1 : Creation table de base :Table1
bool GererBase:: CTB_Table1(QString nomTable, tirages *pRef)
{
    QSqlQuery query;
    QString msg1;
    stTiragesDef ref;
    bool ret = true;
    int zone = 0;
    int j = 0;
    int max_boules = 0;

    // Recuperation de la config jeu
    pRef->getConfig(&ref);

    // Creation Table Reference Boules des Zones
    msg1 =  "create table "+nomTable+" (id Integer primary key,";
    for(zone=0;(zone<ref.nb_zone);zone++)
    {
        msg1 = msg1 + "z"+QString::number(zone+1)+" int";
        if(zone+1 < ref.nb_zone)
        {
            msg1 = msg1 + ",";
            max_boules = BMAX(ref.limites[zone].max,ref.limites[zone+1].max);
        }
    }
    msg1 = msg1 + ");";
    ret = query.exec(msg1);
    query.finish();

    // Creation des ids englobant
    for(j=1;j<=max_boules && ret == true;j++)
    {
        msg1 = "insert into "+nomTable+" (id";

        // Colonnes a mettre
        for(zone=0;(zone<ref.nb_zone);zone++)
        {
            for(int bz = j; (bz<=j) && j <= ref.limites[zone].max;bz++ )
            {
                msg1 = msg1 + ", z"
                        +QString::number(zone+1);
            }

        }

        // Mise en place des valeurs
        msg1 = msg1 + ") values (NULL";
        for(zone=0;(zone<ref.nb_zone);zone++)
        {
            for(int bz = j; (bz<=j) && j <= ref.limites[zone].max;bz++ )
            {
                msg1 = msg1 + ","
                        +QString::number(j);
            }

        }
        msg1 = msg1 + ");";

        ret = query.exec(msg1);
        query.finish();
    }

    return ret;
}
