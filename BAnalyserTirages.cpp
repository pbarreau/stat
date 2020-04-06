#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QMessageBox>
#include <QSqlDatabase>
#include  <QSqlError>

#include "BAnalyserTirages.h"
#include "db_tools.h"
#include "cnp_AvecRepetition.h"

BAnalyserTirages::BAnalyserTirages(stGameConf *pGame)
{
 addr = nullptr;
 QString cnx=pGame->db_ref->cnx;
 QString tbl_tirages = pGame->db_ref->fdj;

 // Etablir connexion a la base
 db_1 = QSqlDatabase::database(cnx);
 if(db_1.isValid()==false){
  QString str_error = db_1.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }
 addr=this; /// memo de cet objet

 /// Verifier si les tables minimales sont presentes
 if(isPresentUsefullTables(pGame, tbl_tirages, cnx)){
  startAnalyse(pGame);
 }
}

BAnalyserTirages * BAnalyserTirages::self()
{
 return addr;
}

bool BAnalyserTirages::isPresentUsefullTables(stGameConf *pGame, QString tbl_tirages, QString cnx)
{
 bool isOk = true;

 typedef struct _stdbMinLstTables{
  QString tbName;
  bool (BAnalyserTirages::*ptrFunc)(stGameConf *pGame, QString tbl_tirages,QSqlQuery *query);
 }stdbMinLstTables;

 stdbMinLstTables lstTable[]={
  {"B_elm", &BAnalyserTirages::mkTblLstElm},
  {"B_cmb", &BAnalyserTirages::mkTblLstCmb},
  {"B_def", &BAnalyserTirages::mkTblGmeDef}
 };
 int totTables = sizeof(lstTable)/sizeof(stdbMinLstTables);


 /// Verifier si on a bien la table des tirages
 if((isOk = DB_Tools::isDbGotTbl(tbl_tirages, cnx)) == false)
 {
  QString str_error = "Missing table : " + tbl_tirages + "in DataBase";
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
 }

 /// Verifier existance des tables
 for(int uneTable=0;(uneTable<totTables) && isOk;uneTable++)
 {
  /// Nom de la table
  QString tbName = lstTable[uneTable].tbName;
  QSqlQuery query(db_1);

	/// Verifier si la table est deja cree
	if(isOk && (DB_Tools::isDbGotTbl(tbName, cnx)==false)){
	 /// Fonction de traitement de la creation
	 isOk=(this->*(lstTable[uneTable].ptrFunc))(pGame, tbName, &query);
	}
	/// Analyser le retour de traitement
	if(!isOk){
	 QString msg = "Erreur creation table : " + tbName;
	 DB_Tools::DisplayError(tbName,&query,msg);
	}
 }

 return isOk;
}

void BAnalyserTirages::startAnalyse(stGameConf *pGame)
{
}

bool BAnalyserTirages::mkTblLstElm(stGameConf *pGame, QString tbName,QSqlQuery *query)
{
 /// Creation des listes de reference des noms
 /// des boules et du nombre par zone

 bool isOk = true;

 QString msg = "";

 QString colsDef = "";
 QString argsDef = "";
 QString def_1 = "z%1 int, tz%1 int";
 QString def_2 = ":arg%1, :arg%2";

 int totDef=pGame->znCount;
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
	 argsDef = argsDef + ",";

	 /// Maximum d'element
	 maxElemts = BMAX_2(pGame->limites[def].max,
											pGame->limites[def+1].max);
	}
 }

 msg = "create table if not exists "
       + tbName
       + "(id integer primary key,"
       + colsDef
       +")";

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
		int maxItems = pGame->limites[def].max;

		/// Boules
		if(line<=maxItems){
		 stValues = stValues + QString::number(line);
		}
		else{
		 stValues = stValues +"NULL";
		}
		stValues = stValues + ",";

		/// Nb boules pour gagner
		if(line<=pGame->limites[def].win+1){
		 stValues = stValues + QString::number(line-1);
		}
		else{
		 stValues = stValues +"NULL";
		}

		if(def < totDef -1)
		 stValues = stValues + ",";
	 }
	 msg = msg1 + stValues + ")";
#ifndef QT_NO_DEBUG
	 qDebug() <<msg;
#endif
	 isOk = query->exec(msg);
	}
 }

 return isOk;
}

bool BAnalyserTirages::mkTblLstCmb(stGameConf *pGame, QString tbName,QSqlQuery *query)
{
 bool isOk = true;

 BGnp *combi = new BGnp(pGame, tbName);

 if(combi->self()==nullptr){
  delete combi;
  isOk = false;
 }

 return isOk;
}

bool BAnalyserTirages::mkTblGmeDef(stGameConf *pGame, QString tbName,QSqlQuery *query)
{
 bool isOk= true;
 QString msg = "";

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
  for(int def = 0; (def<pGame->znCount) && isOk;def++)
  {
   query->bindValue(":arg1",pGame->limites[def].min);
   query->bindValue(":arg2",pGame->limites[def].max);
   query->bindValue(":arg3",pGame->limites[def].len);
   query->bindValue(":arg4",pGame->limites[def].win);
   query->bindValue(":arg5",pGame->names[def].abv);
   query->bindValue(":arg6",pGame->names[def].std);

	 /// executer la commande sql
	 isOk = query->exec();
	}
 }

 return isOk;
}
