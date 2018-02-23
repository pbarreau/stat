#ifndef COMPTER_UNE_REPARTITION_H
#define COMPTER_UNE_REPARTITION_H

#include <QTableView>

#include "compter.h"


class BCountLine:public BCount
{
public:
    BCountLine(const QString &in, const int ze, const BGame &pDef, QStringList** lstCri, QSqlDatabase fromDb);
    ~BCountLine();

private:
    static int total;

private:
    QTableView *lgnInfo(int lgn, int zn);


};

#endif // COMPTER_UNE_REPARTITION_H
