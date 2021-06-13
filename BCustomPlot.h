#ifndef BCUSTOMPLOT_H
#define BCUSTOMPLOT_H

#include <QMainWindow>
#include <QFrame>

#include "customPlot/qcustomplot.h"

class BCustomPlot : public QCustomPlot
{
 public:
  BCustomPlot(int fn);

 private:
  void BCP_Tst_01(QCustomPlot *customPlot);
  void BCP_Tst_02(QCustomPlot *customPlot);
  void BCP_Tst_03(QCustomPlot *customPlot);
};

#endif // BCUSTOMPLOT_H
