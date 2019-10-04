#ifndef QT_NO_DEBUG
#include <QDebug>
#include <QSqlError>
#endif

#include <QMessageBox>
#include <QSqlDatabase>
#include <QApplication>
#include <QDate>
#include <QFile>

#include "game.h"
#include "db_tools.h"

int cFdjData::total = 0;

const stParam_1 loto_prm1_zn[2]={{5,1,49,5,0},{1,1,10,1,0}};
const stParam_2 loto_prm2_zn[2]={{"boules","b","none"},{"etoiles","e","none"}};
const stParam_1 euro_prm1_zn[2]={{5,1,50,5,0},{2,1,12,2,0}};
const stParam_2 euro_prm2_zn[2]={{"boules","b","none"},{"etoiles","e","none"}};

const QString gameLabel [eFdjEol]={
 "NonDefini",
 "Loto",
 "Euro"
};


cFdjData::cFdjData(cFdjData const &parent)
{
 fdj_def=parent.fdj_def;
 fdj_db=parent.fdj_db;
 fdj_cnx=parent.fdj_cnx;
 fdj_cnf=parent.fdj_cnf;
 fdj_elm=parent.fdj_elm;
 fdj_lst=parent.fdj_lst;
 fdj_ana=parent.fdj_ana;
 fdj_tot=parent.fdj_tot;
}


cFdjData::cFdjData(etFdjType eFdjType, cFdjData *parent):QObject (parent)
{

 if((eFdjType !=eFdjLoto) && (eFdjType != eFdjEuro)){
  return ;
 }



 fdj_def.eFdjType = eFdjType;
 fdj_def.eTirType = eTirFdj;
 fdj_def.id = total;
 total++;

 if(!parent){
  fdj_cnx="";
  fdj_cnf="";
  fdj_elm="";
  fdj_lst="";
  fdj_ana="";
  fdj_tot="";
  init(eFdjType);
 }

}

cFdjData::~cFdjData()
{
 QString msg = "Game "
               +QString::number(fdj_def.id).rightJustified(2,'0')
               + " ("+gameLabel[fdj_def.eFdjType]+") be destroyed !!";
 int ret= QMessageBox::information(nullptr,"Pgm", msg,QMessageBox::Ok|QMessageBox::Cancel);

#if 0
 if(ret == QMessageBox::Ok){
  delete pGame->limites;
  delete pGame->names;

  delete pGame;
  total--;
 }
#endif
}

void cFdjData::init(etFdjType eFdjType)
{
 bool isOk = false;

 if(eFdjType == eFdjLoto){
  int nbZn = 2;
  fdj_def.znCount = nbZn;
  fdj_def.limites = new stParam_1[nbZn];
  fdj_def.names = new stParam_2[nbZn];

	for(int i =0; i<nbZn;i++){
	 fdj_def.limites[i]=loto_prm1_zn[i];
	 fdj_def.names[i]=loto_prm2_zn[i];
	}
 }

 if(eFdjType == eFdjEuro){
  int nbZn = 2;
  fdj_def.znCount = nbZn;
  fdj_def.limites = new stParam_1[nbZn];
  fdj_def.names = new stParam_2[nbZn];

	for(int i =0; i<nbZn;i++){
	 fdj_def.limites[i]=euro_prm1_zn[i];
	 fdj_def.names[i]=euro_prm2_zn[i];
	}
 }

 QString msg = "Game "
               +QString::number(fdj_def.id).rightJustified(2,'0')
               + " set for : "+gameLabel[eFdjType]+" !!";
 QMessageBox::information(nullptr,"Pgm", msg,QMessageBox::Ok);

 /// Ouvrir un fichier sqlite
 if((eFdjType!=eFdjNotSet) && (isOk = ouvrirBase(eFdjType))==true){
  isOk = FillDataBase();
 }
 else {
  QMessageBox::critical(nullptr,"Pgm", "Ouverture base impossible !!",QMessageBox::Close);
  QApplication::quit();
 }
}

