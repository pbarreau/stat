#ifndef BSTEPPER_H
#define BSTEPPER_H

#include <QWidget>

#include <QHBoxLayout>

#include <QSqlDatabase>
#include <QLinkedList>
#include <QList>
#include <QMap>

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

class BStepper : public QWidget
{
  Q_OBJECT

 public:
  BStepper(const stGameConf *pGame);

 private :
  QWidget *Ihm(const stGameConf *pGame, int start_tir, stTabSteps defSteps);
  QWidget *Ihm_left(const stGameConf *pGame, int id_tir);
  QWidget *Ihm_right(const stGameConf *pGame, stTabSteps defSteps);

  QHBoxLayout *GetBtnSteps(void);

  QString getSqlMsg(const stGameConf *pGame, int zn, int id_tir);
  stTabSteps Kernel(const stGameConf *pGame, int id_tir);
  void TableauRecopier(int l_id);
  void TableauActualiser(int l_id, QSqlQuery query);

 private slots:
  void BSLOT_MoveUp(void);
  void BSlot_ActionButton(int btn_id);


 private:
  QSqlDatabase db_tirages;
  const stGameConf *pGDef;
  bool *isKnown;
  int ballCounter;
  int origin; /// Id tirage de depart
  QList<QList <QStringList *>*> tir_id;
};

#endif // BSTEPPER_H
