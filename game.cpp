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
 fdj_game_cnf=parent.fdj_game_cnf;
 fdj_db=parent.fdj_db;
 fdj_cnx=parent.fdj_cnx;
 fdj_def=parent.fdj_def;
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



 fdj_game_cnf.eFdjType = eFdjType;
 fdj_game_cnf.eTirType = eTirFdj;
 fdj_game_cnf.id = total;
 total++;

 if(!parent){
  fdj_cnx="";
  fdj_def="";
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

void cFdjData::init(etFdjType eFdjType)
{
 bool isOk = false;

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

 msg = msg + QString("-")+QString::number(fdj_game_cnf.id).rightJustified(2,'0');

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

 QString tbName = gameLabel[fdj_game_cnf.eFdjType]
                  +QString("_")
                  + QString::number(fdj_game_cnf.id).rightJustified(3,'0')
                  +QString("_")
                  +tbl_name;

 fdj_def = tbName;

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
  for(int def = 0; (def<fdj_game_cnf.znCount) && isOk;def++)
  {
   query->bindValue(":arg1",fdj_game_cnf.limites[def].min);
   query->bindValue(":arg2",fdj_game_cnf.limites[def].max);
   query->bindValue(":arg3",fdj_game_cnf.limites[def].len);
   query->bindValue(":arg4",fdj_game_cnf.limites[def].win);
   query->bindValue(":arg5",fdj_game_cnf.names[def].abv);
   query->bindValue(":arg6",fdj_game_cnf.names[def].std);

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

 QString tbName = gameLabel[fdj_game_cnf.eFdjType]
                  +QString("_")
                  + QString::number(fdj_game_cnf.id).rightJustified(3,'0')
                  +QString("_")
                  +tbl_name;

 fdj_elm = tbName;

 QString colsDef = "";
 QString argsDef = "";
 QString def_1 = "z%1 int,sz%1 int,tz%1 int";
 QString def_2 = ":arg%1, :arg%2";

 int totDef=fdj_game_cnf.znCount;
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
	 isOk = query->exec(msg);
	}
 }


 return isOk;
}

bool cFdjData::crt_TblFdj(QString tbl_name,QSqlQuery *query)
{
 bool isOk= true;
 QString msg = "";

 QString tbName = gameLabel[fdj_game_cnf.eFdjType]
                  +QString("_")
                  + QString::number(fdj_game_cnf.id).rightJustified(3,'0')
                  +QString("_")
                  +tbl_name;

 /// Cette table contient tous les tirages
 fdj_lst =  tbName;

 QString colsDef = "";
 QString cAsDef = ""; /// column as def


 int totDef=fdj_game_cnf.znCount;
 for(int def = 0; (def<totDef) && isOk;def++)
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


 return isOk;
}

bool cFdjData::chargerDonneesFdjeux(QString destTable)
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
 while((isOk == true) && (nbelemt>0))
 {
  isOk = LireLesTirages(destTable, &LesFichiers[nbelemt-1]);
  nbelemt--;
 };



 return isOk;
}

bool cFdjData::LireLesTirages(QString tblName, stFdjData *def)
{
 bool isOk= true;
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
	isOk = query.exec(msg);

 }  /// Fin while


 if(!isOk)
 {
  QString ErrLoc = "cLesComptages::LireLesTirages";
  DB_Tools::DisplayError(ErrLoc,&query,msg);
 }

 return isOk;
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
 bool isOk= true;
 QString msg = "";

 QString tbName = gameLabel[fdj_game_cnf.eFdjType]
                  +QString("_")
                  + QString::number(fdj_game_cnf.id).rightJustified(3,'0')
                  +QString("_")
                  +tbl_name;

 return isOk;
}
