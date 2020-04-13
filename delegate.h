#ifndef DELEGATE_H
#define DELEGATE_H

#include <QObject>
#include <QItemDelegate>
#include <QSqlQueryModel>

#include "compter.h"
#include "colors.h"

class BDelegateStepper : public QItemDelegate
{
 Q_OBJECT
 public:
 // https://doc.qt.io/archives/qt-4.8/qflags.html#details

 enum JourBoule{pasImportant=0x00,
                  hier=0x01,
                  aujourdhui=0x02,
                  demain=0x04};

 BDelegateStepper(QWidget *parent = nullptr) : QItemDelegate(parent) {}
 void paint(QPainter *painter, const QStyleOptionViewItem &option,
            const QModelIndex &index) const;
};

class BDelegateElmOrCmb : public QItemDelegate
{
 Q_OBJECT

 public:
 typedef struct _stPrmDlgt{
  QWidget *parent = nullptr;
  QString db_cnx;
  int start=0;
  int zne=0;
  int typ=0;
  etCount eTyp = eCountToSet;
 }stPrmDlgt;

 enum Filtre  {isLast=1,isPrevious=1<<1, isWanted=1<<2, isNever=1<<3,
               isPlusOne=1<<4, isMinusOne=1<<5};

 Q_DECLARE_FLAGS(Filtres, Filtre)
 Q_FLAG(Filtres)

 public:
 BDelegateElmOrCmb(stPrmDlgt prm);
 void paint(QPainter *painter, const QStyleOptionViewItem &option,
            const QModelIndex &index) const;

 private:
 QSqlDatabase dbToUse;
 int col_show;
 QString cur_zn;
 QString cur_tp;
 etCount eTyp;
};

class BDelegateFilterGrp : public QItemDelegate
{
 Q_OBJECT
 public:
 BDelegateFilterGrp(QWidget *parent = nullptr) : QItemDelegate(parent) {}
 void paint(QPainter *painter, const QStyleOptionViewItem &option,
            const QModelIndex &index) const;
};



#endif // DELEGATE_H
