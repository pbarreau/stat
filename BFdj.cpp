#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <math.h>

#include <QMessageBox>
#include <QApplication>
#include <QSqlError>
#include <QString>
#include <QFileDialog>
#include <QDate>
#include <QTextStream>

#include <QSqlDriver>
#include "sqlExtensions/inc/sqlite3.h"
#include "sqlExtensions/inc/sqlite3ext.h"

#include "db_tools.h"
#include "BFdj.h"


int BFdj::total_items = 0;
QString BFdj::dsk_db = "";

extern QString FdjDbZip;

BFdj::BFdj(stFdj *prm, QString cnx)
{


 QString use_cnx = cnx;
 QString stConfFile = "";
 bool b_retVal = true;

 cur_item = total_items;
 total_items++;
 fdjConf = nullptr;

 /// Doit on utiliser une connexion deja etablie
 if(!use_cnx.size()){
  b_retVal = ouvrirBase(prm);
 }
 else {
  // Etablir connexion a la base
  fdj_db = QSqlDatabase::database(use_cnx);
  b_retVal = fdj_db.isValid();
 }

 if(b_retVal ==false){
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
 bool b_retVal = true;

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

 dsk_db = mabase;

 /// definition de la base pour ce calcul
 fdj_db.setDatabaseName(mabase);

 // Open database
 b_retVal = fdj_db.open();

 if(b_retVal){

  QSqlQuery query(fdj_db);
  QString st_query = "select sqlite_version();";
  if((b_retVal = query.exec(st_query))){
   query.first();
   QString version =query.value(0).toString();

   if(version < "3.25"){
    st_query = QString("Version sqlite :") + version +QString(" < 3.25\n");
    QMessageBox::critical(nullptr,"Stat",st_query,QMessageBox::Ok);
    b_retVal = false;
   }
   else{
    /// Chargement librairie math
    if(!(b_retVal=AuthoriseChargementExtension())){
     st_query = QString("Chargement sqMath echec !!\n");
     QMessageBox::critical(nullptr,"Stat",st_query,QMessageBox::Ok);
    }
    else {
     /// On optimise les acces base
     b_retVal = OPtimiseAccesBase();
    }
   }
  }
 }

 return b_retVal;
}

bool BFdj::AuthoriseChargementExtension(void)
{
 bool b_retVal = true;
 QSqlQuery query(fdj_db);
 QString msg = "";

 QString cur_rep = QCoreApplication::applicationDirPath();
 QString MonDirLib = cur_rep + ".\\sqlExtensions\\lib";
 QStringList mesLibs = QCoreApplication::libraryPaths();
 mesLibs<<MonDirLib;
 //QCoreApplication::addLibraryPath(MonDirLib);
 QCoreApplication::setLibraryPaths(mesLibs);

 /// http://sqlite.1065341.n5.nabble.com/Using-loadable-extension-with-Qt-td24872.html
 /// https://arstechnica.com/civis/viewtopic.php?f=20&t=64150
 QVariant v = fdj_db.driver()->handle();

 if (v.isValid() && qstrcmp(v.typeName(), "sqlite3*")==0)

 {

  // v.data() returns a pointer to the handle
  sqlite3_initialize();
  sqlite3 *handle = *static_cast<sqlite3 **>(v.data());

  if (handle != 0) { // check that it is not NULL

   //const char *loc = NULL;
   //loc = sqlite_version();

   int ret = sqlite3_enable_load_extension(handle,1);
   //int ret = loadExt(handle,1);

   /// Lancer la requete
   //QString msg = "SELECT load_extension('./sqlExtensions/lib/libStatPgm-sqMath.dll')";
   QString msg = "SELECT load_extension('./sqlExtensions/lib/libStatPgm-extension-functions-i686.dll')";
   b_retVal = query.exec(msg);
#ifndef QT_NO_DEBUG
	 if (query.lastError() .isValid())
	 {
		foreach (const QString &path, QCoreApplication::libraryPaths())
		 qDebug() << path;

		qDebug() << "Error: cannot load extension (" << query.lastError().text()<<")";
		b_retVal = false;
	 }
#endif

  }

 }
 else
 {
  b_retVal = false;
 }
 return b_retVal;
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
 bool b_retVal = true;
 QSqlQuery query(fdj_db);
 QString msg = "";

#undef DBG_SQLITE_COMPILATION

 /// https://www.sqlite.org/pragma.html#pragma_locking_mode
 QString stRequete[]={
 #ifdef DBG_SQLITE_COMPILATION
  "PRAGMA compile_options",
 #endif
  "PRAGMA synchronous = OFF",
  "PRAGMA page_size = 4096",
  "PRAGMA cache_size = 16384",
  "PRAGMA temp_store = MEMORY",
  "PRAGMA journal_mode = OFF",
  "PRAGMA locking_mode = EXCLUSIVE" /// NORMAL
 };
 int items = sizeof(stRequete)/sizeof(QString);

 for(int i=0; (i<items)&& b_retVal ;i++){
  msg = stRequete[i];
  b_retVal = query.exec(msg);

#ifdef DBG_SQLITE_COMPILATION
  if(i==0 && b_retVal){
   query.first();
   do{
    QString value = query.value(0).toString();
   }while(query.next());
  }
#endif
 }

 if(!b_retVal)
 {
  QString ErrLoc = "BFdj::OPtimiseAccesBase";
  DB_Tools::DisplayError(ErrLoc,&query,msg);
 }

 return b_retVal;
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
 ret->db_ref->src = "";
 ret->db_ref->jrs = "";
 ret->db_ref->flt = "B_flt";

 ret->slFlt = nullptr;

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

QString BFdj::getCurDbFile(void)
{
 return dsk_db;
}

bool BFdj::crt_TblFdj(stGameConf *pGame)
{
 bool b_retVal= true;
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
 pGame->db_ref->src = tbName;
 pGame->eTirType = eTirFdj;


 QString cnx = fdj_db.connectionName();
 pGame->db_ref->cnx = cnx;

 /// Utiliser anciennes tables
 if(pGame->db_ref->ihm->use_odb==true){
  if(pGame->db_ref->ihm->fdj_new==false){
   QString jours = DB_Tools::getLstDays(cnx,tbName);
   pGame->db_ref->jrs = jours;
   return b_retVal;
  }
  else {
   /// supprimer la table tremporaire
   msg = "drop table if exists " + tbName;
   b_retVal = query.exec(msg);
  }
 }

 QString colsDef = "";
 QString cAsDef = ""; /// column as def


 int totDef=pGame->znCount;
 for(int def = 0; (def<totDef) && b_retVal;def++)
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

  b_retVal = query.exec(msg);
 }

 /// Les tables sont presentes maintenant
 if(b_retVal)
 {
  /// mettre les infos brut dans la table temporaire
  b_retVal = chargerDonneesFdjeux(pGame, tables[0]);

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

	 b_retVal = query.exec(msg);

	 if(b_retVal){
		/// supprimer la table tremporaire
		msg = "drop table if exists " + tables[0];
		b_retVal = query.exec(msg);
	 }

  }
 }

 QString jours = DB_Tools::getLstDays(cnx,tbName);
 pGame->db_ref->jrs = jours;


 return b_retVal;
}

