#ifndef DB_TOOLS_H
#define DB_TOOLS_H

#include <QString>
#include <QStringList>

class DB_Tools
{
public:
    DB_Tools();
    static QString GEN_Where_3(int loop,
                                    QString tb1,bool inc1,QString op1,
                                    QStringList &tb2,bool inc2,QString op2
                                    );

};

#endif // DB_TOOLS_H
