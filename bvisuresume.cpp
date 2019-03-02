#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QToolTip>

#include <QAbstractItemModel>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QColor>
#include <QComboBox>
#include <QHeaderView>
#include <QTreeView>

#include "bvisuresume.h"
#include "btablevieweditor.h"

#include "db_tools.h"

BVisuResume::BVisuResume(prmBVisuResume param, QTableView *parent)
  :QStyledItemDelegate (parent)
{
  db_0 = QSqlDatabase::database(param.cnx);
  tb_rsm_src = param.tb_rsm;
  tb_tot_src = param.tb_tot;
  recupereMapColor(param.tb_cld);
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

  if(col == COL_VISU_COMBO){
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

/// -----------------------------------

QSize BVisuResume::sizeHint(const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{
  return QStyledItemDelegate::sizeHint(option, index);
}

QWidget *BVisuResume::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{

  if(index.column() == COL_VISU_COMBO){
    QComboBox * tmp_combo = new QComboBox(parent);

    return  tmp_combo;
  }

}

void BVisuResume::setEditorData(QWidget *editor, const QModelIndex &index) const
{
  if(index.column() == COL_VISU_COMBO){
    QComboBox * tmp_combo = qobject_cast<QComboBox *>(editor);
    QTableView *sourceView = new QTableView(); // QTreeview

    //sourceView->setRootIsDecorated(false);
    tmp_combo->setView(sourceView);
    sourceView->setAlternatingRowColors(true);

    sourceView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded); //fine tuning of some options
    sourceView->setSelectionMode(QAbstractItemView::SingleSelection);
    sourceView->setSelectionBehavior(QAbstractItemView::SelectRows);
    //sourceView->setAutoScroll(false);

    //sourceView->hideColumn(0);
    QString sval = (index.sibling(index.row(),COL_VISU_RESUME)).data().toString();
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

    /*
    //sourceView->resizeColumnsToContents();
    int nb_col = sqm_tmp->columnCount();
    for(int i = 0; i< nb_col;i++){
      //sourceView->setColumnWidth(i,50);
    }
    for(int i = 0; i<= COL_VISU_RESUME;i++){
      sourceView->hideColumn(i);
    }
*/
    //sourceView->header()->hide();
    //sourceView->resizeColumnToContents(0);
    tmp_combo->setModel(sqm_tmp);

  }

}

void BVisuResume::setModelData(QWidget *editor, QAbstractItemModel *model,
                               const QModelIndex &index) const
{
  if(index.column() == COL_VISU_COMBO){
  }

}

void BVisuResume::updateEditorGeometry(QWidget *editor,
                                       const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  if(index.column() == COL_VISU_COMBO){
    editor->setGeometry(option.rect);
  }
}
