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
}st_In;

typedef enum _eEnsemble /// Ensemble dans lequel chercher les uplets
{
 eEnsNotSet, /// Ensemble non defini
 eEnsFdj,    /// Liste des tirages de la fdj
 eEnsUsr     /// Selection de l'utilisateur
}eEnsemble;

public:
BUplet(st_In const &param);
BUplet(st_In const &param, int index=0);
BUplet(st_In const &param, QString ensemble="");
BUplet(st_In const &param, int index,QString ensemble);
 ~BUplet();

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
static int usrEnsCounter;

private:
QGroupBox *gpbCreate(int index);
QTableView *doTabShowUplet(QString tbl_src);
int  getNbLines(QString tbl_src);
QString getUpletFromIndex(int nb_uplet, int index, QString tbl_src);
QString getBoulesTirage(int index);
QString getJourTirage(int index);
//bool DoCreateTblUplet(QString tbl);
QString FN2_getFieldsFromZone(int zn, QString alias);

bool do_SqlCnpCount(int uplet_id);
QString sql_CnpMkUplet(int nb, QString col, QString tbl_in="B_elm");
QString sql_CnpCountUplet(int nb, QString tbl_cnp, QString tbl_in="B_fdj");
QString sql_UsrCountUplet(int nb, QString tbl_cnp, QString tbl_in="B_fdj");


QString sql_UsrSelectedTirages(const QModelIndex & index, QObject * origine);
};

#endif // BUPLET_H

class BUplWidget: public QWidget
{

 public:
 //BUplWidget(QString cnx, QWidget *parent=0);
 //BUplWidget(QString cnx, int index, QWidget *parent=0);
 //BUplWidget(QString cnx, QString usr_ens, QWidget *parent=0);
 BUplWidget(QString cnx, int index=0, QString usr_ens="", QWidget *parent=0);

};
