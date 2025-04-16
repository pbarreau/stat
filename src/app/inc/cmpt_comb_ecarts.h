#ifndef C_CMBECARTS_H
#define C_CMBECARTS_H

#include <QItemDelegate>
#include <QPainter>
#include <QModelIndex>
#include <QSqlQueryModel>

#include "compter.h"

class C_CmbEcarts: public BCount
{
    Q_OBJECT

public:
    C_CmbEcarts(const QString &in, const B_Game &pDef, QSqlDatabase fromDb);
    ~C_CmbEcarts();
    QTableView * getTbv(int zn);


private:
    QTableView *Compter(QString *pname, int zn);
    bool createThatTable(QString tblName, int zn);
    QString RechercherLesTirages(int boule, int zn);
    bool SauverCalculs(int zn,int boule,QString ToTbl, QString fromTbl);

private:
    static int total;
    int hCommon; // taille des tableaux
    QTableView **tbvCalculs;

#if 0
Q_SIGNALS:
    void sig_TotEcart(const QModelIndex &index);

private slots:
    void slot_AideToolTip(const QModelIndex & index);
    void slot_SurligneTirage(const QModelIndex &index);
#endif

};

#endif // C_CMBECARTS_H
