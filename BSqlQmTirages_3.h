#ifndef BSQLQMTIRAGES_3_H
#define BSQLQMTIRAGES_3_H

#include <QSqlQueryModel>
#include <QTableView>

#include "game.h"

class BSqlQmTirages_3 : public QSqlQueryModel
{
 Q_OBJECT

 public:
 explicit BSqlQmTirages_3(stGameConf *conf, QString cnx, QString tbl, QTableView *tab, QObject * parent=nullptr);
 Qt::ItemFlags flags(const QModelIndex & index) const override;
 QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
 bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;

 private:
 bool setVisualChk(int id, Qt::CheckState chk);

 private:
 QString db_cnx;
 QString db_tbl;
 QTableView *db_tbv;
};

#endif // BSQLQMTIRAGES_3_H
