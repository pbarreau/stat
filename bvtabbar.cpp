#include <QTabBar>
#include <QSize>

#include "bvtabbar.h"

BVTabBar::BVTabBar(int new_rot, QTabBar::Shape skind)
{
 rot=new_rot;
 this->setStyleSheet("QTabBar::tab {color: #000000; font-weight: bold; font-size: 10px; font-family: Gotham, Helvetica Neue, Helvetica, Arial, sans-serif;} "
                     "QTabBar::tab:selected {background-color: #FA9944; color: #000000; border-top: 1px solid #FA9944;} "
                     "QTabBar::tab:hover {color: #000000; border-top: 1px solid #FA9944; background-color: #FFFFFF;}");
  this->setShape(skind);
}

QSize BVTabBar::tabSizeHint(int index) const
{
 QSize s = QTabBar::tabSizeHint(index);
 if(rot){
  s.transpose();
 }
 return s;
}

void BVTabBar::paintEvent(QPaintEvent * /*event*/){

 QStylePainter painter(this);
 QStyleOptionTab opt;

 for(int i = 0;i < count();i++)
 {
  initStyleOption(&opt,i);
  painter.drawControl(QStyle::CE_TabBarTabShape, opt);
  painter.save();

	QSize s = opt.rect.size();
	if(rot){
	 s.transpose();
	}
	QRect r(QPoint(), s);
	r.moveCenter(opt.rect.center());
	opt.rect = r;

	QPoint c = tabRect(i).center();
	painter.translate(c);
	painter.rotate(rot);
	painter.translate(-c);
	painter.drawControl(QStyle::CE_TabBarTabLabel,opt);

  painter.restore();
 }
}

BVTabWidget::BVTabWidget(QTabWidget::TabPosition pos, QTabBar::Shape skind, QWidget *parent):QTabWidget(parent)
{
 int rot = 0;

 if(pos == QTabWidget::East){
  rot = -90;
 }

 if(pos == QTabWidget::West){
  rot = 90;
 }

 QTabBar * tab = new BVTabBar(rot, skind);


 setTabBar(tab);
 setTabPosition(pos);
}
