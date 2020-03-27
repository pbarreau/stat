#include <QSortFilterProxyModel>

#include "BFpm_1.h"


BSimFlt::BSimFlt(eFlt type)
{
 cur = type;
}

#if 1
QStandardItem * BSimFlt::clone() const
{
 return new BSimFlt(eFlt::efltNone);
}
#endif

// -------------------
BFpm_1::BFpm_1()
{
 flt = eFlt::efltNone;
 str_key = "";
}

BFpm_1::BFpm_1(const BFpm_1 &)
{

}

bool BFpm_1::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
 bool ret = true;
 QModelIndex un_index;
 QString celVal="";

 switch(flt){
  case eFlt::efltNone:
   break;

  case eFlt::efltJour:
   /// La colonne Jour est a la position 2 dans la table RefTirages
   un_index = sourceModel()->index(sourceRow, 2, sourceParent);
   celVal = un_index.data().toString();
   ret = celVal.contains(str_key,Qt::CaseInsensitive);
   break;


	case eFlt::efltEnd:
	default:
	 ret = false;
	 break;
 }
 return ret;
}

void BFpm_1::setFlt(eFlt flt_value)
{
 flt=flt_value;
}

void BFpm_1::setStringKey(QString str_value)
{
 str_key=str_value;
 invalidateFilter();
// filterChanged();
}
