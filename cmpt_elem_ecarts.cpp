#ifndef QT_NO_DEBUG
#include <QDebug>
#include <QSqlError>
#endif

#include <QToolTip>

#include <QTableView>
#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSortFilterProxyModel>
#include <QHeaderView>
#include <QItemDelegate>
#include <cmath>

#include "cmpt_elem_ecarts.h"
#include "db_tools.h"

const QColor fond[4]={QColor(255,156,86,190),//Orange
                      QColor(140,255,124,190), //Vert
                      QColor(70,160,220,190),//bleu
                      QColor(255,40,180,190)//Violet
                     };

int C_ElmEcarts::total = 0;

C_ElmEcarts::~C_ElmEcarts()
{
    total --;
}

QTableView * C_ElmEcarts::getTbv(int zn)
{
    return(tbv_memo[zn]);
}

C_ElmEcarts::C_ElmEcarts(const QString &in, const int ze, const BGame &pDef,  QSqlDatabase fromDb)
    :BCount(pDef,in,fromDb,NULL,eCountElm)
{
    QString name = "";
    QTableView *qtv_tmp = NULL;
    int nb_zones = myGame.znCount;

    if (ze< nb_zones && ze >=0)
    {
        if(nb_zones == 1){
            hCommon = CEL2_H *(floor(myGame.limites[ze].max/10)+1);
        }
        else{
            if(ze<nb_zones-1)
                hCommon = CEL2_H * BMAX_2((floor(myGame.limites[ze].max/10)+1),(floor(myGame.limites[ze+1].max/10)+1));
        }
    }

    qtv_tmp = Compter(&name,ze);

    qtv_tmp->setParent(this);
    total++;
}

QTableView * C_ElmEcarts::Compter(QString *pname, int zn)
{
    QTableView *qtv_tmp = new QTableView;
    QString qtv_name = QString::fromLatin1(cClc_eca)
            +"_"+ QString::fromLatin1(cClc_elm)
            +"_"+ QString::number(total).rightJustified(3,'0')
            + "_z"+QString::number(zn+1);
    qtv_tmp->setObjectName(qtv_name);


    /// Creation de la table dans la base
    /// pour garder les resultat
    if(!createThatTable(qtv_name,zn)){
        /// stopper les calculs

        return   qtv_tmp;
    }

    /// suite du traitement
    BSqmColorizeEcart *sqm_tmp = new BSqmColorizeEcart;
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

    tbv_memo[zn] = qtv_tmp;
    return   qtv_tmp;
}

void C_ElmEcarts::slot_SurligneTirage(const QModelIndex &index)
{
    int col=index.column();

    if( (col >0) && (col<3) )
    {
        /// CODE A SUPPRIMER
        emit(sig_TotEcart(index));
    }
}

void C_ElmEcarts::slot_AideToolTip(const QModelIndex & index)
{
    QTableView *view = qobject_cast<QTableView *>(sender());

    QBrush C = index.data(Qt::BackgroundRole).value<QBrush>();
    QBrush vide;

    QString msg = "";
    QString msgAdd = "";
    int val = index.model()->index(index.row(),0).data().toInt();
    int cln = index.column();

    const QAbstractItemModel * pModel = index.model();
    QVariant vCol = pModel->headerData(cln,Qt::Horizontal);
    QString colName = vCol.toString();

    if(colName == "Es"){
        msg = "Ecart à la moyenne\n";
    }

    msg = msg + "Boule " + QString::number(val)+"\n";

    //qDebug()<<"Brush:"<<C<<",col:"<<index.column();

    if(C == QBrush(Qt::green))
    {
        msgAdd = "pas encore sortie.";
    }

    if(C==QBrush(fond[0]))
    {
        msgAdd = QString("Ec proche de Ep");
    }

    if(C==QBrush(fond[1]))
    {
        msgAdd = QString("Ec proche Em");
    }

    if(C==QBrush(fond[2]))
    {
        msgAdd = QString("Ep proche Em");
    }

    if(C==QBrush(fond[3]))
    {
        msgAdd = QString("Ep2 proche Em");
    }

    if(C==vide && cln > 0)
    {
        QVariant cellVal = index.data();

        msgAdd = colName + " = " + cellVal.toString();
    }

    msg = msg + msgAdd;
    QToolTip::showText (QCursor::pos(), msg);
}

