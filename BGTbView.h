#ifndef BGTBVIEW_H
#define BGTBVIEW_H

#include <QTableView>
#include <QGroupBox>

class BGTbView : public QTableView
{
public:
BGTbView(QWidget *parent = nullptr);
~BGTbView();
void setTitle(QString title, bool visible=true);
QGroupBox * getSquare(void);
QGroupBox *getScreen(bool spacer=false);
void addUpLayout(QLayout *up_in);
int getMinWidth(int delta=3);
int getMinHeight();

protected:
void mouseMoveEvent( QMouseEvent * inEvent );

protected:
QGroupBox *square;

private:
QLayout *up;

};

#endif // BGTBVIEW_H