bool cFdjData::FillDataBase(void)
{
 bool isOk= true;
 QSqlQuery query(fdj_db);

 stTblFill lstTbl[]={
  {"def",&cFdjData::crt_TblDef}, /// Table definition zones
  {"elm",&cFdjData::crt_TblElm}, /// Table definition elments zones
  {"fdj",&cFdjData::crt_TblFdj}, /// Table des tirages Fdj
  {"ana",&cFdjData::crt_TblAna}  /// Table de l'analyses des tirages
 };

 int nbACreer = sizeof(lstTbl)/sizeof(stTblFill);
 for(int uneTable=0;(uneTable<nbACreer) && isOk;uneTable++)
 {
  /// Nom de la table
  QString tbName = lstTbl[uneTable].tbDef;

	/// Fonction de traitement de la creation
	isOk=(this->*(lstTbl[uneTable].pFuncInit))(tbName,&query);

	/// Analyser le retour de traitement
	if(!isOk){
	 QString ErrLoc = "cFdjData::FillDataBase";
	 DB_Tools::DisplayError(ErrLoc,&query,tbName);
	 QApplication::quit();
	}
 }

 return isOk;
}

QString cFdjData::mk_IdDsk(etFdjType type, etTirType eTirtype)
{
 QDate myDate = QDate::currentDate();
 QString toDay = myDate.toString("dd-MM-yyyy");
 QString game = "";

 QFile myFileName;
 QString testName = "";

 game = gameLabel[type] + QString("_V2_");

 if(eTirtype==eTirFdj){
  game = game +toDay+QString("_");

	int counter = 0;
	do{
	 testName = game + QString::number(counter).rightJustified(3,'0')+QString(".sqlite");

	 myFileName.setFileName(testName);
	 counter = (counter + 1)%999;
	}while(myFileName.exists());
 }

 if(eTirtype==eTirGen){
  testName=game+QString("Ref_Cnp.sqlite");
 }


 return testName;
}

QString cFdjData::mk_IdCnx(etFdjType type, etTirType eTirtype)
{
 QString msg="cnx_NotSetYet";

 if((type <= eFdjNotSet) || (type>=eFdjEol)){
  etFdjType err = eFdjNotSet;
  QMessageBox::warning(nullptr,"cFdjData","Jeu "+gameLabel[err]+" inconnu !!",QMessageBox::Ok);
  QApplication::quit();
 }

 msg=QString("cnx_V2_")+gameLabel[type];

 if(eTirtype==eTirGen){
  msg=msg+QString("-")+QString("Ref_Cnp");
 }

 msg = msg + QString("-")+QString::number(fdj_def.id).rightJustified(2,'0');

 return (msg);
}

bool cFdjData::ouvrirBase(etFdjType eFdjType)
{
 bool isOk = true;
 etDbPlace cible = eDbSetOnDsk;
 fdj_cnx = mk_IdCnx(eFdjType,eTirFdj);
 fdj_db = QSqlDatabase::addDatabase("QSQLITE",fdj_cnx);

 QString mabase = "";

 switch(cible)
 {
  case eDbSetOnRam:
   mabase = ":memory:";
   break;

	case eDbSetOnDsk:
	default:
	 mabase = mk_IdDsk(eFdjType,eTirFdj);
	 break;
 }

 /// definition de la base pour ce calcul
 fdj_db.setDatabaseName(mabase);

 // Open database
 isOk = fdj_db.open();

 if(isOk)
  isOk = OPtimiseAccesBase();


 return isOk;
}

bool cFdjData::OPtimiseAccesBase(void)
{
 bool isOk = true;
 QSqlQuery query(fdj_db);
 QString msg = "";

 QString stRequete[]={
  "PRAGMA synchronous = OFF",
  "PRAGMA page_size = 4096",
  "PRAGMA cache_size = 16384",
  "PRAGMA temp_store = MEMORY",
  "PRAGMA journal_mode = OFF",
  "PRAGMA locking_mode = EXCLUSIVE"
 };
 int items = sizeof(stRequete)/sizeof(QString);

 for(int i=0; (i<items)&& isOk ;i++){
  msg = stRequete[i];
  isOk = query.exec(msg);
 }

 if(!isOk)
 {
  QString ErrLoc = "LesComptages::OPtimiseAccesBase";
  DB_Tools::DisplayError(ErrLoc,&query,msg);
 }

 return isOk;
}

