#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>

#include "gererbase.h"

void GEN_ListForAnalyse(QStringList pList[], int nbBoules,stTiragesDef *ref)
{
    QStringList lst_col;
    QStringList lst_cri;

    lst_cri<<"z1%2=0"<<"z1<"+QString::number((ref->limites[0].max)/2);
    lst_col<<"P"<<"G";

    // F
    for(int j=0;j<=9;j++)
    {
        lst_cri<< "z1 like '%" + QString::number(j) + "'";
        lst_col<< "F"+ QString::number(j);
    }

    for(int j=0;j<=nbBoules;j++)
    {
        lst_cri<< "z1 >="+QString::number(10*j)+ " and z1<="+QString::number((10*j)+9);
        lst_col<< "bd"+ QString::number(j);
    }

    pList[0] = lst_col;
    pList[1] = lst_cri;
}

QString GEN_FieldsForAnalyse(stTiragesDef *pConf)
{
    int nbBoules = floor(pConf->limites[0].max/10);

    QString str_fxx = "";

    str_fxx = "C int, P int, G int,";

    // Boule Finissant par xx
    for(int i=0; i<10;i++)
    {
        str_fxx = str_fxx + "F" +QString::number(i)+ " int,";
    }

    // Unite/d/v/t/q/c
    for(int i=0; i<=nbBoules;i++)
    {
        str_fxx = str_fxx + "bd" +QString::number(i)+ " int,";
    }

    // Consecutive
    for(int i=2; i<=pConf->nbElmZone[0];i++)
    {
        str_fxx = str_fxx + "N" +QString::number(i)+ " int,";
    }

    //enlever la derniere ,
    str_fxx.remove(str_fxx.length()-1,1);

    return str_fxx;
}

QString GEN_FieldsForTables(stTiragesDef *pConf, QString prefix)
{
    QString st_tmp = "";
    QString st_suite = " int,";

    int nb_zone = pConf->nb_zone;

    for(int zn=0; zn <nb_zone; zn++)
    {
        int nb_elem = pConf->nbElmZone[zn];
        for(int boule=0;boule<nb_elem;boule++)
        {
            st_tmp = st_tmp + prefix +
                    pConf->nomZone[zn] +
                    QString::number(boule+1)+
                    st_suite;
        }
    }
    st_tmp.remove(st_tmp.length()-1,1);


#ifndef QT_NO_DEBUG
    qDebug() << st_tmp;
#endif

    return st_tmp;
}

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

// Reecriture debut
bool GererBase::CreationTablesDeLaBDD(tirages *pRef)
{
    bool status;
    stTiragesDef ref;

    // Recuperation de la config jeu
    pRef->getConfig(&ref);


    QString st_conf1= "";
    st_conf1= GEN_FieldsForTables(&ref);

    QString st_conf2= "";
    st_conf2= GEN_FieldsForTables(&ref,"t");

    QString st_conf3= "";
    st_conf3 = GEN_FieldsForAnalyse(&ref);

    QString tables[]=
    {
        {QString::fromLocal8Bit(TB_BASE)+";id integer primary key,jour_tirage text, date_tirage text,"+st_conf1},
        {QString::fromLocal8Bit(TB_BASE)+"_trie;id integer primary key,"+st_conf2},
        {QString::fromLocal8Bit(TB_ANLS)+";id integer primary key,"+st_conf3+", id_poids int"},
        {QString::fromLocal8Bit(CL_TCOUV)+ ref.nomZone[0]+ ";id integer primary key,depart int, fin int, taille int"},
        {QString::fromLocal8Bit(CL_TCOUV)+ ref.nomZone[1]+ ";id integer primary key,depart int, fin int, taille int"},
        {QString::fromLocal8Bit(CL_TOARR)+ ref.nomZone[0]+ ";id integer primary key,couv int, boule int"},
        {QString::fromLocal8Bit(CL_TOARR)+ ref.nomZone[1]+ ";id integer primary key,couv int, boule int"}
    };

    int nb_table = sizeof(tables)/sizeof(QString);
    status = true;
    for(int i = 0; i<nb_table && status ;i++)
    {
        QStringList lst_tmp = tables[i].split(";");
        QString st_sql = "";
        QSqlQuery sql_req("");

        st_sql = "create table if not exists " +
                lst_tmp.at(0) +
                "(" +
                lst_tmp.at(1)+
                ");";

#ifndef QT_NO_DEBUG
        qDebug() << st_sql;
#endif

        status = sql_req.exec(st_sql);
    }

    if(status)
        status = BaseCreerTableBnrz(&ref);

#if 0
    if(status)
        status = BaseCreerTableCombinaison(&ref);
#endif

    return status;
}

bool GererBase:: BaseCreerTableBnrz(stTiragesDef *pRef)
{
    QSqlQuery query;

    QString msg1 = "";
    QString nomTable = QString::fromLocal8Bit(TB_BNRZ);

    bool ret = true;
    int zone = 0;
    int j = 0;
    int max_boules = 0;


    // Creation Table Reference Boules des Zones
    msg1 =  "create table if not exists "+nomTable+" (id Integer primary key,";
    for(zone=0;(zone<pRef->nb_zone);zone++)
    {
        msg1 = msg1 + "z"+QString::number(zone+1)+" int";
        if(zone+1 < pRef->nb_zone)
        {
            msg1 = msg1 + ",";
            max_boules = BMAX(pRef->limites[zone].max,pRef->limites[zone+1].max);
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
        for(zone=0;(zone<pRef->nb_zone);zone++)
        {
            for(int bz = j; (bz<=j) && j <= pRef->limites[zone].max;bz++ )
            {
                msg1 = msg1 + ", z"
                        +QString::number(zone+1);
            }

        }

        // Mise en place des valeurs
        msg1 = msg1 + ") values (NULL";
        for(zone=0;(zone<pRef->nb_zone);zone++)
        {
            for(int bz = j; (bz<=j) && j <= pRef->limites[zone].max;bz++ )
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

// Reecriture fin
// --------------------
bool GererBase::OLD_CreationTablesDeLaBDD(tirages *pRef)
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

// -------------------------


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
