#ifndef SHOWSTEPPER_H
#define SHOWSTEPPER_H

#include <QObject>
#include <QSqlQueryModel>
#include <QLabel>
#include <QHBoxLayout>

class ShowStepper: public QObject
{
    Q_OBJECT
public:
    ~ShowStepper();
    ShowStepper(int cid, int tid);

private:
    void ExecSql(int cid,int tid);
    void setLabel(int tid);
    QHBoxLayout *setTiragesLayout(void);

public slots:
    void toPrevious(void);
    void toNext(void);
    void slot_EndResultat(QObject*);

private:
    int my_tCol;
    int tid_cur;
    int tid_start;
    int cid_start;
    QSqlQueryModel *my_model;
    QString useTable;
    QLabel * dNext;
    QLabel * dCurr;
    QLabel * dPrev;

};

#endif // SHOWSTEPPER_H
