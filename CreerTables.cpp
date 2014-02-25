#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#
#include "gererbase.h"

bool GererBase::CreerTableTirages(tirages *pRef)
{
QSqlQuery query;
QString msg1;
stTiragesDef ref;
bool ret = true;
int zone = 0;
int j = 0;

// Recuperation de la config jeu
pRef->getConfig(&ref);

// Creation table pour la couverture
for(zone=0;(zone<ref.nb_zone && ret == true);zone++)
{
  msg1 =  "create table " + QString::fromLocal8Bit(CL_TCOUV) + ref.nomZone[zone] +
      " (id INTEGER PRIMARY KEY, depart int, fin int, taille int)";
  ret = query.exec(msg1);
  query.finish();
}

// Creation table pour ordre arrivee des boules pour la couverture donne
for(zone=0;(zone<ref.nb_zone) && (ret == true);zone++)
{
  //    msg1 =  "create table " + CL_TCOUV + "%1 (id INTEGER PRIMARY KEY)";
  //    msg1 = msg1.arg(zone+1);
  msg1 =  "create table " + QString::fromLocal8Bit(CL_TOARR) + ref.nomZone[zone] +
      " (id INTEGER PRIMARY KEY, boule int)";
  ret = query.exec(msg1);

  // Preparer les boules de la zone
  if(ret){
      msg1 = "insert into " + QString::fromLocal8Bit(CL_TOARR) + ref.nomZone[zone] +
          " (id, boule) values (:id, :boule)";
      ret = query.prepare(msg1);

      for(j=0;(j<ref.nbElmZone[zone])&& ret;j++)
        {
          query.bindValue(":boule", j+1);
          ret = query.exec();
        }
      query.finish();
    }
}


// Creer une chaine pour stocker le nom des champs de la table des tirages
msg1 = pRef->s_LibColBase(&ref);


if((msg1.length() != 0) && ret)
{
  // Retirer le premier element
  msg1.remove("jour, ");
  msg1.replace(",", " int,");
  msg1 = msg1 + " int";

  msg1 =  "create table tirages (id integer primary key,jour TEXT," +
      msg1 + ")";

  if (db.isOpen())
    {

      ret = query.exec(msg1);

    }

}
return ret;
}
