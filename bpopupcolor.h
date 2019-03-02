#ifndef BPOPUPCOLOR_H
#define BPOPUPCOLOR_H

#include <QObject>
#include <QStyledItemDelegate>
#include <QSqlDatabase>
#include <QString>
#include <QMap>
#include <QPainter>
#include <QStyleOptionViewItem>

#include <QModelIndex>
#include <QTableView>

#include "sqlqmdetails.h"

class BPopupColor : public QStyledItemDelegate
{
    Q_OBJECT

  public:
    enum Columns{
      ColorC=0, ColorCb, ValBc, ValT, ValB,
      ValTb,ValTbT,ValCe,Count
    };

    struct PopParam{
        QString cnx;
        QString tb_cld; /// Table colors def
    };

    explicit BPopupColor(PopParam param, QTableView *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;


  private:
      bool recupereMapColor(QString tbl_def);

  private:
      QSqlDatabase db_0;
      QString tColorDefs;
      int nb_colors;
      QString tb_rsm_src;
      QString tb_tot_src;
      QMap<BOrdColor,int> map_colors;

};

#endif // BPOPUPCOLOR_H
