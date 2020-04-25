#ifndef QT_NO_DEBUG
#include <QDebug>
#include <QSqlError>
#endif

#include <QTextStream>
#include <QRegExp>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QApplication>
#include <QDate>
#include <QFile>

#include "cnp_SansRepetition.h"
#include "cFdjData.h"
#include "db_tools.h"

int cFdjData::total = 0;
bool cFdjData::bFdjSeen[eFdjEol]={false,false,false};
const QString cFdjData::tblKey [eTblEol]={"gme","def","elm","tir","ana","cnp","grp"};

cFdjData::cFdjData(){}

cFdjData::cFdjData(cFdjData const &parent)
{
 fdj_game_cnf=parent.fdj_game_cnf;
 fdj_db=parent.fdj_db;
 fdj_cnx=parent.fdj_cnx;
 for (int i = 0; i< eTblEol; i++) {
  dbTbl[i]=parent.dbTbl[i];
 }
}


cFdjData::cFdjData(etFdj eFdjType, cFdjData *parent)//:QObject (parent)
{

 if((eFdjType !=eFdjLoto) && (eFdjType != eFdjEuro)){
  return ;
 }

 if(!parent && ((eFdjType == eFdjLoto) || (eFdjType == eFdjEuro))){
  if(bFdjSeen[eFdjType]){
   return;
  }
  bFdjSeen[eFdjType]=true;
 }

 fdj_game_cnf.eFdjType = eFdjType;
 fdj_game_cnf.id = total;
 total++;

 if(!parent){
  fdj_game_cnf.eTirType = eTirFdj;

	fdj_cnx="";
	dbTbl[eTblDef]="";
	dbTbl[eTblElm]="";
	dbTbl[eTblTir]="";
	dbTbl[eTblAna]="";
	dbTbl[eTblGrp]="";
	init(eFdjType);
 }
 else {
  fdj_game_cnf.eTirType = eTirGen;
 }

}

