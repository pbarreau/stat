#ifndef SHOWSTEPPER_H
#define SHOWSTEPPER_H

#include <QObject>
#include <QSqlQueryModel>
#include <QLabel>
#include <QHBoxLayout>
#include <QSplitter>
#include <QCheckBox>

#include "tirages.h"

typedef struct _stStepperNeeds
{
    int *nbElmZone;
    QString *nomZone;
    stBornes *limites;
    //QString *FullNameZone;
    //unsigned char nb_zone;
}stStepperNeeds;

class ShowStepper: public QObject
{
    Q_OBJECT
public:
    ~ShowStepper();
    ShowStepper(stStepperNeeds *pdef);
    //ShowStepper(stTiragesDef *pdef);
    RunStepper(int cid, int tid);

private:
    void ExecSql(int cid,int tid);
    void ExecSql_2(int cid,int tid);
    void ExecSql_3(int cid,int tid);
    void setLabel(int tid);
    QHBoxLayout *setTiragesLayout(void);
    QHBoxLayout *setCheckBoxes(void);
    QSplitter *SetDataSplitter_1(int col, int cid, int tid);
    QSplitter *SetDataSplitter_2(int col, int cid, int tid);
    QSplitter *SetDataSplitter_3(int col, int cid, int tid);
    void SetBgColorCell(int tbl, int cid, int tid, int bid);
#ifndef USE_CODE_IN_SG
    void MemoriserProgression(QString table, stMyHeadedList *h, stMyLinkedList *l, int start, int y, int cid, int tid);
    void MettreCouleur(int start, int cur);
    void PresenterResultat(int cid, int tid);
#endif



public slots:
    void slot_BtnPrev(void);
    void slot_BtnNext(void);
    void slot_EndResultat(QObject*);
    void slot_chkLess(int);
    void slot_chkThis(int);
    void slot_chkAdd(int);
	void slot_MontrerBoule(QModelIndex index);
#ifndef USE_SG_CODE
    void slot_MaFonctionDeCalcul(const QModelIndex &my_index);
#endif


private:
    int my_tCol;
    int tid_cur;
    int tid_start;
    int cid_start;
    //stTiragesDef *pGlobConf;
    stStepperNeeds *pGlobConf;
    QSqlQueryModel *my_model;
    QSqlQueryModel *my_model_2;
    QSqlQueryModel *my_model_3;
    QString useTable;
    QLabel * dNext;
    QLabel * dCurr;
    QLabel * dPrev;
    QCheckBox *checkbox_1;
    QCheckBox *checkbox_2;
    QCheckBox *checkbox_3;


};

#endif // SHOWSTEPPER_H