bool BFdj::chargerDonneesFdjeux(stGameConf *pGame, QString destTable)
{
 bool b_retVal= true;

 stFdjData *LesFichiers;
 int nbelemt = 0;
 int fId = 0;


 /// avec les differentes version des jeux
 /// le format des fichiers repertoriant les resultats
 /// a change

 /// File format description : Loto
 ///pour une zone : {col depart, longueur, val_min, val_max}
 stZnDef ff_loto_1[] =
 {
  {4,5,1,49},
  {9,1,1,10}
 };
 stZnDef p2BisZn[] =
 {
  {32,5,1,49}
 };

 /// Nombre de tirage par jour et ptr vers description
 stRes resLoto[]=
 {
  {2, &ff_loto_1[0]},
  {1, &p2BisZn[0]}
 };

 stRes resLoto_1[]=
 {
  {1, &p2BisZn[0]},
  {2, &ff_loto_1[0]}
 };

 /// File format description : Euro
 stZnDef fd_euro_1[] =
 {
  {4,5,1,50},
  {9,2,1,10}
 };
 stRes resEuro_1[]={{2,&fd_euro_1[0]}};

 stZnDef fd_euro_2[] =
 {
  {4,5,1,50},
  {9,2,1,11}
 };
 stRes resEuro_2[]={{2,&fd_euro_2[0]}};

 stZnDef fd_euro_3[] =
 {
  {5,5,1,50},
  {10,2,1,12}
 };
 stRes resEuro_3[]={{2,&fd_euro_3[0]}};

 /// Liste des fichiers pour Euromillions
 fId = 0;
#if 0
 stFdjData euroMillions[]=
 {
  {"euromillions_202002.csv",fId++,
   {false,2,1,2,&fd_euro_3[0]}
  },
  {"euromillions_201902.csv",fId++,
   {false,2,1,2,&fd_euro_3[0]}
  },
  {"euromillions_4.csv",fId++,
   {false,2,1,2,&fd_euro_3[0]}
  },
  {"euromillions_3.csv",fId++,
   {false,2,1,2,&fd_euro_2[0]}
  },
  {"euromillions_2.csv",fId++,
   {false,2,1,2,&fd_euro_2[0]}
  },
  {"euromillions.csv",fId++,
   {false,2,1,2,&fd_euro_1[0]}
  }
 };
#endif

 stFdjData euroMillions[]=
 {
  /// {filename, uid,{dowload,colDate,ColJour,nbTirageJour,ptrDescTirage}}
  {"euromillions_202002.csv",fId++,{false,2,1,1,&resEuro_3[0]}},
  {"euromillions_201902.csv",fId++,{false,2,1,1,&resEuro_3[0]}},
  {"euromillions_4.csv",fId++,{false,2,1,1,&resEuro_3[0]}},
  {"euromillions_3.csv",fId++,{false,2,1,1,&resEuro_2[0]}},
  {"euromillions_2.csv",fId++,{false,2,1,1,&resEuro_2[0]}},
  {"euromillions.csv",fId++,{false,2,1,1,&resEuro_1[0]}}
 };

 /// Liste des fichiers pour loto
 fId = 0;
 stFdjData loto[]=
 {
  {"grandloto_201912.csv",fId++, {false,2,1,1,&resLoto[0]} },
  {"lotonoel2017.csv",fId++, {false,2,1,1,&resLoto[0]} },
  {"nouveau_superloto.csv",fId++, {false,2,1,1,&resLoto[0]} },
  {"superloto2017.csv",fId++, {false,2,1,1,&resLoto[0]} },
  {"superloto_201907.csv",fId++, {false,2,1,1,&resLoto[0]} },
  {"nouveau_loto.csv",fId++, {false,2,1,1,&resLoto[0]} },
  {"loto2017.csv",fId++, {false,2,1,1,&resLoto[0]} },
  {"loto_201902.csv",fId++, {false,2,1,1,&resLoto[0]} },
  {"loto_201911.csv",fId++, {false,2,1,2,&resLoto_1[0]} }
 };

 ///   {"loto.csv",fId++, {false,2,1,1,&resLoto[0]} }, 6B+1E
 ///   {"sloto.csv",fId++, {false,2,1,1,&resLoto[0]} },

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
 while((b_retVal == true) && (nbelemt>0))
 {
  b_retVal = LireLesTirages(pGame, &LesFichiers[nbelemt-1], destTable);
  nbelemt--;
 };



 return b_retVal;
}

