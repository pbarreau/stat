#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QGridLayout>
#include <QTableView>
#include <QSqlQuery>
#include <QHeaderView>

#include "ctabfilterzncount.h"

cTabFilterZnCount::cTabFilterZnCount(QString in, stTiragesDef *def)
{
    db_data = in;
    conf = def;
    tab_Top = new QTabWidget;

    int nb_zones = def->nb_zone;

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

QTableView *cTabFilterZnCount::znCalculRegroupement(QString * pName, int i)
{
    QTableView *qtv_tmp = new QTableView;
    int nbLgn = conf->nbElmZone[i] + 1;
    (* pName) = conf->nomZone[i];

    QStandardItemModel * tmpStdItem = NULL;
    QSqlQuery query ;

    QStringList *maRef = CreateFilterForData(i,conf);
    int nbCol = maRef[0].size();

    //Creer un tableau d'element standard
    if(nbCol)
    {
        tmpStdItem =  new QStandardItemModel(nbLgn,nbCol);
        qtv_tmp->setModel(tmpStdItem);

        QStringList tmp=maRef[1];
        tmp.insert(0,"Nb");
        tmpStdItem->setHorizontalHeaderLabels(tmp);
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
        qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
        qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
        qtv_tmp->setStyleSheet("QTableView {selection-background-color: #939BFF;}");

        qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
        qtv_tmp->setAlternatingRowColors(true);
        qtv_tmp->setFixedSize(CLargeur1*1.8,CHauteur1);

        qtv_tmp->setSortingEnabled(true);
        qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
        qtv_tmp->verticalHeader()->hide();

        QHeaderView *htop = qtv_tmp->horizontalHeader();
        htop->setSectionResizeMode(QHeaderView::Fixed);
        qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    }

    bool status = true;
    for(int j=0; (j< nbCol) && (status == true);j++)
    {
        // Creer Requete pour compter items
        QString msg1 = maRef[0].at(j);
        QString sqlReq = "";
        sqlReq = ApplayFilters(db_data,msg1,i,conf);

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
    QStringList *tmp = new QStringList [2];
    QString fields = "z"+QString::number(zn+1);

    int maxElems = pConf->limites[zn].max;
    int nbBoules = floor(maxElems/10)+1;


    // Parite & nb elment dans groupe
    tmp[0] <<fields+"%2=0"<<fields+"<"+QString::number(maxElems/2);
    tmp[1] << "P" << "G";


    // Boule finissant par [0..9]
    for(int j=0;j<=9;j++)
    {
        tmp[0]<< fields+" like '%" + QString::number(j) + "'";
        tmp[1] << "F"+ QString::number(j);
    }

    // Nombre de 10zaine
    for(int j=0;j<nbBoules;j++)
    {
        tmp[0]<< fields+" >="+QString::number(10*j)+
                 " and "+fields+"<="+QString::number((10*j)+9);
        tmp[1] << "U"+ QString::number(j);
    }

    return tmp;
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
            "select * from (" + st_tirages.remove(";")
            +") as r1 "
             ") as tb1 "
             "left join "
             "("
             "select id as B from Bnrz where (z1 not null  and ("+st_cri
            +")) "
             ") as tb2 "
             "on "
             "("
            +st_tmp+
            ") group by tb1.id order by Nb desc "
            ")"
            "group by Nb;";

    return(st_return);
}

QString cTabFilterZnCount::ActionElmZone(QString critere , QString operateur, int zone, stTiragesDef *pConf)
{
    QString ret_msg = "";

    // Operateur : or | and
    // critere : = | <>
    // b1=0 or b2=0 or ..
    for(int i = 0; i<pConf->nbElmZone[zone];i++)
    {
        ret_msg = ret_msg +"tb2.B "+ critere +" tb1."
                + pConf->nomZone[zone]+QString::number(i+1)
                + " " + operateur+ " ";
    }
    int len_flag = operateur.length();
    ret_msg.remove(ret_msg.length()-len_flag-1, len_flag+1);

    return ret_msg;
}
