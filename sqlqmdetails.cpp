#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QColor>
#include <QPainter>
#include <QToolTip>
#include <QSortFilterProxyModel>
#include <QStandardItem>

#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlError>

#include <QMessageBox>

#include "bdelegatecouleurfond.h"
#include "sqlqmdetails.h"
#include "delegate.h"
#include "db_tools.h"

sqlqmDetails::sqlqmDetails(st_sqlmqDetailsNeeds param,QObject *parent):QSqlQueryModel(parent)
{
  db_0 = QSqlDatabase::database(param.cnx);
  this->setQuery(param.sql,db_0);
  nbJ = this->columnCount() -BDelegateCouleurFond::Columns::TotalElement -2;

  //b_min = param.b_min;
  //b_max = param.b_max;

  /*
  PreparerTableau();

  QSortFilterProxyModel *m=new QSortFilterProxyModel();
  m->setDynamicSortFilter(true);
  m->setSourceModel(this);
  param.view->setModel(m);
  */

  /*
  BDelegateCouleurFond::st_ColorNeeds a;
  a.ori = this;
  a.cnx = param.cnx;
  a.wko = param.wko;
  a.b_min = b_min;
  a.b_max = b_max;
  a.len =6;
  BDelegateCouleurFond *color = new BDelegateCouleurFond(a,param.view);
  param.view->setItemDelegate(color);
  */

}

void sqlqmDetails::PreparerTableau(void)
{
  int nbcol = this->columnCount();
  b_min=0;
  b_max=0;

}

QVariant sqlqmDetails::data(const QModelIndex &index, int role)const
{
  int col = index.column();

  if(col >= BDelegateCouleurFond::Columns::EcartCourant
     &&
     col < (BDelegateCouleurFond::Columns::TotalElement+nbJ)  )
  {
    int val = QSqlQueryModel::data(index,role).toInt();
    if(role == Qt::DisplayRole)
    {
      if(val <=9){
        QString sval = QString::number(val).rightJustified(2,'0');
        return sval;
      }
    }
  }

  if( (col == BDelegateCouleurFond::Columns::EcartCourant)
      ||
      (col == BDelegateCouleurFond::Columns::TotalElement))
  {
    if(role == Qt::TextColorRole)
    {
      return QColor(Qt::red);
    }
  }

  /// Par defaut retourner contenu initial
  return QSqlQueryModel::data(index,role);
}
