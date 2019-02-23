#ifndef SQLQMDETAILS_H
#define SQLQMDETAILS_H

#include <QSqlQueryModel>
#include <QString>
#include <QTableView>
#include <QItemDelegate>

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

class BDelegateCouleurFond : public QItemDelegate
{
    Q_OBJECT
public:
     BDelegateCouleurFond(int b_min, int b_max, int len, QWidget *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

private:
    QColor MonSetColor(const QModelIndex &index) const;
    bool  calcul(int centre, int pos)const;

private:
    int b_min;
    int b_max;
    int len;
    int nb_colors;
    int *val_color;
    //int val_v;
    //int val_r;
};

#endif // SQLQMDETAILS_H
