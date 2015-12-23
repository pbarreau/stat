#ifndef DISTANCEPOURTIRAGE_H
#define DISTANCEPOURTIRAGE_H

#include <QLineEdit>
#include <QSqlQueryModel>
#include <QTableView>

class DistancePourTirage : public QLineEdit
{
    //Q_OBJECT

private:
    QLineEdit *distance;
    QSqlQueryModel *laRequete;
    QTableView * leTableau;


public:
    explicit DistancePourTirage(int dst, QSqlQueryModel *req, QTableView *tab, QWidget *parent = 0);
    QSqlQueryModel *getAssociatedModel(void);
    QTableView * getAssociatedVue(void);
    int getValue(void);
    void setValue(int val);

signals:

public slots:

};

#endif // DISTANCEPOURTIRAGE_H
