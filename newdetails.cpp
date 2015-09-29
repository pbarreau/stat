#ifndef QT_NO_DEBUG
#include <QDebug>
#endif


#include <QtGui>
#include <QSqlQuery>

#include "SyntheseDetails.h"

void SyntheseDetails::NewSqlGenerique(int dst)
{
    // Tirages avec filtre
    QString st_baseUse = "";
    st_baseUse = pLaDemande->st_baseDef->remove(";");
    QString st_cr1 = "";
    QStringList lst_tmp;

    if(pLaDemande->origine == 1)
    {
        for(int i=0; i< 3 ;i++)
        {
            // A t'on une selection B,E,C
            if(pLaDemande->lst_boules[i].size())
            {
              st_cr1 = st_cr1 ;
            }
        }
    }
    lst_tmp << "tb2.b";
    st_cr1 =  GEN_Where_3(5,"tb1.boule",false,"=",lst_tmp,true,"or");
    QString st_msg1 =
            "select tb1.boule as B, count(tb2.id) as T, "
            + *(pLaDemande->st_jourDef) +
            " "
            "from  "
            "("
            "select id as boule from Bnrz where (z1 not null ) "
            ") as tb1 "
            "left join "
            "("
            "select tb2.* from "
            "("
            +st_baseUse+
            " )as tb1"
            ","
            "("
            +st_baseUse+
            ")as tb2 "
            "where"
            "("
            "tb2.id=tb1.id + "
            +QString::number(dst) +
            ")"
            ") as tb2 "
            "on "
            "("
            +st_cr1+
            ") group by tb1.boule;";

#ifndef QT_NO_DEBUG
    qDebug()<< st_msg1;
#endif

}
