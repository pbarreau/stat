#ifndef BFPM_UPL_H
#define BFPM_UPL_H

#include <QSortFilterProxyModel>

class BFpm_upl : public QSortFilterProxyModel
{
  Q_OBJECT

 public:
  BFpm_upl(int cStart, int cLen);
  void setLen(int len);
  void setStart(int start);
  void setSearchItems(QString items);

  protected:
  bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

  private:
  bool isInSearchZone(int startCol, QModelIndex un_index) const;

  private:
  int col_end;
  QString lst_items;
  int col_start;
};

#endif // BFPM_UPL_H
