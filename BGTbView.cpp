#include <QMouseEvent>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QHeaderView>

#include "BGTbView.h"

BGTbView::BGTbView(QWidget *parent):QTableView(parent)
{
 square = new QGroupBox;
 up = nullptr;
}

BGTbView::~BGTbView()
{
 delete square;
}

QGroupBox * BGTbView::getSquare(void)
{
 return square;
}

void BGTbView::setTitle(QString title, bool visible)
{
 if(square != nullptr){
  square->setTitle(title);
  square->setEnabled(visible);
 }
}

void BGTbView::addUpLayout(QLayout *up_in)
{
 up = up_in;
}

QGroupBox *BGTbView::getScreen(bool spacer)
{
 QVBoxLayout *layout = new QVBoxLayout;
 if(up != nullptr){
  layout->addLayout(up, Qt::AlignCenter|Qt::AlignTop);
 }
 layout->addWidget(this);
 if(spacer==true){
  QSpacerItem *ecart = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
  layout->addItem(ecart);
 }
 square->setLayout(layout);

 return square;
}
void BGTbView::mouseMoveEvent( QMouseEvent * inEvent )
{
 /// https://doc.qt.io/archives/4.6/eventsandfilters.html

 /// On filtre le cas selection multiple par la souris
 /// pour ne pas etre traite

 if(inEvent->buttons() == Qt::MouseButton::LeftButton){
  inEvent->accept();
  return;
 }

 QTableView::mouseMoveEvent(inEvent);
}

int BGTbView::getMinWidth(int delta)
{
 QTableView *qtv_tmp = this;
 int l = 0;
 int count=qtv_tmp->horizontalHeader()->count();

 int scrollBar = qtv_tmp->verticalScrollBar()->width();
 int Header = qtv_tmp->verticalHeader()->width();
 int col = 0;
 for (int i = 0; i < count-delta; ++i) {
  if(!qtv_tmp->horizontalHeader()->isSectionHidden(i)){
   col+=qtv_tmp->horizontalHeader()->sectionSize(i);
  }
 }
 l= Header+col+scrollBar;
 return l;
}

int BGTbView::getMinHeight()
{
 QTableView *qtv_tmp = this;
 /// https://savolai.net/notes/how-do-i-adjust-a-qtableview-height-according-to-contents/
 int l = 0;
 int count=qtv_tmp->verticalHeader()->count();

 int scrollBar=qtv_tmp->horizontalScrollBar()->height();
 int Header=qtv_tmp->horizontalHeader()->height();
 int row=0;
 for (int i = 0; i < count; ++i) {
  row+=qtv_tmp->verticalHeader()->sectionSize(i);
 }
 l= Header+row+scrollBar;
 return l;
}
