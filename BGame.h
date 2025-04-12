#ifndef BGAME_H
#define BGAME_H

#include <QAction>
#include <QObject>
#include "game.h"

class BGame : public QAction
{
    Q_OBJECT
public:
    explicit BGame(etFdj efdjGame=eFdjNone, QObject *parent = nullptr);

public slots:
    void BSlot_onTriggered(void);

signals:
    void BSig_GameType(const etFdj efdjGame);

private:
    etFdj game;
};

#endif // BGAME_H
