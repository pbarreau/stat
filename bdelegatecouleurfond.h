#ifndef BDELEGATECOULEURFOND_H
#define BDELEGATECOULEURFOND_H

#include <QItemDelegate>
#include <QTableView>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QString>
#include <QSqlDatabase>

#include "sqlqmdetails.h"
#include "bordcolor.h"






class BDelegateCouleurFond : public QItemDelegate
{
    Q_OBJECT

public:
    struct st_ColorNeeds{
        QString cnx;
        QString wko;
        sqlqmDetails *ori;
        int b_min;
        int b_max;
        int len;
    };
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


#endif // BDELEGATECOULEURFOND_H