bool C_ElmEcarts::createThatTable(QString tblName, int zn)
{
    QString tableBoule = "";
    bool isOk = true;
    QString msg = "";
    QSqlQuery query(dbToUse);
    int max_boules = myGame.limites[zn].max;

    ///  creer la table
    msg = "create table if not exists "
            +tblName
            +"(B int, Ec int, Ep int, Em real, M int, Es float, T int, A int);";
#ifndef QT_NO_DEBUG
    qDebug() <<msg;
#endif
    isOk = query.exec(msg);

    ///  Rechercher chaque boule
    for(int boule = 0; (boule < max_boules) & isOk ; boule++){
        ///  faire les calculs necessaires
        tableBoule = RechercherLesTirages(boule,zn);
        ///  sauvegarder les resultats
        isOk = SauverCalculs(boule, tblName, tableBoule);
    }

    if(isOk){
        msg = "drop table if exists " +tableBoule + ";";
        if( isOk = query.exec(msg)){
            isOk = CalculerSqrt(tblName, "Es");
        }
    }

    return isOk;
}

QString C_ElmEcarts::RechercherLesTirages(int boule, int zn)
{
    QString msg = "";
    bool isOk = true;
    QSqlQuery query(dbToUse);
    QString tmpTbl = "tmp_rch";

#if 0
    QString tmpTbl = "tmp_rch_z"
            +QString::number(zn+1)
            +QString("_")
            +QString::number(boule+1);
#endif

    msg = "drop table if exists " +tmpTbl + ";";

#ifndef QT_NO_DEBUG
    qDebug() <<msg;
#endif
    if( !(isOk = query.exec(msg))){
        return "";
    }

    QString ref_1 = myGame.names[zn].abv+"%1 int";
    QString ref_2 = "";
    int tot_items = myGame.limites[zn].len;
    msg="";
    for(int items=0;items<tot_items;items++){
        msg = msg+ref_1.arg(items+1);
        if(items<tot_items-1) msg = msg+",";
    }
    msg = "create table if not exists "
            + tmpTbl
            +"(lgn integer primary key, id int,"
            + msg
            +")";

#ifndef QT_NO_DEBUG
    qDebug() <<msg;
#endif
    if( !(isOk = query.exec(msg))){
        return "";
    }

    msg="";
    QString clause="";
    ref_1 = "t1."+myGame.names[zn].abv+"%1";
    ref_2 = "(t1."+myGame.names[zn].abv+"%1=%2)";
    for(int items=0;items<tot_items;items++){
        msg = msg+ref_1.arg(items+1);
        clause = clause + ref_2.arg(items+1).arg("%1");
        if(items<tot_items-1){
            msg = msg+",";
            clause = clause+"or";
        }
    }
#ifndef QT_NO_DEBUG
    qDebug() <<msg;
#endif
    msg = "insert into "
            + tmpTbl
            +" select null, t1.id,"
            + msg
            +" from("
            +db_data
            +") as t1 where";

    ///remplir la table
    QString sql = clause.arg(boule+1);
    sql = msg
            +"("
            + sql
            +")";
#ifndef QT_NO_DEBUG
    qDebug() <<sql;
#endif
    if( !(isOk = query.exec(sql))){
        return "";
    }

    query.finish();

    return tmpTbl;
}

