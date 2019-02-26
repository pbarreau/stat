#ifndef SQLQMDETAILS_H
#define SQLQMDETAILS_H


#include <QSqlQueryModel>
#include <QString>
#include <QTableView>
#include <QItemDelegate>
#include <QColor>
#include <QMap>

#include "SyntheseGenerale.h"

#define COL_VISU    1

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

class sqlqmDetails : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit sqlqmDetails(st_sqlmqDetailsNeeds param,QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const;
    //bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    void PreparerTableau(void);


private:
    QSqlDatabase db_0;
    int b_min;
    int b_max;
    st_sqlmqDetailsNeeds a;
};

class BOrdColor:public QColor
{
public:
    BOrdColor(int r, int g, int b, int a = 255):QColor(r,g,b,a){}
    BOrdColor(QColor a):QColor(a){}
    BOrdColor():QColor(){}

    bool operator<(const BOrdColor  &b)const;

};

typedef struct _st_ColorNeeds{
    QString cnx;
    QString wko;
    sqlqmDetails *ori;
    int b_min;
    int b_max;
    int len;
}st_ColorNeeds;

class BDelegateCouleurFond : public QItemDelegate
{
    Q_OBJECT
public:
    BDelegateCouleurFond(st_ColorNeeds param, QTableView *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;


private:
    QColor CalculerCouleur(const QModelIndex &index) const;
    bool  isOnDisk(int centre, int pos)const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole)const;
    void CreationTableauClefDeCouleurs(void);
    void AffectationCouleurResultat(QTableView *tbv_cible);
    bool SauverTableauPriotiteCouleurs();

private slots:
    void slot_AideToolTip(const QModelIndex & index);


Q_SIGNALS:
    void sig_TableDesCouleurs(const QString &my_table);

private:
    QSqlDatabase db_0;
    sqlqmDetails *origine; //sqlqmDetails
    QString working_on;
    int b_min;
    int b_max;
    int len;
    int nb_colors;
    QColor *val_colors; // static
    QColor *resu_color;
    QMap<BOrdColor,int> map_FromColor; // static
};


#endif // SQLQMDETAILS_H
