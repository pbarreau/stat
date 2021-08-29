#ifndef BCUSTOMPLOT_H
#define BCUSTOMPLOT_H

#include <QMainWindow>
#include <QFrame>
#include <QSqlDatabase>

#include "BView.h"
#include "game.h"
#include "customPlot/qcustomplot.h"

class BCustomPlot : public QCustomPlot
{
  Q_OBJECT

 public:
  BCustomPlot(const stGameConf *pGame, BView *lesTirages, int zn, int my_start=1, int my_end=2);

 private:
  void BCP_Tst_01(QCustomPlot *customPlot);
  void BCP_Tst_02(QCustomPlot *customPlot);
  void BCP_Tst_03(QCustomPlot *customPlot);
  void BCP_Tst_04(QCustomPlot *customPlot);

 private slots:
  void BSLOT_graphClicked(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent*event);

 private:
  BCustomPlot* ptr_self;
  QSqlDatabase db_1;
  BView *tirages;
};

#endif // BCUSTOMPLOT_H
