#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QColor>


#include "bvisuresume.h"
#include "db_tools.h"

BVisuResume::BVisuResume(prmBVisuResume param, QTableView *parent):QItemDelegate(parent)
{
    db_0 = QSqlDatabase::database(param.cnx);
    recupereMapColor(param.cld);
}

void BVisuResume::paint(QPainter *painter, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const
{
    int col = index.column();

    if(col == COL_VISU ){
        int val_col_2 = (index.sibling(index.row(),0)).data().toInt();
        QColor leFond = map_colors.key(val_col_2);
        painter->fillRect(option.rect, leFond);
    }

    QItemDelegate::paint(painter, option, index);

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
/// ----------------------------
BVisuResume_sql::BVisuResume_sql(stBVisuResume_sql param,QObject *parent):QSqlQueryModel(parent)
{
    db_0 = QSqlDatabase::database(param.cnx);

}

QVariant BVisuResume_sql::data(const QModelIndex &index, int role)const
{
    int col = index.column();

    if(col == 3  )
    {
        int val = QSqlQueryModel::data(index,role).toInt();
        if(role == Qt::TextColorRole)
        {
            return QColor(Qt::red);
        }

        if(role == Qt::DisplayRole)
        {
            if(val <=9){
                QString sval = QString::number(val).rightJustified(3,'0');
                return sval;
            }
        }
    }
    /// Par defaut retourner contenu initial
    return QSqlQueryModel::data(index,role);
}
