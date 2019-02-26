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

/// http://www.sqlitetutorial.net/sqlite-primary-key/
bool DB_Tools::myCreateTableAs(QSqlQuery query, QString tblName, QString pid, QString asCode)
{
    bool isOk=false;
    QSqlQuery q_atom(query);
    QString msg = "select sql from sqlite_master where (type = 'table' and tbl_name='"+
            tblName+"'); ";
    //QString msg_s = "";
#if 0
    QString atomic[]=
    {
        {"PRAGMA foreign_keys=off;"},
        {"BEGIN TRANSACTION;"},
        {"ALTER TABLE "+tblName+" RENAME TO old_"+tblName+";"},
        {msg+";"},
        {"INSERT INTO "+tblName+" SELECT * FROM old_"+tblName+";"},
        {"drop table if exists old_"+tblName+";"},
        {"COMMIT;"},
        {"PRAGMA foreign_keys=on;"}
    };

#endif

    if((isOk=q_atom.exec(asCode))){
        if((isOk=q_atom.exec(msg))){
            q_atom.first();
            if((isOk=q_atom.isValid())){
                msg=q_atom.value(0).toString();
                msg = msg.simplified();
                msg = msg.replace("\"","'");
                msg = msg.remove("INT");
                msg = msg.replace(","," integer,");
                msg = msg.replace(")"," integer, primary key ("+pid+"))");
                QString atomic[]=
                {
                    {"PRAGMA foreign_keys=off;"},
                    {"BEGIN TRANSACTION;"},
                    {"ALTER TABLE "+tblName+" RENAME TO old_"+tblName+";"},
                    {msg+";"},
                    {"INSERT INTO "+tblName+" SELECT * FROM old_"+tblName+";"},
                    {"drop table if exists old_"+tblName+";"},
                    {"COMMIT;"},
                    {"PRAGMA foreign_keys=on;"}
                };
                int items = sizeof(atomic)/sizeof(QString);
                for(int item=0; (item<items) && isOk; item++){
                    msg = atomic[item];
#ifndef QT_NO_DEBUG
                    qDebug() << atomic[item];
#endif
                    isOk = q_atom.exec(msg);

                }
            }
        }
    }
#ifndef QT_NO_DEBUG
    qDebug() << "SQL msg:\n"<<msg<<"\n-------";
#endif
    if(!isOk){
        QString ErrLoc = "DB_Tools::myCreateTableAs";
        DB_Tools::DisplayError(ErrLoc,&q_atom,msg);
    }

    return isOk;
}

#if 0
QString DB_Tools::makeTableFromSelect(QString select)
{
    QString msg = "";
    msg = select.simplified();
    msg = msg.remove("INT");
    msg = msg.replace(","," integer,");
    msg = msg + tr(",primary key (id)");
    return msg;
}
#endif

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

    QString sqlError = "";
    QString sqlText = "";
    QString sqlGood = "";

    if(pCurrent !=NULL)
    {
        sqlGood = pCurrent->executedQuery();
        sqlError = pCurrent->lastError().text();
        sqlText = pCurrent->lastQuery();
    }
    else
    {
        sqlError = "Not in query";
        sqlText = "Can not say";
    }
#ifndef QT_NO_DEBUG
    qDebug() << "Fonction:"<<fnName;
    qDebug() << "Derniere bonne requete : "<<sqlGood;
    qDebug() << "Requete fautive : "<<sqlText;
    qDebug() << "Erreur :"<<sqlError;
    qDebug() << "Code wanted:"<<sqlCode<<"\n--------------";
#endif

#if SET_DBG_TOOL
    QString msg = QString("Fn:")+fnName + "\n"
            +QString("Gr:")+sqlGood + "\n"
            +QString("Rf:")+sqlText + "\n"
            +QString("Er:")+sqlError + "\n"
            +QString("Cw:")+sqlCode + "\n";
        QMessageBox::information(NULL, "Pgm", msg,QMessageBox::Yes);
#endif

    QApplication::exit();

}

