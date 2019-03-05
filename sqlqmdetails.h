#ifndef SQLQMDETAILS_H
#define SQLQMDETAILS_H


#include <QSqlQueryModel>
#include <QString>
#include <QTableView>
#include <QItemDelegate>
#include <QColor>
#include <QMap>

#include "SyntheseGenerale.h"

#define COL_VISU_ECART    2

/*
typedef struct _st_sqlmqDetailsNeeds
{
    QString cnx;
    QString sql;
    QString wko; /// Working on Table
    QTableView *view;
    SyntheseGenerale *ori; /// origine
    int *b_min;
    int *b_max;

}st_sqlmqDetailsNeeds;
*/

class sqlqmDetails : public QSqlQueryModel
{
    Q_OBJECT

  public:
    struct st_sqlmqDetailsNeeds
    {
        QString cnx;
        QString sql;
        QString wko; /// Working on Table
        QTableView *view;
        SyntheseGenerale *ori; /// origine
        int b_min;
        int b_max;
    };


    explicit sqlqmDetails(st_sqlmqDetailsNeeds param,QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const;
    //bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    void PreparerTableau(void);


private:
    QSqlDatabase db_0;
    int b_min;
    int b_max;
    int nbJ;
};



#endif // SQLQMDETAILS_H
