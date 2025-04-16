#ifndef BFPMELM_H
#define BFPMELM_H

#include <QSortFilterProxyModel>
#include "bstflt.h"
#include "BView_1.h"


class BFpmElm : public QSortFilterProxyModel
{
  Q_OBJECT

 public:
  BFpmElm();
  void setFilterText(QString txt);

 protected:
  bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

 private:
  QString filter;

};

#endif // BFPMELM_H
