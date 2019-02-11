#ifndef DELEGATE_H
#define DELEGATE_H

#include <QObject>
#include <QItemDelegate>
#include <QSqlQueryModel>

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

class BSqmColorizePriority:public QSqlQueryModel
{
    Q_OBJECT

public: ///
    explicit BSqmColorizePriority(QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const;
};

#endif // DELEGATE_H
