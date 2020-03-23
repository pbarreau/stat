#include <QTableView>

#include "blineedit.h"

BLineEdit::BLineEdit(QTableView *view)
{
 pView=view;
}

QTableView *BLineEdit::getView()
{
 return pView;
}
