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
#endif // DELEGATE_H
