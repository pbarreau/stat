#include <QSqlQueryModel>
#include <QTableView>

#include "distancepourtirage.h"

DistancePourTirage::DistancePourTirage(int dst, QSqlQueryModel *req, QTableView *tab, QWidget *parent) :
    QLineEdit(parent)
{
    this->setText(QString::number(dst));
    distance = this;
    laRequete = req;
    leTableau = tab;
}

int DistancePourTirage::getValue(void)
{
    int value = this->text().toInt();

    return value;
}

void DistancePourTirage::setValue(int val)
{
    this->setText(QString::number(val));
}

QSqlQueryModel *DistancePourTirage::getAssociatedModel(void)
{
    return laRequete;
}

QTableView * DistancePourTirage::getAssociatedVue(void)
{
    return leTableau;
}
