#ifndef BORDCOLOR_H
#define BORDCOLOR_H

#include <QColor>


class BOrdColor:public QColor
{
public:
    BOrdColor(int r, int g, int b, int a = 255):QColor(r,g,b,a){}
    BOrdColor(QColor a):QColor(a){}
    BOrdColor():QColor(){}

    bool operator<(const BOrdColor  &b)const;

};

#endif // BORDCOLOR_H
