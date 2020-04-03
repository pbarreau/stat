#include <QSqlQueryModel>

#include "BSqlQmTirages_3.h"

BSqlQmTirages_3::BSqlQmTirages_3(stGameConf *conf, QObject *parent):QSqlQueryModel(parent)
{

}

Qt::ItemFlags BSqlQmTirages_3::flags(const QModelIndex & index) const
{
 //Qt::ItemFlags l_Flags ( Qt::ItemIsEnabled | Qt::ItemIsSelectable );

 if(index.column() == 7){
  return  QSqlQueryModel::flags(index)|Qt::ItemIsUserCheckable;
 }
 else {
  return QSqlQueryModel::flags(index);
 }
}

QVariant BSqlQmTirages_3::data(const QModelIndex & index, int role) const
{
 if(index.column() == 7)
 {
  if(role == Qt::CheckStateRole){
   int val = index.data().toInt();
   if (val == 0){
    return Qt::Checked;
   }
  }
  else {
   return "";
  }
 }

 return QSqlQueryModel::data(index, role);
}


bool BSqlQmTirages_3::setData(const QModelIndex & index, const QVariant & value, int role)
{
 if(index.column() == 7 && role == Qt::CheckStateRole)
 {
  //implement your logic to set the check state
  //....
  return false;
 }
 else
  return QSqlQueryModel::setData(index, value, role);
}