bool BFdj::LireLesTirages(stGameConf *pGame, stFdjData *def, QString tblName)
{
 bool b_retVal= true;
 QSqlQuery query(fdj_db);

 QString fileName_2 = FdjDbZip+ "\\" +def->fname;
 QFile fichier(fileName_2);

 // On ouvre notre fichier en lecture seule et on verifie l'ouverture
 if (!fichier.open(QIODevice::ReadOnly | QIODevice::Text))
 {
  QMessageBox::critical(nullptr, "BFdj::LireLesTirages", "Erreur chargement !:\n"+fileName_2,QMessageBox::Yes);
  return false;
 }

 /// Variables traitement
 QTextStream flux(&fichier);
 QString ligne = "";
 QStringList list1;
 QString msgDateJour = "";
 QString valDateJour = "";
 QString data = "";
 QString msg = "";
 stErr2 retErr;

 // --- DEBUT ANALYSE DU FICHIER
 // Passer la premiere ligne description des champs du fichier
 ligne = flux.readLine();

 list1 = ligne.split(";");
 int refNbFields = list1.size();

 // Analyse des suivantes
 int nb_lignes=0;
 while((! flux.atEnd() )&& (b_retVal == true))
 {
  ligne = flux.readLine();
  nb_lignes++;
  msgDateJour = "";
  valDateJour = "";

  //traitement de la ligne
  list1 = ligne.split(";");

  if(list1.size() != refNbFields){
   msg = "Fichier :"+fileName_2
         +"\nErreur taille ligne !!\n\nLigne : "
         +QString::number(nb_lignes);
   QMessageBox::critical(nullptr, "BFdj::LireLesTirages", msg,QMessageBox::Yes);
   return false;
  }

  // Recuperation du date_tirage (D)
  data = DateAnormer(list1.at(def->param.colDate));
  // Presentation de la date
  msgDateJour = msgDateJour + "D,";
  valDateJour = valDateJour + "'"
                + data+ "',";

  // Recuperation et verification du jour (J) en fonction de la date
  data = JourFromDate(data, list1.at(def->param.colDay),&retErr);
  if(retErr.status == false)
  {
   msg = retErr.msg;
   msg = "Fic:"+fileName_2+",lg:"+QString::number(nb_lignes-1)+"\n"+msg;
   QMessageBox::critical(nullptr, "cLesComptages::LireLesTirages", msg,QMessageBox::Yes);
   return false;
  }
  msgDateJour = msgDateJour + "J,";
  valDateJour = valDateJour + "'"+data + "',";

  /// Parcour de chacun des resultats d'une ligne
  int nbResuLgn = def->param.nbResu;
  for(int un_resu=0;(un_resu<nbResuLgn) && b_retVal; un_resu++)
  {
   stRes *ptrResu = &(def->param.tabRes[un_resu]);
   QString msgColZn = "";
   QString msgValZn = "";

   // Nombre de zones composanrt ce resultat
   int max_zone = ptrResu->nbZone;

   /// Parcourir chacune des zones pour lires les boules
   for(int zone=0;zone< max_zone;zone++)
   {
    stZnDef *curZn = &(ptrResu->pZn[zone]);

    int maxValZone = curZn->max;
    int minValZone = curZn->min;
    int maxElmZone = curZn->len;

    for(int ElmZone=0;ElmZone < maxElmZone;ElmZone++)
    {
     // Recuperation de la valeur
     int val1 = list1.at(curZn->pos+ElmZone).toInt();

     // verification coherence
     if((val1 >= minValZone)
        &&
        (val1 <=maxValZone))
     {
      /// On rajoute a Req values
      msgColZn = msgColZn+pGame->names[zone].abv+QString::number(ElmZone+1);
      msgValZn = msgValZn + QString::number(val1);
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
      msgColZn = msgColZn + ",";
      msgValZn = msgValZn + ",";
     }

    }

    /// voir si passage a nouvelle zone
    if(zone< max_zone-1){
     msgColZn = msgColZn + ",";
     msgValZn = msgValZn + ",";
    }

   }

   ///------------------------------
   /// Toutes les zones sont faites, ecrire dans la base
   msgColZn = msgDateJour + msgColZn;
   msgValZn = valDateJour + msgValZn;
   msg = "insert into "
         +tblName+"("
         +msgColZn+",file)values("
         + msgValZn +","+QString::number(def->id)
         + ")";
#ifndef QT_NO_DEBUG
   qDebug() <<msg;
#endif
   b_retVal = query.exec(msg);

   /// Voir reultat suivant de la ligne
  }

 }  /// Fin while


 if(!b_retVal)
 {
  QString ErrLoc = "cFdjData::LireLesTirages";
  DB_Tools::DisplayError(ErrLoc,&query,msg);
 }

 return b_retVal;
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

 if(retval.left(2).toUpper() != verif.trimmed().left(2).toUpper())
 {
  retErr->status = false;
  retErr->msg = "Err:JourFromDate->" + LaDate+"\n"+retval+" != "+verif.trimmed();
 }

 return retval;
}