bool cFdjData::crt_TblDef(QString tbl_name,QSqlQuery *query)
{
 bool isOk= true;
 QString msg = "";

 QString tbName = gameLabel[fdj_def.eFdjType]
                  +QString("_")
                  + QString::number(fdj_def.id).rightJustified(3,'0')
                  +QString("_")
                  +tbl_name;

 QString colsDef = "min int, max int, len int, win int, abv text, std text";
 msg = "create table if not exists "
       + tbName
       + "(id integer primary key,"
       + colsDef
       +");";

 isOk = query->exec(msg);

 /// preparation des insertions
 msg = "insert into "
       +tbName
       +"(id,min,max,len,win,abv,std)values(NULL,:arg1, :arg2, :arg3, :arg4, :arg5, :arg6)";
 query->prepare(msg);
#ifndef QT_NO_DEBUG
 qDebug() <<msg;
#endif

 /// la table est cree mettre les infos
 if(isOk)
 {
  /// Parcourir toutes les definition
  for(int def = 0; (def<fdj_def.znCount) && isOk;def++)
  {
   query->bindValue(":arg1",fdj_def.limites[def].min);
   query->bindValue(":arg2",fdj_def.limites[def].max);
   query->bindValue(":arg3",fdj_def.limites[def].len);
   query->bindValue(":arg4",fdj_def.limites[def].win);
   query->bindValue(":arg5",fdj_def.names[def].abv);
   query->bindValue(":arg6",fdj_def.names[def].std);

	 /// executer la commande sql
	 isOk = query->exec();
	}
 }
 return isOk;
}

bool cFdjData::crt_TblElm(QString tbl_name,QSqlQuery *query)
{
 bool isOk= true;
 QString msg = "";

 QString tbName = gameLabel[fdj_def.eFdjType]
                  +QString("_")
                  + QString::number(fdj_def.id).rightJustified(3,'0')
                  +QString("_")
                  +tbl_name;

 QString colsDef = "";
 QString argsDef = "";
 QString def_1 = "z%1 int,sz%1 int,tz%1 int";
 QString def_2 = ":arg%1, :arg%2";

 int totDef=fdj_def.znCount;
 int maxElemts = 0;
 for(int def = 0; (def<totDef) && isOk;def++)
 {
  /// Noms des colonnes a mettre
  colsDef=colsDef + def_1.arg(def+1);

	/// valeurs
	argsDef = argsDef + def_2.arg((def*2)+1).arg((def*2)+2);

	/// derniere zone a traiter
	if(def<totDef-1){
	 colsDef = colsDef + ",";
	 argsDef = argsDef + ", NULL";

	 /// Maximum d'element
	 maxElemts = BMAX_2(fdj_def.limites[def].max,
											fdj_def.limites[def+1].max);
	}
 }

 msg = "create table if not exists "
       + tbName
       + "(id integer primary key, "
       + colsDef
       +");";

#if 0 //#ifndef QT_NO_DEBUG
 qDebug() <<msg;
#endif

 isOk = query->exec(msg);

 if(isOk)
 {
  /// Preparer la requete Sql
  colsDef.remove("int");
  QString msg1 = "insert into "
                 +tbName
                 +"(id,"+colsDef+")values(NULL,";

	/// mettre des valeurs en sequence
	for(int line=1;(line <maxElemts+1)&& isOk;line++)
	{
	 QString stValues="";
	 for(int def = 0; (def<totDef) ;def++)
	 {
		int maxItems = fdj_def.limites[def].max;
		//int nbDizaine = floor(maxItems/10)+1;

		/// Boules
		if(line<=maxItems){
		 stValues = stValues + QString::number(line);
		}
		else{
		 stValues = stValues +"NULL";
		}
		stValues = stValues + ",";

		/// Total des boules
		stValues = stValues +"NULL,";

		/// Nb boules pour gagner
		if(line<=fdj_def.limites[def].win+1){
		 stValues = stValues + QString::number(line-1);
		}
		else{
		 stValues = stValues +"NULL";
		}

		if(def < totDef -1)
		 stValues = stValues + ",";
	 }

	 msg = msg1 + stValues + ")";
#if 0 //#ifndef QT_NO_DEBUG
	 qDebug() <<msg;
#endif
	 isOk = query->exec(msg);
	}
 }


 return isOk;
}

bool cFdjData::crt_TblFdj(QString tbl_name,QSqlQuery *query)
{
 bool isOk= true;
 QString msg = "";

 QString tbName = gameLabel[fdj_def.eFdjType]
                  +QString("_")
                  + QString::number(fdj_def.id).rightJustified(3,'0')
                  +QString("_")
                  +tbl_name;

 return isOk;
}

bool cFdjData::crt_TblAna(QString tbl_name,QSqlQuery *query)
{
 bool isOk= true;
 QString msg = "";

 QString tbName = gameLabel[fdj_def.eFdjType]
                  +QString("_")
                  + QString::number(fdj_def.id).rightJustified(3,'0')
                  +QString("_")
                  +tbl_name;

 return isOk;
}
