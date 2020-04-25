#ifndef MYQABSTRACTITEMVIEWOFQTABLE_H
#define MYQABSTRACTITEMVIEWOFQTABLE_H

#include <QTableView>
#include <QAbstractItemView>

class myQAbstractItemViewOfQTable : public QAbstractItemView
{
    Q_OBJECT
public:
    explicit myQAbstractItemViewOfQTable(QWidget *parent = 0);

signals:
    void MydoubleClicked(const QTableView &view, const QModelIndex & index);

public slots:

};

#endif // MYQABSTRACTITEMVIEWOFQTABLE_H
