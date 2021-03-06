#ifndef BGTBVIEW_H
#define BGTBVIEW_H

#include <QTableView>
#include <QGroupBox>

class BGTbView : public QTableView
{
 Q_OBJECT

 public:
 BGTbView(QWidget *parent = nullptr);
 ~BGTbView();
 void setTitle(QString title, bool visible=true);
 QGroupBox * getSquare(void);
 QGroupBox *getScreen(bool spacer=false);
 void addUpLayout(QLayout *up_in);
 int getMinWidth(int delta=2);
 int getMinHeight();
 int getZone();
 void setZone(int zn_in);

 protected:
 void mouseMoveEvent( QMouseEvent * inEvent );

 protected:
 QGroupBox *square;

 private:
 QLayout *up;
 int zone;

};

#endif // BGTBVIEW_H
