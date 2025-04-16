#ifndef IDLGTTIRAGES_H
#define IDLGTTIRAGES_H

#include <QItemDelegate>


class idlgtTirages : public QItemDelegate
{
    Q_OBJECT
public:
    idlgtTirages(QWidget *parent = 0) : QItemDelegate(parent) {}
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
};

#endif // IDLGTTIRAGES_H
