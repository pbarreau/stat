#include <QEvent>
#include <QModelIndex>

#include "BPushButton.h"

BPushButton::BPushButton(QLabel *target, QString color, eRole act, QWidget *parent):QPushButton (parent)
{
 this->setMouseTracking(true);
 lab = target;
 col = color;
 rol = act;
 cible = nullptr;
}

QString BPushButton::getColor()
{
 return col;
}

BPushButton::eRole BPushButton::getRole()
{
 return rol;
}

void BPushButton::enterEvent( QEvent * e )
{
 if(e->type() == QEvent::Enter){
  Q_EMIT unSurvol(lab);
 }

 // don't forget to forward the event
 QWidget::enterEvent( e );
}

void BPushButton::slot_tbvClicked(const QModelIndex &index)
{
 QTableView *src = qobject_cast<QTableView*>(sender());
 cible = src;
 return;

 if(rol == eOk){
  QString lgn = "";
  for (int i =2;i<2+5;i++) {
   QString val = index.model()->index(index.row(),i).data().toString().rightJustified(2,'0');
   lgn= lgn + val;
   if(i<2+5-1){
    lgn = lgn + ",";
   }
  }

  lab->setText(lgn);
 }
}
