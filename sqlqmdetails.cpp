#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QColor>
#include <QPainter>
#include <QToolTip>
#include <QSortFilterProxyModel>
#include <QStandardItem>

#include <QMessageBox>

#include "sqlqmdetails.h"
#include "delegate.h"

QColor *BDelegateCouleurFond::val_colors = NULL;
QMap<BOrdColor,int> BDelegateCouleurFond::map_FromColor;

sqlqmDetails::sqlqmDetails(st_sqlmqDetailsNeeds param,QObject *parent):QSqlQueryModel(parent)
{
    db_0 = QSqlDatabase::database(param.cnx);
    this->setQuery(param.sql,db_0);
    PreparerTableau();

    QSortFilterProxyModel *m=new QSortFilterProxyModel();
    m->setDynamicSortFilter(true);
    m->setSourceModel(this);
    param.view->setModel(m);


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



/// -------------------
void BDelegateCouleurFond::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const
{
    int col = index.column();

    ///QStyleOptionViewItem maModif(option);


    if(col == 1 ){
        QColor leFond = resu_color[index.row()];
        painter->fillRect(option.rect, leFond);
    }

    if(col >= b_min && col < b_max){
        painter->fillRect(option.rect, COULEUR_FOND_DETAILS);
    }

    if(col >= b_max && col < b_max+len){
        painter->fillRect(option.rect, COULEUR_FOND_ECARTS);
    }

    QItemDelegate::paint(painter, option, index);
}

bool BDelegateCouleurFond::isOnDisk(int centre, int pos)const
{
    /// On regarde si sur disque de rayon centre
    bool ret_val = false;
    int distance = abs(centre-pos);

    if(distance <= 12){

        if(distance<= abs(centre)){
            // oui
            ret_val = true;
        }
        else{
            // non
            ret_val = false;
        }
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
    QString msg2 = "";
    if(index.column()==COL_VISU){
        QColor a = resu_color[index.row()];
        BOrdColor b(a);
        //QMap <BOrdColor,int> color_brush;
        int ligne = map_FromColor.value(b,-1);

        if(map_FromColor.contains(b)){
            msg = "Critere ecart : "+ QString::number(ligne).rightJustified(2,'0')
                    +" sur "
                    + QString::number(nb_colors).rightJustified(2,'0')
                    + " -> "
                    + "("+QString::number(a.red()).rightJustified(3,'0')
                    + ","+QString::number(a.green()).rightJustified(3,'0')
                    + ","+QString::number(a.blue()).rightJustified(3,'0')
                    + ","+QString::number(a.alpha()).rightJustified(3,'0')
                    +")\n";

            msg2 = "("+QString::number(b.red()).rightJustified(3,'0')
                    + ","+QString::number(b.green()).rightJustified(3,'0')
                    + ","+QString::number(b.blue()).rightJustified(3,'0')
                    + ","+QString::number(b.alpha()).rightJustified(3,'0')
                    +")\n";
        }
    }

    msg = QString("Boule ") + boule +QString("\n")+msg;

#ifndef QT_NO_DEBUG
    qDebug() << "Tooltips :" << msg;
    qDebug() << "msg2 :" << msg2;
#endif

    QToolTip::showText (QCursor::pos(), msg);

}

BDelegateCouleurFond::BDelegateCouleurFond(st_ColorNeeds param, QTableView *parent)
    :QItemDelegate(parent),b_min(param.b_min),b_max(param.b_max),len(param.len),origine(param.parent)
{
    /// Mise en place d'un toolstips
    parent->setMouseTracking(true);
    connect(parent,
            SIGNAL(entered(QModelIndex)),
            this,SLOT(slot_AideToolTip(QModelIndex)));


    CreationTableauClefDeCouleurs();

    AffectationCouleurResultat(parent);


}

void BDelegateCouleurFond::AffectationCouleurResultat(QTableView *tbv_cible)
{
    sqlqmDetails *sqm_tmp= qobject_cast<sqlqmDetails*>((this->origine));

    int nb_row = sqm_tmp->rowCount();
    resu_color = new QColor[nb_row];
    for(int row=0; row < nb_row;row++){
        QModelIndex Ec = sqm_tmp->index(row,0);
        QColor leFond = CalculerCouleur(Ec);
        resu_color[row]=leFond;
    }

}
void BDelegateCouleurFond::CreationTableauClefDeCouleurs(void)
{
    /// creation d'un tableau des couleurs
    /// pour chacun des calculs d'ecart
    ///  + la couche alpha
    nb_colors = pow(2,len-1)*2;
    int mid_color = nb_colors/2;
    int step_colors = 255/mid_color;
    int alpha_start = 90;
    int step_alpha = (255-alpha_start)/mid_color;
    val_colors = new QColor[nb_colors];


#ifndef QT_NO_DEBUG
    qDebug() << "Couleur : id (R,V,B,A)\n";
#endif
    for(int i = 1; i<= nb_colors; i++){
        QColor tmp_color;


        if(i<=nb_colors/2){
            tmp_color.setGreen(i*step_colors);
            tmp_color.setRed(255);
        }
        else{
            tmp_color.setGreen(255);
            tmp_color.setRed((i%(mid_color))*step_colors);
        }
        tmp_color.setBlue(0);
        tmp_color.setAlpha(alpha_start+((i-1)*step_alpha));

#ifndef QT_NO_DEBUG
        if(i == (nb_colors/2)+1){
            qDebug() << "\n\n---------\n\n";
        }

        qDebug() << "tmp_color : "<< QString::number(i-1).rightJustified(2,'0')
                 << "("<<QString::number(tmp_color.red()).rightJustified(3,'0')
                 << ","<<QString::number(tmp_color.green()).rightJustified(3,'0')
                 << ","<<QString::number(tmp_color.blue()).rightJustified(3,'0')
                 << ","<<QString::number(tmp_color.alpha()).rightJustified(3,'0')
                 <<")\n";
#endif
        val_colors[i-1]=tmp_color;
        BOrdColor a(tmp_color);

        /// Rechercher si cette couleur existe
        /// deja comme clef
        if(map_FromColor.contains(a)){
#ifndef QT_NO_DEBUG
            qDebug() << "Cette couleur comme clef existe deja\n\n";
#endif
            ///QMessageBox::critical(NULL, "Pgm", "Clef Couleur deja presente\n",QMessageBox::Ok);
        }
        else{
            map_FromColor.insert(a,i-1);
        }
    }

#ifndef QT_NO_DEBUG
    QMapIterator<BOrdColor, int>  val (map_FromColor);
    while (val.hasNext()) {
        val.next();
        qDebug() << val.key() << ": " << val.value() << endl;
    }
    ;
#endif


}

QColor BDelegateCouleurFond::CalculerCouleur(const QModelIndex &index) const
{
    QColor color;

    int val = 0;
    int r = 2;

    /// Ep,Em,M,Es,Me
    int tab_pri[5]={3,4,1,2,5};
    int nb_pri = sizeof (tab_pri)/sizeof (int);

    int Ec = (index.sibling(index.row(),b_max)).data().toInt();
    for(int i=1;i<=nb_pri;i++){
        int centre = (index.sibling(index.row(),b_max+i)).data().toInt();
        int pri = tab_pri[i-1]-1;

        int item_couleur = isOnDisk(centre,Ec);
        int item_alpha = isOnDisk(r,(centre-Ec));

        if(i==2){
            /// Retirer cas R=Max Ecart
            /// sauf si vraiment tout proche
            item_couleur = item_alpha * item_couleur;
        }

        val = val + pow(2,pri) * (item_couleur*pow(2,item_alpha));

    }

    /// ----------------
    if(val == 0){
        color.setRgb(255,255,255,255);
    }
    else{
        if(val<nb_colors){
            color = val_colors[val];
        }
        else{
            /// Erreur de calcul !!
            color.setRed(39);
            color.setGreen(93);
            color.setBlue(219);
            color.setAlpha(255);
        }


    }

    return color;
}

bool BDelegateCouleurFond::setData(const QModelIndex &index, const QVariant &value, int role) const
{

    return(origine->setData(index,value,Qt::BackgroundRole));
}

bool BOrdColor::operator<(const BOrdColor  &b)const
{
    bool isOk = (this->red()<b.red()) ||
            (this->red()==b.red() && this->green()<b.green())||
            (this->green()==b.green() && this->blue()<b.blue())||
            (this->blue()==b.blue() && this->alpha() < b.alpha());;

    return isOk;

#ifndef QT_NO_DEBUG
    QString msg_1 =  "("+QString::number(this->red()).rightJustified(3,'0')
            + ","+QString::number(this->green()).rightJustified(3,'0')
            + ","+QString::number(this->blue()).rightJustified(3,'0')
            + ","+QString::number(this->alpha()).rightJustified(3,'0')
            +")";
    QString msg_2 = "("+QString::number(b.red()).rightJustified(3,'0')
            + ","+QString::number(b.green()).rightJustified(3,'0')
            + ","+QString::number(b.blue()).rightJustified(3,'0')
            + ","+QString::number(b.alpha()).rightJustified(3,'0')
            +")";

    QString msg = msg_1 + QString(" < ") + msg_2 + QString(" ? -> ")
            +QString::number(isOk);
    qDebug()<< msg;
#endif

}
