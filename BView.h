#ifndef BGTBVIEW_H
#define BGTBVIEW_H

#include <QTableView>
#include <QGroupBox>

class BView : public QTableView
{
 Q_OBJECT

 public:
 BView(QWidget *parent = nullptr);
 ~BView();
 void setTitle(QString title, bool visible=true);
 QString getTitle(void);
 QString getTblName(void);
 QGroupBox *getScreen(bool spacer=false);
 void addUpLayout(QLayout *up_in);
 int getMinWidth(int delta=2);
 int getMinHeight();
 int getZid();
 void setZid(int z_in);
 int getGid();
 void setGid(int g_in);
 void setUseTable(QString tbl = "");
 void *getUserDataPtr(void);
 void setUserDataPtr(void *userDataPtr);

 protected:
 void mouseMoveEvent( QMouseEvent * inEvent );

 protected:
 QGroupBox *square;

 private:
 QLayout *up;
 void * userData;
 int z_id;
 int g_id;
 QString tbInDb;
};

#endif // BGTBVIEW_H
