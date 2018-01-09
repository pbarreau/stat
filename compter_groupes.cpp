#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QGridLayout>
#include <QTableView>
#include <QSqlQuery>
#include <QHeaderView>
#include <QToolTip>
#include <QStackedWidget>

#include "compter_groupes.h"

int cCompterGroupes::total = 0;

cCompterGroupes::~cCompterGroupes()
{
    total --;
}

cCompterGroupes::cCompterGroupes(QString in):B_Comptage(&in)
{
    total++;
    QTabWidget *tab_Top = new QTabWidget;

    int nb_zones = nbZone;
    maRef = new  QStringList* [nb_zones] ;
    lesSelections = new QModelIndexList [nb_zones];

    QTableView *(cCompterGroupes::*ptrFunc[])(QString *, int) =
    {
            &cCompterGroupes::Compter,
            &cCompterGroupes::Compter

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

QTableView *cCompterGroupes::Compter(QString * pName, int zn)
{
    QTableView *qtv_tmp = new QTableView;
    int nbLgn = limites[zn].len + 1;
    (* pName) = names[zn].court;

    QStandardItemModel * sqm_tmp = NULL;
    QSqlQuery query ;

    maRef[zn] = CreateFilterForData(zn);
    int nbCol = maRef[zn][0].size();

    //Creer un tableau d'element standard
    if(nbCol)
    {
        sqm_tmp =  new QStandardItemModel(nbLgn,nbCol);
        qtv_tmp->setModel(sqm_tmp);

        QStringList tmp=maRef[zn][1];
        tmp.insert(0,"Nb");
        sqm_tmp->setHorizontalHeaderLabels(tmp);

        QStringList tooltips=maRef[zn][2];
        tooltips.insert(0,"Total");
        for(int pos=0;pos <=nbCol;pos++)
        {
            QStandardItem *item = sqm_tmp->horizontalHeaderItem(pos);
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
                sqm_tmp->setItem(lgn,pos,item);
                //qtv_tmp->setColumnWidth(pos,LCELL);
            }
        }
        // Gestion du QTableView
        qtv_tmp->setSelectionMode(QAbstractItemView::MultiSelection);
        qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
        qtv_tmp->setStyleSheet("QTableView {selection-background-color: #939BFF;}");

        qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);
        qtv_tmp->setAlternatingRowColors(true);

        qtv_tmp->setSortingEnabled(true);
        qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
        qtv_tmp->verticalHeader()->hide();

        //largeur des colonnes
        qtv_tmp->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        qtv_tmp->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

        int b = qtv_tmp->columnWidth(0);
        int n = sqm_tmp->columnCount();
        qtv_tmp->setFixedWidth((b*n)+5);

        //n = (qtv_tmp->horizontalHeader()->height()) * (nbLgn+1.2);
        b = qtv_tmp->rowHeight(0);
        n =nbLgn+1;
        qtv_tmp->setFixedHeight((b*n)+5);
        //qtv_tmp->setFixedSize(CLargeur1*1.8,CHauteur1);

        // simple click dans fenetre  pour selectionner boules
        connect( qtv_tmp, SIGNAL(clicked(QModelIndex)) ,
                 this, SLOT(slot_ClicDeSelectionTableau( QModelIndex) ) );

        // Double click dans fenetre  pour creer requete
        connect( qtv_tmp, SIGNAL(doubleClicked(QModelIndex)) ,
                 this, SLOT(slot_RequeteFromSelection( QModelIndex) ) );

        qtv_tmp->setMouseTracking(true);
        connect(qtv_tmp,
                SIGNAL(entered(QModelIndex)),this,SLOT(slot_AideToolTip(QModelIndex)));

        ///------------------------------
        RecalculGroupement(zn,nbCol,sqm_tmp);
    }
    return qtv_tmp;
}

void cCompterGroupes::RecalculGroupement(int zn,int nbCol,QStandardItemModel *sqm_tmp)
{
    bool status = true;
    QSqlQuery query ;

    for(int j=0; (j< nbCol) && (status == true);j++)
    {
        //Effacer calcul precedent
        for(int k=0; k<limites[zn].len+1;k++)
        {
            QStandardItem * item_1 = sqm_tmp->item(k,j+1);
            item_1->setData("",Qt::DisplayRole);
            sqm_tmp->setItem(k,j+1,item_1);
        }

        // Creer Requete pour compter items
        QString msg1 = maRef[zn][0].at(j);
        QString sqlReq = "";
        sqlReq = CriteresAppliquer(db_data,msg1,zn);

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

                QStandardItem * item_1 = sqm_tmp->item(nb,j+1);
                item_1->setData(tot,Qt::DisplayRole);
                sqm_tmp->setItem(nb,j+1,item_1);
            }while(query.next() && status);
        }
    }

}

