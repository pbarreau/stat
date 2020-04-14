#ifndef BFLAGS_H
#define BFLAGS_H

#include <QItemDelegate>
#include <QSqlQueryModel>

#include "compter.h"
#include "colors.h"

class BFlags : public QItemDelegate
{
 Q_OBJECT

 public:
 typedef struct _stPrmDlgt{
  QWidget *parent;
  QString db_cnx;
  int start;
  int zne;
  int typ;
  etCount eTyp;
 }stPrmDlgt;

 enum Filtre  {isLast=1,isPrevious=1<<1, isWanted=1<<2, isNever=1<<3,
               isPlusOne=1<<4, isMinusOne=1<<5};

 Q_DECLARE_FLAGS(Filtres, Filtre)
 Q_FLAG(Filtres)

 public:
 BFlags(stPrmDlgt prm);
 void paint(QPainter *painter, const QStyleOptionViewItem &option,
            const QModelIndex &index) const;

 private:
 QSqlDatabase db_1;
 int col_show;
 QString cur_zn;
 QString cur_tp;
 etCount eTyp;
};


#endif // BFLAGS_H
