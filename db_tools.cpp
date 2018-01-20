#ifndef QT_NO_DEBUG
#include <QDebug>
#include <QSqlError>
#endif


#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QApplication>

#include "db_tools.h"

/// cette fonction construit une chaine sous contrainte
/// on peut mettre n indice (loop) apres tb1 si inc1 = vrai
/// idem pour chacun des elments de tb2 si inc2 = vrai
/// chacun des elements construit de tb1 sera mis en
/// relation avec 1 element de tb2 par l'operateur op1
/// chaque element de tb2 etant separé par op2
/// ex : F(5,"tb3.",true, " as ", lst, true, "," =
/// avec lst ayant b
/// 'tb3.b1 as b1,'...'tb5.b5 as b5,'
QString DB_Tools::GEN_Where_3(int loop,
                              QString tb1,bool inc1,QString op1,
                              QStringList &tb2,bool inc2,QString op2
                              )
{
    QString ret_msg = "";
    QString ind_1 = "";
    QString ind_2 = "";

    QString flag = " and ";

    for(int j=0; j< tb2.size();j++)
    {
        ret_msg = ret_msg + "(";
        for(int i = 0; i<loop;i++)
        {
            // Mettre un nombre apres  1er table
            if(inc1)
            {
                ind_1 = tb1+QString::number(i+1);
            }
            else
            {
                ind_1 = tb1;
            }

            // Mettre un nombre apres  2eme table
            if(inc2)
            {
                ind_2 = tb2.at(j)+QString::number(i+1);
            }
            else
            {
                ind_2 = tb2.at(j);
            }

            // Construire message
            ret_msg = ret_msg
                    + ind_1
                    + op1
                    + ind_2
                    + " " + op2 + " ";
        }
        // retirer le dernier operateur (op2)
        ret_msg.remove(ret_msg.length()-op2.length()-1, op2.length()+1);

        ret_msg =  ret_msg + ")";
        ret_msg = ret_msg + flag;
    }
    // retirer le dernier operateur
    ret_msg.remove(ret_msg.length()-flag.length(),flag.length());

#ifndef QT_NO_DEBUG
    qDebug() << "GEN_Where_3\n";
    qDebug() << "SQL msg:\n"<<ret_msg<<"\n-------";
#endif

    return ret_msg;
}

QString DB_Tools::innerJoin(stJoinArgs ja)
{
    QString arg1 = ja.arg1;
    QString arg2 = ja.arg2;
    QString arg3 = ja.arg3;
    QString arg4 = ja.arg4;
    QString msg = "";

    msg = "select " + arg1 + " from ("+arg2+")as tbLeft "
                                            "inner join ("+arg3+")as tbRight "
                                                                "on ("+arg4+")";

#ifndef QT_NO_DEBUG
    qDebug() << "DB_Tools::innerJoin\n";
    qDebug() << "msg:\n"<<msg<<"\n-------";
#endif

    return msg;
}

QString DB_Tools::leftJoin(stJoinArgs ja)
{
    QString arg1 = ja.arg1;
    QString arg2 = ja.arg2;
    QString arg3 = ja.arg3;
    QString arg4 = ja.arg4;
    QString msg = "";

    msg = "select " + arg1 + " from ("+arg2+")as tbLeft "
                                            "left join ("+arg3+")as tbRight "
                                                               "on ("+arg4+")";

#ifndef QT_NO_DEBUG
    qDebug() << "DB_Tools::leftJoin";
    qDebug() << msg<<"\n-------\n";
#endif

    return msg;
}

QString DB_Tools::innerJoinFiltered(stJoinArgs ja,QString arg5)
{
    QString msg = "";

    msg = innerJoin(ja)+"where("+arg5+")";

#ifndef QT_NO_DEBUG
    qDebug() << "DB_Tools::innerJoinFiltered\n";
    qDebug() << "msg:\n"<<msg<<"\n-------";
#endif

    return msg;
}

QString DB_Tools::leftJoinFiltered(stJoinArgs ja,QString arg5)
{
    QString msg = "";

    msg = leftJoin(ja)+"where("+arg5+")";

    DB_Tools::DisplayError("DB_Tools::leftJoinFiltered",NULL,msg);

    return msg;
}

void DB_Tools::DisplayError(QString fnName, QSqlQuery *pCurrent,QString sqlCode)
{
    //un message d'information
    QMessageBox::critical(NULL, fnName, "Erreur traitement !",QMessageBox::Yes);

#ifndef QT_NO_DEBUG
    QString sqlError = "";
    QString sqlText = "";

    if(pCurrent !=NULL)
    {
        sqlError = pCurrent->lastError().text();
        sqlText = pCurrent->lastQuery();
    }
    else
    {
        sqlError = "Not in query";
        sqlText = "Can not say";
    }
    qDebug() << "Fonction:"<<fnName;
    qDebug() << "Error from Query:"<<sqlError;
    qDebug() << "Code of Query:"<<sqlText;
    qDebug() << "Code wanted:"<<sqlCode<<"\n--------------";
#endif
    QApplication::exit();

}

