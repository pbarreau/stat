#ifndef BVISURESUME_H
#define BVISURESUME_H


#include <QTableView>
#include <QItemDelegate>
#include <QSqlDatabase>

#include "sqlqmdetails.h"

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

private:
    QSqlDatabase db_0;
    QString tColorDefs;
    QMap<BOrdColor,int> map_colors;

};

#endif // BVISURESUME_H
