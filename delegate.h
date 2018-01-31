#ifndef DELEGATE_H
#define DELEGATE_H

#include <QObject>
#include <QItemDelegate>

class Delegate : public QItemDelegate
{
    Q_OBJECT
public:
    Delegate(QWidget *parent = 0) : QItemDelegate(parent) {}
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
};

class Dlgt_Combi : public QItemDelegate
{
    Q_OBJECT
public:
    Dlgt_Combi(QWidget *parent = 0) : QItemDelegate(parent) {}
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
};

class Dlgt_grp : public QItemDelegate
{
    Q_OBJECT
public:
    Dlgt_grp(QWidget *parent = 0) : QItemDelegate(parent) {}
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
};

#endif // DELEGATE_H
