#ifndef BGTBVIEW_H
#define BGTBVIEW_H

#include <QTableView>
#include <QGroupBox>

#include "ns_upl.h"

class BView : public QTableView
{
 Q_OBJECT

 public:
 BView(int in_zn = -2, etCount in_typ = E_CountToSet);
 ~BView();
 void setTitle(QString title, bool visible=true);
 QString getTitle(void);
 QString getTblName(void);
 QGroupBox *getScreen(bool spacer=false);
 void addUpLayout(QLayout *up_in);
 int getMinWidth(int delta=2);
 int getMinHeight();
 int getLid();
 int setLid(int l_in);
 int getZid();
 void setZid(int z_in);
 etCount getCid();
 int getGid();
 void setGid(int g_in);
 void setUseTable(QString tbl = "");
 void *getUserDataPtr(void);
 void setUserDataPtr(void *userDataPtr);

 protected:
 void mouseMoveEvent( QMouseEvent * inEvent );

 protected:
 QGroupBox *square;
 int l_id; /// represente la position de la ligne dans les tirages
 int z_id;
 int g_id;
 etCount countType;
 QString tbInDb;

 private:
 QLayout *up;
 void * userData;
};

#endif // BGTBVIEW_H
