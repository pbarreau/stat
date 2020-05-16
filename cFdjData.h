#ifndef CFDJDATA_H
#define CFDJDATA_H

#include "game.h"

class cFdjData//:public QObject
{
 //Q_OBJECT

 private:
 /// Reperes pour tables a creer
 typedef enum _etTblId{
  eTblGme, /// Table liste des jeux
  eTblDef,
  eTblElm,
  eTblTir,
  eTblAna,
  eTblCnp,
  eTblGrp,
  eTblEol
 }etTblId;




 typedef struct _stErr2
 {
  bool status;
  QString msg;
 }stErr2;

 typedef struct _stTblFill
 {
  QString tbDef; /// nom de la table
  bool (cFdjData::*pFuncInit)(QString tbName,QSqlQuery *query); /// fonction traitant la creation
 }stTblFill;

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
 explicit cFdjData(void);
 explicit cFdjData(cFdjData const &parent);
 explicit cFdjData(etFdj eFdjType,cFdjData *parent=nullptr);
 ~cFdjData();

 void init(etFdj eFdjType);

 private:
 QString mk_IdDsk(etFdj type, etTir eTirtype);
 bool ouvrirBase(etFdj eFdjType);
 bool OPtimiseAccesBase(void);
 bool AuthoriseChargementExtension(void);

 bool FillDataBase(void);
 bool crt_TbGme(QString tbl_name,QSqlQuery *query);
 bool crt_TblDef(QString tbl_name,QSqlQuery *query);
 bool crt_TblElm(QString tbl_name,QSqlQuery *query);
 bool crt_TblFdj(QString tbl_name,QSqlQuery *query);
 bool upd_TblElm(QString tbl_name,QSqlQuery *query);
 bool crt_TblCnp(QString tbl_name,QSqlQuery *query);
 bool crt_TblAna(QString tbl_name,QSqlQuery *query);
 bool crt_TblGrp(QString tbl_name,QSqlQuery *query);

 bool add_TblAna_IdComb(int zn, QString source, QString destination, QSqlQuery *query);

 QStringList * CreateFilterForData(int zn);

 bool chargerDonneesFdjeux(QString destTable);
 bool LireLesTirages(QString tblName, stFdjData *def);
 QString DateAnormer(QString input);
 QString JourFromDate(QString LaDate, QString verif, stErr2 *retErr);

 bool TraitementCodeVueCombi(int zn,QSqlQuery *query);
 bool TraitementCodeTblCombi(QString tbName,int zn,QSqlQuery *query);
 bool TraitementCodeTblCombi_2(QString tbName, QString tbCnp, int zn, QSqlQuery *query);

 bool AnalyserEnsembleTirage(QString tblIn, QString tblCible, const stGameConf &onGame, int zn, QSqlQuery *query);
 bool SupprimerVueIntermediaires(void);

 protected:
 QString mk_IdCnx(etFdj type, etTir eTirtype);
 QString getFieldsFromZone(int zn, QString alias);
 bool isPresentInDataBase(QString table, QString schema="");




 protected:
 stGameConf fdj_game_cnf;
 QSqlDatabase fdj_db; /// Base de donnees pour les jeux

 QStringList **slFlt; /// Tableau des codes Sql pour analyser les tirages
 QString fdj_cnx;     /// Nom de la connexion a la base de donnee
 QString dbTbl[eTblEol]; /// Liste des noms des tables


 private:
 static int total;    /// compteur des jeux actifs
 static bool bFdjSeen[eFdjEol];
 static const QString tblKey[eTblEol];

};


#endif // CFDJDATA_H
