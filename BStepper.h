#ifndef BSTEPPER_H
#define BSTEPPER_H

#include <QWidget>

#include <QHBoxLayout>

#include <QSqlDatabase>
#include <QSqlQueryModel>

#include <QLinkedList>
#include <QList>
#include <QMap>

#include <QStyledItemDelegate>

#include "BView.h"
#include "Bc.h"

#include "game.h"

#if 0
typedef struct _myStepNumber
{
  int x;
  int y;
  bool isPrevious;
  bool isCurrent;
  bool isNext;
}stStepData;

typedef struct _stStepList
{
  QMap<int,stStepData *> *key;
  QLinkedList <stStepData *> *data;
}stStepList;
#endif

typedef struct _stTabSteps
{
  int maxSteps;
  int maxItems;
}stTabSteps;

//typedef enum _enumTbv{none,left,right}ETbvId;

/// -------------------------------------
/// Class helper 1
#if 0
class BTrackStepper:public QSqlQueryModel
{
  Q_OBJECT

 public: ///
  explicit BTrackStepper(int *val, int pos, QObject *parent = nullptr);
  void setPtr(int pos);
  QVariant data(const QModelIndex &index, int role) const;

 private:
  int *ptrVal;
  int ptrPos;
};
#endif
/// -------------------------------------
/// Class helper 2

/// -------------------------------------

class BStepper : public QWidget
{
  Q_OBJECT

 public:
  BStepper(const stGameConf *pGame);

 public slots:
  void BSlot_FindBall(BView *tbvTarget, int id);

 private :
  QWidget *Ihm(const stGameConf *pGame, int start_tir, stTabSteps defSteps);
  QWidget *Ihm_left(const stGameConf *pGame, int id_tir);
  QWidget *Ihm_right(const stGameConf *pGame, stTabSteps defSteps);

  QHBoxLayout *GetBtnSteps(void);

  QString getSqlMsg(const stGameConf *pGame, int zn, int id_tir);
  stTabSteps Kernel(const stGameConf *pGame, int id_tir);
  void TableauRecopier(int l_id);
  void TableauActualiser(int l_id, QSqlQuery query);

  void FillTbViews(int id_tir, int id_bal);
  void Fill_Left(int id_tir, int id_bal);
  void Fill_Right(int id_tir, int id_bal);
  void RazTbvR(void);

  QString GetLeftTitle(const stGameConf *pGame, int zn, int id_tir);

 private slots:
  void BSlot_ActionButton(int btn_id);
  void BSlotTirId(void);
  void BSlot_ShowBall(const QModelIndex &index);

 public:
 Q_SIGNALS:
  void BSig_FindBall(BView *tbvTarget, int id);

 private:
  QSqlDatabase db_tirages;
  const stGameConf *pGDef;
  bool *isKnown;
  int ballCounter;
  int *curTirVal;
  int *prvTirVal;
  stTabSteps defMax;
  int origin; /// Id tirage de depart
  int ptrCurTir;
  QSlider *showPos;
  QList<QList <QStringList *>*> tir_id;
  BView *ptrTbvL;
  BView *ptrTbvR;
};

#endif // BSTEPPER_H
