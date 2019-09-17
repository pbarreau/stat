#ifndef QT_NO_DEBUG
#include <QDebug>
#include <QSqlError>
#endif

#include <QFile>
#include <QDate>

#include <QApplication>
#include <QString>
#include <QFormLayout>
#include <QTabWidget>
#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlRecord>
#include <QTextStream>
#include <QVBoxLayout>
#include <QLabel>

#include <QMessageBox>

#include "compter_zones.h"
#include "compter_combinaisons.h"
#include "compter_groupes.h"
#include "cbarycentre.h"

#include "labelclickable.h"

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

BPrevision::BPrevision(eGame game, eFrom from, eBddUse def)
{
 cur_item = total_items;
 total_items++;

 onGame.type = game;
 onGame.from = from;
 if(ouvrirBase(def,game)==true)
 {
  effectuerTraitement(game);
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

QString BPrevision::mk_IdDsk(eGame type)
{
 QDate myDate = QDate::currentDate();
 QString toDay = myDate.toString("dd-MM-yyyy");
 QString game = "";

 QFile myFileName;
 QString testName = "";

 game = gameLabel[type] + QString("_V1_")+toDay+QString("_");

 int counter = 0;
 do{
  testName = game + QString::number(counter).rightJustified(3,'0')+QString("-");
  testName = testName + QString::number(cur_item).rightJustified(2,'0')+QString(".sqlite");
  myFileName.setFileName(testName);
  counter = (counter + 1)%999;
 }while(myFileName.exists());

 return testName;
}

QString BPrevision::mk_IdCnx(eGame type)
{
 if((type <= eGameToSet) || (type>=eGameDefEnd)){
  eGame err = eGameToSet;
  QMessageBox::warning(NULL,"Prevision","Jeu "+gameLabel[err]+" inconnu !!",QMessageBox::Ok);
  QApplication::quit();
 }
 return (QString("cnx_V1_")+gameLabel[type]+QString("-")+QString::number(cur_item).rightJustified(2,'0'));
}

bool BPrevision::ouvrirBase(eBddUse cible, eGame game)
{
 bool isOk = true;

 cnx_db_1 = mk_IdCnx(game);
 db_1 = QSqlDatabase::addDatabase("QSQLITE",cnx_db_1);

 QString mabase = "";

 switch(cible)
 {
  case eBddUseRam:
   mabase = ":memory:";
   break;

	case eBddUseDisk:
	default:
	 mabase = mk_IdDsk(game);
	 break;
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
  QString ErrLoc = "cLesComptages::OPtimiseAccesBase";
  DB_Tools::DisplayError(ErrLoc,&query,msg);
 }

 return isOk;
}

void BPrevision::effectuerTraitement(eGame game)
{
 QString source = C_TBL_3;
 source = "B_" + source;
#if (SET_DBG_LIVE&&SET_DBG_LEV1)
 QMessageBox::information(NULL, "Pgm", "step 1!",QMessageBox::Yes);
#endif
 definirConstantesDuJeu(game);

#if (SET_DBG_LIVE&&SET_DBG_LEV1)
 QMessageBox::information(NULL, "Pgm", "step 2!",QMessageBox::Yes);
#endif
 creerTablesDeLaBase();

#if (SET_DBG_LIVE&&SET_DBG_LEV1)
 QMessageBox::information(NULL, "Pgm", "step 3!",QMessageBox::Yes);
#endif
 analyserTirages(source, onGame);


}

BGame * BPrevision::definirConstantesDuJeu(eGame game)
{
 /// Pour l'instant en loto ou en euro il y a 2 'zones'
 /// une pour les boules
 /// une pour les etoiles
 onGame.znCount = 2; /// boules + etoiles
 //onGame.type = game;
 //onGame.from = eFdj;

 switch(game)
 {
  case eGameLoto:
   onGame.limites = new stParam_1 [onGame.znCount];
   onGame.names = new stParam_2 [onGame.znCount];

	 /// z1 : boules
	 onGame.limites[0].min=1;
	 onGame.limites[0].max=49;
	 onGame.limites[0].len=5;
	 onGame.limites[0].win=5;
	 onGame.names[0].std = "Boules";
	 onGame.names[0].abv = "b";

	 /// z2 : etoiles
	 onGame.limites[1].min=1;
	 onGame.limites[1].max=10;
	 onGame.limites[1].len=1;
	 onGame.limites[1].win=1;
	 onGame.names[1].std = "Etoiles";
	 onGame.names[1].abv = "e";
	 break;

	case eGameEuro:
	 onGame.limites = new stParam_1 [onGame.znCount];
	 onGame.names = new stParam_2 [onGame.znCount];

	 /// boules
	 onGame.limites[0].min=1;
	 onGame.limites[0].max=50;
	 onGame.limites[0].len=5;
	 onGame.limites[0].win=5;
	 onGame.names[0].std = "Boules";
	 onGame.names[0].abv = "b";

	 /// etoiles
	 onGame.limites[1].min=1;
	 onGame.limites[1].max=12;
	 onGame.limites[1].len=2;
	 onGame.limites[1].win=2;
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
 QString st_sqldf = "";

 stCreateTable creerTables[]={
  {C_TBL_3,&BPrevision::f3},   /// Table des tirages
  {C_TBL_1,&BPrevision::f1},   /// Table des nom des zones et abregees
  {C_TBL_2,&BPrevision::f2},   /// Liste des boules par zone
  {T_CMB,&BPrevision::f4},    /// Table des combinaisons
  {T_ANA,&BPrevision::f5},    /// Table des Analyses
  {C_TBL_6,&BPrevision::f6},    /// Selection utilisateur
  {U_CMB,&BPrevision::f6},    /// Selection utilisateur
  {U_GRP,&BPrevision::f6},    /// Selection utilisateur
  {C_TBL_A,&BPrevision::f6}    /// Selection utilisateur
 };

 /// Preparation de la suppression/modification de f6
 if(isOk){
  /// Preparation nouvelle table pour supprimer ancienne def
  st_sqldf =  "create table Filtres (id Integer primary key, zne int, typ int, lgn int, col int, val int, pri int, flt int);";
  isOk = q.exec(st_sqldf);
 }

 int nbACreer = sizeof(creerTables)/sizeof(stCreateTable);
 for(int uneTable=0;(uneTable<nbACreer) && isOk;uneTable++)
 {
  /// Nom de la table
  QString tbName = creerTables[uneTable].tbDef;

	/// Fonction de traitement de la creation
	isOk=(this->*(creerTables[uneTable].pFuncInit))(tbName,&q);

	/// Analyser le retour de traitement
	if(!isOk){
	 //un message d'information
	 QMessageBox::critical(0, tbName, "Erreur traitement !",QMessageBox::Yes);
#ifndef QT_NO_DEBUG
	 qDebug() <<q.lastError().text();
#endif
	 QApplication::quit();
	}
 }

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

bool BPrevision::f3(QString tb,QSqlQuery *query)
{
 bool isOk= true;
 QString msg = "";
 QString colsDef = "";
 QString cAsDef = ""; /// column as def

 /// Cette table contient tous les tirages
 QString tbName = "B_" + tb;
 tblTirages =  tbName;

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
 QString tables[]={"tmp_"+tbName,tbName};
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
bool BPrevision::f4(QString tb, QSqlQuery *query)
{
 Q_UNUSED(query)

 bool isOk = true;
 int nbZone = onGame.znCount;

 QString tblUse = "B_" + tb;

 for (int zn=0;(zn < nbZone) && isOk;zn++ )
 {
  if(onGame.limites[zn].win>2){
   isOk = TraitementCodeVueCombi(zn);

	 if(isOk)
		isOk = TraitementCodeTblCombi(tblUse,zn);
	}
	else
	{
	 int n = onGame.limites[zn].max;
	 int p = onGame.limites[zn].win;
	 QString tbName = tblUse+ "_z"+QString::number(zn+1);
	 // calculer les combinaisons avec repetition
	 //            BCnp *a = new BCnp(n,p,db_1.connectionName(),tbName);
	 BCnp *a = new BCnp(n,p,db_1.connectionName());
	 tbName = a->getDbTblName();
	 if(tbName.isEmpty()){
		QMessageBox::information(NULL, "Pgm", "tbName is null" ,QMessageBox::Yes);
		QApplication::quit();
	 }

	 isOk = TraitementCodeTblCombi_2(tblUse,tbName,zn);
	}
 }

 if(!isOk)
 {
  QString ErrLoc = "f4:";
  DB_Tools::DisplayError(ErrLoc,NULL,"");
 }

 return isOk;
}

bool BPrevision::f5(QString tb, QSqlQuery *query)
{
 Q_UNUSED(query)

 bool isOk = true;
 int nbZone = onGame.znCount;
 slFlt = new  QStringList* [nbZone] ;
 QString tbUse = "";
 QString source = "";
 QString destination ="";

 destination = "B_"+tb;
 source = tblTirages;

 for (int zn=0;(zn < nbZone) && isOk;zn++ )
 {
  slFlt[zn] = CreateFilterForData(zn);
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

bool BPrevision::f6(QString tb, QSqlQuery *query)
{
 bool isOk = true;

 QString st_sqldf = ""; /// sql definition
 QString st_table = "";

 // Creation des tables permettant la sauvegarde des selections
 // pour creation de filtres
 int nb_zone = onGame.znCount;

 for(int zone=0;(zone<nb_zone)&& isOk;zone++)
 {
  st_table = tb + "_z"+QString::number(zone+1);
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

bool BPrevision::FaireTableauSynthese(QString tblIn, const BGame &onGame,int zn)
{
 bool isOk = true;
 QString msg = "";
 QSqlQuery query(db_1);
 QString stDefBoules = C_TBL_2;
 QString prvName = "";
 QString curName  ="";
 QString TblCompact = T_GRP;

 QString tblToUse = tblIn + "_z"+QString::number(zn+1);
#if 0
    QString viewName = "r_"
            +db_data+ "_"+ QString::number(total-1)
            +"_"+label[type]
            +"_z"+QString::number(zn+1);


    if(onGame.from == eFdj){
        TblCompact = "B_"+TblCompact;
    }
    else{
        tblToUse = tblIn +"_"T_ANA "_z"+QString::number(zn+1);
        TblCompact = tblIn + "_"+TblCompact ;
    }
#endif

 QString stCurTable = tblToUse;

 /// Verifier si des tables existent deja
 if(SupprimerVueIntermediaires())
 {
  /// Plus de table intermediaire commencer
  curName = "vt_0";
  msg = "create view if not exists "
        +curName+" as select Choix.tz"
        +QString::number(zn+1)+ " as Nb"
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
				 +" as select tbleft.*, (case when count(tbRight.id)!=0 then count(tbRight.id) end)as "
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
   }
  };

 if(onGame.type == eGameEuro){
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
  QMessageBox::critical(0, "LireLesTirages", "Erreur chargement !:\n"+fileName_2,QMessageBox::Yes);
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
#if 0
        QStringList tmp = data.split("-");
        reqValues = reqValues + "'"
                + tmp.at(2)+"/"
                + tmp.at(1)+"/"
                + tmp.at(0)+ "',";
#endif
	// Recuperation et verification du jour (J) en fonction de la date
	data = JourFromDate(data, list1.at(1),&retErr);
	if(retErr.status == false)
	{
	 msg = retErr.msg;
	 msg = "Fic:"+fileName_2+",lg:"+QString::number(nb_lignes-1)+"\n"+msg;
	 QMessageBox::critical(0, "cLesComptages::LireLesTirages", msg,QMessageBox::Yes);
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
		if((val1 >= def->param.pZn[zone].min)
				&&
				(val1 <=def->param.pZn[zone].max))
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

void BPrevision::analyserTirages(QString source,const BGame &config)
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

 /// greffon pour calculer barycentre des tirages
 stNeedsOfBary param;
 param.db = db_1;
 param.ncx = db_1.connectionName();
 param.tbl_in=source;
 if(source=="E1"){
  param.tbl_ana = tr("U_E1_ana_z1");
 }else
 {
  param.tbl_ana = source+tr("_ana_z1");
 }
 param.tbl_flt = tr("U_b_z1"); /// source+tr("_flt_z1");
 param.pDef = onGame;
 param.origine = this;
 c= new CBaryCentre(param);


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
 pConteneur[1]->addWidget(c,1,0);
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
void BPrevision::slot_UGL_ClrFilters()
{
 QSqlQuery query(db_1);
 bool isOk = true;
 QString msg = "";
 msg = "update Filtres set flt=(case when ((flt&0x"+
       QString::number(BDelegateElmOrCmb::isWanted)+")==0x"+QString::number(BDelegateElmOrCmb::isWanted)+
       " and (flt>0)) then (flt&~(0x"+QString::number(BDelegateElmOrCmb::isWanted)+")) ELSE flt END)";

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
 QString source = "E1";
 QString analys = "U_E1_ana_z1";

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
 msg = "select tb1.b1 as b1, tb1.b2 as b2, tb1.b3 as b3, tb1.b4 as b4, tb1.b5 as b5 from ("
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
 lignes->setText(tot);

}

/// slot de reemission de la boule selectionnee
void BPrevision::slot_emitThatClickedBall(const QModelIndex &index)
{
 emit sig_isClickedOnBall(index);
}

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
		 if(m == n){
			QString tbl_cible = a->getDbTblName();
			QString tbl_cible_ana = "U_E1_ana";
			ContinuerCreation(tbl_cible, tbl_cible_ana);
		 }
		 else{
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

 //monJeu;

 monJeu.type = onGame.type;
 monJeu.from = eUsr;
 monJeu.znCount = 1;
 monJeu.limites = &(onGame.limites[0]);
 monJeu.names = &(onGame.names[0]);


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
 if(!isOk) return;

 /// ---------------------
 ///  EFFFECTUER LA SUITE
 ContinuerCreation(tbl_cible, tbl_cible_ana);

 isOk = true;

}

void BPrevision::ContinuerCreation(QString tbl_cible, QString tbl_cible_ana)
{
 bool isOk=false;
 QString msg = "";

 int zn=0;
 isOk = AnalyserEnsembleTirage(tbl_cible,monJeu, zn);
 if(isOk)
  isOk = FaireTableauSynthese(tbl_cible_ana,monJeu,zn);

 analyserTirages(tbl_cible,monJeu);

 static bool OneShot = false;
 if(OneShot==false){
  OneShot = true;

	/// Montrer resultats
	msg="select * from ("+tbl_cible+")";
	QTableView *qtv_tmp = new QTableView;
	sqm_resu = new QSqlQueryModel;

	sqm_resu->setQuery(msg,db_1);
	qtv_tmp->setModel(sqm_resu);

	lignes =new QLabel;
	int nbLignes = sqm_resu->rowCount();
	QSqlQuery nvll(db_1);
	isOk=nvll.exec("select count(*) from ("+tbl_cible+")");
	if(isOk){
	 nvll.first();
	 if(nvll.isValid()){
		nbLignes = nvll.value(0).toInt();
	 }
	}
	QString tot = "Total : " + QString::number(nbLignes);
	lignes->setText(tot);

	QWidget *Affiche = new QWidget;
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(lignes,0,Qt::AlignLeft|Qt::AlignTop);
	layout->addWidget(qtv_tmp,1,Qt::AlignLeft|Qt::AlignTop);


	int nbCol = sqm_resu->columnCount();
	for(int col=0;col<nbCol;col++)
	{
	 qtv_tmp->setColumnWidth(col,CEL2_L);
	}
	qtv_tmp->setFixedHeight(700);
	qtv_tmp->setFixedWidth((nbCol+1)*CEL2_L);

	Affiche->setLayout(layout);
	Affiche->setWindowTitle("Ensemble:"+ tbl_cible);
	Affiche->show();
 }
}

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

bool BPrevision::AnalyserEnsembleTirage(QString tblIn, const BGame &onGame, int zn)
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
 if(onGame.from == eFdj){
  tbLabAna = "B_" + tbLabAna;
  tblToUse = tblTirages;
 }
 else{
  tblToUse = tblIn ;
  tbLabAna = "U_" + tblToUse + "_" +tbLabAna;
 }
 tbLabAna =tbLabAna+"_z"+QString::number(zn+1);

 QString ref="(tbleft.%1%2=tbRight.B)";

 /// sur quel nom des elements de la zone
 st_OnDef=""; /// remettre a zero pour chacune des zones
 int znLen = onGame.limites[zn].len;
 for(int j=0;j<znLen;j++)
 {
  st_OnDef = st_OnDef + ref.arg(onGame.names[zn].abv).arg(j+1);
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
          +onGame.names[zn].abv+QString::number(colId)+" as "
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

	 if(onGame.type == eGameEuro && zn == 1){
		ref_1 = "((tbLeft.U%3 = tbRight."+onGame.names[zn].abv+"%1)"
						+"or"+
						"(tbLeft.U%3 = tbRight."+onGame.names[zn].abv+"%2))";
		stLien = " and ";
	 }

	 int znLen = onGame.limites[zn].len;
	 for(int pos=0;pos<znLen;pos++){
		if(onGame.type == eGameEuro && zn == 1){
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
// Ce tableau comporte 2 elements
// Element 0 liste des requetes construites
// Element 1 Liste des titres assosies a la requete
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

