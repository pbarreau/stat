#ifndef SHOWSTEPPER_H
#define SHOWSTEPPER_H

#include <QObject>
#include <QSqlQueryModel>
#include <QLabel>
#include <QHBoxLayout>
#include <QSplitter>

class ShowStepper: public QObject
{
    Q_OBJECT
public:
    ~ShowStepper();
    ShowStepper(int cid, int tid);

private:
    void ExecSql(int cid,int tid);
    void ExecSql_2(int cid,int tid);
    void setLabel(int tid);
    QHBoxLayout *setTiragesLayout(void);
    QHBoxLayout *setCheckBoxes(void);
    QSplitter *SetDataSplitter_1(int col, int cid, int tid);
    QSplitter *SetDataSplitter_2(int col, int cid, int tid);

public slots:
    void toPrevious(void);
    void toNext(void);
    void slot_EndResultat(QObject*);
    void slot_chkLess(int);
    void slot_chkThis(int);
    void slot_chkAdd(int);

private:
    int my_tCol;
    int tid_cur;
    int tid_start;
    int cid_start;
    QSqlQueryModel *my_model;
    QSqlQueryModel *my_model_2;
    QString useTable;
    QLabel * dNext;
    QLabel * dCurr;
    QLabel * dPrev;

};

#endif // SHOWSTEPPER_H
