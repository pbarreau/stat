#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QGridLayout>
#include <QTableView>
#include <QSqlQuery>
#include <QHeaderView>
#include <QToolTip>
#include <QStackedWidget>

#include "ctabfilterzncount.h"

cTabFilterZnCount::cTabFilterZnCount(QString in, stTiragesDef *def)
{
    db_data = in;
    conf = def;
    QTabWidget *tab_Top = new QTabWidget;

    int nb_zones = def->nb_zone;
    maRef = new  QStringList* [nb_zones] ;

    QTableView *(cTabFilterZnCount::*ptrFunc[])(QString *, int) =
    {
            &cTabFilterZnCount::znCalculRegroupement,
            &cTabFilterZnCount::znCalculRegroupement

};

    for(int i = 0; i< nb_zones; i++)
    {
        QString *name = new QString;
        QTableView *calcul = (this->*ptrFunc[i])(name, i);
        tab_Top->addTab(calcul,tr((*name).toUtf8()));
    }

    QWidget * Resultats = new QWidget;
    QGridLayout *layout = new QGridLayout();
    layout->addWidget(tab_Top);
    Resultats->setLayout(layout);
    Resultats->setWindowTitle("Test");
    Resultats->show();

}

QTableView *cTabFilterZnCount::znCalculRegroupement(QString * pName, int zn)
{
    QTableView *qtv_tmp = new QTableView;
    int nbLgn = conf->nbElmZone[zn] + 1;
    (* pName) = conf->nomZone[zn];

    QStandardItemModel * tmpStdItem = NULL;
    QSqlQuery query ;

    maRef[zn] = CreateFilterForData(zn,conf);
    int nbCol = maRef[zn][0].size();

    //Creer un tableau d'element standard
    if(nbCol)
    {
        tmpStdItem =  new QStandardItemModel(nbLgn,nbCol);
        qtv_tmp->setModel(tmpStdItem);

        QStringList tmp=maRef[zn][1];
        tmp.insert(0,"Nb");
        tmpStdItem->setHorizontalHeaderLabels(tmp);

        QStringList tooltips=maRef[zn][2];
        tooltips.insert(0,"Total");
        for(int pos=0;pos <=nbCol;pos++)
        {
            QStandardItem *item = tmpStdItem->horizontalHeaderItem(pos);
            item->setToolTip(tooltips.at(pos));
        }

        for(int lgn=0;lgn<nbLgn;lgn++)
        {
            for(int pos=0;pos <=nbCol;pos++)
            {
                QStandardItem *item = new QStandardItem();

                if(pos == 0){
                    item->setData(lgn,Qt::DisplayRole);
                }
                tmpStdItem->setItem(lgn,pos,item);
                qtv_tmp->setColumnWidth(pos,LCELL);
            }
        }
        // Gestion du QTableView
        qtv_tmp->setSelectionMode(QAbstractItemView::MultiSelection);
        qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
        qtv_tmp->setStyleSheet("QTableView {selection-background-color: #939BFF;}");

        qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
        qtv_tmp->setAlternatingRowColors(true);
        qtv_tmp->setFixedSize(CLargeur1*1.8,CHauteur1);

        qtv_tmp->setSortingEnabled(true);
        qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
        qtv_tmp->verticalHeader()->hide();

        QHeaderView *htop = qtv_tmp->horizontalHeader();
        htop->setSectionResizeMode(QHeaderView::ResizeToContents);
        qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

        // simple click dans fenetre  pour selectionner boule
        connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
                 this, SLOT(slot_ClicDeSelectionTableau( QModelIndex) ) );


        qtv_tmp->setMouseTracking(true);
        connect(qtv_tmp,
                SIGNAL(entered(QModelIndex)),this,SLOT(slot_AideToolTip(QModelIndex)));

        ///------------------------------
        bool status = true;
        for(int j=0; (j< nbCol) && (status == true);j++)
        {
            // Creer Requete pour compter items
            QString msg1 = maRef[zn][0].at(j);
            QString sqlReq = "";
            sqlReq = ApplayFilters(db_data,msg1,zn,conf);

#ifndef QT_NO_DEBUG
            qDebug() << sqlReq;
#endif

            status = query.exec(sqlReq);

            // Mise a jour de la tables des resultats
            if(status)
            {
                query.first();
                do
                {
                    int nb = query.value(0).toInt();
                    int tot = query.value(1).toInt();

                    QStandardItem * item_1 = tmpStdItem->item(nb,j+1);
                    item_1->setData(tot,Qt::DisplayRole);
                    tmpStdItem->setItem(nb,j+1,item_1);
                }while(query.next() && status);
            }
        }
    }
    return qtv_tmp;
}

