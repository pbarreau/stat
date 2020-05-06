#ifndef BGTBVIEW_H
#define BGTBVIEW_H

#include <QTableView>
#include <QGroupBox>

class BGTbView : public QTableView
{
public:
BGTbView(QWidget *parent = nullptr);
~BGTbView();
QGroupBox *getScreen();
void addUpLayout(QLayout *up_in);
int getMinWidth();
int getMinHeight();

protected:
void mouseMoveEvent( QMouseEvent * inEvent );
void setTitle(QString title);

protected:
QGroupBox *square;

private:
QLayout *up;

};

#endif // BGTBVIEW_H
