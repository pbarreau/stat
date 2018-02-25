#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QVBoxLayout>
#include <QWidget>
#include <QHeaderView>
#include <QSqlQueryModel>
#include <QPainter>

#include "montrer_tirages.h"

int BTirages::total = 0;

void BdgtTirages::paint(QPainter *painter, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const
{
    //QStyleOptionViewItem maModif(option);
    int val = 0;

    QColor u[]= {QColor(201,230,255,255), QColor(255,195,155,255), QColor(170,255,255,255),
                 QColor(224,255,147,255),QColor(231,181,255,255),QColor(73,179,255,255),
                 Qt::red,Qt::white};

    /// Mettre une couleur en fonction du groupe u,dizaine,v,...
    if(index.column()>3 && index.column()<8)
    {
        val =  index.data().toInt();
        val = (int)floor(val/10);

        if(val < 0 || val >6){
            val = (sizeof(u)/sizeof(QColor))-2;
        }
        else{
            val = (sizeof(u)/sizeof(QColor))-1;
        }

        painter->fillRect(option.rect, u[val]);
    }

    QItemDelegate::paint(painter, option, index);
}

BsqmTirages::BsqmTirages(const BGame &pDef,QObject *parent)
    :QSqlQueryModel(parent),leJeu(pDef)
{
    debzone=3;
}

QVariant BsqmTirages::data(const QModelIndex &index, int role)const
{
    if(index.column()>debzone  )
    {
        int val = QSqlQueryModel::data(index,role).toInt();

        if(role == Qt::DisplayRole)
        {
            if(val <=9)
            {
                QString sval = "0"+QString::number(val);

                return sval;
            }
        }

    }

    if(index.column()>(debzone+leJeu.limites[0].len))
    {
        if((role == Qt::TextColorRole))
        {
            return QColor(Qt::red);
        }
    }

    /// Par defaut retourner contenu initial
    return QSqlQueryModel::data(index,role);
}

BTirages::~BTirages()
{
    total --;
}

BTirages::BTirages(const QString &in,  const BGame &pDef, QSqlDatabase fromDb, QWidget *parent)
    :sqlSource(in),ceJeu(pDef),dbDesTirages(fromDb)

{
    QVBoxLayout *conteneur = new QVBoxLayout;
    QTableView * item = Visuel_1(in,pDef);
    conteneur->addWidget(item);
    this->addLayout(conteneur);
}

QTableView *BTirages::Visuel_1(const QString &source,const BGame &config)
{
    QTableView *qtv_tmp = new QTableView;

    QString qtv_name = "LesTirages";
    qtv_tmp->setObjectName(qtv_name);

    BsqmTirages *sqm_tmp = new BsqmTirages(config);

    QString st_msg1 =
            "select t1.J as J, t1.D as D,"
            "t2.tip as C1, t3.tip as C2,"
            "t1.b1, t1.b2,t1.b3,t1.b4,t1.b5,"
            "t1.e1 from B_fdj as t1, B_cmb_z1 as t2,"
            "B_cmb_z2 as t3, B_ana_z1 as t4, B_ana_z2 as t5 "
            "where"
            "("
            "t4.id=t1.id and t2.id=t4.idcomb "
            "and "
            "t5.id=t1.id and t3.id=t5.idcomb"
            ")";
#ifndef QT_NO_DEBUG
    qDebug() <<st_msg1;
#endif

    sqm_tmp->setQuery(st_msg1,dbDesTirages);
    qtv_tmp->setModel(sqm_tmp);
    qtv_tmp->setItemDelegate(new BdgtTirages);
    qtv_tmp->setSortingEnabled(false);
    qtv_tmp->setAlternatingRowColors(true);
    qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

    int taille_L = 0;
    // Formattage de largeur de colonnes
    for(int j=0;j<=3;j++){
        qtv_tmp->setColumnWidth(j,75);
        taille_L +=75;
    }

    for(int j=4;j<=sqm_tmp->columnCount();j++){
        qtv_tmp->setColumnWidth(j,30);
        taille_L +=30;
    }
    taille_L+=30;

    // Bloquer largeur des colonnes
    qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    // Taille tableau
    qtv_tmp->setFixedWidth(taille_L);
    //qtv_tmp->setFixedSize(520,400);

    return qtv_tmp;
}
