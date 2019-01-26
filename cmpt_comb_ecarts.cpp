#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QTableView>
#include <QToolTip>

#include <QTableView>
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSortFilterProxyModel>
#include <QHeaderView>
#include <QItemDelegate>
#include <QSqlRecord>

#include "cmpt_comb_ecarts.h"

int C_CmbEcarts::total = 0;

C_CmbEcarts::~C_CmbEcarts()
{
    total --;
}

QTableView * C_CmbEcarts::getTbv(int zn)
{
    return(tbvCalculs[zn]);
}

C_CmbEcarts::C_CmbEcarts(const QString &in, const int ze, const BGame &pDef,  QSqlDatabase fromDb)
    :BCount(pDef,in,fromDb,NULL,eCountElm)
{
    countId = total;
    unNom = "'Ecart Combinaisons'";
    total++;

    int nb_zones = myGame.znCount;
    tbvCalculs = new QTableView *[nb_zones];

    QTableView *(C_CmbEcarts::*ptrFunc[])(QString *, int) =
    {
            &C_CmbEcarts::Compter,
            &C_CmbEcarts::Compter

};


    for (int zn = 0; zn< nb_zones ;zn++)
    {
        QString *name = new QString;
        QTableView *calcul = (this->*ptrFunc[zn])(name, zn);
        calcul->setParent(this);
        tbvCalculs[zn]=calcul;
    }

}

QTableView * C_CmbEcarts::Compter(QString *pname, int zn)
{
    bool isOk = true;
    QTableView *qtv_tmp = new QTableView;
    QString qtv_name = QString::fromLatin1(cClc_eca)
            +"_"+ QString::fromLatin1(cClc_cmb)
            +"_"+ QString::number(total).rightJustified(3,'0')
            + "_z"+QString::number(zn+1);
    qtv_tmp->setObjectName(qtv_name);


    /// Creation de la table dans la base
    /// pour garder les resultat
    if(!(isOk = createThatTable(qtv_name,zn))){
        /// stopper les calculs

        return   qtv_tmp;
    }

    /// suite du traitement
    QSqlQueryModel *sqm_tmp = new QSqlQueryModel;
    QString st_msg1 = "select * from ("
            + qtv_name
            +") order by B desc;";

#ifndef QT_NO_DEBUG
    qDebug() <<st_msg1;
#endif

    sqm_tmp->setQuery(st_msg1,dbToUse);

    qtv_tmp->setAlternatingRowColors(true);
    qtv_tmp->setSelectionMode(QAbstractItemView::SingleSelection);
    qtv_tmp->setSelectionBehavior(QAbstractItemView::SelectItems);
    qtv_tmp->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QSortFilterProxyModel *m=new QSortFilterProxyModel();
    m->setDynamicSortFilter(true);
    m->setSourceModel(sqm_tmp);

    qtv_tmp->setModel(m);
    qtv_tmp->setItemDelegate(new BDlgEcart); /// Delegation

    qtv_tmp->verticalHeader()->hide();
    qtv_tmp->setSortingEnabled(true);
    qtv_tmp->sortByColumn(0,Qt::AscendingOrder);
    qtv_tmp->setStyleSheet("QTableView {selection-background-color: #939BFF;}");

    //largeur des colonnes
    int nbCol = sqm_tmp->columnCount();
    for(int pos=0;pos<nbCol;pos++)
    {
        qtv_tmp->setColumnWidth(pos,CEL2_L);
    }
    int l = CEL2_L * (nbCol+1);
    qtv_tmp->setFixedWidth(l);

    //qtv_tmp->setFixedHeight(hCommon);
    qtv_tmp->setFixedHeight(CEL2_H*7);

    qtv_tmp->setMouseTracking(true);
    connect(qtv_tmp,
            SIGNAL(entered(QModelIndex)),this,SLOT(slot_AideToolTip(QModelIndex)));

    /// Montrer le tirage quand click sur Ec ou Ep
    connect( qtv_tmp, SIGNAL(clicked (QModelIndex)) ,
             this, SLOT( slot_SurligneTirage( QModelIndex) ) );

    //tbvCalculs[zn] = qtv_tmp;
    return   qtv_tmp;
}

