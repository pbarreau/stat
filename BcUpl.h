#ifndef BUPLET_H
#define BUPLET_H

#include <QObject>
#include <QWidget>
#include <QSqlDatabase>
#include <QGroupBox>

#include <QLineEdit>
#include <QTableView>

#include <QTableView>
#include <QDialog>

#include "bvtabbar.h"
#include "game.h"
#include "BView.h"
#include "Bc.h"

#define  C_MIN_UPL 1
#define  C_MAX_UPL 3

class BcUpl: public BCount
{
 Q_OBJECT

 public:
 typedef struct _stIn{
  int uplet;   /// Valeur du n-uplet
  QString cnx; /// Nom de la connexion
  int dst;     /// distance vis a vis uplet
 }st_In;

 typedef enum _eEnsemble /// Ensemble dans lequel chercher les uplets
 {
  eEnsNotSet, /// Ensemble non defini
  eEnsFdj,    /// Liste des tirages de la fdj
  eEnsUsr     /// Selection de l'utilisateur
 }eEnsemble;

 typedef enum _eCalcul
 {
  eCalNotSet,
  eCalTot, /// Calcul sur boule
  eCalCmb, /// Calcul sur Combinaison
  eCalBrc  /// Calcul sur barycentre
 }eCalcul;

 public:
 //BUplet(st_In const &param);
 //BUplet(st_In const &param, int index=0);
 //BUplet(st_In const &param, QString ensemble="");
 explicit BcUpl(const stGameConf *pGame, const int nb=2, const QString tbl="tb6");
 BcUpl(st_In const &param, int index=0, eCalcul eCal=eCalNotSet,const QModelIndex &ligne=QModelIndex(), const QString & data="", QWidget *parent=nullptr);
 ~BcUpl();
 int getUpl(void);
 QString sql_UsrSelectedTirages(const QModelIndex & index, int pos);
 QString findUplets(const stGameConf *pGame, const int zn =0, const int loop=2, const int key=-1, QString tb_def="tb6", const int ref_day=1, const int delta=-1);

 public slots:
 void slot_Selection(const QString& lstBoules);
 void slot_FindNewUplet(const QModelIndex & index);

 private slots:
 void BSlot_clicked(const QModelIndex &index);

 private:
 QSqlDatabase db_0;
 st_In input;
 QGroupBox *gpb_upl;
 QTableView *qtv_upl;
 QString gpb_title;
 eEnsemble useData;
 QString ens_ref;
 static int tot_upl;

 /*
 private:
 int upl_items;
 int upl_zn;
 QString upl_tbInternal;
 BGTbView **upl_TbView;
 */

 private:
 QGroupBox *gpbCreate(int index, eCalcul eCal, const QModelIndex & ligne, const QString &data, QWidget *parent);
 QTableView *doTabShowUplet(QString tbl_src, const QModelIndex &ligne);
 int  getNbLines(QString tbl_src);
 QString getUpletFromIndex(int nb_uplet, int index, QString tbl_src);
 QString getBoulesTirage(int index);

 QString getJourTirage(int index);
 QString getCmbTirage(int index);
 QString getBrcTirage(int index);

 //bool DoCreateTblUplet(QString tbl);
 QString FN2_getFieldsFromZone(int zn, QString alias);

 bool do_SqlCnpCount(int uplet_id);
 QString sql_CnpMkUplet(int nb, QString col, QString tbl_in="B_elm");
 QString sql_CnpCountUplet(int nb, QString tbl_cnp, QString tbl_in="B_fdj");
 QString sql_UsrCountUplet(int nb, QString tbl_cnp, QString tbl_in="B_fdj");

 private:
 virtual  QTabWidget *startCount(const stGameConf *pGame, const etCount eCalcul);
 virtual bool usr_MkTbl(const stGameConf *pDef, const stMkLocal prm, const int zn);
 virtual void usr_TagLast(const stGameConf *pGame, BView_1 *view, const etCount eType, const int zn);
 //virtual QLayout * usr_UpperItems(int zn, BTbView *cur_tbv);
 QGridLayout *Compter(QString * pName, int zn);

 private:
 QWidget *getMainTbv(const stGameConf *pGame, int zn, int i);
 QWidget *showUplFromRef(const stGameConf *pGame, int zn, int upl_ref);
 QWidget *getUplDetails(const stGameConf *pGame, int zn, int src_upl, int relativeDay, int nb_recherche);
 QWidget *calUplFromDistance(const stGameConf *pGame, int zn, int src_upl, int relativeDay, int dst_upl);


};

#endif // BUPLET_H

class BUplWidget: public QWidget
{

 public:
 //BUplWidget(QString cnx, QWidget *parent=0);
 //BUplWidget(QString cnx, int index, QWidget *parent=0);
 //BUplWidget(QString cnx, QString usr_ens, QWidget *parent=0);
 BUplWidget(QString cnx, int index=0, const QModelIndex & ligne=QModelIndex(), const QString & data="", BcUpl *origine=0, QWidget *parent=0);

 private:
 QString sql_lstTirCmb(int ligne, int dst);
 QString sql_lstTirBrc(int ligne, int dst);

};

