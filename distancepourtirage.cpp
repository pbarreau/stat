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

void DistancePourTirage::keepPtr(int ong, QSqlQueryModel *req, QTableView *tab)
{
    if(ong >=0 and ong < 3){
        qmVoisin[ong] = req;
        tvVoisin[ong] = tab;
    }
}

void DistancePourTirage::keepFiltre(FiltreCombinaisons *ptr)
{
    pFiltre = ptr;
}

FiltreCombinaisons *DistancePourTirage::GetFiltre(void)
{
    return pFiltre;
}


QSqlQueryModel *DistancePourTirage::getAssociatedModel(void)
{
    return laRequete;
}

QSqlQueryModel *DistancePourTirage::getAssociatedModel(int i)
{
    if(i>=0 and i<3)
        return qmVoisin[i];
}

QTableView * DistancePourTirage::getAssociatedVue(void)
{
    return leTableau;
}

QTableView *DistancePourTirage::getAssociatedVue(int i)
{
    if(i>=0 and i<3)
        return tvVoisin[i];
}
