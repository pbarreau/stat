#include "BFpm_upl.h"

BFpm_upl::BFpm_upl(int cStart, int cLen)
{
 col_start=cStart;
 col_end = cLen;
 lst_items = "";
}

bool BFpm_upl::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
 bool b_retVal = true;

 if(lst_items.isEmpty())
  return b_retVal;

 int col_in = col_start;

 QModelIndex un_index = sourceModel()->index(sourceRow, col_in, sourceParent);;

 if(un_index.column()==col_in){
   b_retVal = isInSearchZone(col_in,un_index);
 }

 return b_retVal;
}

bool BFpm_upl::isInSearchZone(int startCol, QModelIndex un_index)const
{
 bool b_retVal = true;

 QStringList lst_boules = lst_items.split(",");
 lst_boules.replaceInStrings(QRegExp("\\s+"),""); // supression espaces
 int nb_sel = lst_boules.size();

 for (int b_pos = 0; (b_pos< nb_sel) && b_retVal;b_pos++) {
  int usr_bval = lst_boules[b_pos].toInt();

  bool good_lgn = false;

  for (int col_id = startCol; (col_id  < startCol+col_end) && !good_lgn; col_id ++) {

	 un_index = sourceModel()->index(un_index.row(), col_id , QModelIndex());
	 int tbl_bval = un_index.data().toInt();

	 if(tbl_bval==usr_bval){
		good_lgn = true;
		break;
	 }
	} /// for colonnes
	b_retVal = b_retVal && good_lgn;
 }
 return b_retVal;
}

void  BFpm_upl::setSearchItems(QString items)
{
 lst_items = items;
 invalidateFilter();
}

void BFpm_upl::setStart(int start)
{
 col_start = start;
}

void BFpm_upl::setLen(int len)
{
 col_end = len;
}
