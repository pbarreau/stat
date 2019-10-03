#include <QMessageBox>
#include <QSqlDatabase>
#include <QApplication>
#include <QDate>
#include <QFile>

#include "game.h"
#include "db_tools.h"

int cGame::total = 0;

const stParam_1 loto_prm1_zn[2]={{5,1,49,5,0},{1,1,10,1,0}};
const stParam_2 loto_prm2_zn[2]={{"boules","b","none"},{"etoiles","e","none"}};
const stParam_1 euro_prm1_zn[2]={{5,1,50,5,0},{1,1,12,2,0}};
const stParam_2 euro_prm2_zn[2]={{"boules","b","none"},{"etoiles","e","none"}};

const QString gameLabel [eFdjEol]={
 "NonDefini",
 "Loto",
 "Euro"
};

cGame::cGame():cGame(eFdjNotSet, eTirNotSet){}
cGame::cGame(etFdjType eFdjType):cGame(eFdjType, eTirNotSet){}

cGame::cGame(etFdjType eFdjType , etTirType eTirType)
{
 pGame = new stGameConf;

 if(pGame){
  pGame->id = total;
  total++;

  pGame->eFdjType = eFdjType;
  pGame->eTirType = eTirType;

  if(eFdjType==eFdjNotSet){
   pGame->znCount=0;
   pGame->limites = nullptr;
   pGame->names = nullptr;
   QMessageBox::information(nullptr,"Pgm","Game not set yet !!",QMessageBox::Ok);
  }
  else{
   init(eFdjType);
  }
 }
 else {
  QMessageBox::critical(nullptr,"Pgm","Out of memory !!",QMessageBox::Cancel);
 }
}

cGame::~cGame()
{
 QString msg = "Game "
               +QString::number(pGame->id).rightJustified(2,'0')
               + " ("+gameLabel[pGame->eFdjType]+") be destroyed !!";
 int ret= QMessageBox::information(nullptr,"Pgm", msg,QMessageBox::Ok|QMessageBox::Cancel);

 if(ret == QMessageBox::Ok){
  delete pGame->limites;
  delete pGame->names;

  delete pGame;
  total--;
 }
}

void cGame::init(etFdjType eFdjType)
{

 if(eFdjType == eFdjLoto){
  int nbZn = 2;
  pGame->znCount = nbZn;
  pGame->limites = new stParam_1[nbZn];
  pGame->names = new stParam_2[nbZn];

  for(int i =0; i<nbZn;i++){
   pGame->limites[i]=loto_prm1_zn[i];
   pGame->names[i]=loto_prm2_zn[i];
  }
 }

 if(eFdjType == eFdjEuro){
  int nbZn = 2;
  pGame->znCount = nbZn;
  pGame->limites = new stParam_1[nbZn];
  pGame->names = new stParam_2[nbZn];

  for(int i =0; i<nbZn;i++){
   pGame->limites[i]=euro_prm1_zn[i];
   pGame->names[i]=euro_prm2_zn[i];
  }
 }

 QString msg = "Game "
               +QString::number(pGame->id).rightJustified(2,'0')
               + " set for : "+gameLabel[eFdjType]+" !!";
 QMessageBox::information(nullptr,"Pgm", msg,QMessageBox::Ok);

 /// Ouvrir un fichier sqlite
 if((pGame->id==0) && (pGame->eFdjType != eFdjNotSet)){
  if(ouvrirBase(eFdjType)){
   //loadFdjData();
  }
 }
}

QString cGame::mk_IdDsk(etFdjType type, etTirType eTirtype)
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
   testName = game + QString::number(counter).rightJustified(3,'0');
   myFileName.setFileName(testName);
   counter = (counter + 1)%999;
  }while(myFileName.exists());
 }

 if(eTirtype==eTirGen){
  testName=game+QString("Ref_Cnp");
 }

 testName=testName+QString(".sqlite");

 return testName;
}

QString cGame::mk_IdCnx(etFdjType type, etTirType eTirtype)
{
 QString msg="cnx_NotSetYet";

 if((type <= eFdjNotSet) || (type>=eFdjEol)){
  etFdjType err = eFdjNotSet;
  QMessageBox::warning(nullptr,"Prevision","Jeu "+gameLabel[err]+" inconnu !!",QMessageBox::Ok);
  QApplication::quit();
 }

 msg=QString("cnx_V2_")+gameLabel[type];

 if(eTirtype==eTirGen){
  msg=msg+QString("-")+QString("Ref_Cnp");
 }

 msg = msg + QString("-")+QString::number(pGame->id).rightJustified(2,'0');

 return (msg);
}

bool cGame::ouvrirBase(etFdjType eFdjType)
{
 bool isOk = true;
 etDbPlace cible = eDbSetOnRam;
 QString cnx_db_1 = mk_IdCnx(eFdjType,eTirFdj);
 db = QSqlDatabase::addDatabase("QSQLITE",cnx_db_1);

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
 db.setDatabaseName(mabase);

 // Open database
 isOk = db.open();

 if(isOk)
  isOk = OPtimiseAccesBase();


 return isOk;
}

bool cGame::OPtimiseAccesBase(void)
{
 bool isOk = true;
 QSqlQuery query(db);
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
