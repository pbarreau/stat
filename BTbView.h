#ifndef BTBVIEW_H
#define BTBVIEW_H

#include <QTableView>
#include <QGroupBox>
#include <QSqlDatabase>

#include "bstflt.h"

class BTbView : public QTableView
{
 Q_OBJECT

 public :
 BTbView(int in_zn, etCount in_typ, QString cnx, QTableView *parent=nullptr);
 QGroupBox * getScreen();
 void updateTitle();

 ~BTbView();

 private:
 QString mkTitle(int zn, etCount eCalcul, QTableView *view);

 private:
 QSqlDatabase db_tbv;
 int zn;
 etCount cal;
 QGroupBox *myGpb;
};

#endif // BTBVIEW_H
