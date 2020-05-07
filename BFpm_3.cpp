#include "BFpm_3.h"

//---------------
BFpm_3::BFpm_3(int uplet, int start, QObject *parent): QSortFilterProxyModel(parent)
{
 col_tot=uplet;
 col_deb=start;
}

bool BFpm_3::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
 bool ret = true;

 if(lst_usr.isEmpty())
  return ret;

 for (int j = 0; (j< lst_usr.size()) && ret;j++) {
  int sel = lst_usr[j].toInt();

  bool lgn = false;

	for (int i= col_deb; (i < col_deb+col_tot) && !lgn; i++) {
	 QModelIndex cur_index = sourceModel()->index(sourceRow, i, sourceParent);
	 int boule = sourceModel()->data(cur_index).toInt();

	 if(boule==sel){
		lgn = true;
		break;
	 }
	} /// for colonnes
	ret = ret && lgn;

 }

 //int nb_lgn = sourceModel()->rowCount();
 return ret;
}

void BFpm_3::BSlot_MakeUplets(const QString& lstBoules)
{
 if(lstBoules.size()){
  lst_usr = lstBoules.split(",");
 }
 else {
  lst_usr.clear();
 }
 invalidateFilter();
}
