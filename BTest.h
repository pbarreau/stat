#ifndef BTEST_H
#define BTEST_H

#include <QWidget>
#include "game.h"

namespace BTest
{
 bool montestRapideSql(const stGameConf *pGame, int zn=0);
 QString getFieldsFromZone(const stGameConf *pGame, int zn, QString alias="", bool visual=false);
 };

#endif // BTEST_H
