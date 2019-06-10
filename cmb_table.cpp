#include "SyntheseGenerale.h"
#include "db_tools.h"

/// Creation des tables pour les combinaisons a rechercher
/// Normalement il faudrait chercher les Cnp
/// dans le cas loto C(49,5) = 1906884
/// autre approche pour gagner il faut 5 boules
/// reparties sur 4 dizaines. on a donc 6 valeurs
/// possible (0-5) a repartir sur chaque dizaine
/// tel que le total soit 5 (le nombre de boule a avoir
/// pour gagner.
/// Pour les etoiles les combinaisons  sont
/// moins nombreuses, on fait le calcul Cnp classique
bool SyntheseGenerale::do_CmbRef(void)
{

 bool isOk = true;
 QString msg = "";
 QSqlQuery query(db_0);

 int nbZone = pMaConf->nb_zone;

 QString tblUse = "A_cmb";

 for (int zn=0;(zn < nbZone) && isOk;zn++ )
 {
  if(pMaConf->limites[zn].win>2){
   isOk = TraitementCodeVueCombi(zn);

	 if(isOk)
		isOk = TraitementCodeTblCombi(tblUse,zn);
	}
	else
	{
	 int n = pMaConf->limites[zn].max;
	 int p = pMaConf->limites[zn].win;
	 QString tbName = tblUse+ "_z"+QString::number(zn+1);
	 // calculer les combinaisons avec repetition
	 BCnp *a = new BCnp(n,p,db_0.connectionName());
	 tbName = a->getDbTblName();
	 isOk = TraitementCodeTblCombi_2(tblUse,tbName,zn);
	}

	/// Efface table temporaire
	if(isOk){
	 msg = "drop view if exists tb_view_"+QString::number(zn+1);
	 isOk=query.exec(msg);
	}
 }

 if(!isOk)
 {
  QString ErrLoc = "SyntheseGenerale::do_CmbRef";
  DB_Tools::DisplayError(ErrLoc,&query,"");
 }

 return isOk;
}

bool SyntheseGenerale::TraitementCodeVueCombi(int zn)
{
 bool isOk = true;
 QSqlQuery query(db_0);
 QString msg = "";
 QString ref_1 = "";

 QString viewCode[]=
  {
   "drop view if exists tb_view_%1;",
   "create view if not exists tb_view_%1 as select tbChoix.tz%1 as "
    +pMaConf->names[zn].abv+ " "
                               "from (%2 as tbChoix)where(tbChoix.tz%1 is not null);"
  };
 int argViewCount[]={1,2};

 /// Traitement de la vue
 int nbLgnCode = sizeof(viewCode)/sizeof(QString);
 for(int lgnCode=0;(lgnCode<nbLgnCode) && isOk;lgnCode++){
  msg = "";
  switch(argViewCount[lgnCode]){
   case 1:
    msg = msg + viewCode[lgnCode].arg(zn+1);
    break;
   case 2:
    ref_1 = "Bnrz";
    msg = msg + viewCode[lgnCode].arg(zn+1).arg(ref_1);
    break;
   default:
    msg = "Error on the number of args";
    break;
  }
#ifndef QT_NO_DEBUG
  qDebug() << msg;
#endif

  isOk = query.exec(msg);
 }

 if(!isOk)
 {
  QString ErrLoc = "TraitementCodeVueCombi:";
  DB_Tools::DisplayError(ErrLoc,&query,msg);
 }

 query.finish();

 return isOk;
}

