#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QSqlQuery>
#include <QStyledItemDelegate>
#include <QSqlError>

#include "bpopupcolor.h"
#include "db_tools.h"

BPopupColor::BPopupColor(PopParam param, QTableView *parent)
:QStyledItemDelegate (parent)
{
 db_0 = QSqlDatabase::database(param.cnx);
 recupereMapColor(param.tb_cld);
}

void BPopupColor::paint(QPainter *painter, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const
{
  int col = index.column();

  if(index.column() == Columns::ValB ){
    int val_col_2 = (index.sibling(index.row(),Columns::ColorCb)).data().toInt();
    QColor leFond = map_colors.key(val_col_2);
    painter->fillRect(option.rect, leFond);
  }


  QStyledItemDelegate::paint(painter, option, index);

}

QSize BPopupColor::sizeHint(const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{
  return QStyledItemDelegate::sizeHint(option, index);
}

bool BPopupColor::recupereMapColor(QString tbl_def)
{
  bool b_retVal = true;
  QSqlQuery query(db_0);
  QString msg = "Select * from "+tbl_def;

  if((b_retVal = query.exec(msg)))
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
        if(!(b_retVal=map_colors.contains(a))){
          map_colors.insert(a,key-1);
        }
      }while(query.next()&& (b_retVal==false));
      nb_colors = map_colors.size();
      if(b_retVal){
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

  return b_retVal;
}
