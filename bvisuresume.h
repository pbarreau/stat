#ifndef BVISURESUME_H
#define BVISURESUME_H


#include <QTableView>
#include <QItemDelegate>
#include <QSqlDatabase>

#include "sqlqmdetails.h"

#define COL_VISU_RESUME 1

typedef struct _prmBVisuResume{
    QString cnx;
    QString wko;
    QString cld; /// Table colors def

} prmBVisuResume;
class BVisuResume:public QItemDelegate
{
    Q_OBJECT
public:
    BVisuResume(prmBVisuResume param, QTableView *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
private:
    bool recupereMapColor(QString tbl_def);

private slots:
    void slot_AideToolTip(const QModelIndex & index);

private:
    QSqlDatabase db_0;
    QString tColorDefs;
    int nb_colors;
    QMap<BOrdColor,int> map_colors;

};

typedef struct _stBVisuResume_sql
{
    QString cnx;
    QString sql;
    QString wko; /// Working on Table
    QTableView *view;
    SyntheseGenerale *ori; /// origine
    int *b_min;
    int *b_max;
}stBVisuResume_sql;

class BVisuResume_sql : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit BVisuResume_sql(stBVisuResume_sql param,QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const;


private:
    QSqlDatabase db_0;
};

#endif // BVISURESUME_H
