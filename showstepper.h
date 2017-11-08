#ifndef SHOWSTEPPER_H
#define SHOWSTEPPER_H

#include <QObject>
#include <QSqlQueryModel>

class ShowStepper: public QObject
{
    Q_OBJECT
public:
    ~ShowStepper();
    ShowStepper(int cid, int tid);
    void ExecSql(int cid,int tid);

public slots:
    void toPrevious(void);
    void toNext(void);

private:
    int my_tCol;
    int tid_cur;
    int tid_start;
    int cid_start;
    QSqlQueryModel *my_model;
};

#endif // SHOWSTEPPER_H
