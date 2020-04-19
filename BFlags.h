#ifndef BFLAGS_H
#define BFLAGS_H

#include <QItemDelegate>
#include <QStyledItemDelegate>
#include <QSqlQueryModel>

#include "colors.h"

/// -------------
namespace Bp {
 /// https://wiki.qt.io/QFlags_tutorial
 /// https://www.qtcentre.org/threads/49096-Use-QFlags
 /// https://stackoverflow.com/questions/43478059/how-to-you-use-operator-int-of-qflags

 enum Filtering{
  isNotSet  = 0x0000,
  isLastTir	= 0x0001,
  isPrevTir	=	0x0002,
  isWanted	=	0x0004,
  isFiltred	= 0x0008,
  isNotSeen	=	0x0010,
  isSeenBfr	=	0x0020,
  isSeenAft	=	0x0040
 };
 Q_DECLARE_FLAGS(Filterings, Filtering)

 enum FltLayer{
  lyFiltred
 };

}
Q_DECLARE_OPERATORS_FOR_FLAGS(Bp::Filterings)
Q_DECLARE_METATYPE(Bp::Filterings)
/// -------------


typedef enum{
 eCountToSet,    /// Pas de definition
 eCountElm,      /// Comptage des boules de zones
 eCountCmb,      /// ... des combinaisons
 eCountGrp,       /// ... des regroupements
 eCountBrc,      /// ... des barycentres
 eCountEnd
}etCount;


class BFlags : public QStyledItemDelegate
{
 Q_OBJECT

 public:
 typedef struct _stPrmDlgt{
  QWidget *parent;
  QString db_cnx;
  int start;
  int zne;
  Bp::Filterings b_flt;
  int typ;
  etCount eTyp;
  QSqlQueryModel *mod;
 }stPrmDlgt;

 /// https://stackoverflow.com/questions/46180506/qt-retrieve-qflags-form-qvariant
 enum Filtre  {isNotSet=0, isLastTir=1,isPrevious=1<<1, isFiltred=1<<2, isNever=1<<3,
               isPlusOne=1<<4, isMinusOne=1<<5, isWanted=1<< 6, isTerminated=1<<7 };

 Q_DECLARE_FLAGS(Filtres, Filtre)
 Q_FLAG(Filtres)

 public:
 BFlags(stPrmDlgt prm);
 virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,
            const QModelIndex &index) const;

 private:
 void v1_paint(QPainter *painter, const QStyleOptionViewItem &option,
                    const QModelIndex &index) const;
 void v2_paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
 void item_Wanted(QStyleOptionViewItem &opt) const;


 private:
 QSqlDatabase db_1;
 stPrmDlgt flt;
 QSqlQueryModel *model;
 //int flt.start;
 //QString flt.zne;
 //QString cur_tp;
 //etCount flt.eTyp;
 //Bp::Filterings flt.b_flt;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(BFlags::Filtres)
Q_DECLARE_METATYPE(BFlags::Filtre)

#endif // BFLAGS_H
