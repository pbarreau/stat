#ifndef BFpm_1_H
#define BFpm_1_H

#include <QStandardItem>
#include <QSortFilterProxyModel>

#include "tirages.h"

class BFpm_1;

enum class eFlt
{
 efltNone,
 efltJour,
 efltDate,
 efltComb,
 efltZn_1,
 efltZn_2,
 efltEnd
};
Q_DECLARE_METATYPE(eFlt);

#if 1
class BSimFlt:public QStandardItem
{
 public:
 BSimFlt();
 BSimFlt(eFlt type);
 virtual QStandardItem *clone() const;

 private:
 eFlt cur;
};
//Q_DECLARE_METATYPE(BSimFlt);
#endif

class BFpm_1 : public QSortFilterProxyModel
{
 Q_OBJECT
 public:

public:
explicit BFpm_1(stTiragesDef *def);
~ BFpm_1() = default;
BFpm_1(const BFpm_1 &);


void setFlt(eFlt flt_value);
void setStringKey(QString str_value);

protected:
bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
eFlt flt;
QStringList str_key;
stTiragesDef *conf;
};

#endif // BFpm_1_H
