#ifndef BTEST_H
#define BTEST_H

#include <QWidget>
#include "game.h"

namespace BTest
{
 bool montestRapideSql(const stGameConf *pGame, int zn=0, int loop=2);
 QString getFieldsFromZone(const stGameConf *pGame, int zn, QString alias="", bool visual=false);
 void writetoFile(QString file_name, QString msg, bool append=true);
 };

#endif // BTEST_H