// Cette fonction retourne un pointeur sur un tableau de QStringList
// Ce tableau comporte 2 elements
// Element 0 liste des requetes construites
// Element 1 Liste des titres assosies a la requete
// En fonction de la zone a etudier les requetes sont adaptees
// pour integrer le nombre maxi de boules a prendre en compte
QStringList * cTabFilterZnCount::CreateFilterForData(int zn, stTiragesDef *pConf)
{
    QStringList *sl_filter = new QStringList [3];
    QString fields = "z"+QString::number(zn+1);

    int maxElems = pConf->limites[zn].max;
    int nbBoules = floor(maxElems/10)+1;


    // Parite & nb elment dans groupe
    sl_filter[0] <<fields+"%2=0"<<fields+"<"+QString::number(maxElems/2);
    sl_filter[1] << "P" << "G";
    sl_filter[2] << "Pair" << "< E/2";


    // Boule finissant par [0..9]
    for(int j=0;j<=9;j++)
    {
        sl_filter[0]<< fields+" like '%" + QString::number(j) + "'";
        sl_filter[1] << "F"+ QString::number(j);
        sl_filter[2] << "Finissant par: "+ QString::number(j);
    }

    // Nombre de 10zaine
    for(int j=0;j<nbBoules;j++)
    {
        sl_filter[0]<< fields+" >="+QString::number(10*j)+
                       " and "+fields+"<="+QString::number((10*j)+9);
        sl_filter[1] << "U"+ QString::number(j);
        sl_filter[2] << "Entre:"+ QString::number(j*10)+" et "+ QString::number(((j+1)*10)-1);
    }

    return sl_filter;
}
QString cTabFilterZnCount::TrouverTirages(int col, int nb, QString st_tirages, QString st_cri,int zn, stTiragesDef *pConf)
{
    QString st_tmp =  ActionElmZone("=","or",zn,pConf);
    QString st_return =
            "select tb1.*, count(tb2.B) as N"+QString::number(col)+ " "+
            "from (" + st_tirages.remove(";")+
            ") as tb1 "
            "left join "
            "("
            "select id as B from Bnrz where (z"+QString::number(zn+1)+
            " not null  and ("+st_cri+")) ) as tb2 " +
            "on "+
            "("
            +st_tmp+
            ") group by tb1.id";

#ifndef QT_NO_DEBUG
    qDebug() << st_return;
#endif
    st_return ="select * from("+
            st_return+
            ")as tb1 where(tb1.N"+QString::number(col)+ "="+
            QString::number(nb)+");";

#ifndef QT_NO_DEBUG
    qDebug() << st_return;
#endif

    return(st_return);
}

