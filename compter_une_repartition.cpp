#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QTableView>
#include <QSqlQuery>
#include <QHeaderView>
#include <QToolTip>

#include "compter_une_repartition.h"


int BCountLine::total = 0;

BCountLine::~BCountLine()
{
    total --;
}

BCountLine::BCountLine(const QString &in, const int ze, const BGame &pDef, QStringList** lstCri, QSqlDatabase fromDb)
    :BCount(pDef,in,fromDb,NULL,eCountGrp)
{
    total++;

}

QTableView *BCountLine::lgnInfo(int lgn, int zn)
{
    QTableView *qtv_tmp = new QTableView;


    return qtv_tmp;
}
