#include "BFpmCmb.h"
#include "bstflt.h"
#include "BView_1.h"
BFpmCmb::BFpmCmb(const stGameConf *pGame,BView_1 * in_tbview):gme_conf(pGame),use_view(in_tbview)
{

}

bool BFpmCmb::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent)
{
Q_UNUSED(sourceParent)

 bool b_retVal = true;

 if(filterKeyColumn()==0){
  return true;
 }

 QModelIndex un_index = this->index(sourceRow,Bp::colTxt);

 if(un_index.column()==Bp::colTxt){
  QString val = un_index.data().toString();

	QRegExp chk(filterRegExp());
	if(chk.isEmpty()==false){
	 this->invalidateFilter();
	 b_retVal = chk.exactMatch(val);
	}
 }

 return b_retVal;
}
