#include <QTableView>

#include "blineedit.h"

BLineEdit::BLineEdit(BView *view)
{
 pView=view;
}

BView *BLineEdit::getView()
{
 return pView;
}
