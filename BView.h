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
 int getZone();
 void setZone(int zn_in);
 void setUseTable(QString tbl = "");

 protected:
 void mouseMoveEvent( QMouseEvent * inEvent );

 protected:
 QGroupBox *square;

 private:
 QLayout *up;
 int zone;
 QString t_use;

};

#endif // BGTBVIEW_H