#if 0
///------------------
void SyntheseDetails::RecalculGroupement(QString st_tirages,int nbCol,QStandardItemModel *tmpStdItem)
{
    QSqlQuery query ;
    //int nbCol = tab->horizontalHeader()->count();
    bool status = true;
    int zn = 0;
    for(int j=0; (j< nbCol-1) && (status == true);j++)
    {
        //Effacer calcul precedent
        for(int k =0;k<(pLaDemande->ref->nbElmZone[zn])+1;k++)
        {
            QStandardItem * item_1 = tmpStdItem->item(k,j+1);
            item_1->setData("",Qt::DisplayRole);
            tmpStdItem->setItem(k,j+1,item_1);
        }

        // Creer Requete pour compter items
        QString msg1 = maRef[zn][0].at(j);
        QString sqlReq = "";
        //QStandardItemModel * tmpStdItem = qobject_cast<QStandardItemModel *>(tab->model());
        sqlReq = sql_RegroupeSelonCritere(st_tirages,msg1);

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
#endif
/// --------------------
#if 0
QTableView *cCompterGroupes::Compter(QString * pName, int zn,int id)
{

    QStackedWidget *curOnglet = qobject_cast<QStackedWidget *>(view->parent());
    QItemSelectionModel *selectionModel = view->selectionModel();
    tab_index = curOnglet->currentIndex();


}

void cCompterGroupes::slot_ShowDetails(const QModelIndex & index)
{
    QTableView *view = qobject_cast<QTableView *>(sender());
    static int sortir = 0;

    int zn = 0; // A remplacer par detection du tableview ayant recut le click

    // recuperer la ligne de la table
    int lgn = index.model()->index(index.row(),0).data().toInt();

    if(sortir != lgn)
    {
        sortir = lgn;
    }
    else
    {
        return;
    }

    QSqlQuery query;
    QStandardItemModel *tmpStdItem = p_qsim_3;

    int nbCol = codeSqlDeRegroupementSurZnId[zn][0].size();
    bool status = true;
    for(int i=0; (i< nbCol) && (status == true);i++)
    {
        // Creer Requete pour compter items
        QString msg1 = codeSqlDeRegroupementSurZnId[zn][0].at(i);
        QString sqlReq = "";
        sqlReq = sql_ComptePourUnTirage(lgn,p_stRefTirages,msg1);

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
                //int id = query.value(0).toInt();
                int tot = query.value(1).toInt();

                QStandardItem * item_1 = tmpStdItem->item(0,i);
                item_1->setData(tot,Qt::DisplayRole);
                tmpStdItem->setItem(0,i,item_1);
            }while(query.next() && status);
        }
    }

}
#endif
// Cette fonction retourne un pointeur sur un tableau de QStringList
// Ce tableau comporte 2 elements
// Element 0 liste des requetes construites
// Element 1 Liste des titres assosies a la requete
// En fonction de la zone a etudier les requetes sont adaptees
// pour integrer le nombre maxi de boules a prendre en compte
QStringList * cCompterGroupes::CreateFilterForData(int zn)
{
    QStringList *sl_filter = new QStringList [3];
    QString fields = "z"+QString::number(zn+1);

    //int maxElems = pConf->limites[zn].max;
    int maxElems = limites[zn].max;
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
QString cCompterGroupes::TrouverTirages(int col, int nb, QString st_tirages, QString st_cri, int zn)
{
    QString st_tmp =  CriteresCreer("=","or",zn);
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

QString cCompterGroupes::CriteresAppliquer(QString st_tirages, QString st_cri, int zn)
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

    QString st_tmp =  CriteresCreer("=","or",zn);
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

void cCompterGroupes::slot_ClicDeSelectionTableau(const QModelIndex &index)
{
    // L'onglet implique le tableau...
    int tab_index = 0;
    QTableView *view = qobject_cast<QTableView *>(sender());
    QStackedWidget *curOnglet = qobject_cast<QStackedWidget *>(view->parent());
    QItemSelectionModel *selectionModel = view->selectionModel();
    tab_index = curOnglet->currentIndex();
    lesSelections[tab_index]= selectionModel->selectedIndexes();
}

void cCompterGroupes::slot_RequeteFromSelection(const QModelIndex &index)
{
    QString st_titre = "";
    QVariant vCol;
    QString headName;
    const QAbstractItemModel * pModel = index.model();

    QString st_critere = "";
    QString sqlReq ="";
    QTableView *view = qobject_cast<QTableView *>(sender());
    QStackedWidget *curOnglet = qobject_cast<QStackedWidget *>(view->parent());

    /// il y a t'il une selection
    ///parcourir tous les onglets
    sqlReq = db_data;
    int nb_item = curOnglet->count();
    for(int onglet = 0; onglet<nb_item;onglet++)
    {
        QModelIndexList indexes =  lesSelections[onglet];

        if(indexes.size())
        {
            QModelIndex un_index;
            int curCol = 0;
            int occure = 0;

            /// Parcourir les selections
            foreach(un_index, indexes)
            {
                curCol = un_index.model()->index(un_index.row(), un_index.column()).column();
                occure = un_index.model()->index(un_index.row(), 0).data().toInt();
                if(curCol)
                {
                    vCol = pModel->headerData(curCol,Qt::Horizontal);
                    headName = vCol.toString();
                    st_titre = st_titre + "("+headName+"," + QString::number(occure) + "),";

                    st_critere = "("+maRef[onglet][0].at(curCol-1)+")";
                    sqlReq =TrouverTirages(curCol,occure,sqlReq,st_critere,onglet);
                }
            }
            st_titre.remove(st_titre.length()-1,1);

            // signaler que cet objet a construit la requete
            a.db_data = sqlReq;
            a.tb_data = "g"+QString::number(total)+":"+st_titre;
            emit sig_ComptageReady(a);

#ifndef QT_NO_DEBUG
            qDebug() << sqlReq;
#endif

        }

    }

}
