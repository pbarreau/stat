#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QTableView>
#include <QSortFilterProxyModel>

#include "BSqlQmTirages_3.h"

BSqlQmTirages_3::BSqlQmTirages_3(stGameConf *conf, QString cnx, QString tbl, QTableView *tab, QObject *parent):QSqlQueryModel(parent)
{
 db_cnx = cnx;
 db_tbl = tbl;
 db_tbv = tab;
}

Qt::ItemFlags BSqlQmTirages_3::flags(const QModelIndex & index) const
{
 Qt::ItemFlags l_Flags = QSqlQueryModel::flags(index);

 if(index.column() == 7){
  l_Flags= (Qt::ItemIsUserCheckable) | l_Flags;
 }

 return l_Flags;
}

QVariant BSqlQmTirages_3::data(const QModelIndex & index, int role) const
{
 Qt::ItemDataRole l_role = static_cast<Qt::ItemDataRole>(role);

 if(index.column() == 7)
 {
  if(role==Qt::CheckStateRole){
   int val = QSqlQueryModel::data(index,Qt::DisplayRole).toInt();
   return(static_cast<Qt::CheckState>(val));
  }
  else {
   return ("");
  }
 }

 return QSqlQueryModel::data(index, role);
}


/// https://stackoverflow.com/questions/35305801/qt-checkboxes-in-qtableview
/// https://doc.qt.io/qt-5.12/qtsql-querymodel-editablesqlmodel-cpp.html
/// https://www.qtcentre.org/threads/43849-Checkboxes-in-QTableView-with-my-custom-model
bool BSqlQmTirages_3::setData(const QModelIndex & index, const QVariant & value, int role)
{
 Qt::ItemDataRole l_role = static_cast<Qt::ItemDataRole>(role);
 bool ret = true;

 if(index.column()<7)
  ret = QSqlQueryModel::setData(index, value, role);

 QModelIndex primaryKeyIndex = QSqlQueryModel::index(index.row(), 0);
 int id = data(primaryKeyIndex).toInt();


 if( (index.column() == 7) && (role == Qt::CheckStateRole) && (ret == true))
 {
  int chk_val = value.toInt();
  Qt::CheckState chk = static_cast<Qt::CheckState>(chk_val);
  ret = setVisualChk(id,chk);
  QPersistentModelIndex per_index(index);
  Q_EMIT sig_chkChanged(per_index,chk);
 }

 return ret;
}

bool BSqlQmTirages_3::setVisualChk(int id, Qt::CheckState chk)
{
 QSqlDatabase db = QSqlDatabase::database(db_cnx);
 QSqlQuery query(db);
 bool b_retVal = false;

 /// Recuperer le code ayant permis les resultats
#if 0
 QAbstractItemModel *qtv_model = db_tbv->model();
 QSortFilterProxyModel *A1 = qobject_cast<QSortFilterProxyModel*>(qtv_model);
 BSqlQmTirages_3 *A2 = qobject_cast<BSqlQmTirages_3*>(A1->sourceModel());
 QString s_tmp = A2->query().executedQuery();
#endif
 QString s_tmp = this->query().executedQuery();

 /// Preparer la mise a jour
 QString msg = "update "
               +db_tbl
               +" set chk = "
               +QString::number(chk)
               +" where (id = "
               +QString::number(id)
               +")";

 /// Execution
 b_retVal = query.exec(msg);

 /// Mise a jour ?
 if(b_retVal){
  this->query().clear();
  this->setQuery(s_tmp,db);
#if 0
  A2->query().clear();
  A2->setQuery(s_tmp,db);
  A1->setSourceModel(A2);
  db_tbv->setModel(A1);
#endif

 }

 return  b_retVal;
}

QTableView *BSqlQmTirages_3::getTbv()
{
 return db_tbv;
}
