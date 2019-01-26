#ifndef COMPTER_ECART_H
#define COMPTER_ECART_H

#include <QItemDelegate>
#include <QPainter>
#include <QModelIndex>
#include <QSqlQueryModel>

#include "compter.h"
class C_ElmEcarts : public BCount
{
    Q_OBJECT

public:
    C_ElmEcarts(const QString &in, const BGame &pDef, QSqlDatabase fromDb);
    ~C_ElmEcarts();
    QTableView * getTbv(int zn);


private:
    QTableView *Compter(QString *pname, int zn);
    bool createThatTable(QString tblName, int zn);
    //bool CalculerSqrt(QString tblName, QString colVariance);
    QString RechercherLesTirages(int boule, int zn);
    bool SauverCalculs(int boule,QString ToTbl, QString fromTbl);

private:
    static int total;
    int hCommon; // taille des tableaux
    QTableView **tbvCalculs;

Q_SIGNALS:
    void sig_TotEcart(const QModelIndex &index);

private slots:
    void slot_AideToolTip(const QModelIndex & index);
    void slot_SurligneTirage(const QModelIndex &index);
};


#endif // COMPTER_ECART_H
