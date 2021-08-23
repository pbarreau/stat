#ifndef BSTEPPER_H
#define BSTEPPER_H

#include <QWidget>

#include <QSqlDatabase>
#include <QLinkedList>
#include <QList>
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

 private :
  QWidget *Ihm(const stGameConf *pGame);
  QWidget *Ihm_left(const stGameConf *pGame, int id_tir);
  QWidget *Ihm_right(const stGameConf *pGame, int id_tir);

  QString getSqlMsg(const stGameConf *pGame, int zn, int id_tir);
  void Kernel(const stGameConf *pGame, int id_tir);
  void TableauRecopier(int l_id);
  void TableauActualiser(int l_id, QSqlQuery query);

 private slots:
  void BSLOT_MoveUp(void);

 private:
  QSqlDatabase db_tirages;
  const stGameConf *pGDef;
  bool *isKnown;
  int *posY;
  int ballCounter;
  int origin; /// Id tirage de depart
  QList<QList <QStringList *>*> tir_id;
};

#endif // BSTEPPER_H
