#include <QColor>
#include <QPainter>

#include "sqlqmdetails.h"
#include "delegate.h"

sqlqmDetails::sqlqmDetails(st_sqlmqDetailsNeeds param,QObject *parent):QSqlQueryModel(parent)
{
    db_0 = QSqlDatabase::database(param.cnx);
    this->setQuery(param.sql,db_0);
    PreparerTableau();
    BDelegateCouleurFond *color = new BDelegateCouleurFond(b_min,b_max,6,param.view);
    param.view->setItemDelegate(color);
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

    if(col >= b_min && col < b_max  )
    {
        //if(index.data().canConvert(QMetaType::Int))
        {
            int val = QSqlQueryModel::data(index,role).toInt();
            //int val = index.data().toInt();
            if(role == Qt::DisplayRole)
            {
                if(val <=9){
                    QString sval = QString::number(val).rightJustified(2,'0');
                    return sval;
                }
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

void BDelegateCouleurFond::paint(QPainter *painter, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    int col = index.column();
    //int row = index.row();
    //int nbCol = index.model()->columnCount();
    //int val = 0;
    QStyleOptionViewItem maModif(option);

    if(col == 2 ){
        QColor leFond = MonSetColor(index);
        painter->fillRect(option.rect, leFond);
    }
    if(col >= b_min && col < b_max){
        painter->fillRect(option.rect, COULEUR_FOND_DETAILS);
    }

    if(col >= b_max && col < b_max+len){
        painter->fillRect(option.rect, COULEUR_FOND_ECARTS);
    }

    QItemDelegate::paint(painter, maModif, index);
}

QColor BDelegateCouleurFond::MonSetColor(const QModelIndex &index) const
{
   QColor color;

   float r = 2;
   float Ec = (index.sibling(index.row(),b_max)).data().toFloat();
   int Ep = (index.sibling(index.row(),b_max+1)).data().toInt();
   float Em = (index.sibling(index.row(),b_max+2)).data().toFloat();
   float EM = (index.sibling(index.row(),b_max+3)).data().toFloat();
   float Es = (index.sibling(index.row(),b_max+4)).data().toFloat();
   float Me = (index.sibling(index.row(),b_max+5)).data().toFloat();

   int val = 0;
   int lgt = 0;
   // Disque Ep
   if((abs(Ep-Ec)%Ep)<= Ep){
       val = val | (0x1<<0);
       // Chaud - Froid
       if(abs(Ep-Ec)<=r){
          lgt = lgt | (0x1<<0);
       }
       else{
         lgt = lgt & ~(0x1<<0);
       }
   }
   else{
       val = val & ~(0x1<<0);
   }

   // Disque Em
   if(abs(Em-Ec)<Em){
       val = val | (0x1<<1);
       // Chaud - Froid
       if(abs(Em-Ec)<=r){
          lgt = lgt | (0x1<<1);
       }
       else{
         lgt = lgt & ~(0x1<<1);
       }
   }
   else{
       val = val & ~(0x1<<1);
   }

   // Disque EM
   if(abs(EM-Ec)<EM){
       val = val | (0x1<<2);
       // Chaud - Froid
       if(abs(EM-Ec)<=r){
          lgt = lgt | (0x1<<2);
       }
       else{
         lgt = lgt & ~(0x1<<2);
       }
   }
   else{
       val = val & ~(0x1<<2);
   }


   // Disque Es
   if(abs(Es-Ec)<Es){
       val = val | (0x1<<3);
       // Chaud - Froid
       if(abs(Es-Ec)<=r){
          lgt = lgt | (0x1<<3);
       }
       else{
         lgt = lgt & ~(0x1<<3);
       }
   }
   else{
       val = val & ~(0x1<<3);
   }


   // Disque Me
   if(abs(Me-Ec)<Me){
       val = val | (0x1<<4);
       // Chaud - Froid
       if(abs(Me-Ec)<=r){
          lgt = lgt | (0x1<<4);
       }
       else{
         lgt = lgt & ~(0x1<<4);
       }
   }
   else{
       val = val & ~(0x1<<4);
   }

   /// ----------------
   if(val == 0){
       color.setRgb(255,255,255,255);
   }
   else{
       //color.setRgb((val*8),255-(val*8),(val*8),lgt*8);
       color.setRed(val*8);
       color.setGreen(255-(val*8));
       color.setBlue(255-(val*8));
       color.setAlpha(lgt*8);

   }

   return color;
}