bool C_ElmEcarts::SauverCalculs(int boule, QString tblName, QString tmpTbl)
{
    QString msg = "";
    bool isOk = true;
    QSqlQuery query(dbToUse);
    QString colVariance = "Es";

    if(tmpTbl == ""){
        return false;
    }

    QString msg_var = " select ("
                      " printf(\"%.1f\","
                      " sum (((t2.id-t1.id) - t1.Em) * ((t2.id-t1.id) - t1.Em))/count(*)"
                      " )) as "
            +colVariance
            +
            " from"
            " ("
            " select lgn,id,Em from "
            +tmpTbl
            +" left join"
             " ( select"
             " printf(\"%.1f\",avg(t2.id-t1.id))as Em"
             " from "
            +tmpTbl
            +" as t1, "
            +tmpTbl
            +" as t2 where (t2.lgn = t1.lgn+1))) as t1,"
             " ("
             " select lgn,id,Em from "
            +tmpTbl
            +" left join"
             " ( select"
             " printf(\"%.1f\",avg(t2.id-t1.id))as Em"
             " from "
            +tmpTbl
            +" as t1, "
            +tmpTbl
            +" as t2 where (t2.lgn = t1.lgn+1))) as t2"
             " where"
             " (t2.lgn = t1.lgn +1)";

#ifndef QT_NO_DEBUG
    qDebug() <<msg_var;
#endif

    QString sql = "insert into "
            + tblName
            +" select max(B)as B, max(Ec) as Ec, max(Ep) as Ep,"
             "printf(\"%.1f\",avg(E))as Em,max(E) as M, "
            +colVariance
            +" as Es, "
             "count(B) as T, 0"
             " from "
             "("
             "select "
            +QString::number(boule+1)
            +" as B,(case when t1.lgn=1 then t1.id -1 end)as Ec,"
             "(case when t1.lgn=1 then (t2.id-t1.id)  end)as Ep,"
             "(t2.id-t1.id) as E, ("
            + msg_var +
            ") as "
            +colVariance
            +" from "
            +tmpTbl
            +" as t1, "
            +tmpTbl
            +" as t2 "
             "where"
             "(t2.lgn=t1.lgn+1)"
             ")";
#ifndef QT_NO_DEBUG
    qDebug() <<sql;
#endif
    if((isOk = query.exec(sql))){
        if(isOk = query.isActive()){
            query.finish();
        }
    }

    return isOk;
}

#if 0
bool BCountEcart::CalculerSqrt(QString tblName, QString colVariance)
{
    bool isOk = true;
    QString msg = "";
    QSqlQuery query(dbToUse);

    msg= "select B, "+colVariance+" from "+tblName+";";

#ifndef QT_NO_DEBUG
    qDebug() <<msg;
#endif

    if (isOk = query.exec(msg))
    {
        query.first();
        if(query.isValid())
        {
            QSqlQuery req_2(dbToUse);
            QString msg2 = "";
            do
            {
                int bId = query.value(0).toInt();
                float variance = query.value(1).toFloat();

                QString strSqrt = QString::number((float)sqrt(variance),'f',1);

                msg2 = "update "+tblName+" set "+colVariance+"="
                        +strSqrt
                        + " "
                          "where "+tblName+".B="+QString::number(bId)+";";
#ifndef QT_NO_DEBUG
                qDebug() <<msg2;
#endif
                isOk = req_2.exec(msg2);

            }while(query.next() && isOk);
        }
    }
    return isOk;
}
#endif
///-----------------------------
/// Delegation couleur sur tableau ecart
void BDlgEcart::paint(QPainter *painter, const QStyleOptionViewItem &option,
                      const QModelIndex &index) const
{
#if 0
    int col = index.column();
    int row = index.row();
    int nbCol = index.model()->columnCount();
    int valCel = 0;
    int valAna = 0;
    double radix = 1.5;
    int calc = 0;
    QModelIndex local_index = index;

    QStyleOptionViewItem maModif(option);
    /*QColor fond[]= {QColor(255,106,0,255),
                 QColor(255,191,0,255),
                 QColor(101,148,255,255)};*/
    const QColor fond[4]={QColor(255,156,86,190),
                          QColor(140,255,124,190),
                          QColor(70,160,220,190),
                          QColor(255,40,180,190)
                         };


    //index.data().ca
    //if(index.data().canConvert(QMetaType::Int))
    {
        valCel = index.data().toInt();
    }
    valAna = index.model()->index(index.row(),1).data().toInt();

    if(valCel)
        calc = abs(valAna-valCel)% valCel;

    if(col==1){
        calc = index.model()->index(index.row(),2).data().toInt();
        valAna = index.model()->index(index.row(),3).data().toInt();
        calc = abs(valAna-calc)% valAna;
        if(valAna && calc<=radix){
            //painter->fillRect(option.rect, fond[2]);
        }
    }

    if(col==2){
        /// colonne Ep
        if(valAna && calc<=radix){
            //index.model()->setData(local_index,fond[0],Qt::DecorationRole);
            //painter->fillRect(option.rect, fond[0]);
        }
    }
    if(col==3){
        /// colonne Em
        if(valAna && calc<=radix){
            //painter->fillRect(option.rect, fond[1]);
        }
    }
    if(col==4){
        /// colonne EM
        if(valAna && calc<=radix){
            //painter->fillRect(option.rect, fond[2]);
        }
    }

    /*

    /// Regarder la valeur de la derniere colonne
    /// Elle indique que mettre comme couleur
    if(index.model()->index(index.row(),nbCol-1).data().canConvert(QMetaType::Int))
    {
        val =  index.model()->index(index.row(),nbCol-1).data().toInt();
    }


    if(col>0 && col<(nbCol-1)){
        /// bit actif
        if(val & (1<<col-1)){
            painter->fillRect(option.rect, u[0]);
        }
    }
    */
    QItemDelegate::paint(painter, maModif, index);
#endif
    QItemDelegate::paint(painter, option, index);
}


