#ifndef COMPTER_ECART_H
#define COMPTER_ECART_H

#include <QItemDelegate>
#include <QPainter>
#include <QModelIndex>
#include <QSqlQueryModel>

#include "compter.h"
class BCountEcart : public BCount
{
    Q_OBJECT

public:
    BCountEcart(const QString &in, const int ze, const BGame &pDef, QSqlDatabase fromDb);
    ~BCountEcart();
    QTableView * getTbv(int zn);


private:
    QTableView *Compter(QString *pname, int zn);
    bool createThatTable(QString tblName, int zn);

private:
    static int total;
    int hCommon; // taille des tableaux
    QTableView *tbv_memo[2];

Q_SIGNALS:
    void sig_TotEcart(const QModelIndex &index);

private slots:
    void slot_AideToolTip(const QModelIndex & index);
    void slot_SurligneTirage(const QModelIndex &index);
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
    //Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
};

#endif // COMPTER_ECART_H