QString cTabFilterZnCount::ApplayFilters(QString st_tirages, QString st_cri,int zn, stTiragesDef *pConf)
{
#if 0
    --- Requete recherche parite sur base pour tirages
            -- requete de groupement des paritees
            select Nb, count(Nb) as Tp from
            (
                -- Req_1 : pour compter le nombre de boules pair par tirages
                select tb1.id as Tid, count(tb2.B) as Nb from
                (
                    select * from tirages
                    ) as tb1
                left join
                (
                    select id as B from Bnrz where (z1 not null  and (z1%2 = 0))
                    ) as tb2
                on
                (
                    tb2.B = tb1.b1 or
            tb2.B = tb1.b2 or
            tb2.B = tb1.b3 or
            tb2.B = tb1.b4 or
            tb2.B = tb1.b5
            ) group by tb1.id order by Nb desc
                -- fin req_1
                )
            group by Nb;
    ////---data, range, name, filter
#endif

    QString st_tmp =  ActionElmZone("=","or",zn,pConf);
    QString st_return =
            "select Nb, count(Nb) as Tp from "
            "("
            "select tb1.id as Tid, count(tb2.B) as Nb from "
            "("
            "select * from (" + st_tirages.remove(";")+") as r1 "
                                                       ") as tb1 "+
            "left join "
            "("
            "select id as B from Bnrz where (z"+QString::number(zn+1)+
            " not null  and ("+st_cri+")) ) as tb2 " +
            "on "+
            "("
            +st_tmp+
            ") group by tb1.id order by Nb desc "
            ")"
            "group by Nb;";

#ifndef QT_NO_DEBUG
    qDebug() << st_return;
#endif

    return(st_return);
}

QString cTabFilterZnCount::ActionElmZone(QString critere , QString operateur, int zone, stTiragesDef *pConf)
{
    QString ret_msg = "";

    // Operateur : or | and
    // critere : = | <>
    for(int i = 0; i<pConf->nbElmZone[zone];i++)
    {
        ret_msg = ret_msg +"tb2.B "+ critere +" tb1."
                + pConf->nomZone[zone]+QString::number(i+1)
                + " " + operateur+ " ";
    }
    int len_flag = operateur.length();
    ret_msg.remove(ret_msg.length()-len_flag-1, len_flag+1);

#ifndef QT_NO_DEBUG
    qDebug() << ret_msg;
#endif

    return ret_msg;
}

void cTabFilterZnCount::slot_AideToolTip(const QModelIndex & index)
{
    QString msg="";
    const QAbstractItemModel * pModel = index.model();
    int col = index.column();

    //QTableView *view = qobject_cast<QTableView *>(sender());

    QVariant vCol = pModel->headerData(col,Qt::Horizontal);
    QString headName = vCol.toString();

    if (col >=1)
    {
        //int val = index.model()->index(index.row(),col).data().toInt();
        QString s_nb = index.model()->index(index.row(),0).data().toString();
        QString s_va = index.model()->index(index.row(),col).data().toString();
        QString s_hd = headName;
        msg = msg + QString("%1 tirage(s) \nayant %2 boule(s)%3").arg(s_va).arg(s_nb).arg(s_hd);
    }
    if(msg.length())
        QToolTip::showText (QCursor::pos(), msg);
}

void cTabFilterZnCount::slot_ClicDeSelectionTableau(const QModelIndex &index)
{
    // L'onglet implique le tableau...
    int tab_index = 0;
    QString st_critere = "";
    QTableView *view = qobject_cast<QTableView *>(sender());
    QStackedWidget *curOnglet = qobject_cast<QStackedWidget *>(view->parent());
    QItemSelectionModel *selectionModel = view->selectionModel();
    tab_index = curOnglet->currentIndex();
    QModelIndexList indexes = selectionModel->selectedIndexes();

    /// il y a t'il une selection
    if(indexes.size())
    {
        QModelIndex un_index;
        QString st_logical = " and ";
        QString sqlReq ="";
        int curCol = 0;
        int occure = 0;

        /// Parcourir les selections
        sqlReq = db_data;
        foreach(un_index, indexes)
        {
            curCol = un_index.model()->index(un_index.row(), un_index.column()).column();
            occure = un_index.model()->index(un_index.row(), 0).data().toInt();
            if(curCol)
            {
                st_critere = "("+maRef[tab_index][0].at(curCol-1)+")";
                sqlReq =TrouverTirages(curCol,occure,sqlReq,st_critere,tab_index,conf);
                //st_critere = st_critere + st_logical;
            }
        }
        //st_critere.remove(st_critere.length()-st_logical.length(),st_logical.length());

        // Creation requete
        //sqlReq = ApplayFilters(db_data,st_critere,tab_index,conf);
#ifndef QT_NO_DEBUG
        qDebug() << sqlReq;
#endif

    }
}
