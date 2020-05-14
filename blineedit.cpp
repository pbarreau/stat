#include <QTableView>

#include "blineedit.h"

BLineEdit::BLineEdit(BGTbView *view)
{
 pView=view;
}

BGTbView *BLineEdit::getView()
{
 return pView;
}
