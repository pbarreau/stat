#ifndef DISTANCEPOURTIRAGE_H
#define DISTANCEPOURTIRAGE_H

#include <QLineEdit>
#include <QSqlQueryModel>
#include <QTableView>

#include "filtrecombinaisons.h"
#include <QStandardItemModel>

class DistancePourTirage : public QLineEdit
{
    //Q_OBJECT

private:
    QLineEdit *distance;
    QSqlQueryModel *laRequete;
    QTableView * leTableau;

    QSqlQueryModel *qmVoisin[4];
    QTableView * tvVoisin[4];
    QSortFilterProxyModel *fpVoisin[3];
    FiltreCombinaisons *pFiltre;
    QStandardItemModel *model;


public:
    explicit DistancePourTirage(int dst, QSqlQueryModel *req, QTableView *tab, QWidget *parent = 0);

    QSqlQueryModel *getAssociatedModel(void);
    QTableView * getAssociatedVue(void);
    QStandardItemModel *GetStandardModel(void);

    QSqlQueryModel *getAssociatedModel(int i);
    QTableView * getAssociatedVue(int i);
    FiltreCombinaisons *GetFiltre(void);
    QSortFilterProxyModel *GetProxyModel(int i);


    int getValue(void);
    void setValue(int val);
    void keepPtr(int ong, QSqlQueryModel *req, QTableView *tab, QSortFilterProxyModel *sfp);
    void keepPtr(QStandardItemModel *ong);
    void keepFiltre(FiltreCombinaisons *ptr);

signals:

public slots:

};

#endif // DISTANCEPOURTIRAGE_H
