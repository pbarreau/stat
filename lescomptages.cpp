#ifndef QT_NO_DEBUG
#include <QDebug>
#include <QSqlError>
#endif

#include <QFile>
#include <QDate>

#include <QApplication>
#include <QString>
#include <QFormLayout>
#include <QLineEdit>
#include <QTabWidget>
#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlRecord>
#include <QTextStream>
#include <QVBoxLayout>
#include <QLabel>

#include <QMessageBox>
#include <QFileDialog>

#include <QList>
#include <QPair>

#include "BGrbGenTirages.h"
#include "blineedit.h"
#include "BFpm_2.h"

#include "compter.h"
#include "compter_zones.h"
#include "compter_combinaisons.h"
#include "compter_groupes.h"
#include "compter_barycentre.h"

#include "labelclickable.h"

//#include "delegate.h"
#include "BFlags.h"

#include "lescomptages.h"
#include "cnp_AvecRepetition.h"
#include "db_tools.h"

int BPrevision::total_items = 0;

BPrevision::~BPrevision()
{
 total_items --;
}

void BPrevision::slot_changerTitreZone(QString le_titre)
{
#if 0
    titre[0] = le_titre;
    QString cur_titre = "";

    cur_titre = "Z:"+titre[0]+"C:"+titre[1]+"G:"+titre[2];
#endif


 //selection[0].setText("Z:"+le_titre);
}

BPrevision::BPrevision(stGameConf *pGame, stPrmPrevision *prm)
{
 cur_item = total_items;
 total_items++;
 conf = *prm;

 onGame = conf.gameInfo;

 if(ouvrirBase(pGame, prm->bddStore)==true)
 {
  effectuerTraitement(pGame->eFdjType);
  //dbInUse.close();
 }
}

#if 0
BPrevision::BPrevision(eGame game, eBddUse def, QString stLesTirages)
{
    Q_UNUSED(stLesTirages)

    onGame.type = game;
    if(ouvrirBase(def,game)==true)
    {
        effectuerTraitement(game);
        //dbInUse.close(); /// Autrement les requetes donnent base fermee
    }
}
#endif

QString BPrevision::mk_IdDsk(etFdj type, etDbUsage eDbUsage)
{
 QDate myDate = QDate::currentDate();
 QString toDay = myDate.toString("dd-MM-yyyy");
 QString game = "";

 QFile myFileName;
 QString testName = "";
 QString ext=".sqlite";

 game = gameLabel[type] + QString("_V1_");

 if(eDbUsage==eDbForFdj){
  game = game +toDay+QString("_");

	int counter = 0;
	do{
	 testName = game + QString::number(counter).rightJustified(3,'0')+QString("-");
	 testName = testName + QString::number(cur_item).rightJustified(2,'0')+ ext;
	 myFileName.setFileName(testName);
	 counter = (counter + 1)%999;
	}while(myFileName.exists());
 }

 if(eDbUsage==eDbForCnp){
  testName=game+QString("Ref_Cnp")+ext;
 }


 return testName;
}

QString BPrevision::mk_IdCnx(etFdj type, etDbUsage eDbUsage)
{
 QString msg="cnx_NotSetYet";

 if((type <= eFdjNone) || (type>=eFdjEol)){
  etFdj err = eFdjNone;
  QMessageBox::warning(NULL,"Prevision","Jeu "+gameLabel[err]+" inconnu !!",QMessageBox::Ok);
  QApplication::quit();
 }

 if(eDbUsage==eDbForCnp){
  msg=QString("cnx_V1_")+gameLabel[type]+QString("-")+QString("Ref_Cnp");
 }

 if(eDbUsage==eDbForFdj){
  msg=QString("cnx_V1_")+gameLabel[type];
 }

 msg = msg + QString("-")+QString::number(cur_item).rightJustified(2,'0');
 return (msg);
}

bool BPrevision::ouvrirBase(stGameConf *pConf, etDb cible)
{
 ;
 etFdj game = pConf->eFdjType;

 bool isOk = true;

 cnx_db_1 = mk_IdCnx(game,eDbForFdj);
 db_1 = QSqlDatabase::addDatabase("QSQLITE",cnx_db_1);

 QString mabase = "";

 switch(cible)
 {
  case eDbRam:
   mabase = ":memory:";
   break;

	case eDbDsk:
	default:
	 /// Reutiliser existant ?
	 if(conf.gameInfo.bUseMadeBdd){
		QString myTitle = "Selectionnner un fichier " + gameLabel[game];
		QString myFilter = gameLabel[game]+"_V1*.sqlite";
		mabase = QFileDialog::getOpenFileName(nullptr,myTitle,".",myFilter);
	 }
	 break;
 }

 if(mabase.isEmpty()){
  /// pas de selection de base pre remplie
  conf.gameInfo.bUseMadeBdd = false;
  mabase = mk_IdDsk(game,eDbForFdj);
 }

 /// definition de la base pour ce calcul
 db_1.setDatabaseName(mabase);

 // Open database
 isOk = db_1.open();
#if (SET_DBG_LIVE&&SET_DBG_LEV1)
 QMessageBox::information(NULL, "Open", "step 1->"+QString::number(isOk),QMessageBox::Yes);
#endif

 if(isOk)
  isOk = OPtimiseAccesBase();


 return isOk;
}

bool BPrevision::OPtimiseAccesBase(void)
{
 bool isOk = true;
 QSqlQuery query(db_1);
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

void BPrevision::effectuerTraitement(etFdj game)
{
 QString source = "";//C_TBL_3;
 if(conf.gameInfo.eTirType==etTirType::eTirFdj){
  conf.tblFdj_dta = "B_fdj";
  source = conf.tblFdj_dta;
 }
 else {
  conf.tblUsr_dta = "Usr_001";
  source=conf.tblUsr_dta;
 }


 definirConstantesDuJeu(game);

 /// Liste des recherches
 int nbZone = onGame.znCount;
 slFlt = new  QStringList* [nbZone] ;
 for (int zn=0;zn < nbZone;zn++ )
 {
  slFlt[zn] = CreateFilterForData(zn);
 }


 if(conf.gameInfo.bUseMadeBdd == false){

  creerTablesDeLaBase();

	conf.gameInfo = onGame;
	analyserTirages(conf,source, onGame);
 }
 else {
  conf.gameInfo = onGame;
  analyserTirages(conf,source, onGame);
 }
 int b;
}

stGameConf * BPrevision::definirConstantesDuJeu(etFdj game)
{
 /// Pour l'instant en loto ou en euro il y a 2 'zones'
 /// une pour les boules
 /// une pour les etoiles
 onGame.znCount = 2; /// boules + etoiles
 //onGame.type = game;
 //onGame.from = eFdj;

 switch(game)
 {
  case eFdjLoto:
   onGame.limites = new stParam_1 [onGame.znCount];
   onGame.names = new stParam_2 [onGame.znCount];

	 /// z1 : boules
	 onGame.limites[0].min=1;
	 onGame.limites[0].max=49;
	 onGame.limites[0].len=5;
	 onGame.limites[0].win=5;
	 onGame.limites[0].usr=0;
	 onGame.names[0].std = "Boules";
	 onGame.names[0].abv = "b";

	 /// z2 : etoiles
	 onGame.limites[1].min=1;
	 onGame.limites[1].max=10;
	 onGame.limites[1].len=1;
	 onGame.limites[1].win=1;
	 onGame.limites[1].usr=0;
	 onGame.names[1].std = "Etoiles";
	 onGame.names[1].abv = "e";
	 break;

	case eFdjEuro:
	 onGame.limites = new stParam_1 [onGame.znCount];
	 onGame.names = new stParam_2 [onGame.znCount];

	 /// boules
	 onGame.limites[0].min=1;
	 onGame.limites[0].max=50;
	 onGame.limites[0].len=5;
	 onGame.limites[0].win=5;
	 onGame.limites[0].usr=0;
	 onGame.names[0].std = "Boules";
	 onGame.names[0].abv = "b";

	 /// etoiles
	 onGame.limites[1].min=1;
	 onGame.limites[1].max=12;
	 onGame.limites[1].len=2;
	 onGame.limites[1].win=2;
	 onGame.limites[1].usr=0;
	 onGame.names[1].std = "Etoiles";
	 onGame.names[1].abv = "e";
	 break;

	default:
	 onGame.znCount = 0; /// boules + etoiles
	 onGame.limites = NULL;
	 onGame.names = NULL;
	 break;
 }

 /// config effectuee
 return(&onGame);
}

bool BPrevision::creerTablesDeLaBase(void)
{
 bool isOk= true;
 QSqlQuery q(db_1);
 QString msg = "";
 QString tbName="";

 QString chemin = db_1.databaseName();

 stCreateTable creerTables[]={
  {C_TBL_3,&BPrevision::f3},   /// Table des tirages
  {"Filtres",&BPrevision::f1_TbFiltre},   /// Table des nom des zones et abregees
  {C_TBL_1,&BPrevision::f1},   /// Table des nom des zones et abregees
  {C_TBL_2,&BPrevision::f2},   /// Liste des boules par zone
  {T_CMB,&BPrevision::f4},    /// Table des combinaisons
  {T_ANA,&BPrevision::f5},    /// Table des Analyses
  {C_TBL_6,&BPrevision::f6},    /// Selection utilisateur
  {U_CMB,&BPrevision::f6}
 };
#if 0
,    /// Selection utilisateur
  {U_GRP,&BPrevision::f6},    /// Selection utilisateur
  {C_TBL_A,&BPrevision::f6}    /// Selection utilisateur

#endif

 int nbACreer = 0;
 if(conf.gameInfo.bUseMadeBdd){
  nbACreer=1;
 }
 else {
  nbACreer = sizeof(creerTables)/sizeof(stCreateTable);
 }

 for(int uneTable=0;(uneTable<nbACreer) && isOk;uneTable++)
 {
  msg="";
  /// Nom de la table
  tbName = creerTables[uneTable].tbDef;

	/// Fonction de traitement de la creation
	isOk=(this->*(creerTables[uneTable].pFuncInit))(tbName,&q);

 }

 /// Analyser le retour de traitement
 if(!isOk){
  //un message d'information
  DB_Tools::DisplayError(tbName,&q,msg);
  //QMessageBox::critical(0, tbName, "Erreur traitement !",QMessageBox::Yes);
#ifndef QT_NO_DEBUG
  qDebug() <<q.lastError().text();
#endif
  QApplication::quit();
 }

 return isOk;
}

bool BPrevision::f1_TbFiltre(QString tbName,QSqlQuery *query)
{
 bool isOk= true;
 QString msg = "";

 /// Preparation de la suppression/modification de f6
 msg =  "create table "+tbName
       +" (id Integer primary key, zne int, typ int, lgn int, col int, val int, pri int, flt int);";


 isOk = query->exec(msg);
 return isOk;
}

/// Creation de la table donnant les carateristique du jeu
bool BPrevision::f1(QString tbName,QSqlQuery *query)
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
  for(int def = 0; (def<onGame.znCount) && isOk;def++)
  {
   query->bindValue(":arg1",onGame.limites[def].min);
   query->bindValue(":arg2",onGame.limites[def].max);
   query->bindValue(":arg3",onGame.limites[def].len);
   query->bindValue(":arg4",onGame.limites[def].win);
   query->bindValue(":arg5",onGame.names[def].abv);
   query->bindValue(":arg6",onGame.names[def].std);

	 /// executer la commande sql
	 isOk = query->exec();
	}
 }

 if(!isOk)
 {
  QString ErrLoc = "cLesComptages::f1";
  DB_Tools::DisplayError(ErrLoc,query,msg);
 }

 return isOk;
}