bool SyntheseGenerale::TraitementCodeTblCombi(QString tbName,int zn)
{
 bool isOk = true;
 QSqlQuery query(db_0);
 QString msg = "";

 QString tblCode[]=
  {
   "drop table if exists "+tbName+"_z%1;",
   "create table if not exists "+tbName+"_z%1 (id integer primary key,%2);",
   "insert into "+tbName+"_z%1 select NULL,%2 from (%3) where(%4="
    +QString::number(+pMaConf->limites[zn].win)+");"
  };
 int argTblCount[]={1,2,4};

 QString ref_1 = "";
 QString ref_2 = "";
 QString ref_3 = "";
 QString ref_4 = "";
 QString ref_5 = "";
 int nbLgnCode= 0;


 /// traitement creation table en fonction 10zaine
 int lenZn = floor(pMaConf->limites[zn].max/10)+1;
 ref_1="t%1."+pMaConf->names[zn].abv+" as "+pMaConf->names[zn].abv+"%1";
 QString msg1 = "";
 for(int pos=0;pos<lenZn;pos++){
  msg1 = msg1 + ref_1.arg(pos+1);
  if(pos < lenZn -1)
   msg1 = msg1 + ",";
 }

 nbLgnCode = sizeof(tblCode)/sizeof(QString);
 for(int lgnCode=0;(lgnCode<nbLgnCode) && isOk;lgnCode++){
  msg="";
  switch(argTblCount[lgnCode]){
   case 1:
    msg = tblCode[lgnCode].arg(zn+1);
    break;
   case 2:{
    ref_1=msg1+",";
    ref_1.replace(QRegExp("t\\d\."
                          +pMaConf->names[zn].abv
                          +"\\s+as\\s+"),"");
    ref_1.replace(",", " int,");
    ref_1=ref_1 + "tip text, poids real";
    msg = tblCode[lgnCode].arg(zn+1).arg(ref_1);
   }
   break;
   case 4:{
    ref_1="%d";
    ref_2="t%1."+pMaConf->names[zn].abv;
    ref_3="(%1*t%2."+pMaConf->names[zn].abv+")";
    ref_4="tb_view_%1 as t%2";
    ref_5=pMaConf->names[zn].abv+"%1";
    QString msg2 = "";
    QString msg3 = "";
    QString msg4 = "";
    QString msg5 = "";
    for(int pos=0;pos<lenZn;pos++){
     msg2 = msg2 + ref_2.arg(pos+1);
     msg3 = msg3 + ref_3.arg(1<<pos).arg(pos+1);
     msg4 = msg4 + ref_4.arg(zn+1).arg(pos+1);
     msg5 = msg5 + ref_5.arg(pos+1);
     if(pos < lenZn -1){
      ref_1 = ref_1 + "/%d";
      msg2 = msg2 + ",";
      msg3 = msg3 + "+";
      msg4 = msg4 + ",";
      msg5 = msg5 + "+";
     }
    }

		ref_2=msg1+","+QString::fromLocal8Bit("printf('%1',%2)as tip,(%3) as poids");
		ref_1 = ref_2.arg(ref_1).arg(msg2).arg(msg3);
		ref_2 = msg4;
		ref_3 = msg5;
		msg = tblCode[lgnCode].arg(QString::number(zn+1),ref_1,ref_2,ref_3);
	 }
	 break;
	 default:
		msg = "Error on the number of args";
		break;
	}

#ifndef QT_NO_DEBUG
	qDebug() << msg;
#endif

  isOk = query.exec(msg);
 }

 if(!isOk)
 {
  QString ErrLoc = "TraitementCodeVueCombi:";
  DB_Tools::DisplayError(ErrLoc,&query,msg);
 }

 query.finish();

 return isOk;
}

/// Creer une table intermediaire
/// Def_comb_z2Cnp_12_2
bool SyntheseGenerale::TraitementCodeTblCombi_2(QString tbName, QString tbCnp, int zn)
{
 bool isOk = true;
 QSqlQuery query(db_0);
 QString msg = "";

 msg = "drop table if exists "+tbName+"_z"+QString::number(zn+1);
 isOk = query.exec(msg);
#ifndef QT_NO_DEBUG
 qDebug() << msg;
#endif

 if(isOk){
  /// traitement creation table
  int lenZn = pMaConf->limites[zn].len;
  QString ref_1="c%1 int";
  QString ref_2="(%1*c%2)";//"+gameInfo.nom[zn].abv+"
  QString msg1 = "";
  QString msg2 = "";
  for(int pos=0;pos<lenZn;pos++){
   msg1 = msg1 + ref_1.arg(pos+1);
   msg2 = msg2 + ref_2.arg(1<<(pos)).arg((pos+1));
   if(pos < lenZn -1){
    msg1 = msg1 + ",";
    msg2 = msg2 + "+";
   }
  }

	ref_1 = msg1;
	ref_1 = ref_1.replace("c",pMaConf->names[zn].abv) + +",tip text, poids real" ;
	msg = "create table if not exists "
				+tbName+"_z"+QString::number(zn+1)
				+"(id integer primary key,"
				+ref_1
				+");";
#ifndef QT_NO_DEBUG
	qDebug() << msg;
#endif

	isOk = query.exec(msg);

	if(isOk){
	 msg1 = msg1.remove("int");
	 msg = msg1;
	 msg = msg.replace(QRegExp("c\\d\\s+"),"%02d");
	 msg = msg.replace(",","/");
	 /// traitement insertion dans table
	 msg = "insert into "+tbName+"_z"
				 +QString::number(zn+1)
				 +" select NULL,"
				 +msg1 + ",(printf('"+msg+"',"+msg1+"))as tip,("+msg2+") as poids "
				 +"from ("+tbCnp+")";
#ifndef QT_NO_DEBUG
	 qDebug() << msg;
#endif
	 query.exec(msg);
	 if(isOk){
		/// Supprimer la table Cnp
		msg = "drop table if exists "+tbCnp;
		isOk = query.exec(msg);
	 }
	}

 }

 if(!isOk)
 {
  QString ErrLoc = "TraitementCodeVueCombi:";
  DB_Tools::DisplayError(ErrLoc,&query,msg);
 }

 query.finish();

 return isOk;
}