cFdjData::~cFdjData()
{
 QString msg = "Game "
               +QString::number(fdj_game_cnf.id).rightJustified(2,'0')
               + " ("+gameLabel[fdj_game_cnf.eFdjType]+") be destroyed !!";
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

void cFdjData::init(etFdj eFdjType)
{
 bool b_retVal = false;

 if(eFdjType == eFdjLoto){
  int nbZn = 2;
  fdj_game_cnf.znCount = nbZn;
  fdj_game_cnf.limites = new stParam_1[nbZn];
  fdj_game_cnf.names = new stParam_2[nbZn];

	for(int i =0; i<nbZn;i++){
	 fdj_game_cnf.limites[i]=loto_prm1_zn[i];
	 fdj_game_cnf.names[i]=loto_prm2_zn[i];
	}
 }

 if(eFdjType == eFdjEuro){
  int nbZn = 2;
  fdj_game_cnf.znCount = nbZn;
  fdj_game_cnf.limites = new stParam_1[nbZn];
  fdj_game_cnf.names = new stParam_2[nbZn];

	for(int i =0; i<nbZn;i++){
	 fdj_game_cnf.limites[i]=euro_prm1_zn[i];
	 fdj_game_cnf.names[i]=euro_prm2_zn[i];
	}
 }


 QString msg = "Game "
               +QString::number(fdj_game_cnf.id).rightJustified(2,'0')
               + " set for : "+gameLabel[eFdjType]+" !!";
 QMessageBox::information(nullptr,"Pgm", msg,QMessageBox::Ok);

 /// Ouvrir un fichier sqlite
 if((eFdjType!=eFdjNone) && (b_retVal = ouvrirBase(eFdjType))==true){
  b_retVal = FillDataBase();
 }
 else {
  QMessageBox::critical(nullptr,"Pgm", "Ouverture base impossible !!",QMessageBox::Close);
  QApplication::quit();
 }
}

bool cFdjData::FillDataBase(void)
{
 bool b_retVal= true;
 QSqlQuery query(fdj_db);

 stTblFill lstTbl[]={
  {tblKey[eTblGme],&cFdjData::crt_TbGme}, /// Table liste des jeux
  {tblKey[eTblDef],&cFdjData::crt_TblDef}, /// Table definition zones
  {tblKey[eTblElm],&cFdjData::crt_TblElm}, /// Table definition elments zones
  {tblKey[eTblTir],&cFdjData::crt_TblFdj}, /// Table des tirages Fdj
  {tblKey[eTblElm],&cFdjData::upd_TblElm}, /// update Table definition elments zones
  {tblKey[eTblCnp],&cFdjData::crt_TblCnp}, /// Table definition combinaison zones
  {tblKey[eTblAna],&cFdjData::crt_TblAna}, /// Table de l'analyses des tirages
  {tblKey[eTblGrp],&cFdjData::crt_TblGrp} /// Table regroupement l'analyses des tirages
 };

 int nbACreer = sizeof(lstTbl)/sizeof(stTblFill);
 for(int uneTable=0;(uneTable<nbACreer) && b_retVal;uneTable++)
 {
  /// Nom de la table
  QString tbName = lstTbl[uneTable].tbDef;

	/// Fonction de traitement de la creation
	b_retVal=(this->*(lstTbl[uneTable].pFuncInit))(tbName,&query);

	/// Analyser le retour de traitement
	if(!b_retVal){
	 QString ErrLoc = "cFdjData::FillDataBase";
	 DB_Tools::DisplayError(ErrLoc,&query,tbName);
	 QApplication::quit();
	}
 }

 return b_retVal;
}

QString cFdjData::mk_IdDsk(etFdj type, etTirType eTirtype)
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

QString cFdjData::mk_IdCnx(etFdj type, etTirType eTirtype)
{
 QString msg="cnx_NotSetYet";

 if((type <= eFdjNone) || (type>=eFdjEol)){
  etFdj err = eFdjNone;
  QMessageBox::warning(nullptr,"cFdjData","Jeu "+gameLabel[err]+" inconnu !!",QMessageBox::Ok);
  QApplication::quit();
 }

 msg=QString("cnx_V2_")+gameLabel[type];

 if(eTirtype==eTirGen){
  msg=msg+QString("-")+QString("Ref_Cnp");
 }

 msg = msg + QString("-")+QString::number(fdj_game_cnf.id).rightJustified(2,'0');

 return (msg);
}

bool cFdjData::ouvrirBase(etFdj eFdjType)
{
 bool b_retVal = true;
 etDb cible = eDbDsk;
 fdj_cnx = mk_IdCnx(eFdjType,eTirFdj);
 fdj_db = QSqlDatabase::addDatabase("QSQLITE",fdj_cnx);

 QString mabase = "";

 switch(cible)
 {
  case eDbRam:
   mabase = ":memory:";
   break;

	case eDbDsk:
	default:
	 mabase = mk_IdDsk(eFdjType,eTirFdj);
	 break;
 }

 /// definition de la base pour ce calcul
 fdj_db.setDatabaseName(mabase);

 // Open database
 b_retVal = fdj_db.open();

 if(b_retVal)
  b_retVal = OPtimiseAccesBase();


 return b_retVal;
}

bool cFdjData::OPtimiseAccesBase(void)
{
 bool b_retVal = true;
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

 for(int i=0; (i<items)&& b_retVal ;i++){
  msg = stRequete[i];
  b_retVal = query.exec(msg);
 }

 if(!b_retVal)
 {
  QString ErrLoc = "LesComptages::OPtimiseAccesBase";
  DB_Tools::DisplayError(ErrLoc,&query,msg);
 }

 return b_retVal;
}

bool cFdjData::crt_TblDef(QString tbl_name,QSqlQuery *query)
{
 bool b_retVal= true;
 QString msg = "";

#if 0
 QString tbName = gameLabel[fdj_game_cnf.eFdjType]
                  +QString("_")
                  + QString::number(fdj_game_cnf.id).rightJustified(3,'0')
                  +QString("_")
                  +tbl_name;
#endif

 QString tbName = gameLabel[fdj_game_cnf.eFdjType]
                  +QString("_fdj_")
                  +tbl_name;

 /// si la table existe deja sortir
 if(isPresentInDataBase(tbName)) return b_retVal;

 dbTbl[eTblDef] = tbName;

 QString colsDef = "min int, max int, len int, win int, abv text, std text";
 msg = "create table if not exists "
       + tbName
       + "(id integer primary key,"
       + colsDef
       +");";

 b_retVal = query->exec(msg);

 /// preparation des insertions
 msg = "insert into "
       +tbName
       +"(id,min,max,len,win,abv,std)values(NULL,:arg1, :arg2, :arg3, :arg4, :arg5, :arg6)";
 query->prepare(msg);
#ifndef QT_NO_DEBUG
 qDebug() <<msg;
#endif

 /// la table est cree mettre les infos
 if(b_retVal)
 {
  /// Parcourir toutes les definition
  for(int def = 0; (def<fdj_game_cnf.znCount) && b_retVal;def++)
  {
   query->bindValue(":arg1",fdj_game_cnf.limites[def].min);
   query->bindValue(":arg2",fdj_game_cnf.limites[def].max);
   query->bindValue(":arg3",fdj_game_cnf.limites[def].len);
   query->bindValue(":arg4",fdj_game_cnf.limites[def].win);
   query->bindValue(":arg5",fdj_game_cnf.names[def].abv);
   query->bindValue(":arg6",fdj_game_cnf.names[def].std);

	 /// executer la commande sql
	 b_retVal = query->exec();
	}
 }
 return b_retVal;
}

bool cFdjData::crt_TbGme(QString tbl_name,QSqlQuery *query)
{
 bool b_retVal= true;
 QString msg = "";

 QString tbName =  gameLabel[fdj_game_cnf.eFdjType]
                  +QString("_")
                  +tbl_name;

 /// si la table existe deja sortir
 if(isPresentInDataBase(tbName)) return b_retVal;

 dbTbl[eTblGme] = tbName;

 QString colsDef = "id integer primary key, gme int, tpe text, sel text,";
 QString argsDef ="";
 QString valsDef ="";

 for (int i = eTblDef; i< eTblEol; i++) {
  colsDef = colsDef + tblKey[i] + " text";
  argsDef = argsDef+ "arg:"+QString::number(i);
  valsDef = valsDef + "'"+gameLabel[fdj_game_cnf.eFdjType]+QString("_fdj_")+tblKey[i]+"'";
  //dbTbl[i]= gameLabel[fdj_game_cnf.eFdjType]+QString("_")+tblKey[i];
  if(i<eTblEol-1){
   colsDef = colsDef + ", ";
   argsDef = argsDef + ", ";
   valsDef = valsDef + ", ";
  }
 }

 msg = "create table if not exists "
       + tbName
       + "("
       + colsDef
       +");";

#ifndef QT_NO_DEBUG
 qDebug() << "Cols : " << colsDef;
 qDebug() << "Args : " << argsDef;
 qDebug() << "Vals : " << valsDef;
 qDebug() << "msg : " << msg;
#endif

 b_retVal = query->exec(msg);

 /// preparation des insertions
 colsDef = colsDef.remove(QRegExp("\\s+inte.*y"));
#ifndef QT_NO_DEBUG
 qDebug() << "Cols : " << colsDef;
#endif

 colsDef = colsDef.remove(QRegExp("\\s+int"));
#ifndef QT_NO_DEBUG
 qDebug() << "Cols : " << colsDef;
#endif

 colsDef = colsDef.remove(QRegExp("\\s+text"));
#ifndef QT_NO_DEBUG
 qDebug() << "Cols : " << colsDef;
#endif

 msg = "insert into "
       +tbName
       +"("+colsDef+")values(NULL, "+QString::number(fdj_game_cnf.id)
       +",'fdj', '', "+valsDef+")";
#ifndef QT_NO_DEBUG
 qDebug() <<msg;
#endif

#if 0
 query->prepare(msg);

 /// la table est cree mettre les infos
 if(b_retVal)
 {
  /// Parcourir toutes les definition
  for (int i = eTblDef; i< eTblEol; i++) {
   {
    query->bindValue(":arg"+QString::number(i),fdj_game_cnf.limites[def].min);
   }
  }
#endif

 /// executer la commande sql
 b_retVal = query->exec(msg);
 return b_retVal;
}

bool cFdjData::crt_TblElm(QString tbl_name,QSqlQuery *query)
{
 bool b_retVal= true;
 QString msg = "";
 QString tbName = gameLabel[fdj_game_cnf.eFdjType];

 if(fdj_game_cnf.eTirType==eTirFdj){
  tbName = tbName
           +QString("_fdj_")
           +tbl_name;
 }
 else {
  tbName = tbName
           +QString("_")
           + QString::number(fdj_game_cnf.id).rightJustified(3,'0')
           +QString("_")
           +tbl_name;
 }

 /// si la table existe deja sortir
 if(isPresentInDataBase(tbName)) return b_retVal;

 dbTbl[eTblElm] = tbName;

 QString colsDef = "";
 QString argsDef = "";
 QString def_1 = "z%1 int,sz%1 int,tz%1 int";
 QString def_2 = ":arg%1, :arg%2";

 int totDef=fdj_game_cnf.znCount;
 int maxElemts = 0;
 for(int def = 0; (def<totDef) && b_retVal;def++)
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
	 maxElemts = BMAX_2(fdj_game_cnf.limites[def].max,
											fdj_game_cnf.limites[def+1].max);
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

 b_retVal = query->exec(msg);

 if(b_retVal)
 {
  /// Preparer la requete Sql
  colsDef.remove("int");
  QString msg1 = "insert into "
                 +tbName
                 +"(id,"+colsDef+")values(NULL,";

	/// mettre des valeurs en sequence
	for(int line=1;(line <maxElemts+1)&& b_retVal;line++)
	{
	 QString stValues="";
	 for(int def = 0; (def<totDef) ;def++)
	 {
		int maxItems = fdj_game_cnf.limites[def].max;
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
		if(line<=fdj_game_cnf.limites[def].win+1){
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
	 b_retVal = query->exec(msg);
	}
 }


 return b_retVal;
}

bool cFdjData::crt_TblFdj(QString tbl_name,QSqlQuery *query)
{
 bool b_retVal= true;
 QString msg = "";

 QString tbName = gameLabel[fdj_game_cnf.eFdjType];

 if(fdj_game_cnf.eTirType==eTirFdj){
  tbName = tbName
           +QString("_fdj_")
           +tbl_name;
 }
 else {
  tbName = tbName
           +QString("_")
           + QString::number(fdj_game_cnf.id).rightJustified(3,'0')
           +QString("_")
           +tbl_name;
 }

 /// si la table existe deja sortir
 if(isPresentInDataBase(tbName)) return b_retVal;

 /// Cette table contient tous les tirages
 dbTbl[eTblTir] =  tbName;

 QString colsDef = "";
 QString cAsDef = ""; /// column as def


 int totDef=fdj_game_cnf.znCount;
 for(int def = 0; (def<totDef) && b_retVal;def++)
 {
  QString ref = fdj_game_cnf.names[def].abv+"%1 int";
  QString ref_2 = "printf('%02d',"+fdj_game_cnf.names[def].abv+"%1)as "+fdj_game_cnf.names[def].abv+"%1";
  int totElm = fdj_game_cnf.limites[def].len;
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
 for(int i=0; i<2 && b_retVal;i++)
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

  b_retVal = query->exec(msg);
 }

 /// Les tables sont presentes maintenant
 if(b_retVal)
 {
  /// mettre les infos brut dans la table temporaire
  b_retVal = chargerDonneesFdjeux(tables[0]);

	if(b_retVal)
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

	 b_retVal = query->exec(msg);

	 if(b_retVal){
		/// supprimer la table tremporaire
		msg = "drop table if exists " + tables[0];
		b_retVal = query->exec(msg);
	 }

  }
 }


 return b_retVal;
}

bool cFdjData::chargerDonneesFdjeux(QString destTable)
{
 bool b_retVal= true;

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

 if(fdj_game_cnf.eFdjType == eFdjEuro){
  nbelemt = sizeof(euroMillions)/sizeof(stFdjData);
  LesFichiers = euroMillions;
 }
 else
 {
  nbelemt = sizeof(loto)/sizeof(stFdjData);
  LesFichiers = loto;
 }

 // Lectures des fichiers de la Fd jeux
 while((b_retVal == true) && (nbelemt>0))
 {
  b_retVal = LireLesTirages(destTable, &LesFichiers[nbelemt-1]);
  nbelemt--;
 };



 return b_retVal;
}

bool cFdjData::LireLesTirages(QString tblName, stFdjData *def)
{
 bool b_retVal= true;
 QSqlQuery query(fdj_db);

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
 while((! flux.atEnd() )&& (b_retVal == true))
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
	int max_zone = fdj_game_cnf.znCount;
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
		 reqCols = reqCols+fdj_game_cnf.names[zone].abv+QString::number(ElmZone+1);
		 reqValues = reqValues + QString::number(val1);
		}
		else
		{
		 /// Bug pour la valeur lue
		 msg = "Fic:"+fileName_2+",lg:"+QString::number(nb_lignes-1);
		 msg= msg +"\nzn:"+QString::number(zone)+",el:"+QString::number(ElmZone);
		 msg= msg +",val:"+QString::number(val1);
		 QMessageBox::critical(nullptr, "LireLesTirages", msg,QMessageBox::Yes);
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
	b_retVal = query.exec(msg);

 }  /// Fin while


 if(!b_retVal)
 {
  QString ErrLoc = "cFdjData::LireLesTirages";
  DB_Tools::DisplayError(ErrLoc,&query,msg);
 }

 return b_retVal;
}

QString cFdjData::DateAnormer(QString input)
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

QString cFdjData::JourFromDate(QString LaDate, QString verif, stErr2 *retErr)
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

bool cFdjData::crt_TblAna(QString tbl_name,QSqlQuery *query)
{
 bool b_retVal= true;
 QString msg = "";
 QString tbName = gameLabel[fdj_game_cnf.eFdjType];

 int nbZone = fdj_game_cnf.znCount;

 if(fdj_game_cnf.eTirType==eTirFdj){
  slFlt = new  QStringList* [nbZone] ;
  for (int zn=0;(zn < nbZone) && b_retVal;zn++ )
  {
   slFlt[zn] = CreateFilterForData(zn);
  }

	tbName = tbName
					 +QString("_fdj_")
					 +tbl_name;
 }
 else {
  tbName = tbName
           +QString("_")
           + QString::number(fdj_game_cnf.id).rightJustified(3,'0')
           +QString("_")
           +tbl_name;
 }

 dbTbl[eTblAna] = tbName;

 //----
 QString source = "";
 QString destination ="";

 source = dbTbl[eTblTir];

 for (int zn=0;(zn < nbZone) && b_retVal;zn++ )
 {
  destination = tbName + "_z"+QString::number(zn+1);

	/// si la table existe deja sortir
	if(isPresentInDataBase(destination)) continue;

	b_retVal = AnalyserEnsembleTirage(source,destination,fdj_game_cnf, zn, query);
#if 0
   if(b_retVal)
    b_retVal = FaireTableauSynthese(destination,onGame,zn);
#endif
 }


 //-------


 return b_retVal;
}

bool cFdjData::crt_TblGrp(QString tbl_name,QSqlQuery *query)
{
 bool b_retVal= true;
 QString msg = "";
 QString tbName = gameLabel[fdj_game_cnf.eFdjType];
 QString tbRef = "";
 QString tbLst = "";


 if(fdj_game_cnf.eTirType==eTirFdj){
  tbName = tbName
           +QString("_fdj_")
           +tbl_name;
  tbRef = dbTbl[eTblAna];
  tbLst = dbTbl[eTblTir];
 }
 else {
  tbName = tbName
           +QString("_")
           + QString::number(fdj_game_cnf.id).rightJustified(3,'0')
           +QString("_")
           +tbl_name;
 }


 int nbZone = fdj_game_cnf.znCount;
 QStringList **pList = slFlt;
 for (int zn=0;(zn < nbZone) && b_retVal;zn++ )
 {
  int nbCalc = pList[zn][1].size();

	QString destination = tbName+"_z"+QString::number(zn+1);
	QString tbAna = tbRef+"_z"+QString::number(zn+1);

	msg = "CREATE TABLE if not exists "
				+ destination
				+" as select tb1.tz"+QString::number(zn+1)+" as Nb from ("
				+dbTbl[eTblElm] + ") as tb1 WHERE (tz"+QString::number(zn+1)+" not NULL)";
	b_retVal = query->exec(msg);

	/// faire en sequence les calculs a rajouter a cette table
	for (int item=0;(item<nbCalc) && b_retVal;item++) {

	 /// Etape 1 renomerla table
	 msg = "alter TABLE "+destination+" RENAME to old_"+destination;
	 b_retVal = query->exec(msg);

	 /// Etape 2 recreer en rajoutant colonne
	 if(b_retVal){
		QString col_name = pList[zn][1].at(item);

		msg="create TABLE if not exists "+destination+" as SELECT tb1.*, tb2.t as "
					+col_name
					+" from(old_"
					+destination
					+") as tb1 "
						"left join ( "
						"select r1.B as Id, count(*)  as T  "
						"FROM (select t1.B as B, "
						" ROW_NUMBER() OVER (PARTITION by t1.B order by t1.id) as LID, "
						" lag(id,1,0) OVER (PARTITION by t1.B order by t1.id) as my_id, "
						"(t1.id -(lag(id,1,0) OVER (PARTITION by t1.B order by t1.id))) as E, "
						"t1.*  "
						"from ( select t1."
					+col_name
					+" as B, t1.id as Id     "
						"from ("+tbAna+") as t1, "
									" (select * from "+tbLst+") as t2   "
									"where ((t1.id=t2.id)) "
									") as t1 )as r1 group by b "
									") as tb2 on (tb1.Nb=tb2.Id) ";
		b_retVal = query->exec(msg);

		/// Etape 3 supprimer table old
		if(b_retVal){
		 msg = "drop table if exists old_"+destination;
		 b_retVal = query->exec(msg);
		}
	 }
	}
 }


 //----
 return b_retVal;
}

bool cFdjData::upd_TblElm(QString tbl_name,QSqlQuery *query)
{
 bool b_retVal= true;
 QString msg = "";

 int nbZone=fdj_game_cnf.znCount;

 for(int zn=0; (zn < nbZone) && (b_retVal == true); zn++){
  QString fields = getFieldsFromZone(zn,"tb2");
  QString sZn = QString::number(zn+1);

	msg = "update " + dbTbl[eTblElm] + " as tb1 set sz"
				+sZn+"=(select T from (select tb1.z"
				+sZn+" as B, count(*) as T from ("
				+dbTbl[eTblElm]+") as tb1 left join ("+dbTbl[eTblTir]+") as tb2 on (tb1.z"
				+sZn+" in("+fields+")) group by tb1.z"
				+sZn+") as tb2 where(tb1.z"
				+sZn+"=tb2.B))";

#ifndef QT_NO_DEBUG
	qDebug() <<msg;
#endif

  b_retVal= query->exec(msg);
 }

 return b_retVal;
}

QString cFdjData::getFieldsFromZone(int zn, QString alias)
{
 int len_zn = fdj_game_cnf.limites[zn].len;

 QString use_alias = "";

 if(alias.size()){
  use_alias = alias+".";
 }
 QString ref = use_alias+fdj_game_cnf.names[zn].abv+QString("%1");
 QString st_items = "";
 for(int i=0;i<len_zn;i++){
  st_items = st_items + ref.arg(i+1);
  if(i<(len_zn-1)){
   st_items=st_items+QString(",");
  }
 }
 return   st_items;
}

bool cFdjData::isPresentInDataBase(QString table, QString schema)
{
 bool b_retVal=true;
 QSqlQuery query(fdj_db);

 if(schema.size()){
  schema=schema+".";
 }

 /// Verifier si existance table
 QString msg = "SELECT name FROM "+schema+"sqlite_master "
                                              "WHERE type='table' AND name='"+table+"';";

 if((b_retVal = query.exec(msg)))
 {
  /// A t'on une reponse
  b_retVal = query.first();
 }
 else {
  QString ErrLoc = "cFdjData::isPresentInDataBase";
  DB_Tools::DisplayError(ErrLoc,&query,msg);
 }

 return b_retVal;
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
bool cFdjData::crt_TblCnp(QString tbl_name,QSqlQuery *query)
{
 Q_UNUSED(query)

 bool b_retVal = true;
 int nbZone = fdj_game_cnf.znCount;

 QString tblUse = gameLabel[fdj_game_cnf.eFdjType]
                  +QString("_") + tbl_name;

 dbTbl[eTblCnp] = tblUse;

 for (int zn=0;(zn < nbZone) && b_retVal;zn++ )
 {
  QString tbName = tblUse+ "_z"+QString::number(zn+1);

	/// si la table existe deja sortir
	if(isPresentInDataBase(tbName)) continue;

	if(fdj_game_cnf.limites[zn].win>2){
	 b_retVal = TraitementCodeTblCombi(tblUse,zn, query);
	}
	else
	{
	 int n = fdj_game_cnf.limites[zn].max;
	 int p = fdj_game_cnf.limites[zn].win;

	 // calculer les combinaisons avec repetition
	 BCnp *a = new BCnp(n,p,fdj_cnx);
	 tbName = a->getDbTblName();
	 if(tbName.isEmpty()){
		QMessageBox::information(nullptr, "Pgm", "tbName is null" ,QMessageBox::Yes);
		QApplication::quit();
	 }

	 b_retVal = TraitementCodeTblCombi_2(tblUse,tbName,zn,query);
	}
 }
 return b_retVal;
}

bool cFdjData::TraitementCodeVueCombi(int zn,QSqlQuery *query)
{
 bool b_retVal = true;

 QString msg = "";
 QString ref_1 = "";

 QString viewCode[]=
  {
   "drop view if exists tbr%1;",
   "create view if not exists tbr%1 as select tb1.tz%1 as "
    +fdj_game_cnf.names[zn].abv
    +" from (%2 as tb1)where(tb1.tz%1 is not null);"
  };

 /// Traitement de la vue
 int nbLgnCode = sizeof(viewCode)/sizeof(QString);
 for(int lgnCode=0;(lgnCode<nbLgnCode) && b_retVal;lgnCode++){
  msg = "";
  if(lgnCode==0){
   msg = msg + viewCode[lgnCode].arg(zn+1);
  }
  else {
   msg = msg + viewCode[lgnCode].arg(zn+1).arg(dbTbl[eTblElm]);
  }

#ifndef QT_NO_DEBUG
  qDebug() << msg;
#endif

  b_retVal = query->exec(msg);
 }


 return b_retVal;
}

bool cFdjData::TraitementCodeTblCombi(QString tbName,int zn,QSqlQuery *query)
{
 bool b_retVal = true;
 QString msg = "";

 /// Creation d'une vue intermediaire
 b_retVal = TraitementCodeVueCombi(zn, query);

 if(!b_retVal) return b_retVal;

 QString tblCode[]=
  {
   "drop table if exists "+tbName+"_z%1;",
   "create table if not exists "+tbName+"_z%1 (id integer primary key,%2);",
   "insert into "+tbName+"_z%1 select NULL,%2 from (%3) where(%4="
    +QString::number(+fdj_game_cnf.limites[zn].win)+");"
  };
 int argTblCount[]={1,2,4};

 QString ref_1 = "";
 QString ref_2 = "";
 QString ref_3 = "";
 QString ref_4 = "";
 QString ref_5 = "";
 int nbLgnCode= 0;


 /// traitement creation table en fonction 10zaine
 int lenZn = floor(fdj_game_cnf.limites[zn].max/10)+1;
 ref_1="t%1."+fdj_game_cnf.names[zn].abv+" as "+fdj_game_cnf.names[zn].abv+"%1";
 QString msg1 = "";
 for(int pos=0;pos<lenZn;pos++){
  msg1 = msg1 + ref_1.arg(pos+1);
  if(pos < lenZn -1)
   msg1 = msg1 + ",";
 }

 nbLgnCode = sizeof(tblCode)/sizeof(QString);
 for(int lgnCode=0;(lgnCode<nbLgnCode) && b_retVal;lgnCode++){
  msg="";
  switch(argTblCount[lgnCode]){
   case 1:
    msg = tblCode[lgnCode].arg(zn+1);
    break;
   case 2:{
    ref_1=msg1+",";
    ref_1.replace(QRegExp("t\\d\."
                          +fdj_game_cnf.names[zn].abv
                          +"\\s+as\\s+"),"");
    ref_1.replace(",", " int,");
    ref_1=ref_1 + "tip text, poids real";
    msg = tblCode[lgnCode].arg(zn+1).arg(ref_1);
   }
   break;
   case 4:{
    ref_1="%d";
    ref_2="t%1."+fdj_game_cnf.names[zn].abv;
    ref_3="(%1*t%2."+fdj_game_cnf.names[zn].abv+")";
    ref_4="tbr%1 as t%2";
    ref_5=fdj_game_cnf.names[zn].abv+"%1";
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

  b_retVal = query->exec(msg);
 }

 /// Attendre la creation puis supprimer vue intermediaire
 int query_pos = QSql::BeforeFirstRow;
 while (query->isActive()&& b_retVal){
  query_pos = query->at();
  if(query_pos==QSql::AfterLastRow){
   break;
  }
 }

 if (b_retVal){
  msg = "drop view if exists tbr"+QString::number(zn+1);
  b_retVal = query->exec(msg);
 }

 return b_retVal;
}

/// Utiliser une table intermediaire
/// Cnp_12_2
bool cFdjData::TraitementCodeTblCombi_2(QString tbName, QString tbCnp, int zn, QSqlQuery *query)
{
 bool b_retVal = true;
 //QSqlQuery query(db_1);
 QString msg = "";

 msg = "drop table if exists "+tbName+"_z"+QString::number(zn+1);
 b_retVal = query->exec(msg);
#ifndef QT_NO_DEBUG
 qDebug() << msg;
#endif

 if(b_retVal){
  /// traitement creation table
  int lenZn = fdj_game_cnf.limites[zn].len;
  QString ref_1="c%1 int";
  QString ref_2="(%1*c%2)";
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
	ref_1 = ref_1.replace("c",fdj_game_cnf.names[zn].abv) + +",tip text, poids real" ;
	msg = "create table if not exists "
				+tbName+"_z"+QString::number(zn+1)
				+"(id integer primary key,"
				+ref_1
				+");";
#ifndef QT_NO_DEBUG
	qDebug() << msg;
#endif

	b_retVal = query->exec(msg);

	if(b_retVal){
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
	 query->exec(msg);
	}

 }


 return b_retVal;
}

// Cette fonction retourne un pointeur sur un tableau de QStringList
// Ce tableau comporte 3 elements
// Element 0 liste des requetes construites
// Element 1 Liste des titres assosies a la requete
// Element 2 Liste des tooltips assosies au titres
// En fonction de la zone a etudier les requetes sont adaptees
// pour integrer le nombre maxi de boules a prendre en compte
QStringList * cFdjData::CreateFilterForData(int zn)
{
 QStringList *sl_filter = new QStringList [3];
 QString fields = "z"+QString::number(zn+1);

 int maxElems = fdj_game_cnf.limites[zn].max;
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

bool cFdjData::AnalyserEnsembleTirage(QString tblIn, QString tblCible, const stGameConf &onGame, int zn, QSqlQuery *query)
{
 /// Verifier si des vues temporaires precedentes sont encore presentes
 /// Si oui les effacer
 /// Si non prendre la liste des criteres a appliquer sur l'ensemble
 /// puis faire tant qu'il existe un critere
 /// effectuer la selection comptage vers une nouvelle vu temporaire i
 /// quand on arrive a nombre de criteres total -1 la vue destination
 /// sera OutputTable.

 bool b_retVal = true;
 QString msg = "";
 //QSqlQuery query(db_1);
 QString stDefBoules = dbTbl[eTblElm];
 QString st_OnDef = "";
 QString tbLabAna = "";
 QString tblToUse = "";
 QString tbLabCmb = "";

 tbLabCmb = dbTbl[eTblCnp];
 if(onGame.eTirType == eTirFdj){
  //tbLabAna = fdj_ana;
  tblToUse = dbTbl[eTblTir];
 }
 else{
  tblToUse = tblIn ;
  //tbLabAna = tblCible;
 }
 tbLabAna = tblCible;

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
	 b_retVal = query->exec(msg);

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
	}while(loop < nbTot && b_retVal);

	/// On a fait toutes les recherches simples
	/// faire les recherches complexes
	if(b_retVal){
	 b_retVal = add_TblAna_IdComb(zn, curTarget, tbLabAna, query);
#if 0
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
    b_retVal = query->exec(msg);
#endif
  }

	/// supression tables intermediaires
	if(b_retVal){
	 curTarget = curTarget.remove("view");
	 msg = "drop view if exists " + curTarget;
	 b_retVal= query->exec(msg);

	 if(b_retVal)
		b_retVal = SupprimerVueIntermediaires();
	}
 }

 if(!b_retVal)
 {
  QString ErrLoc = "cFdjData::AnalyserEnsembleTirage:";
  DB_Tools::DisplayError(ErrLoc,query,msg);
 }
 return b_retVal;
}

bool cFdjData::add_TblAna_IdComb(int zn, QString source, QString destination, QSqlQuery *query)
{
 bool b_retVal = true;
 const stGameConf &onGame = fdj_game_cnf;

 /// mise en correspondance de la reference combinaison
 QString msg = "";
 QString ref_1 = "";
 QString stCombi = "";
 QString stLien = "";
 QString  tbLabCmb = dbTbl[eTblCnp];

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
 QString curTarget = source.remove("view");
 msg = "create table if not exists "+destination
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
 b_retVal = query->exec(msg);

 return b_retVal;
}

bool cFdjData::SupprimerVueIntermediaires(void)
{
 bool b_retVal = true;
 QString msg = "";
 QSqlQuery query(fdj_db);
 QSqlQuery qDel(fdj_db);

 msg = "SELECT name FROM sqlite_master "
       "WHERE type='view' AND name like'vt_%';";
 b_retVal = query.exec(msg);

 if(b_retVal)
 {
  query.first();
  if(query.isValid())
  {
   /// il en existe donc les suprimer
   do
   {
    QString viewName = query.value("name").toString();
    msg = "drop view if exists "+viewName;
    b_retVal = qDel.exec(msg);
   }while(query.next()&& b_retVal);
  }
 }

 if(!b_retVal)
 {
  QString ErrLoc = "cFdjData::SupprimerVueIntermediaires:";
  DB_Tools::DisplayError(ErrLoc,&query,msg);
 }

 return b_retVal;
}