/// Creation des listes de reference des noms
/// des boules et du nombre par zone
bool BPrevision::f2(QString tbName,QSqlQuery *query)
{
 bool isOk= true;
 QString msg = "";

 QString colsDef = "";
 QString argsDef = "";
 QString def_1 = "z%1 int, tz%1 int";
 QString def_2 = ":arg%1, :arg%2";

 int totDef=onGame.znCount;
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
	 maxElemts = BMAX_2(onGame.limites[def].max,
											onGame.limites[def+1].max);
	}
 }

 msg = "create table if not exists "
       + tbName
       + "(id integer primary key,"
       + colsDef
       +");";

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
		int maxItems = onGame.limites[def].max;
		//int nbDizaine = floor(maxItems/10)+1;

		/// Boules
		if(line<=maxItems){
		 stValues = stValues + QString::number(line);
		}
		else{
		 stValues = stValues +"NULL";
		}
		stValues = stValues + ",";

		/// Nb boules pour gagner
		if(line<=onGame.limites[def].win+1){
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

 if(!isOk)
 {
  QString ErrLoc = "cLesComptages::f2";
  DB_Tools::DisplayError(ErrLoc,query,msg);
 }

 return isOk;
}

bool BPrevision::f3(QString tbName,QSqlQuery *query)
{
 bool isOk= true;
 QString msg = "";
 QString colsDef = "";
 QString cAsDef = ""; /// column as def

 /// Cette table contient tous les tirages
 QString useName = "B_" + tbName;
 tblTirages =  useName;

 if(conf.gameInfo.bUseMadeBdd){
  msg="drop table if exists " + useName;
  isOk = query->exec(msg);
 }


 int totDef=onGame.znCount;
 for(int def = 0; (def<totDef) && isOk;def++)
 {
  QString ref = onGame.names[def].abv+"%1 int";
  QString ref_2 = "printf('%02d',"+onGame.names[def].abv+"%1)as "+onGame.names[def].abv+"%1";
  int totElm = onGame.limites[def].len;
  for(int elm=0;elm<totElm;elm++){
   colsDef=colsDef + ref.arg(elm+1);
   cAsDef = cAsDef + ref_2.arg(elm+1);
   if(elm<totElm-1){
    colsDef = colsDef + ",";
    cAsDef = cAsDef + ",";
   }
  }
  if(def<totDef-1){
   colsDef = colsDef + ",";
   cAsDef = cAsDef + ",";
  }
 }

 /// J: jour ie Lundi...
 /// D: date xx/yy/nnnn
 /// creation d'une table temporaire et d'une table destination
 QString tables[]={"tmp_"+useName,useName};
 for(int i=0; i<2 && isOk;i++)
 {
  QString stKeyOn = "";
  if (i==1)
   stKeyOn = "id integer primary key,";

	msg = "create table if not exists "
				+ tables[i]
				+ "("+ stKeyOn +"D text, J text,"
				+ colsDef
				+",file int);";

#ifndef QT_NO_DEBUG
	qDebug() <<msg;
#endif

  isOk = query->exec(msg);
 }

 /// Les tables sont presentes maintenant
 if(isOk)
 {
  /// mettre les infos brut dans la table temporaire
  isOk = chargerDonneesFdjeux(tables[0]);

	if(isOk)
	{
	 /// mettre les infos triees dans la table de reference
	 colsDef.remove("int");
	 /// trier les resultats pour la table finale
	 msg = "insert into "
				 + tables[1] + " "
				 + "select NULL,"
				 + "substr(src.D,-2,2)||'/'||substr(src.D,6,2)||'/'||substr(src.D,1,4) as D,"
				 + "src.J,"
				 + cAsDef + ",file from("
				 + tables[0] + " as src)order by date(src.D) desc,src.J desc";
#ifndef QT_NO_DEBUG
	 qDebug() <<msg;
#endif

	 isOk = query->exec(msg);

	 if(isOk){
		/// supprimer la table tremporaire
		msg = "drop table if exists " + tables[0];
		isOk = query->exec(msg);
	 }

  }
 }

 if(!isOk)
 {
  QString ErrLoc = "cLesComptages::f3";
  DB_Tools::DisplayError(ErrLoc,query,msg);
 }

 return isOk;
}

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
bool BPrevision::f4(QString tbName,QSqlQuery *query)
{
 Q_UNUSED(query)

 bool isOk = true;

#if 0
QString msg = "";
QString ens_small = "with small as (select jour_next.* from (SELECT tb1.* from B_fdj as tb1  "
										 "where "
										 "( "
										 "16 in (tb1.b1, tb1.b2, tb1.b3, tb1.b4, tb1.b5) AND "
										 "41 in (tb1.b1, tb1.b2, tb1.b3, tb1.b4, tb1.b5) "
										 ")) as jour_ref "
										 ",  ( SELECT tb1.* from B_fdj as tb1) as jour_next "
										 "where (jour_next.id = jour_ref.id -1) "
										 ") "
										 "select * from small";

 do_SqlCnpCount();
 //sql_CnpCountFromId(1,2);
 QString tmp_sql = sql_CnpCountUplet(2,"Cnp_49_2",ens_small);

 msg = "create table if not exists titi as "
       +tmp_sql;

#ifndef QT_NO_DEBUG
 qDebug() <<tmp_sql;
 qDebug() <<msg;
#endif

 isOk = query->exec(msg);

 sql_CnpCountFromId(1,3);
 sql_CnpCountFromId(1,4);
#endif

 //// TMP: isOk = do_SqlCnpCount(); //do_SqlCnpPrepare();

 int nbZone = onGame.znCount;

 QString useName = "B_" + tbName;

 for (int zn=0;(zn < nbZone) && isOk;zn++ )
 {
  if(onGame.limites[zn].win>2){
   isOk = TraitementCodeVueCombi(zn);

	 if(isOk)
		isOk = TraitementCodeTblCombi(useName,zn);
	}
	else
	{
	 int n = onGame.limites[zn].max;
	 int p = onGame.limites[zn].win;
	 QString tbName = useName+ "_z"+QString::number(zn+1);
	 // calculer les combinaisons avec repetition
	 BCnp *a = new BCnp(n,p,db_1.connectionName());
	 tbName = a->getDbTblName();
	 if(tbName.isEmpty()){
		QMessageBox::information(NULL, "Pgm", "tbName is null" ,QMessageBox::Yes);
		QApplication::quit();
	 }

	 isOk = TraitementCodeTblCombi_2(useName,tbName,zn);
	}
 }

 if(!isOk)
 {
  QString ErrLoc = "f4:";
  DB_Tools::DisplayError(ErrLoc,NULL,"");
 }

 return isOk;
}

bool BPrevision::f5(QString tbName,QSqlQuery *query)
{
 Q_UNUSED(query)

 bool isOk = true;
 int nbZone = onGame.znCount;
 //slFlt = new  QStringList* [nbZone] ; A supprimer
 QString tbUse = "";
 QString source = "";
 QString destination ="";

 destination = "B_"+tbName;
 source = tblTirages;

 for (int zn=0;(zn < nbZone) && isOk;zn++ )
 {
  isOk = AnalyserEnsembleTirage(source,onGame, zn);
  if(isOk)
   isOk = FaireTableauSynthese(destination,onGame,zn);
 }

 if(!isOk)
 {
  QString ErrLoc = "f5:";
  DB_Tools::DisplayError(ErrLoc,NULL,"");
 }

 return isOk;
}

bool BPrevision::f6(QString tbName,QSqlQuery *query)
{
 bool isOk = true;




 //------------------
 QString st_sqldf = ""; /// sql definition
 QString st_table = "";

 // Creation des tables permettant la sauvegarde des selections
 // pour creation de filtres
 int nb_zone = onGame.znCount;

 for(int zone=0;(zone<nb_zone)&& isOk;zone++)
 {
  st_table = tbName + "_z"+QString::number(zone+1);
  st_sqldf =  "create table "+st_table+" (id Integer primary key, val int, p int, f int);";
  isOk = query->exec(st_sqldf);
 }


 if(!isOk)
 {
  QString ErrLoc = "f6:";
  DB_Tools::DisplayError(ErrLoc,NULL,"");
 }

 return isOk;
}

