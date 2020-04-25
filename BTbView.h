#ifndef BTBVIEW_H
#define BTBVIEW_H

#include <QTableView>
#include <QGroupBox>
#include <QSqlDatabase>
#include <QMenu>

#include "bstflt.h"
#include "BGpbMenu.h"

class BTbView : public QTableView
{
 Q_OBJECT

 public :
 BTbView(int in_zn, etCount in_typ, QString cnx, QTableView *parent=nullptr);
 QGroupBox * getScreen();
 BGpbMenu * getGpb();
 void updateTitle();
 void setUpLayout(QLayout *usr_up);

 ~BTbView();

 /*
 protected:
 void mousePressEvent ( QMouseEvent * event ) ;

 signals:
 void sig_ShowMenu(const QGroupBox *cible);

 public slots:
 void slot_ShowMenu(const QGroupBox *cible);
*/
 private:
 QString mkTitle(int zn, etCount eCalcul, QTableView *view);
 void construireMenu(void);

 private:
 QSqlDatabase db_tbv;
 int zn;
 etCount cal;
 BGpbMenu *myGpb;
 QLayout *up;
 QMenu *menu;
};

#endif // BTBVIEW_H
