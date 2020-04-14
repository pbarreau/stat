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


class BDelegateFilterGrp : public QItemDelegate
{
 Q_OBJECT
 public:
 BDelegateFilterGrp(QWidget *parent = nullptr) : QItemDelegate(parent) {}
 void paint(QPainter *painter, const QStyleOptionViewItem &option,
            const QModelIndex &index) const;
};



#endif // DELEGATE_H
