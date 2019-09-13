#ifndef DELEGATE_H
#define DELEGATE_H

#include <QObject>
#include <QItemDelegate>
#include <QSqlQueryModel>
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

 BDelegateStepper(QWidget *parent = 0) : QItemDelegate(parent) {}
 void paint(QPainter *painter, const QStyleOptionViewItem &option,
            const QModelIndex &index) const;
};

class BDelegateElmOrCmb : public QItemDelegate
{
 Q_OBJECT

 public:
 typedef struct _stPrmDlgt{
  QWidget *parent = 0;
  QString db_cnx;
  int zne=0;
  int typ=0;
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
 QString cur_zn;
 QString cur_tp;
};

class BDelegateFilterGrp : public QItemDelegate
{
 Q_OBJECT
 public:
 BDelegateFilterGrp(QWidget *parent = 0) : QItemDelegate(parent) {}
 void paint(QPainter *painter, const QStyleOptionViewItem &option,
            const QModelIndex &index) const;
};


class BSqmColorizePriority:public QSqlQueryModel
{
 Q_OBJECT

 public: ///
 explicit BSqmColorizePriority(QObject *parent = 0);
 QVariant data(const QModelIndex &index, int role) const;
};

#endif // DELEGATE_H
