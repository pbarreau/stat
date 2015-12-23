#ifndef DISTANCEPOURTIRAGE_H
#define DISTANCEPOURTIRAGE_H

#include <QLineEdit>
#include <QSqlQueryModel>
#include <QTableView>

#include "filtrecombinaisons.h"

class DistancePourTirage : public QLineEdit
{
    //Q_OBJECT

private:
    QLineEdit *distance;
    QSqlQueryModel *laRequete;
    QTableView * leTableau;

    QSqlQueryModel *qmVoisin[3];
    QTableView * tvVoisin[3];
    FiltreCombinaisons *pFiltre;


public:
    explicit DistancePourTirage(int dst, QSqlQueryModel *req, QTableView *tab, QWidget *parent = 0);

    QSqlQueryModel *getAssociatedModel(void);
    QTableView * getAssociatedVue(void);

    QSqlQueryModel *getAssociatedModel(int i);
    QTableView * getAssociatedVue(int i);
    FiltreCombinaisons *GetFiltre(void);


    int getValue(void);
    void setValue(int val);
    void keepPtr(int ong, QSqlQueryModel *req, QTableView *tab);
    void keepFiltre(FiltreCombinaisons *ptr);

signals:

public slots:

};

#endif // DISTANCEPOURTIRAGE_H
