#ifndef BLINEEDIT_H
#define BLINEEDIT_H

#include <QLineEdit>
#include <QTableView>

#include "BView.h"

class BLineEdit: public QLineEdit
{
 Q_OBJECT

 public:
 BLineEdit(BView *view);
 BView *getView();

 private:
 BView *pView;
};


#endif // BLINEEDIT_H
