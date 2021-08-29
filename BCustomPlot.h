#ifndef BCUSTOMPLOT_H
#define BCUSTOMPLOT_H

#include <QMainWindow>
#include <QFrame>
#include <QSqlDatabase>

//#include "BView.h"
#include "BTirages.h"
#include "game.h"
#include "customPlot/qcustomplot.h"

class BCustomPlot : public QCustomPlot
{
  Q_OBJECT

 public:
  BCustomPlot(const stGameConf *pGame, BTirages *lesTirages, int zn, QString fn_key, QString fn_tips);

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
  BTirages *tirages;
};

#endif // BCUSTOMPLOT_H
