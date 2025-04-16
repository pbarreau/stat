#include <QSqlQueryModel>
#include <QTableView>
#include <QStandardItemModel>

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

void DistancePourTirage::keepPtr(QStandardItemModel *ong)
{
    model = ong;
}

QStandardItemModel *DistancePourTirage::GetStandardModel(void)
{
    return model;
}

void DistancePourTirage::keepPtr(int ong, QSqlQueryModel *req,
                                 QTableView *tab,
                                 QSortFilterProxyModel *sfp)
{
    if(ong >=0 and ong <= 3){
        qmVoisin[ong] = req;
        tvVoisin[ong] = tab;
        fpVoisin[ong] = sfp;
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

QSortFilterProxyModel *DistancePourTirage::GetProxyModel(int i)
{
    if(i>=0 and i<3)
        return fpVoisin[i];
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
    if(i>=0 and i<=3)
        return tvVoisin[i];
}
