#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QToolTip>

#include <QAbstractItemModel>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QColor>


#include "bvisuresume.h"
#include "btablevieweditor.h"

#include "db_tools.h"

BVisuResume::BVisuResume(prmBVisuResume param, QTableView *parent)
  :QStyledItemDelegate (parent)
{
  db_0 = QSqlDatabase::database(param.cnx);
  tb_test = param.wko;
  a=new QTableWidget(1,10);
  recupereMapColor(param.cld);
}

void BVisuResume::paint(QPainter *painter, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const
{
  int col = index.column();

  if(col== index.model()->columnCount()-2){
    int a=0;
    a++;
  }
  if(col == COL_VISU_RESUME ){
    int val_col_2 = (index.sibling(index.row(),COL_VISU_RESUME-1)).data().toInt();
    QColor leFond = map_colors.key(val_col_2);
    painter->fillRect(option.rect, leFond);
  }

  QStyledItemDelegate::paint(painter, option, index);

}

bool BVisuResume::recupereMapColor(QString tbl_def)
{
  bool isOk = true;
  QSqlQuery query(db_0);
  QString msg = "Select * from "+tbl_def;

  if((isOk = query.exec(msg)))
  {
    query.first();
    if(query.isValid())
    {
      int key = -1;
      /// La table existe
      do{
        key = query.value(0).toInt();
        QString color = query.value(1).toString();
        QColor a;
        a.setNamedColor(color);
        if(!(isOk=map_colors.contains(a))){
          map_colors.insert(a,key-1);
        }
      }while(query.next()&& (isOk==false));
      nb_colors = map_colors.size();
      if(isOk){
#ifndef QT_NO_DEBUG
        qDebug()<<"Erreur : presence couleur deja la pour clef "<<key;
#endif
      }
    }
  }

  if(query.lastError().isValid()){
    DB_Tools::DisplayError("BVisuResume::",&query," recupereMapColor ");
  }

#ifndef QT_NO_DEBUG
  QMapIterator<BOrdColor, int>  val (map_colors);
  while (val.hasNext()) {
    val.next();
    qDebug() << val.key() << ": " << val.value() << endl;
  }
  ;
#endif

  return isOk;
}

void BVisuResume::slot_AideToolTip(const QModelIndex & index)
{
  const QAbstractItemModel * pModel = index.model();
  int col = index.column();

  QVariant vCol = pModel->headerData(col,Qt::Horizontal);
  QString headName = vCol.toString();

  QTableView *view = qobject_cast<QTableView *>(sender());
  QSortFilterProxyModel *m=qobject_cast<QSortFilterProxyModel*>(view->model());
  BVisuResume_sql *sqm_tmp= qobject_cast<BVisuResume_sql*>(m->sourceModel());
  QVariant item1 = sqm_tmp->data(index,Qt::BackgroundRole);

  QString msg1="";
  QString s_nb = index.model()->index(index.row(),COL_VISU_RESUME).data().toString();
  msg1 = QString("Regroupement %1").arg(s_nb);
  if (col == COL_VISU_RESUME+2)
  {
    QString s_va = index.model()->index(index.row(),col).data().toString();
    QString s_hd = headName;
    msg1 = msg1 + QString("\n%1 = %2").arg(s_hd).arg(s_va);
  }

  QString msg2 = "";
  if(index.column()==COL_VISU_RESUME){
    int val_col_2 = (index.sibling(index.row(),COL_VISU_RESUME-1)).data().toInt();
    QColor leFond = map_colors.key(val_col_2);
    int ligne = map_colors.value(leFond,-1);
    double p = (ligne*100)/nb_colors;
    if(map_colors.contains(leFond)){
      msg2 = "Critere ecart : "+ QString::number(ligne).rightJustified(2,'0')
             +" sur "
             + QString::number(nb_colors).rightJustified(2,'0')
             + " ("+QString::number(p)+"%)\nRVBA"
             + "("+QString::number(leFond.red()).rightJustified(3,'0')
             + ","+QString::number(leFond.green()).rightJustified(3,'0')
             + ","+QString::number(leFond.blue()).rightJustified(3,'0')
             + ","+QString::number(leFond.alpha()).rightJustified(3,'0')
             +")";
    }
    else{
      msg2 = "Error !!";
    }
  }

  QString msg = msg1+QString("\n")+msg2;

#ifndef QT_NO_DEBUG
  qDebug() << "Tooltips :" << msg;
#endif

  if(msg.length())
    QToolTip::showText (QCursor::pos(), msg);

}

/// ----------------------------
BVisuResume_sql::BVisuResume_sql(stBVisuResume_sql param,QObject *parent):QSqlQueryModel(parent)
{
  db_0 = QSqlDatabase::database(param.cnx);

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
  /// Par defaut retourner contenu initial
  return QSqlQueryModel::data(index,role);
}

/// --- Rendre la requete editable
Qt::ItemFlags BVisuResume_sql::flags(const QModelIndex &index) const
{
  int col = index.column();


  if (col==index.model()->columnCount()-2){
  }


  return Qt::ItemIsSelectable|Qt::ItemIsEnabled| Qt::ItemIsEditable;// ItemIsEnabled
}

bool BVisuResume_sql::setData(const QModelIndex &index, const QVariant &value, int role)
{
  return true;
}
/// -----------------------------------

QSize BVisuResume::sizeHint(const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{
  return QStyledItemDelegate::sizeHint(option, index);
}

QWidget *BVisuResume::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{

  if(index.column() == 6){
#if 0
    //QTableWidget *editor = new QTableWidget(1,10,parent);
    QTableView *editor = new QTableView(parent);
    //QTableView *qtv_tmp = qobject_cast<QTableView *>(editor);
    //QSqlQuery query(db_0);
    QSqlQueryModel *sqm_tmp = new QSqlQueryModel;
    QString msg = "select  C from " + tb_test;
    sqm_tmp->setQuery(msg,db_0);
    editor->setModel(sqm_tmp);
#endif
    QTableWidget *editor = new QTableWidget(1,10);
    editor->show();
    return editor;
  }

}

void BVisuResume::setEditorData(QWidget *editor, const QModelIndex &index) const
{
  if(index.column() == 6){
#if 0
    //QTableView *qtv_tmp = qobject_cast<QTableView *>(editor);
    QSqlQueryModel *sqm_tmp = new QSqlQueryModel;
    QString msg = "select  C from " + tb_test;

    sqm_tmp->setQuery(msg,db_0);
    a.setModel(sqm_tmp);
#endif
  }

}

void BVisuResume::setModelData(QWidget *editor, QAbstractItemModel *model,
                               const QModelIndex &index) const
{
}

void BVisuResume::updateEditorGeometry(QWidget *editor,
                                       const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  if(index.column() == 6){
    editor->setGeometry(option.rect);
  }
}
