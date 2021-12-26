#ifndef BTHREAD_1_H
#define BTHREAD_1_H

#include <QThread>

#include <QSqlDatabase>

#include "BcUpl.h"

typedef struct _tsk_1{
  const stGameConf *pGame;
  const QModelIndexList *my_indexes;
  eUpl_Ens e_id;
  int z_id;
  int obj_upl;
}stTsk1;


class BThread_1 : public QThread
{

 public:
  BThread_1(stTsk1 *def);


 private:
  void run() override;
  void creationTable();
  bool CreateTable(QString tbl_id);

 private:
  stTsk1 *tsk_1;
  QString cnx;  // nom de la connection a la bdd pour ce process
  QSqlDatabase db_tsk1;
};

#endif // BTHREAD_1_H
