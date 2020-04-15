#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QMessageBox>
#include <QApplication>
#include <QSqlError>
#include <QString>
#include <QFileDialog>
#include <QDate>

#include "db_tools.h"
#include "BFdj.h"


int BFdj::total_items = 0;

BFdj::BFdj(stFdj *prm, QString cnx)
{


 QString use_cnx = cnx;
 QString stConfFile = "";
 bool isOk = true;

 cur_item = total_items;
 total_items++;
 fdjConf = nullptr;

 /// Doit on utiliser une connexion deja etablie
 if(!use_cnx.size()){
  isOk = ouvrirBase(prm);
 }
 else {
  // Etablir connexion a la base
  fdj_db = QSqlDatabase::database(use_cnx);
  isOk = fdj_db.isValid();
 }

 if(isOk ==false){
  QString str_error = fdj_db.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }

 stGameConf *curConf = init(prm);
 crt_TblFdj(curConf);

 fdjConf = curConf;
}

stGameConf * BFdj::getConfig()
{
 return fdjConf;
}


/// ---------------- PRIVATE FUNCTIONS -----------------
bool BFdj::ouvrirBase(stFdj *prm)
{
 bool isOk = true;

 QString use_cnx = mk_IdCnx(prm->typeJeu);
 fdj_db = QSqlDatabase::addDatabase("QSQLITE",use_cnx);

 etFdj game = prm->typeJeu;
 QString mabase = "";

 switch(prm->db_type)
 {
  case eDbRam:
   mabase = ":memory:";
   break;

	case eDbDsk:
	default:
	 /// Reutiliser existant ?
	 if(prm->use_odb){
		QString myTitle = "Selectionnner un fichier " + gameLabel[game];
		QString myFilter = gameLabel[game]+QString(DB_VER)+"*.sqlite";
		mabase = QFileDialog::getOpenFileName(nullptr,myTitle,".",myFilter);
	 }
	 break;
 }

 if(mabase.isEmpty()){
  /// pas de selection de base pre remplie
  mabase = mk_IdDsk(game);
 }

 /// definition de la base pour ce calcul
 fdj_db.setDatabaseName(mabase);

 // Open database
 isOk = fdj_db.open();

 if(isOk)
  isOk = OPtimiseAccesBase();


 return isOk;
}

QString BFdj::mk_IdCnx(etFdj type)
{
 QString msg="cnx_NotSetYet";

 if((type <= eFdjNone) || (type>=eFdjEol)){
  etFdj err = eFdjNone;
  QMessageBox::warning(nullptr,"BFdj","Jeu "+gameLabel[err]+" inconnu !!",QMessageBox::Ok);
  QApplication::quit();
 }

 msg=QString("cnx")+QString(DB_VER)+gameLabel[type];
 msg = msg + QString("-")+QString::number(cur_item).rightJustified(2,'0');

 return (msg);
}

QString BFdj::mk_IdDsk(etFdj type)
{
 QDate myDate = QDate::currentDate();
 QString toDay = myDate.toString("dd-MM-yyyy");
 QString game = "";

 QFile myFileName;
 QString testName = "";
 QString ext=".sqlite";

 game = gameLabel[type] + QString(DB_VER);

 game = game +toDay+QString("_");

 int counter = 0;
 do{
  testName = game + QString::number(counter).rightJustified(3,'0')+ ext;
   myFileName.setFileName(testName);
  counter = (counter + 1)%999;
 }while(myFileName.exists());


 return testName;
}

bool BFdj::OPtimiseAccesBase(void)
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
  QString ErrLoc = "BFdj::OPtimiseAccesBase";
  DB_Tools::DisplayError(ErrLoc,&query,msg);
 }

 return isOk;
}

stGameConf * BFdj::init(stFdj *prm)
{
 stGameConf * ret = new stGameConf;
 etFdj eFdjType = prm->typeJeu;
 bool prev = false;

 ret->id = cur_item;
 ret->bUseMadeBdd = prev;
 ret->eFdjType = eFdjType;
 ret->eTirType = eTirNotSet;

 ret->db_ref = new stParam_3;
 ret->db_ref->ihm = prm;
 ret->db_ref->cnx = "";
 ret->db_ref->fdj = "";

 if(eFdjType == eFdjLoto){
  int nbZn = 2;
  ret->znCount = nbZn;
  ret->limites = new stParam_1[nbZn];
  ret->names = new stParam_2[nbZn];

	for(int i =0; i<nbZn;i++){
	 ret->limites[i]=loto_prm1_zn[i];
	 ret->names[i]=loto_prm2_zn[i];
	}
 }

 if(eFdjType == eFdjEuro){
  int nbZn = 2;
  ret->znCount = nbZn;
  ret->limites = new stParam_1[nbZn];
  ret->names = new stParam_2[nbZn];

	for(int i =0; i<nbZn;i++){
	 ret->limites[i]=euro_prm1_zn[i];
	 ret->names[i]=euro_prm2_zn[i];
	}
 }

 return ret;
}

