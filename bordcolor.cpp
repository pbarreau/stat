#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include "bordcolor.h"


bool BOrdColor::operator<(const BOrdColor  &b)const
{
    /// https://www.developpez.net/forums/d1298928/c-cpp/bibliotheques/qt/qmap-operator-qpoint/
    bool b_retVal = (this->blue()<b.blue()) ||
            ((this->blue()==b.blue()) && (this->green()<b.green()))||
            ((this->green()==b.green()) && (this->red()<b.red()))||
            ((this->red()==b.red()) && (this->alpha() < b.alpha()));

    return b_retVal;

#ifndef QT_NO_DEBUG
    QString msg_1 =  "("+QString::number(this->red()).rightJustified(3,'0')
            + ","+QString::number(this->green()).rightJustified(3,'0')
            + ","+QString::number(this->blue()).rightJustified(3,'0')
            + ","+QString::number(this->alpha()).rightJustified(3,'0')
            +")";
    QString msg_2 = "("+QString::number(b.red()).rightJustified(3,'0')
            + ","+QString::number(b.green()).rightJustified(3,'0')
            + ","+QString::number(b.blue()).rightJustified(3,'0')
            + ","+QString::number(b.alpha()).rightJustified(3,'0')
            +")";

    QString msg = msg_1 + QString(" < ") + msg_2 + QString(" ? -> ")
            +QString::number(b_retVal);
    qDebug()<< msg;
#endif

}

