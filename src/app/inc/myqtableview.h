#ifndef MYQTABLEVIEW_H
#define MYQTABLEVIEW_H

#include <QTableView>

class myQTableView : public QTableView
{
    Q_OBJECT
public:
    explicit myQTableView(QWidget *parent = 0);

signals:
    void MydoubleClicked(const QTableView &view, const QModelIndex & index);

public slots:

};

#endif // MYQTABLEVIEW_H
