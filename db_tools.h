#ifndef DB_TOOLS_H
#define DB_TOOLS_H

#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>

typedef struct _stJoinArgs{
    QString arg1;
    QString arg2;
    QString arg3;
    QString arg4;
}stJoinArgs;

namespace DB_Tools {
QString GEN_Where_3(int loop,
                    QString tb1,bool inc1,QString op1,
                    QStringList &tb2,bool inc2,QString op2
                    );
QString innerJoin(stJoinArgs ja);
QString leftJoin(stJoinArgs ja);
QString innerJoinFiltered(stJoinArgs ja,QString arg5);
QString leftJoinFiltered(stJoinArgs ja,QString arg5);
void DisplayError(QString fnName, QSqlQuery *pCurrent, QString sqlCode);

}

#if 0
class DB_Tools
{
public:
    DB_Tools();

};
#endif

#endif // DB_TOOLS_H
