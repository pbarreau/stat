#ifndef BVISURESUME_SQL_H
#define BVISURESUME_SQL_H

#include <QObject>
#include <QString>
#include <QTableView>
#include <QSqlQueryModel>
#include <QModelIndex>
#include <QVariant>
#include <QSqlDatabase>

#include "SyntheseGenerale.h"

class BVisuResume_sql : public QSqlQueryModel
{
    Q_OBJECT
public:
    struct stBVisuResume_sql
    {
        QString cnx;
        QString sql;
        QString tb_rsm; /// Working on Table
        QString tb_tot; /// Working on Table
        QTableView *view;
        SyntheseGenerale *ori; /// origine
        int *b_min;
        int *b_max;
    };

    explicit BVisuResume_sql(stBVisuResume_sql param,QObject *parent = 0);

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    //int rowCount(const QModelIndex &parent = QModelIndex()) const;
    //int columnCount(const QModelIndex &parent = QModelIndex()) const;

    //bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);




private:
    QSqlDatabase db_0;
    QString tb_tot_src;
    QString tb_rsm_src;
};

#endif // BVISURESUME_SQL_H
