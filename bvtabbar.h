#ifndef BVTABBAR_H
#define BVTABBAR_H

#include <QStyleOptionTab>
#include <QStylePainter>
#include <QTabBar>
#include <QTabWidget>
#include <QSize>

class BVTabBar: public QTabBar
{
public:
BVTabBar(int new_rot=0, QTabBar::Shape skind=QTabBar::RoundedNorth);
QSize tabSizeHint(int index) const;

protected:
void paintEvent(QPaintEvent * /*event*/);

private:
int rot;
};

class BVTabWidget : public QTabWidget
{
 public:
 BVTabWidget(QTabWidget::TabPosition pos, QTabBar::Shape skind=QTabBar::RoundedNorth, QWidget *parent=0);
};

#endif // BVTABBAR_H
