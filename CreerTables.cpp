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
QString msg1, msg2;
stTiragesDef ref;
bool ret = false;

pRef->getConfig(&ref);


// Creation des tables pour la couverture, et le nb d'element touve
msg1 =  "create table couverture (id INTEGER PRIMARY KEY, depart int, fin int, taille int)";
query.exec(msg1);

// detail analyse couverture
//msg1 = "create table b_couvdetail (id integer primary key, couvid int, boule int, ecart int, position int, total int)

msg1 = pRef->s_LibColBase(&ref);


if(msg1.length() != 0){
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
