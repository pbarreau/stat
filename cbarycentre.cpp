#ifndef QT_NO_DEBUG
#include <QDebug>
#include <QSqlError>
#endif

#include <QMessageBox>
#include <QApplication>

#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>

#include <QString>


#include <QTabWidget>
#include <QGridLayout>
#include <QTableView>
#include <QHeaderView>

#include "db_tools.h"
#include "cbarycentre.h"

CBaryCentre::CBaryCentre(const stNeedsOfBary &param)
{
    QTabWidget *tab_Top = new QTabWidget(this);

    db= QSqlDatabase::database(param.ncx);
    //src_tbl = param.tbl_in;
    QString src_data = param.tbl_in;

    hc_RechercheBarycentre(param.tbl_in);

    QGridLayout *(CBaryCentre::*ptrFunc[])(QString) ={
            &CBaryCentre::AssocierTableau,
            &CBaryCentre::AssocierTableau,
            &CBaryCentre::AssocierTableau
};
    int calc = sizeof ((*ptrFunc))/sizeof(QGridLayout *);

    int nb_zones = 1;
    for(int i = 0; i< nb_zones; i++)
    {
        QWidget *tmpw = new QWidget;
        QGridLayout *calcul = (this->*ptrFunc[i])(src_data);
        tmpw->setLayout(calcul);
        tab_Top->addTab(tmpw,tr("b"));
    }

}

QGridLayout *CBaryCentre::AssocierTableau(QString src_tbl)
{
    QGridLayout *lay_return = new QGridLayout;
    QTableView *qtv_tmp = new QTableView;
    QSqlQueryModel *sqm_tmp = new QSqlQueryModel;
    QString src_data="";

    qtv_tmp->setObjectName("MonBarycentre");


    if(src_tbl == "E1"){
        src_data = "select * from E1_brc_z1;";
    }
    else{
        src_data = "select * from B_fdj_brc_z1;";
    }
    sqm_tmp->setQuery(src_data,db);

    qtv_tmp->setAlternatingRowColors(true);
    qtv_tmp->setSelectionMode(QAbstractItemView::ExtendedSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QSortFilterProxyModel *m=new QSortFilterProxyModel();
    m->setDynamicSortFilter(true);
    m->setSourceModel(sqm_tmp);


    qtv_tmp->setModel(m);
    ///qtv_tmp->setItemDelegate(new BDelegateElmOrCmb); /// Delegation

    qtv_tmp->verticalHeader()->hide();
    qtv_tmp->setSortingEnabled(true);
    qtv_tmp->sortByColumn(1,Qt::DescendingOrder);

    int nbCol = sqm_tmp->columnCount();
    for(int pos=0;pos<nbCol;pos++)
    {
        qtv_tmp->setColumnWidth(pos,30);
    }

    lay_return->addWidget(qtv_tmp,0,0,Qt::AlignLeft|Qt::AlignTop);

    return lay_return;

}
void CBaryCentre::hc_RechercheBarycentre(QString tbl_in)
{
    QSqlQuery query(db);
    bool isOk = true;

    /// prendre dans les tirages les boules de la zone
    QString str_data = "select id,b1,b2,b3,b4,b5 from ("
            +tbl_in+")";

    if((isOk = query.exec(str_data))){
        //Verifier si on a des donnees
        query.first();
        if((isOk=query.isValid())){
            /// Poursuivre les calculs
            /// 1 : Transformation de lignes vers 1 colonne
            QString tbl_refBoules = QStringLiteral("B_elm");
            str_data = "select c1.id as Id, r1.z1 as b  FROM ("
                    +tbl_refBoules
                    +") as r1, ("
                    +str_data
                    +" ) as c1 where (b=b1 or b=b2 or b=b3 or b=b4 or b=b5) order by c1.id ";
#ifndef QT_NO_DEBUG
            qDebug() << "str_data:"<<str_data;
#endif
            /// 2: Calcul du barycentre si calcul toltal de chaque boule
            ///  de la base complete
            QString tbl_totalBoule = "r_B_fdj_0_elm_z1";
            if(isTableTotalBoulleReady(tbl_totalBoule)){
                str_data = "Select c1.id as Id, sum(c2.t)/5 as BC From ("
                        +str_data
                        +") as c1, ("
                        +tbl_totalBoule
                        +") as c2 WHERE (c1.b = c2.b) GROUP by c1.id";

#ifndef QT_NO_DEBUG
                qDebug() << "str_data:"<<str_data;
#endif
                /// 3: Creation d'une table regroupant les barycentres
                QString str_tblData = "";
                QString str_tblName = tbl_in+"_brc_z1";
                str_tblData = "select BC, count(BC) as T from ("
                        + str_data
                        + ") as c1 group by BC order by T desc";
                str_tblData = "create table if not exists "
                        +str_tblName
                        +" as "
                        +str_tblData;
#ifndef QT_NO_DEBUG
                qDebug() << "str_tblData:"<<str_tblData;
#endif
                if((isOk = query.exec(str_tblData))){
                    /// mettre dans la table analyse le barycentre de chaque tirage
                    QString str_tblAnalyse = "";
                    if(tbl_in=="E1"){
                        str_tblAnalyse = "U_E1_ana_z1";
                    }
                    else{
                        str_tblAnalyse = "B_ana_z1";
                    }
                    isOk = mettreBarycentre(str_tblAnalyse, str_data);
                }
            }
            else{
                /// Appeller la fonction des sommes de chaque boule
                ;
            }
        }
        else{
            /// On a aucune valeur pour faire les calculs
            ;
        }
    }

    if(!isOk){
        /// analyser erreur
        QString err_msg = query.lastError().text();
        //un message d'information
        QMessageBox::critical(NULL, "Barycentre", err_msg,QMessageBox::Yes);
#ifndef QT_NO_DEBUG
        qDebug() << err_msg;
#endif
        QApplication::quit();
    }
}

bool CBaryCentre::isTableTotalBoulleReady(QString tbl_total)
{
    return true;
}

bool CBaryCentre::mettreBarycentre(QString tbl_dst, QString src_data)
{
    bool isOK = true;
    QSqlQuery query(db);
    QString msg = "";

    /// 1 : Renommer la table resultat
    msg = "ALTER TABLE "+tbl_dst+" RENAME TO old_"+tbl_dst+";";

    if((isOK=query.exec(msg))){
        stJoinArgs param;
        param.arg1 = "tbLeft.*, tbRight.BC as Bc";
        param.arg2 = QString("old_")+tbl_dst;
        param.arg3 = src_data;
        param.arg4 = "tbLeft.id = tbRight.id";

        msg = DB_Tools::leftJoin(param);
#ifndef QT_NO_DEBUG
        qDebug() << msg;
#endif
        //if not exists
        msg = "create table  " + tbl_dst + " as " + msg;

        if((isOK = query.exec(msg))){
            /// Supprimer table old
            msg = "drop table if exists old_"+tbl_dst+";";
            isOK = query.exec(msg);
        }
    }
    return isOK;
}
