#ifndef BTirages_H
#define BTirages_H

#include <QWidget>
#include <QSqlDatabase>

#include "game.h"

class BTirages : public QWidget
{
 Q_OBJECT

public:
explicit BTirages(const stGameConf *pGame, QWidget *parent=nullptr);
QString getGameLabel(void);

protected:
QString game_lab;
QSqlDatabase db_tir;


};

#endif // BTirages_H
