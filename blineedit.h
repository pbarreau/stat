#ifndef BLINEEDIT_H
#define BLINEEDIT_H

#include <QLineEdit>
#include <QTableView>

class BLineEdit: public QLineEdit
{
 Q_OBJECT

 public:
 BLineEdit(QTableView *view);
 QTableView *getView();

 private:
 QTableView *pView;
};


#endif // BLINEEDIT_H
