#ifndef BFPMFDJ_H
#define BFPMFDJ_H

#include <QSortFilterProxyModel>
#include "bstflt.h"

class BFpmFdj : public QSortFilterProxyModel
{
 Q_OBJECT

 public:
 BFpmFdj();
 void setLenZone(int len);
 void setSearchInZone(QString lst_items);
 void setFltRules(QString rules, int col_id);

 protected:
 bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

 private:
 QString getFltRgx(const int gme_zn, const QString &key);
 bool isInSearchZone(int startCol, QModelIndex un_index) const;

 private:
 int len_zn;
 QString def_rules;
 QString def_sel;
 int def_col;

};

#endif // BFPMFDJ_H
