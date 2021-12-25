#ifndef BACTION_1_H
#define BACTION_1_H

#include <QObject>
#include <QAction>
//#include <QTableView>
#include <QPoint>

#include "BView.h"

class BAction_1:public QAction
{
  Q_OBJECT

 public:
  BAction_1(const QString &label, const BView *view, QPoint pos);

 public slots:
  void BSlot_onTriggered(void);

 signals:
  void BSig_ActionAt(const QModelIndex &index);

 private:
  const BView *tbv;
  const QPoint point;
};

#endif // BACTION_1_H
