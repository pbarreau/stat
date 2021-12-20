#include <QTimer>
#include <QDateTime>
#include <QSqlQueryModel>

#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QStylePainter>
#include <QStyleOptionViewItem>
#include <QFontMetrics>
#include <QPalette>
#include <QRect>

#include "BAnimateCell.h"
#include "BFpm_upl.h"
#include "colors.h"

BAnimateCell::BAnimateCell(BView *view):m_view(view),QStyledItemDelegate(nullptr)
{

 QTimer * timer = new QTimer( this );
 connect( timer, &QTimer::timeout, this, &BAnimateCell::BSlot_animate);


 //timer->start( TIME_RESOLUTION );
}

void BAnimateCell::addKey(int key, bool refresh)
{
 mapTimeout.insert( key , QDateTime::currentDateTime() );

 if(refresh)
  emit BSig_Repaint(m_view);
}

void BAnimateCell::delKey(int key, bool refresh)
{
 mapTimeout.remove(key);
 if(refresh)
  emit BSig_Repaint(m_view);
}

bool BAnimateCell::gotKey(int key, bool refresh)
{
 if(refresh)
  emit BSig_Repaint(m_view);

 QMap<int, QVariant>::const_iterator it = mapTimeout.find( key );

 return (it == mapTimeout.end() ?  false : true);
}

void BAnimateCell::setModel(BView *view)
{
 BFpm_upl *m = qobject_cast<BFpm_upl *>(view->model());
 QSqlQueryModel *vl = qobject_cast<QSqlQueryModel *>(m->sourceModel());
 nb_col = vl->columnCount();

}

void BAnimateCell::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
 QStyleOptionViewItem myOpt = option;
 initStyleOption(&myOpt, index);

 QRect cur_rect = myOpt.rect;

 int col = myOpt.index.column();
 int row = myOpt.index.row();

 if((col == nb_col-1)){
  int key = index.sibling(index.row(),0).data().toInt();;
  QMap<int, QVariant>::const_iterator it = mapTimeout.find( key );
  if(it !=mapTimeout.end() ){
   painter->fillRect(cur_rect, COULEUR_FOND_R0);
  }
 }

 QStyledItemDelegate::paint(painter,myOpt,index);
}

QVariant BAnimateCell::data(const QModelIndex &idx, int role) const
{
 if( role != ItemModifiedRole );
 //return BView::data( idx, role );

}

void BAnimateCell::BSlot_animate()
{

}

void BAnimateCell::BSlot_over(const QModelIndex &index)
{
 int lgn = index.row();

 mapTimeout.insert(lgn,QDateTime::currentDateTime());

}
