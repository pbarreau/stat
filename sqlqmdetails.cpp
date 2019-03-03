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
  PreparerTableau();

  QSortFilterProxyModel *m=new QSortFilterProxyModel();
  m->setDynamicSortFilter(true);
  m->setSourceModel(this);
  param.view->setModel(m);


  BDelegateCouleurFond::st_ColorNeeds a;
  a.ori = this;
  a.cnx = param.cnx;
  a.wko = param.wko;
  a.b_min = b_min;
  a.b_max = b_max;
  a.len =6;
  BDelegateCouleurFond *color = new BDelegateCouleurFond(a,param.view);
  param.view->setItemDelegate(color);
  /// Attente du nom de la table des couleurs
  /// ????this->a.ori->slot_ShowTotalBoule()

}

void sqlqmDetails::PreparerTableau(void)
{
  int nbcol = this->columnCount();
  b_min=0;
  b_max=0;
  for(int i = 0; i<nbcol;i++)
  {
    QString headName = this->headerData(i,Qt::Horizontal).toString();

    // Tableau details
    if(headName== "T"){
      b_min = i;
    }

    // Tableau ecart
    if(headName== "Ec"){
      b_max = i;
    }

    if(headName.size()>2)
    {
      this->setHeaderData(i,Qt::Horizontal,headName.left(2));
    }
  }

}

QVariant sqlqmDetails::data(const QModelIndex &index, int role)const
{
  int col = index.column();

  if(col >= b_min && col <= b_max  )
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

  if( (col == b_min) || (col==b_max))
  {
    if(role == Qt::TextColorRole)
    {
      return QColor(Qt::red);
    }
  }

  /// Par defaut retourner contenu initial
  return QSqlQueryModel::data(index,role);
}
