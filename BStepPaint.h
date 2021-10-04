#ifndef BSTEPPAINT_H
#define BSTEPPAINT_H

#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>

#include <QPainter>
#include <QRect>

#include "BView.h"
#include "bstflt.h"

#include "game.h"

class BStepPaint : public QStyledItemDelegate
{
  Q_OBJECT

 public:
  BStepPaint(const stGameConf *pGame, int zone, Bp::ETbvId tbvId, int *nxt, int *cur, int *prev);
  virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,
                     const QModelIndex &index) const;

 private:
  void paintDraw(QPainter *painter, const QStyleOptionViewItem &myOpt) const;
  void paintWrite(QPainter *painter, const QStyleOptionViewItem &myOpt) const;
  void cellWrite(QPainter *painter, QStyle::State state, const QRect curCell, const QString myTxt, Qt::GlobalColor inPen=Qt::black,  bool up=false) const;

 public slots:
  void BSlot_FindBall(BView *tbvTarget, int id);

 private:
  const stGameConf *pGDef;
  int zn;
  int lenTab;
  int dig_ball;
  Bp::ETbvId eTbv;
  int *nxtTir;
  int *curTir;
  int *prvTir;
};

#endif // BSTEPPAINT_H
