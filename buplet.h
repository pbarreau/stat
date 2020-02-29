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


public:
BUplet(st_In const &param, int index=0);
 ~BUplet();

public slots:
 void slot_Selection(const QString& lstBoules);

private:
QSqlDatabase db_0;
st_In input;
QGroupBox *gpb_upl;
QTableView *qtv_upl;
QString gpb_title;

private:
QGroupBox *gpbCreate(int index);
QTableView *doTabShowUplet(QString tbl_src, int index);
int  getNbLines(QString tbl_src);
QString getUpletFromIndex(int nb_uplet, int index, QString tbl_src);
QString getBoulesTirage(int index);
};

#endif // BUPLET_H

class BUplWidget: public QWidget
{

 public:
 BUplWidget(QString cnx, int index = 0, QWidget *parent=0);

};
