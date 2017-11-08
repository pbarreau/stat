#ifndef SHOWSTEPPER_H
#define SHOWSTEPPER_H

#include <QObject>
#include <QWidget>

class ShowStepper: public QObject
{
    Q_OBJECT
public:
    ShowStepper(int cid, int tid);
private:
    int my_tid;
};

#endif // SHOWSTEPPER_H
