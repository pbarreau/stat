#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QColor>
#include <QPainter>
#include <QToolTip>
#include <QSortFilterProxyModel>
#include <QStandardItem>

#include "sqlqmdetails.h"
#include "delegate.h"

sqlqmDetails::sqlqmDetails(st_sqlmqDetailsNeeds param,QObject *parent):QSqlQueryModel(parent)
{
    db_0 = QSqlDatabase::database(param.cnx);
    this->setQuery(param.sql,db_0);
    PreparerTableau();

    st_ColorNeeds a;
    a.b_min = b_min;
    a.b_max = b_max;
    a.len =6;
    a.parent = this;
    BDelegateCouleurFond *color = new BDelegateCouleurFond(a,param.view);

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

    if(col==2){
        if(role == Qt::BackgroundRole){
            return maCouleur;
        }
    }

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

bool sqlqmDetails::setData(const QModelIndex &index, const QVariant &value, int role )
{
    bool isOk = true;

    if(role == Qt::BackgroundRole){
        maCouleur = value.value<QColor>();
#ifndef QT_NO_DEBUG
        QString msg = "Couleur : "+ QString::number(4).rightJustified(2,'0')
                + "("+QString::number(maCouleur.red()).rightJustified(3,'0')
                + ","+QString::number(maCouleur.green()).rightJustified(3,'0')
                + ","+QString::number(maCouleur.blue()).rightJustified(3,'0')
                + ","+QString::number(maCouleur.alpha()).rightJustified(3,'0')
                +")\n";
        qDebug()<< msg;
#endif
        //emit(dataChanged(index, index));
    }
    else{
        //isOk = QSqlQueryModel::setData(index, value, role );
    }
    return isOk;
}

/// -------------------
void BDelegateCouleurFond::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
    int col = index.column();

    QStyleOptionViewItem maModif(option);


    if(col == COL_VISU ){
        QColor leFond = CalculerCouleur(index);
        painter->fillRect(option.rect, leFond);
        this->setData(index,leFond,Qt::BackgroundRole);
    }

    if(col >= b_min && col < b_max){
        painter->fillRect(option.rect, COULEUR_FOND_DETAILS);
    }

    if(col >= b_max && col < b_max+len){
        painter->fillRect(option.rect, COULEUR_FOND_ECARTS);
    }

    QItemDelegate::paint(painter, maModif, index);
}

bool BDelegateCouleurFond::posSurDisque(int centre, int pos)const
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

void BDelegateCouleurFond::slot_AideToolTip(const QModelIndex & index)
{
    int col = index.column();
    QTableView *view = qobject_cast<QTableView *>(sender());
    QSortFilterProxyModel *m=qobject_cast<QSortFilterProxyModel*>(view->model());
    sqlqmDetails *sqm_tmp= qobject_cast<sqlqmDetails*>(m->sourceModel());
    QVariant item1 = sqm_tmp->data(index,Qt::BackgroundRole);

    QString boule = (index.sibling(index.row(),1)).data().toString();

    QString msg = "";
    if(index.column()==COL_VISU){
        QColor color_brush = sqm_tmp->data(index,Qt::BackgroundRole).value<QColor>();

        int i;
        bool trouve = false;
        for(i=0;i<nb_colors;i++){
            if(val_color[i]!=color_brush){
                continue;
            }
            else{
                trouve = true;
                break;
            }
        }
        if(trouve){
            msg = "Critere ecart : "+ QString::number(i).rightJustified(2,'0')
                    +" sur "
                    + QString::number(nb_colors).rightJustified(2,'0')
                    + " -> "
                    + "("+QString::number(color_brush.red()).rightJustified(3,'0')
                    + ","+QString::number(color_brush.green()).rightJustified(3,'0')
                    + ","+QString::number(color_brush.blue()).rightJustified(3,'0')
                    + ","+QString::number(color_brush.alpha()).rightJustified(3,'0')
                    +")\n";
        }

    }

    msg = QString("Boule ") + boule +QString("\n")+msg;
    QToolTip::showText (QCursor::pos(), msg);

}

BDelegateCouleurFond::BDelegateCouleurFond(st_ColorNeeds param, QWidget *parent)
    :QItemDelegate(parent),b_min(param.b_min),b_max(param.b_max),len(param.len),origine(param.parent)
{
    /// Mise en place d'un toolstips
    parent->setMouseTracking(true);
    connect(parent,
            SIGNAL(entered(QModelIndex)),
            this,SLOT(slot_AideToolTip(QModelIndex)));


    /// creation d'un tableau des couleurs
    nb_colors = pow(2,len-1);
    int mid_color = nb_colors/2;
    int step_colors = 255/mid_color;
    val_color = new QColor[nb_colors];

#ifndef QT_NO_DEBUG
    qDebug() << "Couleur : id (R,V,B,A)\n";
#endif
    for(int i = 1; i<= nb_colors; i++){

        if(i<nb_colors/2){
            val_color[i-1].setRed(i*step_colors);
            val_color[i-1].setGreen(255);
        }
        else{
            val_color[i-1].setRed(255);
            val_color[i-1].setGreen((i%(mid_color))*step_colors);
        }
        val_color[i].setBlue(0);

#ifndef QT_NO_DEBUG
        qDebug() << "Couleur : "<< QString::number(i).rightJustified(2,'0')
                 << "("<<QString::number(val_color[i].red()).rightJustified(3,'0')
                 << ","<<QString::number(val_color[i].green()).rightJustified(3,'0')
                 << ","<<QString::number(val_color[i].blue()).rightJustified(3,'0')
                 << ","<<QString::number(val_color[i].alpha()).rightJustified(3,'0')
                 <<")\n";
#endif

    }
}

QColor BDelegateCouleurFond::CalculerCouleur(const QModelIndex &index) const
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

        int item_couleur = posSurDisque(centre,Ec);
        val = val + pow(2,pri) * item_couleur;

        int item_alpha = posSurDisque(r,centre);
        alp = alp + item_alpha;
    }

    /// ----------------
    if(val == 0){
        color.setRgb(255,255,255,255);
    }
    else{
        if(val<nb_colors){
            color = val_color[val];
        }
        else{
            /// Erreur de calcul !!
            color.setRed(39);
            color.setGreen(93);
            color.setBlue(219);
        }

        // proche du centre ?
        color.setAlpha(255);

    }

    return color;
}

bool BDelegateCouleurFond::setData(const QModelIndex &index, const QVariant &value, int role) const
{
    return(origine->setData(index,value,Qt::BackgroundRole));
}
