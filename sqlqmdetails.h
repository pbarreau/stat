#ifndef SQLQMDETAILS_H
#define SQLQMDETAILS_H

#include <QSqlQueryModel>
#include <QString>
#include <QTableView>

typedef struct _st_sqlmqDetailsNeeds
{
    QString cnx;
    QString sql;
    QTableView *view;
    int *b_min;
    int *b_max;

}st_sqlmqDetailsNeeds;

class sqlqmDetails : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit sqlqmDetails(st_sqlmqDetailsNeeds param,QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const;
    void PreparerTableau(void);


private:
    QSqlDatabase db_0;
    int b_min;
    int b_max;
    st_sqlmqDetailsNeeds a;
};

#endif // SQLQMDETAILS_H
