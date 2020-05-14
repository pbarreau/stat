#ifndef BLINEEDIT_H
#define BLINEEDIT_H

#include <QLineEdit>
#include <QTableView>

#include "BGTbView.h"

class BLineEdit: public QLineEdit
{
 Q_OBJECT

 public:
 BLineEdit(BGTbView *view);
 BGTbView *getView();

 private:
 BGTbView *pView;
};


#endif // BLINEEDIT_H
