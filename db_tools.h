#ifndef DB_TOOLS_H
#define DB_TOOLS_H

#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>

#include "properties.h"
#include "bstflt.h"

typedef struct _stJoinArgs{
    QString arg1;
    QString arg2;
    QString arg3;
    QString arg4;
}stJoinArgs;

namespace DB_Tools {
 typedef enum E_TbType{
  etbNotSet,
  etbTempView,
  etbTempTbl,
  etbView,
  etbTable,
  etbEnd
 }tbTypes;

QString GEN_Where_3(int loop,
                    QString tb1,bool inc1,QString op1,
                    QStringList &tb2,bool inc2,QString op2
                    );
bool myCreateTableAs(QSqlQuery query, QString tblName, QString pid, QString asCode);
QString innerJoin(stJoinArgs ja);
QString leftJoin(stJoinArgs ja);
QString innerJoinFiltered(stJoinArgs ja,QString arg5);
QString leftJoinFiltered(stJoinArgs ja,QString arg5);
void DisplayError(QString fnName, QSqlQuery *pCurrent, QString sqlCode);
bool checkHavingTableAndKey(QString tbl, QString key, QString cnx);
bool isDbGotTbl(QString tbl, QString cnx, tbTypes etbtTypes=etbTable, bool silence = true);

QString getLstDays(QString cnx_db_name, QString tbl_ref);
bool tbFltGet(stTbFiltres *in_out, QString cnx);
bool tbFltSet(stTbFiltres *in_out, QString cnx);
void genStop(QString fnName);
bool SupprimerResultatsPrecedent(QString cnx);

}

#endif // DB_TOOLS_H
