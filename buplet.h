#ifndef BUPLET_H
#define BUPLET_H

#include <QObject>
#include <QWidget>
#include <QSqlDatabase>
#include <QGroupBox>
#include <QTableView>
#include <QDialog>

class BUplet: public QWidget
{
 Q_OBJECT

public:
typedef struct _stIn{
 int uplet;   /// Valeur du n-uplet
 QString cnx; /// Nom de la connexion
}st_In;


public:
BUplet(st_In const &param);
 ~BUplet();

private:
QSqlDatabase db_0;
st_In input;

private:
QGroupBox *gpbCreate();
QTableView *doTabShowUplet();
};

#endif // BUPLET_H
