#include "BLstTirages.h"

BLstTirages::BLstTirages(const stGameConf *pGame, QWidget *parent): QWidget(parent)
{

}

QString BLstTirages::getGameLabel(void)
{
 return game_lab;
}
