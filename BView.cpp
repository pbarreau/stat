#include <QMouseEvent>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QHeaderView>

#include "BView.h"

BView::BView(int in_zn, etCount in_typ):QTableView(nullptr)
{
 square = new QGroupBox;
 up = nullptr;
 userData = nullptr;
 z_id=in_zn;
 tbInDb = "";
 c_id = in_typ;
}

BView::~BView()
{
 //delete square;
}

void BView::setTitle(QString title, bool visible, bool bold)
{
 if(square != nullptr){
  square->setTitle(title);
  if(bold){
  square->setStyleSheet("QGroupBox {color:green;font-weight: bold;font: 12pt;}");
  }
  square->setEnabled(visible);
 }
}

QString BView::getTitle(void)
{
 QString ret_val ="";

 if(square != nullptr){
  ret_val = square->title();
 }

 return ret_val;
}

QString BView::getTblName()
{
 return tbInDb;
}

void BView::addUpLayout(QLayout *up_in)
{
 up = up_in;
}

int BView::getZid()
{
 return z_id;
}

void BView::setZid(int z_in)
{
 z_id = z_in;
}

etCount BView::getCid()
{
 return c_id;
}

int BView::getGid()
{
 return g_id;
}

void BView::setGid(int g_in)
{
 g_id = g_in;
}

void BView::setUseTable(QString tbl)
{
 tbInDb = tbl;
}

void *BView::getUserDataPtr()
{
 return userData;
}

void BView::setUserDataPtr(void *userDataPtr)
{
 userData = userDataPtr;
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
