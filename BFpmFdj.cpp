#include "BFpmFdj.h"


BFpmFdj::BFpmFdj()
{
 def_col=0;
 def_rules="";
 def_sel = "";
}

bool BFpmFdj::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
 //Q_UNUSED(sourceParent)

 bool b_retVal = true;

 QRegExp chk(def_rules,Qt::CaseInsensitive);
 int col_flt = def_col;

 if( col_flt== Bp::colId){
  return true;
 }

 //int col_flt = filterKeyColumn();
 QModelIndex un_index = sourceModel()->index(sourceRow, col_flt, sourceParent);;

 if(un_index.column()==col_flt){

	if((col_flt == Bp::colTfdjDate)||(col_flt == Bp::colTfdjJour)){
	 QString st_val = un_index.data().toString();
	 b_retVal = chk.exactMatch(st_val);
	}
	else {
	 b_retVal = isInSearchZone(col_flt,un_index);
	}
 }

 return b_retVal;
}

bool BFpmFdj::isInSearchZone(int startCol, QModelIndex un_index)const
{
 bool b_retVal = true;
 QStringList lst_boules = def_sel.split(",");
 int nb_sel = lst_boules.size();

 for (int b_pos = 0; (b_pos< nb_sel) && b_retVal;b_pos++) {
  int usr_bval = lst_boules[b_pos].toInt();

  bool good_lgn = false;

  for (int col_id = startCol; (col_id  < startCol+len_zn) && !good_lgn; col_id ++) {

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

void BFpmFdj::setFltRules(QString rules, int col_id)
{
 //setFilterRegExp(rules);
 //setFilterKeyColumn(col_id);
 def_rules = rules;
 def_col = col_id;

 /// Voir comment sauvegarder le proxymodel en  cours ???
 /// https://stackoverflow.com/questions/19835618/how-to-make-transparent-proxy-model-qabstractproxymodel
 /// https://doc.qt.io/archives/qt-4.8/qidentityproxymodel.html
 ///
 invalidateFilter();
}

void BFpmFdj::saveSourceProxy(QAbstractItemModel *newSourceModel)
{
 prox = new TTransparentProxyModel();
 prox->setSourceModel(newSourceModel);
}

void BFpmFdj::setLenZone(int len)
{
 len_zn = len;
}

void  BFpmFdj::setSearchInZone(QString lst_items)
{
 def_sel = lst_items;
}
