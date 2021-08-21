#ifndef BSTEPPER_H
#define BSTEPPER_H

#include <QWidget>

#include <QSqlDatabase>
#include <QLinkedList>
#include <QMap>

#include "game.h"

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

class BStepper : public QWidget
{
  Q_OBJECT

 public:
  BStepper(const stGameConf *pGame);

 private slots:
  void BSLOT_MoveUp(void);

 private:
  QSqlDatabase db_tirages;
  const stGameConf *pGDef;
  bool *isKnown;
  int *posY;
  int ballCounter;
  int origin; /// Id tirage de depart
  QLinkedList<stStepList *> tirages;
};

#endif // BSTEPPER_H
