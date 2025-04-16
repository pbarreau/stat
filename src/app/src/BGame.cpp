#include "BGame.h"

BGame::BGame(etFdj efdjGame, QObject *parent)
    : QAction{parent}
{
    game = efdjGame;
    connect(this, SIGNAL(triggered()), this, SLOT(BSlot_onTriggered()));
}

void BGame::BSlot_onTriggered()
{
    emit  BSig_GameType(game);
}