bool C_CmbEcarts::createThatTable(QString tblEcartcombi, int zn)
{
    QString tblTempo = "";
    bool isOk = true;
    QString msg = "";
    QSqlQuery query(dbToUse);
    int max_combi = 0;

    /// Nombre de combinaison de la zone
    msg = "select count(*) from B_"
            +QString(cClc_cmb)
            +"_z"+QString::number(zn+1)+";";

#ifndef QT_NO_DEBUG
    qDebug() <<msg;
#endif

    if((isOk = query.exec(msg))){
        query.first();
        if((isOk = query.isValid()))
        {
            QSqlRecord rec  = query.record();
            max_combi = rec.value(0).toInt();
        }
    }

    ///  creer la table
    msg = "create table if not exists "
            +tblEcartcombi
            +"(B int, C text, Ec int, Ep int, Em real, M int, Es float, T int, A int);";
#ifndef QT_NO_DEBUG
    qDebug() <<msg;
#endif

    if(isOk){
        isOk = query.exec(msg);
    }

    ///  Rechercher chaque boule
    for(int combi = 1; (combi <= max_combi) & isOk ; combi++){
        ///  faire les calculs necessaires
        tblTempo = RechercherLesTirages(combi,zn);
        ///  sauvegarder les resultats
        isOk = SauverCalculs(zn, combi, tblEcartcombi, tblTempo);
    }

    if(isOk){
        msg = "drop table if exists " +tblTempo + ";";
        if( (isOk = query.exec(msg))){
            /// Calculer l'esperance de chaque tirage
            ///  grace a la variance
            isOk = CalculerSqrt(tblEcartcombi, "Es");
        }
    }

    return isOk;
}

QString C_CmbEcarts::RechercherLesTirages(int combi, int zn)
{
    QString msg = "";
    bool isOk = true;
    QSqlQuery query(dbToUse);
    QString tmpTbl = "tmp_rch";


    msg = "drop table if exists " +tmpTbl + ";";

#ifndef QT_NO_DEBUG
    qDebug() <<msg;
#endif
    if( !(isOk = query.exec(msg))){
        return "";
    }

    msg = "create table if not exists "
            + tmpTbl
            +"(lgn integer primary key, id int);";

#ifndef QT_NO_DEBUG
    qDebug() <<msg;
#endif
    if( !(isOk = query.exec(msg))){
        return "";
    }

    msg = "insert into "
            + tmpTbl
            +" select null, t1.id"
            +" from(B_"
            +cRef_ana+"_z"
            +QString::number(zn+1)
            +") as t1 where(t1.idComb="+
            QString::number(combi)+")";

    ///remplir la table

#ifndef QT_NO_DEBUG
    qDebug() <<msg;
#endif
    if( !(isOk = query.exec(msg))){
        return "";
    }

    query.finish();

    return tmpTbl;
}

bool C_CmbEcarts::SauverCalculs(int zn, int combi, QString tblName, QString tmpTbl)
{
    QString msg = "";
    bool isOk = true;
    QSqlQuery query(dbToUse);
    QString colVariance = "Es";
    QString tblCombi = "B_"+QString(cClc_cmb)+"_z"+QString::number(zn+1);
    QString tblAna = "B_"+QString(cRef_ana)+"_z"+QString::number(zn+1);

    if(tmpTbl == ""){
        return false;
    }

    msg ="insert into "
            +tblName
            +" select (B)as B, "
             "(select t1.tip from ("
            +tblCombi
            +")as t1 where(t1.id=B)) as C,"
             "(case when count(Ec)=0 then 0 else max(Ec) end) as Ec, "
             "(case when count(Ep)=0 then 0 else max(Ep) end) as Ep,"
             "printf(\"%.1f\",avg(E))as Em,"
             "(case when count(E)=0 then 0 else max(E) end) as M,"
            +colVariance
            +" as Es,"
             "count(B) as T, "
             "0 "
             "from ("
            +tblAna
            +") as t1, "
             "("
             "select "
            +QString::number(combi)
            +" as B,"
             "(case when t1.lgn=1 then t1.id -1 end)as Ec,"
             "(case when t1.lgn=1 then (t2.id-t1.id)  end)as Ep,"
             "(t2.id-t1.id) as E, "
             "( select ( printf(\"%.1f\", sum (((t2.id-t1.id) - t1.Em) * ((t2.id-t1.id) - t1.Em))/count(*) )) as "
            +colVariance
            +" from ( "
             "select lgn,id,Em "
             "from "
            +tmpTbl+
            " left join ( "
            " select printf(\"%.1f\",avg(t2.id-t1.id))as Em "
            " from "
            +tmpTbl
            +" as t1, "
            +tmpTbl
            +" as t2 "
             " where (t2.lgn = t1.lgn+1))) as t1, "
             " ( "
             " select lgn,"
             " id,"
             " Em from "+tmpTbl+
            " left join "
            " ( "
            " select printf(\"%.1f\",avg(t2.id-t1.id))as Em "
            " from "+tmpTbl+" as t1, "
            +tmpTbl
            +" as t2 where (t2.lgn = t1.lgn+1))) as t2 "
             " where (t2.lgn = t1.lgn +1)) as "
            +colVariance
            +" from "
            +tmpTbl
            +" as t1, "
            +tmpTbl
            +" as t2 where(t2.lgn=t1.lgn+1))"
             " where (t1.id = B)";
#ifndef QT_NO_DEBUG
    qDebug() <<msg;
#endif
    if((isOk = query.exec(msg))){
        if((isOk = query.isActive())){
            query.finish();
        }
    }
    return isOk;
}
