#include <QSortFilterProxyModel>

#include "BFpmElm.h"

BFpmElm::BFpmElm()
{
 setFilterKeyColumn(Bp::colTxt);
 filter = "";
}

void BFpmElm::setFilterText(QString txt)
{
 filter = txt;
 invalidateFilter();
}

bool BFpmElm::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent)const
{
 //Q_UNUSED(sourceParent)

 bool b_retVal = true;

 if(filter == ""){
  return true;
 }

 QModelIndex un_index = sourceModel()->index(sourceRow,Bp::colTxt,sourceParent);

 /*
  *   QString cell = un_index.data().toString();
  QStringList val = cell.split(',');

  for(int pos = 0; pos < val.size() || b_retVal; pos ++){
   b_retVal = val.at(pos).contains(filter);
  }
*/

 if(un_index.column()==Bp::colTxt){
  QStringList usr = filter.split(',');
  bool good_lgn = false;

  for(int i = 0; i< usr.size() && !good_lgn;i++){
   int i_usr = usr.at(i).toInt();

   QString cell = un_index.data().toString();
   QStringList val = cell.split(',');

   for(int j = 0; j < val.size(); j++){
    int j_val = val.at(j).toInt();

    if(i_usr == j_val){
     good_lgn = true;
     break;
    }
   }
   b_retVal = b_retVal && good_lgn;
   if(i< usr.size() -1){
    good_lgn = false;
   }
  }
  //b_retVal = b_retVal && good_lgn;
 }

 return b_retVal;
}