bool BPrevision::FaireTableauSynthese(QString tblIn, const stGameConf &onGame,int zn)
{
 bool isOk = true;
 QString msg = "";
 QSqlQuery query(db_1);
 QString stDefBoules = C_TBL_2;
 QString prvName = "";
 QString curName  ="";

 QString TblCompact ="";
 QString tblToUse = "";

 if(tblIn == "B_ana"){
  tblToUse = tblIn + "_z"+QString::number(zn+1);
  TblCompact = T_GRP;
 }
 else{
  tblToUse = tblIn+ "_z"+QString::number(zn+1);;
  TblCompact = "r_"+tblIn+"_grp";
 }

 QString stCurTable = tblToUse;

 /// Verifier si des tables existent deja
 if(SupprimerVueIntermediaires())
 {
  /// Plus de table intermediaire commencer
  curName = "vt_0";
  msg = "create view if not exists "
        +curName+" as select cast(Choix.tz"
        +QString::number(zn+1)+ " as int) as Nb"
        +" from("+stDefBoules+")as Choix where(Choix.tz"
        +QString::number(zn+1)+ " is not null)";
#ifndef QT_NO_DEBUG
  qDebug() << msg;
#endif

	isOk = query.exec(msg);
	QStringList *slst=&slFlt[zn][0];

	int nbCols = slst[1].size();
	curName = "vt_1";
	QString stGenre = "view";
	for(int loop = 0; (loop < nbCols)&& isOk; loop ++){
	 prvName ="vt_"+QString::number(loop);
	 msg = "create "+stGenre+" if not exists "
				 + curName
				 +" as select tbleft.*, cast((case when count(tbRight.id)!=0 then count(tbRight.id) end)as int) as "
				 +slst[1].at(loop)
				 + " from("+prvName+") as tbLeft "
				 +"left join ("
				 +stCurTable
				 +") as tbRight on (tbLeft.Nb = tbRight."
				 +slst[1].at(loop)+")group by tbLeft.Nb";
#ifndef QT_NO_DEBUG
	 qDebug() << msg;
#endif
	 isOk = query.exec(msg);
	 if(loop<nbCols-1)
		curName ="vt_"+QString::number(loop+2);
	}
	/// Rajouter a la fin une colonne pour fitrage
	if(isOk){
	 msg = "create table if not exists "+TblCompact+"_z"
				 + QString::number(zn+1)
				 +" as select tb1.* from ("+curName+") as tb1";
#ifndef QT_NO_DEBUG
	 qDebug() << msg;
#endif

	 isOk = query.exec(msg);

	 /// Supprimer vues intermediaire
	 if(isOk){
		isOk = SupprimerVueIntermediaires();
	 }
	}
 }

 if(!isOk)
 {
  QString ErrLoc = "FaireTableauSynthese:";
  DB_Tools::DisplayError(ErrLoc,&query,"");
 }

 return isOk;
}
bool BPrevision::TraitementCodeVueCombi(int zn)
{
 bool isOk = true;
 QSqlQuery query(db_1);
 QString msg = "";
 QString ref_1 = "";

 QString viewCode[]=
  {
   "drop view if exists tbr%1;",
   "create view if not exists tbr%1 as select tbChoix.tz%1 as "
    +onGame.names[zn].abv+ " "
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
    ref_1 = C_TBL_2;
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

bool BPrevision::TraitementCodeTblCombi(QString tbName,int zn)
{
 bool isOk = true;
 QSqlQuery query(db_1);
 QString msg = "";

 QString tblCode[]=
  {
   "drop table if exists "+tbName+"_z%1;",
   "create table if not exists "+tbName+"_z%1 (id integer primary key,%2);",
   "insert into "+tbName+"_z%1 select NULL,%2 from (%3) where(%4="
    +QString::number(+onGame.limites[zn].win)+");"
  };
 int argTblCount[]={1,2,4};

 QString ref_1 = "";
 QString ref_2 = "";
 QString ref_3 = "";
 QString ref_4 = "";
 QString ref_5 = "";
 int nbLgnCode= 0;


 /// traitement creation table en fonction 10zaine
 int lenZn = floor(onGame.limites[zn].max/10)+1;
 ref_1="t%1."+onGame.names[zn].abv+" as "+onGame.names[zn].abv+"%1";
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
                          +onGame.names[zn].abv
                          +"\\s+as\\s+"),"");
    ref_1.replace(",", " int,");
    ref_1=ref_1 + "tip text, poids real";
    msg = tblCode[lgnCode].arg(zn+1).arg(ref_1);
   }
   break;
   case 4:{
    ref_1="%d";
    ref_2="t%1."+onGame.names[zn].abv;
    ref_3="(%1*t%2."+onGame.names[zn].abv+")";
    ref_4="tbr%1 as t%2";
    ref_5=onGame.names[zn].abv+"%1";
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
bool BPrevision::TraitementCodeTblCombi_2(QString tbName, QString tbCnp, int zn)
{
 bool isOk = true;
 QSqlQuery query(db_1);
 QString msg = "";

 msg = "drop table if exists "+tbName+"_z"+QString::number(zn+1);
 isOk = query.exec(msg);
#ifndef QT_NO_DEBUG
 qDebug() << msg;
#endif

 if(isOk){
  /// traitement creation table
  int lenZn = onGame.limites[zn].len;
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
	ref_1 = ref_1.replace("c",onGame.names[zn].abv) + +",tip text, poids real" ;
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

bool BPrevision::chargerDonneesFdjeux(QString destTable)
{
 bool isOk= true;

 stFdjData *LesFichiers;
 int nbelemt = 0;
 int fId = 0;


 /// avec les differentes version des jeux
 /// le format des fichiers repertoriant les resultats
 /// a change
 stZnDef p1Zn[] =
  {
   {4,5,1,50},
   {9,2,1,10}
  };
 stZnDef p2Zn[] =
  {
   {4,5,1,49},
   {9,1,1,10}
  };

 stZnDef p3Zn[] =
  {
   {4,5,1,50},
   {9,2,1,11}
  };
 stZnDef p4Zn[] =
  {
   {5,5,1,50},
   {10,2,1,12}
  };

 /// Liste des fichiers pour Euromillions
 fId = 0;
 stFdjData euroMillions[]=
  {
   {"euromillions_4.csv",fId++,
    {false,2,1,2,&p4Zn[0]}
   },
   {"euromillions_3.csv",fId++,
    {false,2,1,2,&p3Zn[0]}
   },
   {"euromillions_2.csv",fId++,
    {false,2,1,2,&p3Zn[0]}
   },
   {"euromillions.csv",fId++,
    {false,2,1,2,&p1Zn[0]}
   }
  };

 /// Liste des fichiers pour loto
 fId = 0;
 stFdjData loto[]=
  {
   {"loto2017.csv",fId++,
    {false,2,1,2,&p2Zn[0]}
   },
   {"superloto2017.csv",fId++,
    {false,2,1,2,&p2Zn[0]}
   },
   {"lotonoel2017.csv",fId++,
    {false,2,1,2,&p2Zn[0]}
   },
   {"nouveau_superloto.csv",fId++,
    {false,2,1,2,&p2Zn[0]}
   },
   {"nouveau_loto.csv",fId++,
    {false,2,1,2,&p2Zn[0]}
   },
   {"loto_201902.csv",fId++,
    {false,2,1,2,&p2Zn[0]}
   },
   {"loto_201911.csv",fId++,
    {false,2,1,2,&p2Zn[0]}
   }
  };

 if(onGame.eFdjType == eFdjEuro){
  nbelemt = sizeof(euroMillions)/sizeof(stFdjData);
  LesFichiers = euroMillions;
 }
 else
 {
  nbelemt = sizeof(loto)/sizeof(stFdjData);
  LesFichiers = loto;
 }

 // Lectures des fichiers de la Fd jeux
 while((isOk == true) && (nbelemt>0))
 {
  isOk = LireLesTirages(destTable, &LesFichiers[nbelemt-1]);
  nbelemt--;
 };



 return isOk;
}

bool BPrevision::LireLesTirages(QString tblName, stFdjData *def)
{
 bool isOk= true;
 QSqlQuery query(db_1);

 QString fileName_2 = def->fname;
 QFile fichier(fileName_2);

 // On ouvre notre fichier en lecture seule et on verifie l'ouverture
 if (!fichier.open(QIODevice::ReadOnly | QIODevice::Text))
 {
  QMessageBox::critical(nullptr, "LireLesTirages", "Erreur chargement !:\n"+fileName_2,QMessageBox::Yes);
  return false;
 }

 /// Variables traitement
 QTextStream flux(&fichier);
 QString ligne = "";
 QStringList list1;
 QString reqCols = "";
 QString reqValues = "";
 QString data = "";
 QString msg = "";
 stErr2 retErr;

 // --- DEBUT ANALYSE DU FICHIER
 // Passer la premiere ligne
 ligne = flux.readLine();

 // Analyse des suivantes
 int nb_lignes=0;
 while((! flux.atEnd() )&& (isOk == true))
 {
  ligne = flux.readLine();
  nb_lignes++;
  reqCols = "";
  reqValues = "";

	//traitement de la ligne
	list1 = ligne.split(";");

	// Recuperation du date_tirage (D)
	data = DateAnormer(list1.at(2));
	// Presentation de la date
	reqCols = reqCols + "D,";
	reqValues = reqValues + "'"
							+ data+ "',";

	// Recuperation et verification du jour (J) en fonction de la date
	data = JourFromDate(data, list1.at(1),&retErr);
	if(retErr.status == false)
	{
	 msg = retErr.msg;
	 msg = "Fic:"+fileName_2+",lg:"+QString::number(nb_lignes-1)+"\n"+msg;
	 QMessageBox::critical(nullptr, "cLesComptages::LireLesTirages", msg,QMessageBox::Yes);
	 return false;
	}
	reqCols = reqCols + "J,";
	reqValues = reqValues + "'"+data + "',";

	// Recuperation des boules
	int max_zone = onGame.znCount;
	for(int zone=0;zone< max_zone;zone++)
	{
	 int maxValZone = def->param.pZn[zone].max;
	 int minValZone = def->param.pZn[zone].min;
	 int maxElmZone = def->param.pZn[zone].len;

	 for(int ElmZone=0;ElmZone < maxElmZone;ElmZone++)
	 {
		// Recuperation de la valeur
		int val1 = list1.at(def->param.pZn[zone].start+ElmZone).toInt();

		// verification coherence
		if((val1 >= minValZone)
				&&
				(val1 <=maxValZone))
		{
		 /// On rajoute a Req values
		 reqCols = reqCols+onGame.names[zone].abv+QString::number(ElmZone+1);
		 reqValues = reqValues + QString::number(val1);
		}
		else
		{
		 /// Bug pour la valeur lue
		 msg = "Fic:"+fileName_2+",lg:"+QString::number(nb_lignes-1);
		 msg= msg +"\nzn:"+QString::number(zone)+",el:"+QString::number(ElmZone);
		 msg= msg +",val:"+QString::number(val1);
		 QMessageBox::critical(0, "LireLesTirages", msg,QMessageBox::Yes);
		 return false;
		}

		/// tous les elements sont vus ?
		if(ElmZone < maxElmZone-1){
		 reqCols = reqCols + ",";
		 reqValues = reqValues + ",";
		}
	 }

	 /// voir si passage a nouvelle zone
	 if(zone< max_zone-1){
		reqCols = reqCols + ",";
		reqValues = reqValues + ",";
	 }
	}
	/// Toutes les zones sont faites, ecrire dans la base
	msg = "insert into "
				+tblName+"("
				+reqCols+",file)values("
				+ reqValues +","+QString::number(def->id)
				+ ")";
#ifndef QT_NO_DEBUG
	qDebug() <<msg;
#endif
	isOk = query.exec(msg);

 }  /// Fin while


 if(!isOk)
 {
  QString ErrLoc = "cLesComptages::LireLesTirages";
  DB_Tools::DisplayError(ErrLoc,&query,msg);
 }

 return isOk;
}

QString BPrevision::DateAnormer(QString input)
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
  if(input.contains("/"))
  {
   // fdj euro million v2 -> JJ/MM/AA
   QStringList tmp = input.split("/");
   ladate = "20" + tmp.at(2) + "-"
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
 }
 return ladate;
}

QString BPrevision::JourFromDate(QString LaDate, QString verif, stErr2 *retErr)
{
 // http://algor.chez.com/date/date.htm
 QString tab[] = {"MARDI","MERCREDI","JEUDI","VENDREDI","SAMEDI","DIMANCHE","LUNDI"};
 QStringList tmp = LaDate.split("-");
 QString retval = "";

 int anne = tmp.at(0).toInt();
 int mois = tmp.at(1).toInt();
 int date = tmp.at(2).toInt();

 retErr->status = true;
 retErr->msg = "Ok";

 int JS = 0;
 double JD = 0.0;
 double  s = 0.0;

 if(mois < 3)
 {
  mois+=12;
  anne--;
 }

 s = anne/100;
 JD = (1720996.5 - s) + (s / 4) + floor(365.25*anne) + floor(30.6001*(mois+1)) + date;
 JD = JD - floor(JD/7)*7;

 JS = (int)floor(JD)%7;

 retval = tab [JS];

 if(retval.left(2) != verif.trimmed().left(2))
 {
  retErr->status = false;
  retErr->msg = "Err:JourFromDate->" + LaDate+"\n"+retval+" != "+verif.trimmed();
 }

 return retval;
}

void BPrevision::analyserTirages(stPrmPrevision calcul,QString source,const stGameConf &config)
{
 QWidget * Resultats = new QWidget;
 QTabWidget *tab_Top = new QTabWidget;


 c1 = new BCountElem(config,source,db_1,Resultats);
 connect(c1,SIGNAL(sig_TitleReady(QString)),this,SLOT(slot_changerTitreZone(QString)));
 /// transfert vers SyntheseGenerale
 connect(c1,
         SIGNAL(sig_isClickedOnBall(QModelIndex)),
         this,
         SLOT(slot_emitThatClickedBall(QModelIndex)));


#if 0
 /// greffon pour calculer barycentre des tirages
 stNeedsOfBary param;
 param.db = db_1;
 param.ncx = db_1.connectionName();
 param.tbl_in=source;
 if(source=="B_fdj"){
  param.tbl_ana = source+tr("_ana_z1");
 }else
 {
  ///REM:param.tbl_ana = "U_"+monJeu.tblUsr_dta+"_ana_z1";
  param.tbl_ana = monJeu.tblUsr_dta+"_ana_z1";
 }
 param.tbl_flt = tr("U_b_z1"); /// source+tr("_flt_z1");
 param.pDef = onGame;
 param.origine = this;
 c= new CBaryCentre(param);
#endif

 c2 = new BCountComb(config,source,db_1);
 c3 = new BCountGroup(config,source,slFlt,db_1);



 QGridLayout **pConteneur = new QGridLayout *[4];
 QWidget **pMonTmpWidget = new QWidget * [4];

 for(int i = 0; i< 4;i++)
 {
  QGridLayout * grd_tmp = new QGridLayout;
  pConteneur[i] = grd_tmp;

	QWidget * wid_tmp = new QWidget;
	pMonTmpWidget [i] = wid_tmp;
 }
 pConteneur[0]->addWidget(c1,1,0);
 //pConteneur[1]->addWidget(c,1,0);
 pConteneur[2]->addWidget(c2,1,0);
 pConteneur[3]->addWidget(c3,1,0);

 pMonTmpWidget[0]->setLayout(pConteneur[0]);
 pMonTmpWidget[1]->setLayout(pConteneur[1]);
 pMonTmpWidget[2]->setLayout(pConteneur[2]);
 pMonTmpWidget[3]->setLayout(pConteneur[3]);

 tab_Top->addTab(pMonTmpWidget[0],tr("Zones"));
 tab_Top->addTab(pMonTmpWidget[1],tr("Barycentre"));
 tab_Top->addTab(pMonTmpWidget[2],tr("Combinaisons"));
 tab_Top->addTab(pMonTmpWidget[3],tr("Groupes"));

 QGridLayout *tmp_layout = new QGridLayout;
 int i = 0;

 QString msg = QString("Selection : %1 sur %2");
 QString s_sel = QString::number(0).rightJustified(2,'0');
 QString s_max = QString::number(MAX_CHOIX_BOULES).rightJustified(2,'0');
 msg = msg.arg(s_sel).arg(s_max);

 LabelClickable *tmp_lab = c1->getLabPriority();
 tmp_lab->setText(msg);

 tmp_layout->addWidget(tmp_lab,i,0);
 i++;
 tmp_layout->addWidget(tab_Top,i,0);

 /*
    QString clef[]={"Z:","C:","G:"};
    int i = 0;
    for(i; i< 3; i++)
    {
        selection[i].setText(clef[i]+"aucun");
        tmp_layout->addWidget(&selection[i],i,0);
    }
*/

#if 0
    connect( selection, SIGNAL( clicked(QString)) ,
             this, SLOT( slot_RazSelection(QString) ) );
#endif



 /// ----------------
 Resultats->setLayout(tmp_layout);
 Resultats->setWindowTitle(source);
 Resultats->show();
}

BCountBrc* BPrevision::getC0()
{
 return c;
}
BCountElem* BPrevision::getC1()
{
 return c1;
}
BCountComb* BPrevision::getC2()
{
 return c2;
}
BCountGroup* BPrevision::getC3()
{
 return c3;
}

QString BPrevision::CreateSqlFrom(QString tbl, int val_p)
{
 bool isOk = true;
 QString msg = "";
 QString sql = "";
 QSqlQuery query(db_1);

 int nblgn = 0;

 msg = "select count (*) from "+ tbl;
 isOk = query.exec(msg);

 if(isOk && query.first()){
  nblgn = query.value(0).toInt();
 }

 msg = "select * from "+ tbl;
 isOk = query.exec(msg);
 if(nblgn && isOk && query.first()){
  int nbcol = query.record().count();
  int culgn = 1;
  sql="";
  QString item = "(e2.b%1 in(e1.b1,e1.b2,e1.b3,e1.b4,e1.b5))";
  do{
   msg="";
   for (int i=2;i<nbcol;i++) {
    msg = msg + item.arg(i-1);
    if(i<(nbcol-1)){
     msg=msg+ "AND";
    }
   }

   sql=sql + "("+ msg + ")";

	 if(culgn<nblgn){
		sql=sql +  "OR";
	 }

	 culgn++;
	}while(query.next());

	msg = "select e2.*, e1.* from B_fdj as e2 "
				"left JOIN B_fdj as e1 on e2.id != e1.id "
				"where(" + sql + ")";
 }

#ifndef QT_NO_DEBUG
 qDebug() <<sql;
 qDebug() <<msg;
#endif

 return sql;
}

QString BPrevision::FN2_getFieldsFromZone(int zn, QString alias)
{
 int len_zn = onGame.limites[zn].len;

 QString use_alias = "";

 if(alias.size()){
  use_alias = alias+".";
 }
 QString ref = use_alias+onGame.names[zn].abv+"%1";
 QString st_items = "";
 for(int i=0;i<len_zn;i++){
  st_items = st_items + ref.arg(i+1);
  if(i<(len_zn-1)){
   st_items=st_items+QString(",");
  }
 }
 return   st_items;
}

QString BPrevision::sql_CnpMkUplet(int nb, QString col, QString tbl_in)
{

 // Selection numeros boule de la zone
 QString tbl_ref = "("+tbl_in+")";
 QString lst_0 =  "(select t1."+col+" as b from "+tbl_ref+" as t1), ";

 QString ref_1 = "lst_%1 as (select * from lst_0)";
 QString ref_2 = "lst_%1.b as b%1";
 QString ref_3 = "lst_%1.b";
 QString ref_4 = "(lst_%1.b<lst_%2.b)";

 QString str_in_1 = "";
 QString str_in_2 = "";
 QString str_in_3 = "";
 QString str_in_4 = "";

 for(int i = 1; i<=nb; i++){
  str_in_1 = str_in_1 + ref_1.arg(i);
  str_in_2 = str_in_2 + ref_2.arg(i);
  str_in_3 = str_in_3 + ref_3.arg(i);

	if(i<nb){
	 str_in_1 = str_in_1 + ", ";
	 str_in_2 = str_in_2 + ", ";
	 str_in_3 = str_in_3 + ", ";

	 str_in_4 = str_in_4 + ref_4.arg(i).arg(i+1);
	 if(i<nb-1){
		str_in_4 = str_in_4 + "and";
	 }
	}
 }

 if(str_in_4.size()){
  str_in_4 = " where("
             +str_in_4
             +")";
 }
 QString str_from = str_in_3;

 QString sql_cnp = "with lst_0 as "
                   +lst_0
                   +str_in_1
                   +", lst_R as (select "
                   +str_in_2
                   +" from "
                   +str_from.remove(".b")
                   + str_in_4
                   + " order by "
                   + str_in_3
                   +")" ;

#ifndef QT_NO_DEBUG
 qDebug() <<sql_cnp;
#endif


 return sql_cnp;
}

QString BPrevision::sql_CnpCountFromId(int tir_id, int uplet)
{
 QString msg = "";
 int zn = 0;

 QString tbl = "B_upl_" +QString::number(uplet)+ "_z"+QString::number(zn+1);

 QString colNames = FN2_getFieldsFromZone(zn,"t2");
 QString tab_1 = "B_elm";
 QString tab_2 = "B_fdj";
 QString tb_0 = "with tb0 as  (SELECT t1.z"
                +QString::number(zn+1)
                +" as boule from "
                +tab_1
                +" as t1, "
                +tab_2
                +" as t2 where (t1.z"+QString::number(zn+1)
                +" in ("
                +colNames
                +") and t2.id = "
                +QString::number(tir_id)
                +")),";

 QString ref_1 = "tb%1";
 QString ref_2 = ref_1+" as (select * from tb0)";
 QString ref_3 = "(tbr.b%2 = tb%2.boule)";

 QString str_in_1 = "";
 QString str_in_2 = "";
 QString str_in_3 = "";

 for(int i = 1; i<=uplet; i++){
  str_in_1 = str_in_1 + ref_1.arg(i);
  str_in_2 = str_in_2 + ref_2.arg(i);
  str_in_3 = str_in_3 + ref_3.arg(i);

	if(i<uplet){
	 str_in_1 = str_in_1 + ",";
	 str_in_2 = str_in_2 + ",";
	 str_in_3 = str_in_3 + "and";
	}
 }

 if(str_in_3.size()){
  str_in_3 = " where("+str_in_3+")";
 }

 msg = tb_0
       +str_in_2
       +" select tbr.* from ("
       + tbl
       +") as tbr, "
       +str_in_1
       +str_in_3
       +" order by total desc";

#ifndef QT_NO_DEBUG
 qDebug() <<tbl;
 qDebug() <<tb_0;
 qDebug() <<str_in_1;
 qDebug() <<str_in_2;
 qDebug() <<str_in_3;
 qDebug() <<msg;
#endif

 return msg;
}

QString BPrevision::sql_CnpCountUplet(int nb, QString tbl_cnp, QString tbl_in)
{
 QString msg = "";
 int zn = 0;

 // Recherche dans base actuelle
 QString tbl_ref = "("+tbl_in+")";
 QString lst_0 =  "lst_R as (select * from("
                 +tbl_cnp
                 +")), tb_0 as (select * from "
                 +tbl_ref
                 +"), ";

 QString ref_1 = "tb_%1 as (select * from tb_0)";
 QString ref_2 = "lst_R.b%1";
 QString ref_3 = ref_2 + " as b%1";
 QString ref_4 = "tb_%1.id";
 QString ref_5 = "(tb_%1.id<=tb_%2.id)";

 QString ref_7 = "tb_R.b%1";

 QString str_in_1 = "";
 QString str_in_2 = "";
 QString str_uple = "";
 QString str_in_3 = "";
 QString str_in_4 = "";
 QString str_in_5 = "";
 QString str_in_6 = "";
 QString str_in_7 = "";
 QString str_in_8 = "";

 QString str_full = "";

 for(int i = 1; i<=nb; i++){
  str_in_1 = str_in_1 + ref_1.arg(i);
  str_in_2 = str_in_2 + ref_2.arg(i);
  str_uple = str_uple + ref_2.arg(i);
  str_in_3 = str_in_3 + ref_3.arg(i);
  str_in_4 = str_in_4 + ref_4.arg(i);
  str_in_7 = str_in_7 + ref_7.arg(i);

	QString tmp_tbl = ref_4.arg(i);
	tmp_tbl=tmp_tbl.remove(".id");
	QString colNames = FN2_getFieldsFromZone(zn,tmp_tbl);
	QString tmp_lgn = "";
	QString str_key = "";
	for (int j = 1; j<=nb;j++) {
	 QString cur_tbl = ref_2.arg(j);
	 tmp_lgn = "("
						 +cur_tbl
						 + " in ("
						 +colNames
						 +"))";
	 str_key = str_key
						 +tmp_lgn;
	 if(j<nb){
		str_key = str_key
							+"and";
	 }
	}

	str_full = str_full + str_key;

	if(i<nb){
	 str_in_1 = str_in_1 + ", ";
	 str_in_2 = str_in_2 + ", ";
	 str_in_3 = str_in_3 + ", ";
	 str_in_4 = str_in_4 + ", ";
	 str_in_7 = str_in_7 + ", ";
	 str_uple = str_uple + "||','||";
	 str_full = str_full + "and";
	 str_in_8 = str_in_8 + ref_4.arg(i);

	 str_in_5 = str_in_5 + ref_5.arg(i).arg(i+1);
	 if(i<nb-1){
		str_in_5 = str_in_5 + "and";
		str_in_8 = str_in_8 + ", ";
	 }
	}
 }

 if(str_in_5.size()){
  str_in_5 = " where("
             +str_in_5
             +"and"
             +str_full
             +")";
 }
 QString str_from = str_in_4;

#ifndef QT_NO_DEBUG
 qDebug() <<lst_0;
 qDebug() <<str_in_1;
 qDebug() <<str_in_3;
 qDebug() <<str_uple;
 qDebug() <<str_from;
 qDebug() <<str_in_5;
 qDebug() <<str_in_2;
 qDebug() <<str_in_8;
 qDebug() <<str_in_7;
#endif

 QString sql_cnp = "with "
                   +lst_0
                   +str_in_1
                   +", tb_R as (select "
                   +str_in_3
                   +","
                   +str_uple
                   +" as uplet, count(*) as nb from lst_R,"
                   +str_from.remove(".id")
                   + str_in_5
                   + " group by "
                   + str_in_2
                   + ","
                   + str_in_8
                   + " order by nb DESC"
                   +")" ;

 QString sql_req = "select "
                   + str_in_7
                   +", tb_R.uplet, max(tb_R.nb) over(PARTITION by tb_R.uplet) as total  from tb_R GROUP by tb_R.uplet order by tb_R.nb DESC";

 msg = sql_cnp + sql_req;
#ifndef QT_NO_DEBUG
 qDebug() <<sql_cnp;
 qDebug() <<str_full;
 qDebug() <<sql_req;
 qDebug() <<msg;
#endif


 return msg;
}

bool BPrevision::do_SqlCnpPrepare(void)
{
 bool isOk = true;
 QString msg = "";
 QSqlQuery query(db_1);

 int zn = 0;
 QString ensemble = "B_elm";
 QString col = "z"+QString::number(zn+1);
 int len = onGame.limites[zn].len;
 int max = onGame.limites[zn].max;

 for(int i = 2; (i<len) && isOk ;i++){
  /// Regarder si table existe deja
  QString tbl = "Cnp_" + QString::number(max)+"_"+QString::number(i);
  if(DB_Tools::isDbGotTbl(tbl,db_1.connectionName())==false){
   msg = sql_CnpMkUplet(i, col);
   QString sql_cnp = "create table if not exists "
                     + tbl
                     + " as "
                     + msg
                     + " select * from lst_R";
#ifndef QT_NO_DEBUG
   qDebug() <<sql_cnp;
#endif

	 isOk= query.exec(sql_cnp);
	}
 }

 return isOk;
}

bool BPrevision::do_SqlCnpCount(void)
{
 bool isOk = true;
 QString msg = "";
 QString sql_cnp="";

 QSqlQuery query(db_1);

 int zn = 0;
 QString ensemble = "B_elm";
 QString col = "z"+QString::number(zn+1);
 int len = onGame.limites[zn].len;
 int max = onGame.limites[zn].max;

 //len -1 pour tester moins de cas
 for(int i = 2; (i<len) && isOk ;i++){
  /// Regarder si table existe deja
  QString tbl = "Cnp_" + QString::number(max)+"_"+QString::number(i);
  if(DB_Tools::isDbGotTbl(tbl,db_1.connectionName())==false){
   msg = sql_CnpMkUplet(i, col);
   sql_cnp = "create table if not exists "
             + tbl
             + " as "
             + msg
             + " select * from lst_R";

#ifndef QT_NO_DEBUG
   qDebug() <<sql_cnp;
#endif

	 isOk= query.exec(sql_cnp);
	}

	/// La table des Cnp est cree
	/// compter les u-plets
	if(isOk){
	 QString upl = "B_upl_"+QString::number(i)+"_z"+QString::number(zn+1);
	 msg = sql_CnpCountUplet(i,tbl);
	 sql_cnp = "create table if not exists "
						 + upl
						 + " as "
						 + msg;

#ifndef QT_NO_DEBUG
	 qDebug() <<sql_cnp;
#endif

	 isOk= query.exec(sql_cnp);
	}
 }

 return isOk;
}

void BPrevision::slot_CnpEnd(const BCnp::Status eStatus,const int val_n, const int val_p){
 BCnp *tmp = qobject_cast<BCnp *>(sender());
 QString tabName = "";
 int n = val_n;
 int p = val_p;

 switch (eStatus) {
  case BCnp::Status::NoSet:
  case BCnp::Status::Failure:
   break;

	case BCnp::Status::Ready:
	case BCnp::Status::Created:
	 tabName = tmp->getDbTblName();
	 break;
 }
}

void BPrevision::slot_UGL_ClrFilters()
{
 QSqlQuery query(db_1);
 bool isOk = true;
 QString msg = "";
 msg = "update Filtres set flt=(case when ((flt&0x"+
       QString::number(BFlags::isWanted)+")==0x"+QString::number(BFlags::isWanted)+
       " and (flt>0)) then (flt&~(0x"+QString::number(BFlags::isWanted)+")) ELSE flt END)";

#ifndef QT_NO_DEBUG
 qDebug() <<msg;
#endif

 isOk = query.exec(msg);
}

void BPrevision::slot_UGL_SetFilters()
{
 QSqlQuery query(db_1);
 bool isOk = true;
 QString msg = "";
 QString source = "E1_001_C49_5";//monJeu.tblUsr_dta;
 QString analys = monJeu.tblUsr_ana+"_z1";///"U_E1_ana_z1";

 //onGame;
 //monJeu;

 /// Verifier si existance table resultat utilisateur
 msg = "SELECT name FROM sqlite_master "
       "WHERE type='table' AND name='"+source+"';";

 if((isOk = query.exec(msg)))
 {
  /// A t'on une reponse
  isOk = query.first();
  if(!isOk)
   return;
 }

 /// recuperation des criteres de filtre
 QString flt_elm = c1->getFilteringData(0);
 QString flt_cmb = c2->getFilteringData(0);
 QString flt_grp = c3->getFilteringData(0);
 QString flt_brc = c->getFilteringData(0);
 QString otherCriteria = "";

 if(flt_elm.size()){
  otherCriteria = "and("+flt_elm+")";
 }

 if(flt_cmb.size()){
  otherCriteria = otherCriteria+"and("+flt_cmb+")";
 }

 if(flt_grp.size()){
  otherCriteria = otherCriteria+"and("+flt_grp+")";
 }

 if(flt_brc.size()){
  otherCriteria = otherCriteria+"and("+flt_brc+")";
 }

 int zn=0;
 QString key_to_use = onGame.names[zn].abv;

 if(source.contains("Cnp")){
  key_to_use="c";
 }
 /// requete a ete execute
 QString ref = "tb1."+key_to_use+"%1 as "+key_to_use+"%1";
 int nb_items = onGame.limites[zn].len;

 QString tmp = "";
 for(int item=0;item<nb_items;item++){
  tmp = tmp + ref.arg(item+1);
  if(item < nb_items -1){
   tmp = tmp + ",";
  }
 }
 //msg = msg + "("+tmp+")"+useJonction;
 /// "select tb1.b1 as b1, tb1.b2 as b2, tb1.b3 as b3, tb1.b4 as b4, tb1.b5 as b5 from ("

 // "select J, "+tmp+" from ("
 msg = "select * from ("
       +source
       +") as tb1,("
       +analys
       +") as tb2 where ((tb1.id=tb2.id) "
       +otherCriteria
       +")";

#ifndef QT_NO_DEBUG
 qDebug() <<flt_elm;
 qDebug() <<flt_cmb;
 qDebug() <<flt_grp;
 qDebug() <<msg;
#endif

 /// Mettre la vue a jour
 //sqm_resu->clear();
 sqm_resu->setQuery(msg,db_1);
 int nbLignes = sqm_resu->rowCount();
 QSqlQuery nvll(db_1);
 isOk=nvll.exec("select count(*) from ("+msg+")");
#ifndef QT_NO_DEBUG
 qDebug() << "msg:"<<msg;
#endif

 if(isOk){
  nvll.first();
  if(nvll.isValid()){
   nbLignes = nvll.value(0).toInt();
  }
 }

 QString tot = "Total : " + QString::number(nbLignes);
 gpb_Tirages->setTitle(tot);

}

/// slot de reemission de la boule selectionnee
void BPrevision::slot_emitThatClickedBall(const QModelIndex &index)
{
 emit sig_isClickedOnBall(index);
}

#if 1
void BPrevision::slot_UGL_Create()
{
 QString cnx_name = db_1.connectionName();

 /// Caracteristique de la generation liste tirages utilistateur
 stGameConf * tmp = new stGameConf;

 /// Donnees initiale pgm
 tmp->bUseMadeBdd = onGame.bUseMadeBdd;
 tmp->eFdjType = onGame.eFdjType;
 tmp->limites = onGame.limites;
 tmp->names = onGame.names;

 /// Specificite de cette recherche
 tmp->znCount = 1;
 tmp->eTirType = eTirUsr; /// A supprimer ?
 tmp->id = 0; /// A supprimer ?

 /// Temps de calcul
 QTime r;
 QTime t;
 QString t_human = "";

 r.setHMS(0,0,0,0);
 t.start();
 BGrbGenTirages *calcul = new BGrbGenTirages(tmp,cnx_name, this);
 r = r.addMSecs(t.elapsed());
 t_human = r.toString("hh:mm:ss:zzz");
 if(calcul->addr != nullptr){
  calcul->show();
 }
 else {
  delete calcul;
 }

 QString msg = " Calcul en : "
       +t_human
       +QString (" (hh:mm:ss:ms)");
 QMessageBox::information(NULL,"UsrGame",msg,QMessageBox::Ok);


 return;

 QSqlQuery query(db_1);
 bool isOk = true;
 //QString msg = "";
 QString SelElemt = C_TBL_6;
 int n = 0;
 int p = onGame.limites[0].win;
 int m = onGame.limites[0].max;

 QString UsrCnp = "";
 static int userRequest = 1;

 /// Reutilisation base
 if(monJeu.gameInfo.bUseMadeBdd){
  msg = "SELECT name as TbUsr FROM sqlite_master WHERE type='table' AND name like 'E1%'";
  isOk = query.exec(msg);
  if(query.first()){
   /// Recherche(s) utilisateur presente(s)
   do{
    UsrCnp = query.value(0).toString();

	 }while (query.next());
	}
	UsrCnp = "E1_"+QString::number(userRequest).rightJustified(3,'0')+"_C"+QString::number(n)+"_"+QString::number(p);
	if(DB_Tools::isDbGotTbl(UsrCnp,"f")){
	 /// Montrer le resultat
	}
 }

 /// Selectionner les boules choisi par l'utilisateur pour en faire
 /// un ensemble d'etude
 ///
 msg = "select count(Choix.pri)  as T from Filtres as Choix where(choix.pri=1 AND choix.zne=0 and choix.typ=0)";

#ifndef QT_NO_DEBUG
 qDebug() <<msg;
#endif

 isOk = query.exec(msg);

 if(!query.first()){
  return; // Pas de selection utilisateur
 }

 n = query.value("T").toInt();

 if(n < p){
  return; /// pas assez d'info pour calcul Cnp
 }

 /// On peut continuer
 if(n<=m){
  UsrCnp = "E1_"+QString::number(userRequest).rightJustified(3,'0')+"_C"+QString::number(n)+"_"+QString::number(p);
  userRequest++;
 }
 else {
  ;
 }
 // Suite
 msg = "create table "
       +UsrCnp
       +" as "
         "with selection as (select ROW_NUMBER () OVER (ORDER by ROWID) id, val from filtres where (pri=1 and zne=0))"
         "SELECT ROW_NUMBER () OVER () id,\"nop\" as J, t1.val as b1, t2.val as b2, t3.val as b3 , t4.val as b4 , t5.val as b5 "
         "FROM selection As t1, selection As t2,  selection As t3,selection As t4,selection As t5 "
         "WHERE ("
         "(t1.id<t2.id) and"
         "(t2.id<t3.id) and"
         "(t3.id<t4.id) and"
         "(t4.id<t5.id)"
         ")"
         "ORDER BY t1.id, t2.id, t2.id, t3.id, t4.id, t5.id";

 isOk = query.exec(msg);

 int zn=0;
 monJeu.gameInfo.eFdjType = onGame.eFdjType;
 monJeu.gameInfo.eTirType = eTirGen;
 monJeu.gameInfo.znCount = 1;
 monJeu.gameInfo.bUseMadeBdd=onGame.bUseMadeBdd;
 monJeu.gameInfo.limites = &(onGame.limites[0]);
 monJeu.gameInfo.names = &(onGame.names[0]);
 monJeu.tblFdj_dta="B_fdj";
 monJeu.tblFdj_brc="r_B_fdj_0_brc_z"+QString::number(zn+1);

 ///REM:QString tbl_cible_ana = "U_"+UsrCnp+"_ana";
 QString tbl_cible_ana = UsrCnp+"_ana";
 monJeu.tblUsr_dta=UsrCnp;
 monJeu.tblUsr_ana=tbl_cible_ana;
 onGame.eTirType=eTirGen;
 ContinuerCreation(UsrCnp, tbl_cible_ana);

}
#else
void BPrevision::slot_UGL_Create()
{
 /// fn UGL_Create (user game list)
 QSqlQuery query(db_1);
 bool isOk = true;
 QString msg = "";
 QString SelElemt = C_TBL_6;
 BCnp *a = NULL;
 int m = 0;
 int n = 0;
 int p = 0;

 /// Selectionner les boules choisi par l'utilisateur pour en faire
 /// un ensemble d'etude
 ///
 msg = "select count(Choix.pri)  as T from Filtres as Choix where(choix.pri=1 AND choix.zne=0 and choix.typ=0)";

#ifndef QT_NO_DEBUG
 qDebug() <<msg;
#endif

 if((isOk = query.exec(msg)))
 {
  query.first();
  n = query.value("T").toInt();
  p = onGame.limites[0].win;
  m = onGame.limites[0].max;

	if(n < p){
	 return; /// pas assez d'info pour calcul Cnp
	}
	else{
	 if(n<=MAX_CHOIX_BOULES){
		int zn=0;
		monJeu.gameInfo.eFdjType = onGame.eFdjType;
		monJeu.gameInfo.eTirType = eTirGen;
		monJeu.gameInfo.znCount = 1;
		monJeu.gameInfo.bUseMadeBdd=onGame.bUseMadeBdd;
		monJeu.gameInfo.limites = &(onGame.limites[0]);
		monJeu.gameInfo.names = &(onGame.names[0]);
		monJeu.tblFdj_dta="B_fdj";
		monJeu.tblFdj_brc="r_B_fdj_0_brc_z"+QString::number(zn+1);


		QTime r;
		QTime t;
		QString t_human = "";

		if(isTableCnpinDb(n,p)==false)
		{
		 r.setHMS(0,0,0,0);
		 t.start();
		 a= new BCnp(n,p,db_1.connectionName());
		 r = r.addMSecs(t.elapsed());
		 t_human = r.toString("hh:mm:ss:zzz");
		 msg = "Creation Cnp("
					 +QString::number(n)
					 +QString(",")
					 +QString::number(p)
					 +QString(") en : ")
					 +t_human
					 +QString (" (hh:mm:ss:ms)");
		 QMessageBox::information(NULL,"UsrGame",msg,QMessageBox::Ok);
		}

		/// supprimer la vue resultat
		msg = "drop table if exists E1";
		isOk = query.exec(msg);
		if(isOk){
		 r.setHMS(0,0,0,0);
		 t.restart();
		 /// Creer une liste de jeux possibles
		 if(n == m){
			int memo_usr = onGame.limites[0].usr;
			etTirType mem_from = onGame.eTirType;

			onGame.limites[0].usr=m;
			QString tbl_cible = a->getDbTblName();
			QString tbl_cible_ana = "U_"+tbl_cible+"_ana";
			monJeu.tblUsr_dta=tbl_cible;
			monJeu.tblUsr_ana=tbl_cible_ana;
			onGame.eTirType=eTirGen;
			ContinuerCreation(tbl_cible, tbl_cible_ana);

			onGame.limites[0].usr=memo_usr;
			onGame.eTirType=mem_from;
		 }
		 else{
			monJeu.tblUsr_dta="E1";
			monJeu.tblUsr_ana="U_E1_ana";
			creerJeuxUtilisateur(n,p);
		 }
		 r = r.addMSecs(t.elapsed());
		 t_human = r.toString("hh:mm:ss:zzz");
		 msg = QString("Creation Liste de jeux en : ")
					 +t_human
					 +QString (" (hh:mm:ss:ms)");
		 QMessageBox::information(NULL,"UsrGame",msg,QMessageBox::Ok);

		}

	 }
	}

 }

#ifndef QT_NO_DEBUG
 qDebug() <<msg;
#endif
}
#endif

bool BPrevision::isPreviousDestroyed(void)
{
 bool isOk = true;
 QSqlQuery query(db_1);
 QString msg = "SELECT name FROM sqlite_master WHERE type='table' AND name like '%E1%';";

 if((isOk=query.exec(msg))){
  query.first();
  if(query.isValid()){
   /// supprimer fenetres precedente
   //delete Affiche_E1;
   //delete Resulta_E1;

	 int ret = QMessageBox::question(NULL,"Calcul","Continuer (supprime precedent) ?",
																	 QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Ok);
	 if(ret == QMessageBox::Cancel){
		return false;
	 }
	 /// on a des tables a detruire
	 QString tbl_tmp;
	 QSqlQuery qdel(db_1);
	 do{
		tbl_tmp = query.value(0).toString();
		msg = "drop table "+tbl_tmp+";";
		isOk = qdel.exec(msg);
	 }while(isOk && query.next());
	}
 }
 return isOk;
}
void BPrevision::creerJeuxUtilisateur(int n, int p)
{
 bool isOk = false;
 QSqlQuery query(db_1);
 QSqlQuery query_2(db_1);

 QString msg = "";
 QString tbl_cible = "E1";
 QString tbl_cible_ana = "U_"+tbl_cible+"_ana";


 msg = ListeDesJeux(0,n,p);
#ifndef QT_NO_DEBUG
 qDebug() << "msg: " <<msg;
#endif

 /// https://www.supinfo.com/articles/single/1160-utilisation-avancee-sqlite
 isOk = query.exec("begin IMMEDIATE Transaction");
 msg = "create table if not exists "+tbl_cible+" as "
       +msg;

 int query_pos = QSql::BeforeFirstRow;
 if(isOk) {
  isOk = query.exec(msg);
 }
 else{
  QMessageBox::critical(0, "UGL", "Erreur UGL !",QMessageBox::Yes);
  QApplication::quit();
 }
 while (query.isActive()&& isOk){
  query_pos = query.at();
  if(query_pos==QSql::AfterLastRow){
   break;
  }
 }
 if (isOk)isOk = query.exec("commit transaction");


 /// Verif execution peut commencer
 if(!isOk)
 {
  QString ErrLoc = "BPrevision::creerJeuxUtilisateur:";
  DB_Tools::DisplayError(ErrLoc,&query,msg);

  return;
 }
 /// ---------------------
 ///  EFFFECTUER LA SUITE
 ContinuerCreation(tbl_cible, tbl_cible_ana);

 isOk = true;

}

void BPrevision::slot_ShowNewTotal(const QString& lstBoules)
{
 BLineEdit *ble_tmp = qobject_cast<BLineEdit *>(sender());

 QTableView *view = ble_tmp->getView();
 BFpm_3 *m = qobject_cast<BFpm_3 *>(view->model());
 QSqlQueryModel *vl = qobject_cast<QSqlQueryModel *>(m->sourceModel());

 int nb_lgn_ftr = m->rowCount();
 int nb_lgn_rel = vl->rowCount();

 QString st_total = "Total : " + QString::number(nb_lgn_ftr)+" sur " + QString::number(nb_lgn_rel);
 gpb_Tirages->setTitle(st_total);

}
#if 1
void BPrevision::ContinuerCreation(QString tbl_cible, QString tbl_cible_ana)
{
 bool isOk=false;
 QString msg = "";

 int zn=0;
 int chk_nb_col = monJeu.gameInfo.limites[zn].len;

#if 0
 if(monJeu.gameInfo.bUseMadeBdd==false){
  isOk = AnalyserEnsembleTirage(tbl_cible,monJeu.gameInfo, zn);

	if(isOk)
	 isOk = FaireTableauSynthese(tbl_cible_ana,monJeu.gameInfo,zn);

	/// Ligne analyse les tirages generees
	analyserTirages(conf,tbl_cible,monJeu.gameInfo);
 }
#endif
 /// Montrer resultats
 msg="select * from ("+tbl_cible+")";
 QTableView *qtv_tmp = new QTableView;

 sqm_resu = new QSqlQueryModel; //QSqlQueryModel *
 sqm_resu->setQuery(msg,db_1);

 BFpm_3 * fpm_tmp = new BFpm_3(chk_nb_col,2);
 fpm_tmp->setDynamicSortFilter(true);
 fpm_tmp->setSourceModel(sqm_resu);
 qtv_tmp->setModel(fpm_tmp);

 //--------------
 QFormLayout *frm_chk = new QFormLayout;
 BLineEdit *le_chk = new BLineEdit(qtv_tmp);
 frm_chk->addRow("Rch :", le_chk);
 le_chk->setToolTip("Recherche");

 QString stPattern = "(\\d{1,2},?){1,"
                     +QString::number(chk_nb_col-1)
                     +"}(\\d{1,2})";
 QValidator *validator = new QRegExpValidator(QRegExp(stPattern));

 le_chk->setValidator(validator);
 connect(le_chk,SIGNAL(textChanged(const QString)),qtv_tmp->model(),SLOT(setUplets(const QString)));
 connect(le_chk,SIGNAL(textChanged(const QString)),this,SLOT(slot_ShowNewTotal(const QString)));
 //--------------


 /// Necessaire pour compter toutes les lignes de reponses
 while (sqm_resu->canFetchMore())
 {
  sqm_resu->fetchMore();
 }


 /// Determination nb ligne par proxymodel
 int nb_lgn_ftr = fpm_tmp->rowCount();
 int nb_lgn_rel = sqm_resu->rowCount();

#if 0
	QSqlQuery nvll(db_1);
	isOk=nvll.exec("select count(*) from ("+tbl_cible+")");
	if(isOk){
	 nvll.first();
	 if(nvll.isValid()){
		nb_lgn_rel = nvll.value(0).toInt();
	 }
	}
#endif

 gpb_Tirages =new QGroupBox;
 QString st_total = "Total : " + QString::number(nb_lgn_ftr)+" sur " + QString::number(nb_lgn_rel);
 gpb_Tirages->setTitle(st_total);

 //QWidget *Affiche = new QWidget;
 QVBoxLayout *layout = new QVBoxLayout;
 layout->addLayout(frm_chk,Qt::AlignLeft|Qt::AlignTop);
 layout->addWidget(qtv_tmp, Qt::AlignLeft|Qt::AlignTop);
 //layout->addWidget(gpb_Tirages,0,Qt::AlignLeft|Qt::AlignTop);
 gpb_Tirages->setLayout(layout);

 int nbCol = sqm_resu->columnCount();
 for(int col=0;col<nbCol;col++)
 {
  qtv_tmp->setColumnWidth(col,CEL2_L);
 }
 qtv_tmp->hideColumn(0);
 qtv_tmp->setFixedHeight(700);
 qtv_tmp->setFixedWidth((nbCol+1)*CEL2_L);

 //Affiche->setLayout(layout);
 gpb_Tirages->setWindowTitle("Ensemble : "+ tbl_cible);
 gpb_Tirages->show();
}
#else
void BPrevision::ContinuerCreation(QString tbl_cible, QString tbl_cible_ana)
{
 bool isOk=false;
 QString msg = "";

 int zn=0;
 int chk_nb_col = monJeu.gameInfo.limites[zn].len;

 if(monJeu.gameInfo.bUseMadeBdd==false){
  isOk = AnalyserEnsembleTirage(tbl_cible,monJeu.gameInfo, zn);

	if(isOk)
	 isOk = FaireTableauSynthese(tbl_cible_ana,monJeu.gameInfo,zn);
 }

 /// Ligne analyse les tirages generees
 analyserTirages(conf,tbl_cible,monJeu.gameInfo);

 static bool OneShot = false;
 if(OneShot==false){
  OneShot = true;

	/// Montrer resultats
	msg="select * from ("+tbl_cible+")";
	QTableView *qtv_tmp = new QTableView;

	sqm_resu = new QSqlQueryModel; //QSqlQueryModel *
	sqm_resu->setQuery(msg,db_1);

	BFpm_3 * fpm_tmp = new BFpm_3(chk_nb_col,2);
	fpm_tmp->setDynamicSortFilter(true);
	fpm_tmp->setSourceModel(sqm_resu);
	qtv_tmp->setModel(fpm_tmp);
	//qtv_tmp->setSortingEnabled(true);

	//--------------
	QFormLayout *frm_chk = new QFormLayout;
	BLineEdit *le_chk = new BLineEdit(qtv_tmp);
	frm_chk->addRow("Rch :", le_chk);
	le_chk->setToolTip("Recherche");

	QString stPattern = "(\\d{1,2},?){1,"
											+QString::number(chk_nb_col-1)
											+"}(\\d{1,2})";
	QValidator *validator = new QRegExpValidator(QRegExp(stPattern));

	le_chk->setValidator(validator);
	connect(le_chk,SIGNAL(textChanged(const QString)),qtv_tmp->model(),SLOT(setUplets(const QString)));
	connect(le_chk,SIGNAL(textChanged(const QString)),this,SLOT(slot_ShowNewTotal(const QString)));
	//--------------


	/// Necessaire pour compter toutes les lignes de reponses
	while (sqm_resu->canFetchMore())
	{
	 sqm_resu->fetchMore();
	}


	/// Determination nb ligne par proxymodel
	int nb_lgn_ftr = fpm_tmp->rowCount();
	int nb_lgn_rel = sqm_resu->rowCount();

#if 0
	QSqlQuery nvll(db_1);
	isOk=nvll.exec("select count(*) from ("+tbl_cible+")");
	if(isOk){
	 nvll.first();
	 if(nvll.isValid()){
		nb_lgn_rel = nvll.value(0).toInt();
	 }
	}
#endif

	gpb_Tirages =new QGroupBox;
	QString st_total = "Total : " + QString::number(nb_lgn_ftr)+" sur " + QString::number(nb_lgn_rel);
	gpb_Tirages->setTitle(st_total);

	//QWidget *Affiche = new QWidget;
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addLayout(frm_chk,Qt::AlignLeft|Qt::AlignTop);
	layout->addWidget(qtv_tmp, Qt::AlignLeft|Qt::AlignTop);
	//layout->addWidget(gpb_Tirages,0,Qt::AlignLeft|Qt::AlignTop);
	gpb_Tirages->setLayout(layout);

	int nbCol = sqm_resu->columnCount();
	for(int col=0;col<nbCol;col++)
	{
	 qtv_tmp->setColumnWidth(col,CEL2_L);
	}
	qtv_tmp->hideColumn(0);
	qtv_tmp->setFixedHeight(700);
	qtv_tmp->setFixedWidth((nbCol+1)*CEL2_L);

	//Affiche->setLayout(layout);
	gpb_Tirages->setWindowTitle("Ensemble : "+ tbl_cible);
	gpb_Tirages->show();
 }
}
#endif

bool BPrevision::isTableCnpinDb(int n, int p)
{
 bool isOk = false;
 QSqlQuery query(db_1);

 return isOk;
}

QString BPrevision::ListeDesJeux(int zn, int n, int p)
{
 ///----------------------
 QString tbSel = C_TBL_6;
 bool isOk = true;
 QSqlQuery query(db_1);

 int loop = 0;
 int len = p;

 /// Creation d'une table temporaire pour sauver choix utilisateur
 int selprio = 1;
 QString src_usr ="Filtres";

 QString tmp_tbl =src_usr
                   +"_p"+QString::number(selprio);

 QString str_req[] ={
  {"drop table if exists "
   +tmp_tbl
   +";"
  },
  {"create table if not exists "
   +tmp_tbl
   +" (Id integer primary key, val integer);"
  },
  {
   "insert into "
   +tmp_tbl
   +" select NULL,choix.val from ("
   +src_usr+
   ") as choix where(choix.pri=1 AND choix.zne=0 and choix.typ=0);"
  }

 };

 /*
  * where((choix.flt&0x"
   +QString::number(BDelegateElmOrCmb::isWanted)+"=0x"+QString::number(BDelegateElmOrCmb::isWanted)+
   ") AND choix.zne=0 and choix.typ=0);"
  */

 /// Faire en sequence les requetes
 int nbSqlmsg = sizeof(str_req)/sizeof(QString);
 for(int un_msg = 0; (un_msg< nbSqlmsg) && isOk ;un_msg++){
#ifndef QT_NO_DEBUG
  qDebug() << str_req[un_msg];
#endif
  isOk = query.exec(str_req[un_msg]);
 }

 if(!isOk){
  QString err_msg = query.lastError().text();
  //un message d'information
  QMessageBox::critical(0, this->objectName(), err_msg,QMessageBox::Yes);
#ifndef QT_NO_DEBUG
  qDebug() << err_msg;
#endif
  QApplication::quit();
 }

 ///-------------------------
 QString msg1 = "";
 QString ref = "tb%1.val as b%2 ";
 for(int i = 0; i< len; i++)
 {
  msg1 = msg1 + ref.arg(i+1).arg(i+1);
  if(i<len-1)
   msg1 = msg1 + ",";
 }
 msg1 = "tbLeft.id, 'DAY_tbd' as J," + msg1;
#ifndef QT_NO_DEBUG
 qDebug() << "msg1: " <<msg1;
#endif

 /// clause left
 QString msg2 = "";
 loop = len;
 ref = "(select tbChoix.id, tbChoix.val from ("
       +tmp_tbl
       +") as tbChoix )as tb%1";
 for(int i = 0; i< len; i++)
 {
  msg2 = msg2 + ref.arg(i+1);
  if(i<len-1)
   msg2 = msg2 + ",";
 }

#ifndef QT_NO_DEBUG
 qDebug() << "msg2: " <<msg2;
#endif

 /// clause on
 QString msg3 = "";
 ref = "(tbLeft.c%1 = tb%2.id)";
 for(int i = 0; i< len; i++)
 {
  msg3 = msg3 + ref.arg(i+1).arg(i+1);
  if(i<len-1)
   msg3 = msg3 + "and";
 }
#ifndef QT_NO_DEBUG
 qDebug() << "msg3: " <<msg3;
#endif

 stJoinArgs args;
 args.arg1 = msg1;
 args.arg2 = "Cnp_"+QString::number(n)+"_"+QString::number(p);
 args.arg3 = msg2;
 args.arg4 = msg3;
 QString msg = DB_Tools::leftJoin(args);

#ifndef QT_NO_DEBUG
 qDebug() << "msg: " <<msg;
#endif

 ///----------------------
 return msg;
}

bool BPrevision::AnalyserEnsembleTirage(QString tblIn, const stGameConf &onGame, int zn)
{
 /// Verifier si des vues temporaires precedentes sont encore presentes
 /// Si oui les effacer
 /// Si non prendre la liste des criteres a appliquer sur l'ensemble
 /// puis faire tant qu'il existe un critere
 /// effectuer la selection comptage vers une nouvelle vu temporaire i
 /// quand on arrive a nombre de criteres total -1 la vue destination
 /// sera OutputTable.

 bool isOk = true;
 QString msg = "";
 QSqlQuery query(db_1);
 QString stDefBoules = C_TBL_2;
 QString st_OnDef = "";
 QString tbLabAna = T_ANA;
 QString tblToUse = "";
 QString tbLabCmb = T_CMB;

 tbLabCmb = "B_" + tbLabCmb;
 if(onGame.eTirType == eTirFdj){
  tbLabAna = "B_" + tbLabAna;
  tblToUse = tblTirages;
 }
 else{
  tblToUse = tblIn ;
  ///REM:tbLabAna = "U_" + tblToUse + "_" +tbLabAna;
  tbLabAna = tblToUse + "_" +tbLabAna;
 }
 tbLabAna =tbLabAna+"_z"+QString::number(zn+1);

 QString ref="(tbleft.%1%2=tbRight.B)";

 /// sur quel nom des elements de la zone
 st_OnDef=""; /// remettre a zero pour chacune des zones
 int znLen = onGame.limites[zn].len;
 /// Ma modification pour utiliser la table CNP
 /// dans le cas jeu utilisateur
 QString key_abv = onGame.names[zn].abv;
 if(onGame.eTirType==eTirGen && (onGame.limites[0].usr == onGame.limites[0].max)){
  key_abv = "c";
 }

 for(int j=0;j<znLen;j++)
 {
  st_OnDef = st_OnDef + ref.arg(key_abv).arg(j+1);
  if(j<znLen-1)
   st_OnDef = st_OnDef + " or ";
 }

#ifndef QT_NO_DEBUG
 qDebug() << "on definition:"<<st_OnDef;
#endif

 QStringList *slst=&slFlt[zn][0];

 /// Verifier si des tables existent deja
 if(SupprimerVueIntermediaires())
 {
  /// les anciennes vues ne sont pas presentes
  ///  on peut faire les calculs
  int loop = 0;
  int nbTot = slst[0].size();
  int colId = 0;
  QString curName = tblToUse;
  QString curTarget = "view vt_0";
  QString lastTitle = "tbLeft.id  as Id,";
  QString curTitle = "tbLeft.*";

	do
	{
	 /// Dans le cas zone etoiles prendre la valeur directe
	 QString colName = slst[1].at(loop);
	 if(zn==1 && colName.contains("U")&&colId<znLen){
		colId++;
		msg = "create " + curTarget
					+" as select "+curTitle+", tbRight."
					+key_abv+QString::number(colId)+" as "
					+ slst[1].at(loop)
					+" from("+curName+")as tbLeft "
					+"left join ( "
					+tblToUse+") as tbRight  on (tbRight.id = tbLeft.id)";

	 }
	 else{
		msg = "create " + curTarget
					+" as select "+curTitle+", count(tbRight.B) as "
					+ slst[1].at(loop)
					+" from("+curName+")as tbLeft "
					+"left join (select c1.id as B from "
					+stDefBoules+" as c1 where (c1.z"
					+QString::number(zn+1)+" not null and (c1."
					+slst[0].at(loop)+"))) as tbRight on ("
					+st_OnDef+") group by tbLeft.id";
	 }

#ifndef QT_NO_DEBUG
	 qDebug() << "msg:"<<msg;
#endif

	 isOk = query.exec(msg);

	 curName = "vt_" +  QString::number(loop);
	 lastTitle = lastTitle
							 + "tbLeft."+slst[1].at(loop)
							 +" as "+slst[1].at(loop);
	 loop++;
	 if(loop <  nbTot-1)
	 {
		curTarget = "view vt_"+QString::number(loop);
		lastTitle = lastTitle + ",";
	 }
	 else
	 {
		curTarget = "view vrz"+QString::number(zn+1)+"_"+tbLabAna;
		curTitle = lastTitle;
	 }
	}while(loop < nbTot && isOk);

	if(isOk){
	 /// mise en correspondance de la reference combinaison
	 QString msg = "";
	 QString ref_1 = "";
	 QString stCombi = "";
	 QString stLien = "";

	 ref_1 = "(tbLeft.U%1 = tbRight."+onGame.names[zn].abv+"%2)";
	 stLien = " and ";

	 if(onGame.eFdjType == eFdjEuro && zn == 1){
		ref_1 = "((tbLeft.U%3 = tbRight."+onGame.names[zn].abv+"%1)"
						+"or"+
						"(tbLeft.U%3 = tbRight."+onGame.names[zn].abv+"%2))";
		stLien = " and ";
	 }

	 int znLen = onGame.limites[zn].len;
	 for(int pos=0;pos<znLen;pos++){
		if(onGame.eFdjType == eFdjEuro && zn == 1){
		 stCombi = stCombi
							 + ref_1.arg((pos%2)+1).arg(((pos+1)%2)+1).arg(pos);

		}else{
		 stCombi = stCombi + ref_1.arg(pos).arg(pos+1);
		}

		if(pos<znLen-1)
		 stCombi = stCombi + stLien;
	 }

	 //curTarget = curTarget.remove("table");
	 curTarget = curTarget.remove("view");
	 msg = "create table if not exists "+tbLabAna
				 +" as select tbLeft.*,tbRight.id as idComb  from ("
				 +curTarget+")as tbLeft left join ("
				 + tbLabCmb+"_z"+QString::number(zn+1)
				 +")as tbRight on("
				 + stCombi
				 +")"
		;
#ifndef QT_NO_DEBUG
	 qDebug() << "msg:"<<msg;
#endif
	 isOk = query.exec(msg);
	}

	/// supression tables intermediaires
	if(isOk){
	 msg = "drop view if exists " + curTarget;
	 isOk= query.exec(msg);

	 if(isOk)
		isOk = SupprimerVueIntermediaires();
	}
 }

 if(!isOk)
 {
  QString ErrLoc = "BPrevision::AnalyserEnsembleTirage:";
  DB_Tools::DisplayError(ErrLoc,&query,msg);
 }
 return isOk;
}

bool BPrevision::SupprimerVueIntermediaires(void)
{
 bool isOk = true;
 QString msg = "";
 QSqlQuery query(db_1);
 QSqlQuery qDel(db_1);

 msg = "SELECT name FROM sqlite_master "
       "WHERE type='view' AND name like'vt_%';";
 isOk = query.exec(msg);

 if(isOk)
 {
  query.first();
  if(query.isValid())
  {
   /// il en existe donc les suprimer
   do
   {
    QString viewName = query.value("name").toString();
    msg = "drop view if exists "+viewName;
    isOk = qDel.exec(msg);
   }while(query.next()&& isOk);
  }
 }

 if(!isOk)
 {
  QString ErrLoc = "SupprimerVueIntermediaires:";
  DB_Tools::DisplayError(ErrLoc,&query,msg);
 }

 return isOk;
}

// Cette fonction retourne un pointeur sur un tableau de QStringList
// Ce tableau comporte 3 elements
// Element 0 liste des requetes construites
// Element 1 Liste des titres assosies a la requete
// Element 2 Liste des tooltips assosies au titres
// En fonction de la zone a etudier les requetes sont adaptees
// pour integrer le nombre maxi de boules a prendre en compte
QStringList * BPrevision::CreateFilterForData(int zn)
{
 QStringList *sl_filter = new QStringList [3];
 QString fields = "z"+QString::number(zn+1);

 int maxElems = onGame.limites[zn].max;
 int nbBoules = floor(maxElems/10)+1;

 // Parite & nb elment dans groupe
 sl_filter[0] <<fields+"%2=0"<<fields+"<"+QString::number(maxElems/2);
 sl_filter[1] << "P" << "G";
 sl_filter[2] << "Pair" << "< E/2";

 // Nombre de 10zaine
 for(int j=0;j<nbBoules;j++)
 {
  sl_filter[0]<< fields+" >="+QString::number(10*j)+
                   " and "+fields+"<="+QString::number((10*j)+9);
  sl_filter[1] << "U"+ QString::number(j);
  sl_filter[2] << "Entre:"+ QString::number(j*10)+" et "+ QString::number(((j+1)*10)-1);
 }

 // Boule finissant par [0..9]
 for(int j=0;j<=9;j++)
 {
  sl_filter[0]<< fields+" like '%" + QString::number(j) + "'";
  sl_filter[1] << "F"+ QString::number(j);
  sl_filter[2] << "Finissant par: "+ QString::number(j);
 }


 return sl_filter;
}