bool BSqmColorizeEcart::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int col = index.column();

    if(col== 7 )
    {
        QModelIndex V = index.sibling(index.row(),7);
        //float variance = sqrt(V.data().toFloat());


        //this->setData(V,variance,Qt::DisplayRole);
        //this->setData(V,"2.0",Qt::DisplayRole);
    }
    return true;
}

QVariant BSqmColorizeEcart::data(const QModelIndex &index, int role) const
{
    /// Mettre les couleurs ??
    ///setData(index,index.data(Qt::BackgroundRole),Qt::BackgroundRole);

    int col = index.column();

    QColor u[]= {
        Qt::black,
        Qt::red,
        Qt::green,
        QColor(255,216,0,255),
        QColor(255,106,0,255),
        QColor(178,0,255,255),
        QColor(211,255,204,255)
    };


    if(col== 0 )
    {
        //int nbCol=index.model()->columnCount();
        double radix = 1.5;
        /// recuperation des l'info de la ligne
        QModelIndex Ec = index.sibling(index.row(),1);
        QModelIndex Ep = index.sibling(index.row(),2);
        QModelIndex Em = index.sibling(index.row(),3);
        QModelIndex ET = index.sibling(index.row(),4);
        QModelIndex Ab = index.sibling(index.row(),6);


        int ec = Ec.data().toInt();
        int ep = Ep.data().toInt();
        int em = Em.data().toInt();
        int et = ET.data().toInt();
        int ab = Ab.data().toInt();

        // Boule deja sortie ?
        if(ab == 0){
            if (role == Qt::BackgroundRole){
                return (u[6]);
            }

        }

        if(ec &&((abs(ec-ep)%ep<=radix)||
                 (abs(ec-em)%em<=radix)
                 )){
            if (role == Qt::TextColorRole){
                return (u[1]);
            }
            if (role == Qt::DecorationRole){
                return (QIcon(":/images/flag_1s.png"));
            }
        }
    }


    ///--------------------------------------
    /// On va mettre une couleur de fond
    /// pour la cellule en fonction
    /// de la valeur de l'ecart courant
    if(role==Qt::BackgroundRole){
        int valCel = 0;
        int valAna = 0;
        double radix = 1.5;
        int calc = 0;

        /// Valeur presente dans la cellule
        valCel = index.data().toInt();

        /// Valeur de l'ecart courant pour cette ligne
        valAna = index.model()->index(index.row(),1).data().toInt();

        if(valCel)
            calc = abs(valAna-valCel)% valCel;

        if(col==1){
            // Montrer si Ec proche de Ep
            calc = index.model()->index(index.row(),2).data().toInt();
            if(valAna){
                calc = abs(valAna-calc)% valAna;
            }
            if(valAna && calc<=radix){
                return(QBrush(fond[0],Qt::SolidPattern));
                //painter->fillRect(option.rect, fond[0]);
            }
        }
        if(col==2){
            // Montrer si Ep proche Em
            // Colonne Ep:Ecart precedent
            calc = index.model()->index(index.row(),2).data().toInt();
            valAna = index.model()->index(index.row(),3).data().toInt();
            if(valAna){
                calc = abs(valAna-calc)% valAna;
            }
            if(valAna && calc<=radix){
                return(QBrush(fond[2],Qt::SolidPattern));
                //painter->fillRect(option.rect, fond[2]);
            }
        }

        if(col==3){
            // Montrer si Ec proche Em
            // colonne Em:Ecart moyen
            if(valAna && calc<=radix){
                return(QBrush(fond[1],Qt::SolidPattern));
                //painter->fillRect(option.rect, fond[1]);
            }
        }
        if(col==4){
            // Montrer si Ec proche ou superieur a EM
            // colonne EM: Ecart maximun
            if(valAna && ((valAna+radix)>=valCel)){
                return(QBrush(fond[3],Qt::SolidPattern));
                //painter->fillRect(option.rect, fond[2]);
            }
        }
    }

    return QSqlQueryModel::data(index,role);
}
