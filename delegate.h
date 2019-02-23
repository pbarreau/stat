#ifndef DELEGATE_H
#define DELEGATE_H

#include <QObject>
#include <QItemDelegate>
#include <QSqlQueryModel>
#include "colors.h"

class BDelegateStepper : public QItemDelegate
{
    Q_OBJECT
public:
    BDelegateStepper(QWidget *parent = 0) : QItemDelegate(parent) {}
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
};

class BDelegateElmOrCmb : public QItemDelegate
{
    Q_OBJECT
public:
    BDelegateElmOrCmb(QWidget *parent = 0) : QItemDelegate(parent) {}
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
};

class BDelegateFilterGrp : public QItemDelegate
{
    Q_OBJECT
public:
    BDelegateFilterGrp(QWidget *parent = 0) : QItemDelegate(parent) {}
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
};

class BDelegateCouleurFond : public QItemDelegate
{
    Q_OBJECT
public:
    BDelegateCouleurFond(int b_min, int b_max, int len, QWidget *parent = 0) :
        b_min(b_min),b_max(b_max),len(len),QItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

private:
    int b_min;
    int b_max;
    int len;
};

class BSqmColorizePriority:public QSqlQueryModel
{
    Q_OBJECT

public: ///
    explicit BSqmColorizePriority(QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const;
};

#endif // DELEGATE_H
