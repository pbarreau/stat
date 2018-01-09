#ifndef QT_NO_DEBUG
#include <QDebug>
#endif

#include <QGridLayout>
#include <QTableView>
#include <QSqlQuery>
#include <QHeaderView>
#include <QToolTip>
#include <QStackedWidget>

#include "compter.h"
#include "tirages.h"

void B_Comptage::CreerCritereJours(void)
{
    QString st_tmp = "";

    QSqlQuery query ;
    QString msg = "";
    QString st_table = "";
    bool status = false;

    if(db_data == TB_BASE){
      st_table =  "jour_tirage";
    }
    else
    {
       st_table = "J";
    }

    msg = "select distinct substr(tb1."+st_table+",1,3) as J from ("+
            db_data+") as tb1 order by J;";

    status = query.exec(msg);

    if(status)
    {
        status = query.first();
        if (query.isValid())
        {
            do
            {
                //count(CASE WHEN  J like 'lundi%' then 1 end) as LUN,
                st_tmp = st_tmp + "count(CASE WHEN  J like '"+
                        query.value(0).toString()+"%' then 1 end) as "+
                        query.value(0).toString()+",";
            }while(status = query.next());

            //supprimer derniere ','
            st_tmp.remove(st_tmp.length()-1,1);
            st_tmp = st_tmp + " ";
        }
    }

#ifndef QT_NO_DEBUG
        qDebug() << "CreerCritereJours ->"<< query.lastError();
        qDebug() << "SQL 1:\n"<<msg<<"\n-------";
        qDebug() << "SQL 2:\n"<<st_tmp<<"\n-------";
#endif

    query.finish();
    db_jours = st_tmp;
}

void B_Comptage::RecupererConfiguration(void)
{
    QSqlQuery query ;
    QString msg = "";
    bool status = false;

    msg = "select count(id) as tot from (" + QString::fromLocal8Bit(TB_RZ) + ");";
    status = query.exec(msg);

    if(status)
    {
        status = query.first();
        if (query.isValid())
        {
            nbZone = query.value(0).toInt();
            names  = new cZonesNames [nbZone];
            limites = new cZonesLimits [nbZone];

            // remplir les infos
            msg = "select tb1.id, tb1.name, tb1.abv, tb2.len, tb2.min, tb2.max from " +
                    QString::fromLocal8Bit(TB_RZ) + " as tb1, " +
                    QString::fromLocal8Bit(TB_RZVA) + " as tb2 " +
                    " where (tb1.id = tb2.id );";
            status = query.exec(msg);

            if(status)
            {
                status = query.first();
                if (query.isValid())
                {
                    for(int i = 0; (i< nbZone) && status; i++)
                    {
                        names[i].complet = query.value(1).toString();
                        names[i].court = query.value(2).toString();
                        limites[i].len = query.value(3).toInt();
                        limites[i].min = query.value(4).toInt();
                        limites[i].max = query.value(5).toInt();
                        status = query.next();
                    }
                }
            }
        }
    }
#ifndef QT_NO_DEBUG
        qDebug() << "RecupererConfiguration ->"<< query.lastError();
        qDebug() << "SQL 1:\n"<<msg<<"\n-------";
#endif

    query.finish();

}

B_Comptage::B_Comptage(QString *in):db_data(*in)
{
    nbZone = 0;
    db_jours = "";
    names = NULL;
    limites = NULL;

    RecupererConfiguration();
    CreerCritereJours();
}

void B_Comptage::slot_AideToolTip(const QModelIndex & index)
{
    QString msg="";
    const QAbstractItemModel * pModel = index.model();
    int col = index.column();

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

void B_Comptage::slot_ClicDeSelectionTableau(const QModelIndex & index)
{
}

void B_Comptage::slot_RequeteFromSelection(const QModelIndex & index)
{
}

QString B_Comptage::CriteresCreer(QString critere , QString operateur, int zone)
{
    QString ret_msg = "";

    // Operateur : or | and
    // critere : = | <>
    int totElements = limites[zone].len;
    for(int i = 0; i<totElements;i++)
    {
        QString zName = names[zone].court;
        ret_msg = ret_msg +"tb2.B "+ critere +" tb1."
                + zName+QString::number(i+1)
                + " " + operateur+ " ";
    }
    int len_flag = operateur.length();
    ret_msg.remove(ret_msg.length()-len_flag-1, len_flag+1);

#ifndef QT_NO_DEBUG
    qDebug() << ret_msg;
#endif

    return ret_msg;
}
QString B_Comptage::CriteresAppliquer(QString st_tirages, QString st_cri, int zn)
{
}