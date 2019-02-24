#ifndef SQLQMDETAILS_H
#define SQLQMDETAILS_H

#include <QSqlQueryModel>
#include <QString>
#include <QTableView>
#include <QItemDelegate>
#include <QColor>

#define COL_VISU    2

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
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    void PreparerTableau(void);


private:
    QColor maCouleur;
    QSqlDatabase db_0;
    int b_min;
    int b_max;
    st_sqlmqDetailsNeeds a;
};

typedef struct _st_ColorNeeds{
    sqlqmDetails *parent;
    int b_min;
    int b_max;
    int len;
}st_ColorNeeds;

class BDelegateCouleurFond : public QItemDelegate
{
    Q_OBJECT
public:
     BDelegateCouleurFond(st_ColorNeeds param, QWidget *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;


private:
    QColor CalculerCouleur(const QModelIndex &index) const;
    bool  posSurDisque(int centre, int pos)const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole)const;

private slots:
    void slot_AideToolTip(const QModelIndex & index);

private:
    sqlqmDetails *origine;
    int b_min;
    int b_max;
    int len;
    int nb_colors;
    QColor *val_color;
    //int val_v;
    //int val_r;
};

#endif // SQLQMDETAILS_H
