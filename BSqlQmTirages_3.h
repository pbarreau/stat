#ifndef BSQLQMTIRAGES_3_H
#define BSQLQMTIRAGES_3_H

#include <QObject>
#include <QSqlQueryModel>

#include "game.h"

class BSqlQmTirages_3:public QSqlQueryModel
{
 Q_OBJECT

 public:
explicit BSqlQmTirages_3(stGameConf *conf, QObject * parent=NULL);
Qt::ItemFlags flags(const QModelIndex & index) const;
QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
};

#endif // BSQLQMTIRAGES_3_H