bool BFdj::crt_TblFdj(stGameConf *pGame)
{
 bool isOk= true;
 QSqlQuery query(fdj_db);
 QString msg = "";

 QString tbName = "";

#ifdef FDJ_FIXED
 tbName = "B_fdj";
#else
 tbName = QString("B_")
          +gameLabel[typeJeu]+"_"
          + QString::number(cur_item).rightJustified(2,'0');
#endif

 pGame->db_ref->fdj = tbName;
 pGame->db_ref->cnx = fdj_db.connectionName();

 /// Utiliser anciennes tables
 if(pGame->db_ref->ihm->use_odb==true){
  if(pGame->db_ref->ihm->fdj_new==false){
   return isOk;
  }
  else {
   /// supprimer la table tremporaire
   msg = "drop table if exists " + tbName;
   isOk = query.exec(msg);
  }
 }

 QString colsDef = "";
 QString cAsDef = ""; /// column as def


 int totDef=pGame->znCount;
 for(int def = 0; (def<totDef) && isOk;def++)
 {
  QString ref = pGame->names[def].abv+"%1 int";
  QString ref_2 = "printf('%02d',"+pGame->names[def].abv+"%1)as "+pGame->names[def].abv+"%1";
  int totElm = pGame->limites[def].len;
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
#ifndef QT_NO_DEBUG
 qDebug() <<"colsDef:"<<colsDef;
 qDebug() <<"cAsDef:"<<cAsDef;
#endif

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

  isOk = query.exec(msg);
 }

 /// Les tables sont presentes maintenant
 if(isOk)
 {
  /// mettre les infos brut dans la table temporaire
  isOk = chargerDonneesFdjeux(pGame, tables[0]);

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

	 isOk = query.exec(msg);

	 if(isOk){
		/// supprimer la table tremporaire
		msg = "drop table if exists " + tables[0];
		isOk = query.exec(msg);
	 }

  }
 }


 return isOk;
}

bool BFdj::chargerDonneesFdjeux(stGameConf *pGame, QString destTable)
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
   {"nouveau_superloto.csv",fId++,
    {false,2,1,2,&p2Zn[0]}
   },
   {"superloto2017.csv",fId++,
    {false,2,1,2,&p2Zn[0]}
   },
   {"superloto_201907.csv",fId++,
    {false,2,1,2,&p2Zn[0]}
   },
   {"grandloto_201912.csv",fId++,
    {false,2,1,2,&p2Zn[0]}
   },
   {"lotonoel2017.csv",fId++,
    {false,2,1,2,&p2Zn[0]}
   },
   {"nouveau_loto.csv",fId++,
    {false,2,1,2,&p2Zn[0]}
   },
   {"loto2017.csv",fId++,
    {false,2,1,2,&p2Zn[0]}
   },
   {"loto_201902.csv",fId++,
    {false,2,1,2,&p2Zn[0]}
   },
   {"loto_201911.csv",fId++,
    {false,2,1,2,&p2Zn[0]}
   }
  };

 if(pGame->eFdjType == eFdjEuro){
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
  isOk = LireLesTirages(pGame, &LesFichiers[nbelemt-1], destTable);
  nbelemt--;
 };



 return isOk;
}

bool BFdj::LireLesTirages(stGameConf *pGame, stFdjData *def, QString tblName)
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
	int max_zone = pGame->znCount;
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
		 reqCols = reqCols+pGame->names[zone].abv+QString::number(ElmZone+1);
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
  QString ErrLoc = "cFdjData::LireLesTirages";
  DB_Tools::DisplayError(ErrLoc,&query,msg);
 }

 return isOk;
}

QString BFdj::DateAnormer(QString input)
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

QString BFdj::JourFromDate(QString LaDate, QString verif, stErr2 *retErr)
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
