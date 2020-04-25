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
BFpm_1::BFpm_1(stTiragesDef *def)
{
 conf=def;
 flt = eFlt::efltNone;
 str_key.clear();
}

BFpm_1::BFpm_1(const BFpm_1 &)
{

}

bool BFpm_1::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
 bool ret = true;

 QModelIndex un_index;
 QString celVal="";
 int col_start=-1;
 int nb_col=0;

 switch(flt){
  case eFlt::efltNone:
   //lgn=true;
   break;

  case eFlt::efltJour:
   /// La colonne Jour est a la position 2 dans la table RefTirages
   col_start=2;
   nb_col=1;
   break;

	case eFlt::efltDate:
	 col_start=3;
	 nb_col=1;
	 break;

	case eFlt::efltComb:
	 col_start=4;
	 nb_col=1;
	 break;

	case eFlt::efltZn_1:
	 col_start=5;
	 nb_col=conf->limites[0].len;
	 break;

	case eFlt::efltZn_2:
	 col_start=5+conf->limites[0].len;
	 nb_col=conf->limites[1].len;
	 break;

	case eFlt::efltEnd:
	default:
	 ret = false;
	 break;
 }

 /// Analyse
 for (int chk_item=0;(chk_item< str_key.size()) && ret;chk_item++) {
  bool lgn = false;

  for (int col_id= col_start; (col_id < col_start+nb_col) && ret && !lgn; col_id++)
  {
   un_index = sourceModel()->index(sourceRow, col_id, sourceParent);

	 if(col_id>=2 && col_id <=4){
		celVal = un_index.data().toString();
		ret = celVal.contains(str_key[chk_item],Qt::CaseInsensitive);
		if(ret){
		 lgn=true;
		}
	 }

	 if(col_id>4){
		int boule = sourceModel()->data(un_index).toInt();
		if(boule==str_key[chk_item].toInt()){
		 lgn = true;
		 break;
		}
	 }
	}

	ret = ret && lgn;

 }

 return ret;
}

void BFpm_1::setFlt(eFlt flt_value)
{
 flt=flt_value;
}

void BFpm_1::setStringKey(QString str_value)
{
 if(str_value.size()){
  str_key=str_value.split(",");
 }
 else {
  str_key.clear();
 }
 invalidateFilter();
}
