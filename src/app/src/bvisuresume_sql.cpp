#include "bvisuresume_sql.h"
#include "bvisuresume.h"

BVisuResume_sql::BVisuResume_sql(stBVisuResume_sql param,QObject *parent):QSqlQueryModel(parent)
{
  db_0 = QSqlDatabase::database(param.cnx);
  tb_tot_src = param.tb_tot;
  tb_rsm_src = param.tb_rsm;

}

QVariant BVisuResume_sql::data(const QModelIndex &index, int role)const
{
  int col = index.column();

  if(col == COL_VISU_RESUME +2  )
  {
    int val = QSqlQueryModel::data(index,role).toInt();
    if(role == Qt::TextColorRole)
    {
      return QColor(Qt::red);
    }

    if(role == Qt::DisplayRole)
    {
      if(val <=99){
        QString sval = QString::number(val).rightJustified(3,'0');
        return sval;
      }
    }
  }

  if(col == COL_VISU_COMBO){
    if(role == Qt::DisplayRole)
    {
      /*
      QString sval = (index.sibling(index.row(),COL_VISU_RESUME)).data().toString();

      QComboBox *qcb_tmp = new QComboBox;

      QTableView *qtv_tmp = new QTableView();


      QSqlQueryModel *sqm_tmp = new QSqlQueryModel;
      QString msg = "select t1.c,t2.c as Cb,t1.bc, t1.T, t1.b,T1.tb,"
                    "printf(\"%.2f%%\",((t1.tb*100)/t1.t)) as 'Tb/T %',"
                    "printf(\"%.2f%%\",((t2.c*100)/65)) as 'Ce %' "
                    "FROM ("
                    +tb_rsm_src
                    +") as t1,("
                    +tb_tot_src
                    +") as t2 "
                     "WHERE (t1.bc ="
                    +sval
                    +" AND t1.b = t2.b) ORDER by t1.t DESC , t1.tb DESC";
#ifndef QT_NO_DEBUG
      qDebug() << "Combo :" << msg;
#endif

      sqm_tmp->setQuery(msg,db_0);
      qcb_tmp->setModel(sqm_tmp);
      qcb_tmp->setView(qtv_tmp);


      return   QVariant::fromValue(qcb_tmp);
      */
    }
  }
  /// Par defaut retourner contenu initial
  return QSqlQueryModel::data(index,role);
}

/// --- Rendre la requete editable
Qt::ItemFlags BVisuResume_sql::flags(const QModelIndex &index) const
{
  return Qt::ItemIsSelectable|Qt::ItemIsEnabled| Qt::ItemIsEditable;// ItemIsEnabled| Qt::ItemIsEditable
}
