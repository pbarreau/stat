#ifndef BFDJ_H
#define BFDJ_H

#include <QString>
#include <QSqlDatabase>

#include "game.h"

#define DB_VER "_V3_"



class BFdj
{
 private:

 typedef struct _stErr2
 {
  bool status;
  QString msg;
 }stErr2;

 typedef struct _stZnDef
 {
  int start;  /// offset de debut zone dans fichier
  int len;    /// taille dans la zone
  int min;    /// valeur mini possible
  int max;    /// valeur maxi possible
 }stZnDef;

 typedef struct _stConfFdjData
 {
  bool wget;  /// A telecharger ?
  int ofdate; /// Offset dans fichier pour avoir la date
  int ofday;  /// Offset dans fichier pour avoir le jour
  int nbZone; /// Nb zone a lire
  stZnDef *pZn; /// Pointeur vers caracteristique de chacune des zones
 }stConfFdjData;

 /// Tirage file format
 typedef struct _stFdjData
 {
  QString fname;  /// fichier en cours de traitement
  int id;
  stConfFdjData param;
 }stFdjData;


public:
BFdj(stFdj *prm, QString cnx = "");
stGameConf *getConfig();
static QString getCurDbFile(void);

private:
 bool ouvrirBase(stFdj *prm);
 bool AuthoriseChargementExtension(void);
 bool OPtimiseAccesBase(void);
 QString mk_IdCnx(etFdj type);
 QString mk_IdDsk(etFdj type);
 stGameConf *init(stFdj *prm);

 bool crt_TblFdj(stGameConf *pGame);
 bool chargerDonneesFdjeux(stGameConf *pGame, QString destTable);
 bool LireLesTirages(stGameConf *pGame, stFdjData *def, QString tblName);
 QString DateAnormer(QString input);
 QString JourFromDate(QString LaDate, QString verif, stErr2 *retErr);

private:
static int total_items;
static QString dsk_db;

int cur_item;
QSqlDatabase fdj_db;
stGameConf *fdjConf;

};

#endif // BFDJ_H
