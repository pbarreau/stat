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

bool BDelegateCouleurFond::calcul(int centre, int pos)const
{
    bool ret_val;
    int distance = abs(centre-pos);

    if(distance<= centre){
        ret_val = true;
    }
    else{
        ret_val = false;
    }

    return ret_val;
}

BDelegateCouleurFond::BDelegateCouleurFond(int b_min, int b_max, int len, QWidget *parent)
     :QItemDelegate(parent),b_min(b_min),b_max(b_max)
{

    /// creation d'un tableau des couleurs
    nb_colors = pow(2,len-1);
    int step_colors = nb_colors/2;
    val_color = new int[step_colors];

    int j=0;
    for(int i = 1; i<= step_colors; i++){
        val_color[i-1]=j;
        j = j + (255/step_colors);
    }
}

QColor BDelegateCouleurFond::MonSetColor(const QModelIndex &index) const
{
    QColor color;

    int val = 0;
    int alp = 0;
    int r = 2;

    int tab_pri[5]={3,4,1,2,5};

    int Ec = (index.sibling(index.row(),b_max)).data().toInt();
    for(int i=1;i<=5;i++){
        int centre = (index.sibling(index.row(),b_max+i)).data().toInt();
        int pri = tab_pri[i-1]-1;

        int item_couleur = calcul(centre,Ec);
        val = val + pow(2,pri) * item_couleur;

        int item_alpha = calcul(r,centre);
        alp = alp + item_alpha;
    }

    /// ----------------
    if(val == 0){
        color.setRgb(255,255,255,255);
    }
    else{
        if(val<nb_colors){
            if(val<(nb_colors/2)){
                /// Partir du vert vers le Jaune
                color.setRed(val_color[val]);
                color.setGreen(255);
                color.setBlue(0);
            }
            else{
                color.setRed(0);
                color.setGreen(val_color[val%(nb_colors/2)]);
                color.setBlue(0);
            }
        }
        else{
            /// Erreur de calcul !!
            color.setRed(39);
            color.setGreen(93);
            color.setBlue(219);
        }

        // proche du centre ?
        color.setAlpha((255-(5*val_color[1])) + val_color[alp]);

    }

    return color;
}