void SyntheseGenerale::do_SetFollower(void)
{

 bool isOk = true;
 QSqlQuery query(db_0);
 QString msg = "";

 QString tblUse []= {"Ana_z","RefTirages", "Bnrz"};

 int nbZone = pMaConf->nb_zone;

 for (int zn=0;(zn < nbZone) && isOk;zn++ )
 {
  int nbwin = pMaConf->limites[zn].win;

  if(nbwin>2){

   msg = " select t1.*, t2.c1 from old_" + tblUse[0]+QString::number(zn+1)+ " as t1 left join (select 0 as c1) as t2 ";

	 QString sql_msg[]={
		"alter table "+tblUse[0]+QString::number(zn+1)+" rename to old_"+tblUse[0]+QString::number(zn+1),
		"create table if not exists " + tblUse[0]+QString::number(zn+1)+ " as "+msg,
		"drop table if exists old_"+tblUse[0]+QString::number(zn+1)
	 };
	 int nb_sql= sizeof(sql_msg)/sizeof(QString);

	 /// Rajout de la colonne c1
	 for (int current=0;(current < nb_sql) && isOk ; current++) {
#ifndef QT_NO_DEBUG
		qDebug() << "msg["<<current<<"]="<<sql_msg[current];
#endif
		isOk = query.exec(sql_msg[current]);
	 }


	 /// la colonne est creee la remplir
	 /// du plus grand au plus petit
	 QString zn_field = getFieldsFromZone(zn,"t1");
	 QString ref="((r%2.z1=r%1.z1+1) and r%2.z1 in ("+zn_field+"))";
	 QString ref2="(r%1.z1 in ("+zn_field+"))";

	 for (int nbloop= nbwin;(nbloop>1) && isOk ;nbloop--) {

		QString aliasZn="";
		for (int k =1; k<=nbloop;k++) {
		 aliasZn =aliasZn + tblUse[2] + " as r" +QString::number(k);
		 if(k<nbloop){
			aliasZn = aliasZn + ",";
		 }
		}

		msg="";
		QString deb = "";
		QString msg1 = "";

		for(int i = nbloop; i>0; i--){
		 deb = "update "+tblUse[0]+QString::number(zn+1)+ " as t1 set c1 = " + QString::number(nbloop+1-i);


		 if(i>1){
			msg1 = ref.arg(i-1).arg(i);
			msg1 = " AND " + msg1;
		 }
		 else {
			msg1 = ref2.arg(i);
		 }

		 msg = msg1 + msg  ;
		}

		msg1 = deb + " where (t1.id in ( select t1.id from "
					 + tblUse[1] + " as t1, " + aliasZn + " where ("
					 + msg + ")) and t1.c1=0)";
#ifndef QT_NO_DEBUG
		qDebug() << "deb="<<deb;
		qDebug() << "msg="<<msg;
		qDebug() << "msg1="<<msg1;
#endif

		isOk = query.exec(msg1);
	 }

  }
 }


 if(!isOk)
 {
  QString ErrLoc = "cmb_table.cpp";
  DB_Tools::DisplayError("SyntheseGenerale::",&query,"do_SetFollower");
 }


}

void SyntheseGenerale::do_SetKcmb(void)
{

 bool isOk = true;
 QSqlQuery query(db_0);
 QString msg = "";

 QString tblUse []= {"A_cmb_z","Ana_z","RefTirages"};

 int nbZone = pMaConf->nb_zone;

 for (int zn=0;(zn < nbZone) && isOk;zn++ )
 {
  if(pMaConf->limites[zn].win>2){
   QString ref="(t1.U%1=t2.b%2)";
   QString st_on = "";
   int loop = pMaConf->limites[zn].len;
   for (int i = 0; i <loop; i++) {
    st_on = st_on + ref.arg(i).arg(i+1);
    if(i<loop-1){
     st_on = st_on + " and ";
    }
   }
   /// Requete
   msg = "select t1.*, t2.id as k_cmb from old_"
         + tblUse[1]+QString::number(zn+1)
         + " as t1 inner join "
         + tblUse[0]+QString::number(zn+1)
         + " as t2 on ("
         +st_on
         +")";
  }
  else
  {
   msg = "select t1.*, t2.id as k_cmb from old_"
         + tblUse[1]+QString::number(zn+1)
         + " as t1,"
         + tblUse[2]
         + " as t3 inner join "
         + tblUse[0]+QString::number(zn+1)
         + " as t2 on ((t2.e1=t3.e1) and (t1.id=t3.id))";
  }


	QString sql_msg[]={
	 "alter table "+tblUse[1]+QString::number(zn+1)+" rename to old_"+tblUse[1]+QString::number(zn+1),
	 "create table if not exists " + tblUse[1]+QString::number(zn+1)+ " as "+msg,
	 "drop table if exists old_"+tblUse[1]+QString::number(zn+1)
	};
	int nb_sql= sizeof(sql_msg)/sizeof(QString);

	for (int current=0;(current < nb_sql) && isOk ; current++) {
#ifndef QT_NO_DEBUG
	 qDebug() << "msg["<<current<<"]="<<sql_msg[current];
#endif
	 isOk = query.exec(sql_msg[current]);
	}

 }

 if(!isOk)
 {
  QString ErrLoc = "cmb_table.cpp";
  DB_Tools::DisplayError("SyntheseGenerale::",&query,"do_SetKcmb");
 }

}
