#ifndef COMPTER_ECART_H
#define COMPTER_ECART_H

#include "compter.h"
class BCountEcart : public BCount
{
public:
    BCountEcart(const QString &in, const int ze, const BGame &pDef, QSqlDatabase fromDb);
    ~BCountEcart();

private:
    QTableView *Compter(QString *pname, int zn);
    bool createThatTable(QString tblName, int zn);

private:
    static int total;
    int hCommon; // taille des tableaux

};

#endif // COMPTER_ECART_H
