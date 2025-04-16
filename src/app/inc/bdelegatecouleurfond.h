#ifndef BDELEGATECOULEURFOND_H
#define BDELEGATECOULEURFOND_H

#include <QItemDelegate>
#include <QTableView>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QString>
#include <QSqlDatabase>

#include <QPointF>
#include <QPolygonF>
#include <QRect>

#include "sqlqmdetails.h"
#include "bordcolor.h"

class BDelegateCouleurFond : public QItemDelegate
{
    Q_OBJECT

  public:
    enum Columns {Id=0, keyColors, vEcart, vFilters,
                  EcartCourant,EcartPrecedent,
                  EcartMoyen,EcartMaximum,
                  EsperanceEcart,EcartMedian,TotalElement};
    /// Priorite=total-2,Filtre=total-1
    ///
    enum Filtre  {isLast=1,isPrevious=1<<1, isWanted=1<<2, isNever=1<<3,
                  isPlusOne=1<<4, isMinusOne=1<<5};

    Q_DECLARE_FLAGS(Filtres, Filtre)
    Q_FLAG(Filtres)

    struct st_ColorNeeds{
        QString cnx;
        QString wko;
        QString src;
        sqlqmDetails *ori;
        int len;
    };
    BDelegateCouleurFond(st_ColorNeeds param, QTableView *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;


  private:
    QColor getColorForValue(const QModelIndex &index) const;
    bool  checkValue(int centre, int pos)const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole)const;

    bool isTablePresent(QString tb_name);
    void MapColorCreate(void);
    void MapColorApply(QTableView *tbv_cible);
    bool MapColorWrite(QString tbl_def);
    bool MapColorRead(QString tbl_def);

  private slots:
    void slot_AideToolTip(const QModelIndex & index);


  Q_SIGNALS:
    void sig_TableDesCouleurs(const QString &my_table);

  private:
    QSqlDatabase db_0;
    sqlqmDetails *origine;
    QString working_on;
    //int b_min;
    //int b_max;
    int nbE;
    int nbJ;
    int nb_colors;
    //QColor *val_colors;
    QColor *resu_color;
    QPolygonF starPolygon;
    QMap<BOrdColor,int> map_FromColor;
    //QMap<BOrdColor,int> map_colors;


};


#endif // BDELEGATECOULEURFOND_H
