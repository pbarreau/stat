#ifndef BFLAGS_H
#define BFLAGS_H

#include <QMainWindow>
#include <QItemDelegate>
#include <QStyledItemDelegate>
#include <QSqlQueryModel>

#include "bstflt.h"
#include "colors.h"
#include "BTbView.h"

class BFlags : public QStyledItemDelegate
{
 Q_OBJECT

 public:
 typedef struct _stPrmDlgt{
  BTbView *parent;
  QString db_cnx;
  int start;
  int zne;
  etCount typ;
  Bp::F_Flts b_flt;
  //int typ;
 }stPrmDlgt;

 /// https://stackoverflow.com/questions/46180506/qt-retrieve-qflags-form-qvariant
 enum Filtre  {isNotSet=0, isLastTir=1,isPrevTir=1<<1, isFiltred=1<<2, isNotSeen=1<<3,
               isSeenBfr=1<<4, isSeenAft=1<<5, isWanted=1<< 6, isTerminated=1<<7 };

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
 void displayTbv_cell(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
 void setWanted(bool state, QPainter *painter, const QStyleOptionViewItem &opt, stTbFiltres *a, const QModelIndex &index) const;
 bool getThisFlt(stTbFiltres *val, const etCount in_typ, const QModelIndex index) const;
 void fltDraw(bool isPresent, stTbFiltres *a, QPainter *painter, const QStyleOptionViewItem &maModif,
               const QModelIndex &index) const;
 void fltWrite(bool isPresent, stTbFiltres *a, QPainter *painter, const QStyleOptionViewItem &maModif,
              const QModelIndex &index) const;
 void setVisual(const bool isPresent, stTbFiltres *a, QPainter *painter, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const;
 void cellWrite(QPainter *painter, QRect curCell, const QString myTxt, Qt::GlobalColor inPen=Qt::black,  bool up=false) const;


 private:
 QSqlDatabase db_1;
 stPrmDlgt flt;
 QSqlQueryModel *model;
 //int flt.start;
 //QString flt.zne;
 //QString cur_tp;
 //etCount flt.eTyp;
 //Bp::F_Flts flt.b_flt;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(BFlags::Filtres)
Q_DECLARE_METATYPE(BFlags::Filtre)

#endif // BFLAGS_H
