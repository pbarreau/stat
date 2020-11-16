#include <QMouseEvent>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QHeaderView>

#include "BView.h"

BView::BView(QWidget *parent):QTableView(parent)
{
 square = new QGroupBox;
 up = nullptr;
 zone=0;
}

BView::~BView()
{
 delete square;
}

void BView::setTitle(QString title, bool visible)
{
 if(square != nullptr){
  square->setTitle(title);
  square->setEnabled(visible);
 }
}

void BView::addUpLayout(QLayout *up_in)
{
 up = up_in;
}

int BView::getZone()
{
 return zone;
}

void BView::setZone(int zn_in)
{
 zone = zn_in;
}

QGroupBox *BView::getScreen(bool spacer)
{
 QVBoxLayout *layout = new QVBoxLayout;
 if(up != nullptr){
  layout->addLayout(up);
  layout->setAlignment(up, Qt::AlignLeft|Qt::AlignTop);
 }
 layout->addWidget(this,Qt::AlignCenter|Qt::AlignTop);
 if(spacer==true){
  QSpacerItem *ecart = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
  layout->addItem(ecart);
 }
 square->setLayout(layout);

 return square;
}
void BView::mouseMoveEvent( QMouseEvent * inEvent )
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

int BView::getMinWidth(int delta)
{
 QTableView *qtv_tmp = this;
 int l = 0;
 int count=qtv_tmp->horizontalHeader()->count();
 qtv_tmp->horizontalHeader()->setMinimumSectionSize(35);

 /*
 int invisible = 0;
 for (int i = 0; i < count; ++i) {
  if(!qtv_tmp->horizontalHeader()->isSectionHidden(i)){
   invisible++;
  }
 }

 int max_col = count - invisible;
 if(max_col<3){
  max_col = 2;
 }
 else {
  max_col = count - delta;
 }
*/

 int scrollBar = qtv_tmp->verticalScrollBar()->width();
 int Header = qtv_tmp->verticalHeader()->width();
 int col = 0;

 if(count){
  for (int i = 0; i < count-delta; ++i) {
   if(!qtv_tmp->horizontalHeader()->isSectionHidden(i)){
    col+=qtv_tmp->horizontalHeader()->sectionSize(i);
   }
  }
 }
 else {
  col = 25;
 }
 l= Header+col+scrollBar;
 return l;
}

int BView::getMinHeight()
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
