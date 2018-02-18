#ifndef COMPTER_ECART_H
#define COMPTER_ECART_H

#include <QItemDelegate>
#include <QPainter>
#include <QModelIndex>
#include <QSqlQueryModel>

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

class BDlgEcart : public QItemDelegate
{
    Q_OBJECT
public:
    BDlgEcart(QWidget *parent = 0) : QItemDelegate(parent) {}
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
};

class BSqmColorizeEcart:public QSqlQueryModel
{
    Q_OBJECT

public:
    BSqmColorizeEcart(QObject *parent=0):QSqlQueryModel(parent){}
    QVariant data(const QModelIndex &index, int role) const;
};

#endif // COMPTER_ECART_H
