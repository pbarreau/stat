#ifndef BFLAGS_H
#define BFLAGS_H

#include <QItemDelegate>
#include <QSqlQueryModel>

#include "colors.h"

typedef enum{
 eCountToSet,    /// Pas de definition
 eCountElm,      /// Comptage des boules de zones
 eCountCmb,      /// ... des combinaisons
 eCountGrp,       /// ... des regroupements
 eCountBrc,      /// ... des barycentres
 eCountEnd
}etCount;


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

 enum Filtre  {isNotSet=0, isLast=1,isPrevious=1<<1, isFiltred=1<<2, isNever=1<<3,
               isPlusOne=1<<4, isMinusOne=1<<5, isWanted=1<< 6, isTerminated=1<<7 };

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
Q_DECLARE_OPERATORS_FOR_FLAGS(BFlags::Filtres)
Q_DECLARE_METATYPE(BFlags::Filtre)

#endif // BFLAGS_H
