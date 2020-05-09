#ifndef BLSTTIRAGES_H
#define BLSTTIRAGES_H

#include <QWidget>
#include "game.h"

class BLstTirages : public QWidget
{
 Q_OBJECT

public:
explicit BLstTirages(const stGameConf *pGame, QWidget *parent=nullptr);
QString getGameLabel(void);

protected:
QString game_lab;

};

#endif // BLSTTIRAGES_H
