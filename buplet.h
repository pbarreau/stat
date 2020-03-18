#ifndef BUPLET_H
#define BUPLET_H

#include <QObject>
#include <QWidget>
#include <QSqlDatabase>
#include <QGroupBox>

#include <QTableView>
#include <QDialog>

#include "bvtabbar.h"

class BUplet: public QWidget
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
BUplet(st_In const &param, int index=0, eCalcul eCal=eCalNotSet,const QModelIndex &ligne=QModelIndex(), const QString & data="", QWidget *parent=0);
 ~BUplet();
 int getUpl(void);
 QString sql_UsrSelectedTirages(const QModelIndex & index, int pos);

public slots:
 void slot_Selection(const QString& lstBoules);
 void slot_FindNewUplet(const QModelIndex & index);

private:
QSqlDatabase db_0;
st_In input;
QGroupBox *gpb_upl;
QTableView *qtv_upl;
QString gpb_title;
eEnsemble useData;
QString ens_ref;
static int tot_upl;

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


};

#endif // BUPLET_H

class BUplWidget: public QWidget
{

 public:
 //BUplWidget(QString cnx, QWidget *parent=0);
 //BUplWidget(QString cnx, int index, QWidget *parent=0);
 //BUplWidget(QString cnx, QString usr_ens, QWidget *parent=0);
 BUplWidget(QString cnx, int index=0, const QModelIndex & ligne=QModelIndex(), const QString & data="", BUplet *origine=0, QWidget *parent=0);

 private:
 QString sql_lstTirCmb(int ligne, int dst);
 QString sql_lstTirBrc(int ligne, int dst);

};
