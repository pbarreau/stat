#ifndef BANALYSERTIRAGES_H
#define BANALYSERTIRAGES_H

#include <QString>
#include <QSqlDatabase>

#include <game.h>

class BAnalyserTirages
{
public:
BAnalyserTirages(stGameConf *pGame, QString cnx, QString st_table);
BAnalyserTirages *self();

private:
bool isPresentUsefullTables(stGameConf *pGame, QString tbl_tirages, QString cnx);
void startAnalyse(void);
bool mkTblLstElm(stGameConf *pGame, QString tbName, QSqlQuery *query);

private:
BAnalyserTirages *addr;
QSqlDatabase db_1;

};

#endif // BANALYSERTIRAGES_H
