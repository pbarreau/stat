#ifndef SHOWSTEPPER_H
#define SHOWSTEPPER_H

#include <QObject>
#include <QSqlQueryModel>
#include <QLabel>
#include <QHBoxLayout>
#include <QSplitter>

#include "tirages.h"

class ShowStepper: public QObject
{
    Q_OBJECT
public:
    ~ShowStepper();
    ShowStepper(stTiragesDef *pdef);
    ShowStepper(int cid, int tid);

private:
    void ExecSql(int cid,int tid);
    void ExecSql_2(int cid,int tid);
    void setLabel(int tid);
    QHBoxLayout *setTiragesLayout(void);
    QHBoxLayout *setCheckBoxes(void);
    QSplitter *SetDataSplitter_1(int col, int cid, int tid);
    QSplitter *SetDataSplitter_2(int col, int cid, int tid);
#ifndef USE_SG_CODE
    void MemoriserProgression(QString table, stMyHeadedList *h, stMyLinkedList *l, int start, int y, int cid, int tid);
    void MettreCouleur(int start, int cur);
    void PresenterResultat(int cid, int tid);
#endif
    // penser au destructeur pour chaque pointeur

public slots:
    void slot_BtnPrev(void);
    void slot_BtnNext(void);
    void slot_EndResultat(QObject*);
    void slot_chkLess(int);
    void slot_chkThis(int);
    void slot_chkAdd(int);
#ifndef USE_SG_CODE
    void slot_MaFonctionDeCalcul(const QModelIndex &my_index, int cid);
#endif


private:
    int my_tCol;
    int tid_cur;
    int tid_start;
    int cid_start;
    stTiragesDef *pGlobConf;
    QSqlQueryModel *my_model;
    QSqlQueryModel *my_model_2;
    QString useTable;
    QLabel * dNext;
    QLabel * dCurr;
    QLabel * dPrev;

};

#endif // SHOWSTEPPER_H
