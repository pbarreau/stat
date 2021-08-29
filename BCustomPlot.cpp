#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QFrame>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>

#include "BCustomPlot.h"
#include "game.h"
#include "bstflt.h"

//#include "BView.h"
#include "BFpmFdj.h"

BCustomPlot::BCustomPlot(const stGameConf *pGame, BView *lesTirages, int zn, int my_start, int my_end):tirages(lesTirages)
{
 ptr_self = nullptr;
 QString cnx=pGame->db_ref->cnx;
 QString tbLabAna = "B_ana_z"+QString::number(zn+1);;

 // Etablir connexion a la base
 db_1 = QSqlDatabase::database(cnx);
 if(db_1.isValid()==false){
  QString str_error = db_1.lastError().text();
  QMessageBox::critical(nullptr, cnx, str_error,QMessageBox::Yes);
  return;
 }


 bool b_retVal = true;
 QString msg = "select * from " + tbLabAna;
 QSqlQuery query(db_1);

 b_retVal = query.exec(msg);
 QString str_error = db_1.lastError().text();
 if(b_retVal && query.first()){
  ptr_self = this;
  int total= 0;
  if(query.last()){
   total=query.at()+1;
   query.first();
  }

  QStringList keys = pGame->slFlt[zn][Bp::colDefTitres];
  QStringList tips = pGame->slFlt[zn][Bp::colDefToolTips];
  int tot_keys = keys.size();
  QVector<double> x(total+2), y(total+2);

  plotLayout()->clear();

  setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                    QCP::iSelectLegend | QCP::iSelectPlottables);

  /// Parcourir chaque resultat pour dessiner les courbes.
  //QCPLayoutGrid *subLayout = new QCPLayoutGrid;
  QCPMarginGroup *marginGroup = new QCPMarginGroup(this);

  bool haveNext = true;

  if(my_start<1) my_start =1;
  if(my_end> tot_keys) my_end = tot_keys -1;
  int start_loop=my_start;//tot_keys-2;
  //int stop_loop = tot_keys;

  for(int j=my_start;j<=my_end;j++){
   query.first();

   for(int i=1;(i<=total) && haveNext ;i++){
    x[i]=i;
    y[i]=query.value(j).toDouble();
    haveNext = query.next();
   }

   QCPAxisRect *wideAxisRect = new QCPAxisRect(this);

   wideAxisRect->setupFullAxesBox(false);
   //wideAxisRect->setRangeDrag(Qt::Horizontal);
   wideAxisRect->setMarginGroup(QCP::msLeft | QCP::msRight, marginGroup);

   wideAxisRect->axis(QCPAxis::atRight, 0)->setTickLabels(true);
   wideAxisRect->axis(QCPAxis::atLeft)->setLabel(keys[j-1]);
   if(j<tot_keys-1){
    wideAxisRect->axis(QCPAxis::atLeft)->setRange(0,pGame->limites[zn].win);
   }

   //wideAxisRect->axis(QCPAxis::atBottom)->setLabel("Tirage id");
   wideAxisRect->axis(QCPAxis::atBottom)->setRange(1,total);
   wideAxisRect->axis(QCPAxis::atBottom, 0)->setTickLabels(true);

   //wideAxisRect->axis(QCPAxis::atBottom, 0)->setRangeLower(1);
   //wideAxisRect->axis(QCPAxis::atBottom)->setRange(1,total);
   //wideAxisRect->axis(tmp_plot->xAxis)->setRange(1,total);
   //my_axys->setLabel("Tirage id");
   //my_axys->setRange(1,total);
   //wideAxisRect->setupFullAxesBox(true);

   //wideAxisRect->axis(QCPAxis::atBottom)->setRangeLower(1);
   //wideAxisRect->axis(QCPAxis::atBottom)->setOffset(1);

   plotLayout()->addElement(j-start_loop, 0, wideAxisRect);

   QCPGraph *tmp = addGraph(wideAxisRect->axis(QCPAxis::atBottom), wideAxisRect->axis(QCPAxis::atLeft));
   QString graph_name = tips[j-1];
   graph(j-start_loop)->setName(graph_name);
   graph(j-start_loop)->setData(x,y);
   graph(j-start_loop)->setLineStyle(QCPGraph::lsLine);
   graph(j-start_loop)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
   tmp->rescaleKeyAxis();

   // connect slot that shows a message in the status bar when a graph is clicked:
   connect(this, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(BSLOT_graphClicked(QCPAbstractPlottable*,int, QMouseEvent*)));

  }

  //BCP_Tst_04(this);

 }



#if 0
 QWidget *centralWidget= nullptr;
 QVBoxLayout *verticalLayout_3= nullptr;
 QFrame *frame_2= nullptr;
 QCustomPlot *customPlot = nullptr;

 centralWidget = new QWidget(this);
 setGeometry(400, 250, 542, 390);

 // Tableau des graphique a effectuer
 void(BCustomPlot::*ptrFunc[])(QCustomPlot *customPlot)=
 {
   &BCustomPlot::BCP_Tst_01,
   &BCustomPlot::BCP_Tst_02,
   &BCustomPlot::BCP_Tst_03
};

 (this->*ptrFunc[fn])(this);
 replot();
#endif
}

void   BCustomPlot::BSLOT_graphClicked(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent* event)
{
 if (dataIndex>1)
 {
  if(event->button() == Qt::LeftButton){
   double dataValue = plottable->interface1D()->dataMainValue(dataIndex);
   QString titre = plottable->name();
   titre = plottable->valueAxis()->label();
   QString message = QString("'%1'\nTirage n° %2\nTotal %3").arg(plottable->name()).arg(dataIndex-1).arg(dataValue);
   QToolTip::showText (QCursor::pos(), message);
   titre = "";

   BView * ptr_qtv = tirages;
   BFpmFdj * fpm_tmp = qobject_cast<BFpmFdj *>( ptr_qtv->model());
   fpm_tmp->sort(0);
   ptr_qtv->scrollTo(fpm_tmp->index(dataIndex-2,0));
   ptr_qtv->clicked(fpm_tmp->index(dataIndex-2,0)); /// pour mettre a jour onglet grp

   QAbstractItemView::SelectionBehavior prevBehav = ptr_qtv->selectionBehavior();
   ptr_qtv->setStyleSheet("QTableView {selection-background-color: red;}");
   ptr_qtv->setEditTriggers(QAbstractItemView::NoEditTriggers);
   ptr_qtv->setSelectionMode(QAbstractItemView::SingleSelection);
   ptr_qtv->setSelectionBehavior(QAbstractItemView::SelectRows);
   ptr_qtv->selectRow(dataIndex-2);
   ptr_qtv->setSelectionBehavior(prevBehav);
  }
 }

}

void BCustomPlot::BCP_Tst_04(QCustomPlot *customPlot)
{
 // create graph and assign data to it:
 customPlot->addGraph();
 //customPlot->graph(0)->setData(x, y);

}

void BCustomPlot::BCP_Tst_01(QCustomPlot *customPlot)
{
 // generate some data:
 QVector<double> x(101), y(101); // initialize with entries 0..100
 for (int i=0; i<101; ++i)
 {
  x[i] = i/50.0 - 1; // x goes from -1 to 1
  y[i] = x[i]*x[i]; // let's plot a quadratic function
 }
 // create graph and assign data to it:
 customPlot->addGraph();
 customPlot->graph(0)->setData(x, y);
 // give the axes some labels:
 customPlot->xAxis->setLabel("x");
 customPlot->xAxis->setVisible(true);
 customPlot->yAxis->setLabel("y");
 customPlot->yAxis->setVisible(true);
 // set axes ranges, so we see all data:
 customPlot->xAxis->setRange(-1, 1);
 customPlot->yAxis->setRange(0, 1);
 customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}

void BCustomPlot::BCP_Tst_02(QCustomPlot *customPlot)
{
 customPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // period as decimal separator and comma as thousand separator
 customPlot->legend->setVisible(true);
 QFont legendFont = font();  // start out with MainWindow's font..
 legendFont.setPointSize(9); // and make a bit smaller for legend
 customPlot->legend->setFont(legendFont);
 customPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));
 // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
 customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);

 // setup for graph 0: key axis left, value axis bottom
 // will contain left maxwell-like function
 customPlot->addGraph(customPlot->yAxis, customPlot->xAxis);
 customPlot->graph(0)->setPen(QPen(QColor(255, 100, 0)));
 customPlot->graph(0)->setBrush(QBrush(QPixmap("./balboa.jpg"))); // fill with texture of specified image
 customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
 customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
 customPlot->graph(0)->setName("Left maxwell function");

 // setup for graph 1: key axis bottom, value axis left (those are the default axes)
 // will contain bottom maxwell-like function with error bars
 customPlot->addGraph();
 customPlot->graph(1)->setPen(QPen(Qt::red));
 customPlot->graph(1)->setBrush(QBrush(QPixmap("./balboa.jpg"))); // same fill as we used for graph 0
 customPlot->graph(1)->setLineStyle(QCPGraph::lsStepCenter);
 customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::red, Qt::white, 7));
 customPlot->graph(1)->setName("Bottom maxwell function");
 QCPErrorBars *errorBars = new QCPErrorBars(customPlot->xAxis, customPlot->yAxis);
 errorBars->removeFromLegend();
 errorBars->setDataPlottable(customPlot->graph(1));

 // setup for graph 2: key axis top, value axis right
 // will contain high frequency sine with low frequency beating:
 customPlot->addGraph(customPlot->xAxis2, customPlot->yAxis2);
 customPlot->graph(2)->setPen(QPen(Qt::blue));
 customPlot->graph(2)->setName("High frequency sine");

 // setup for graph 3: same axes as graph 2
 // will contain low frequency beating envelope of graph 2
 customPlot->addGraph(customPlot->xAxis2, customPlot->yAxis2);
 QPen blueDotPen;
 blueDotPen.setColor(QColor(30, 40, 255, 150));
 blueDotPen.setStyle(Qt::DotLine);
 blueDotPen.setWidthF(4);
 customPlot->graph(3)->setPen(blueDotPen);
 customPlot->graph(3)->setName("Sine envelope");

 // setup for graph 4: key axis right, value axis top
 // will contain parabolically distributed data points with some random perturbance
 customPlot->addGraph(customPlot->yAxis2, customPlot->xAxis2);
 customPlot->graph(4)->setPen(QColor(50, 50, 50, 255));
 customPlot->graph(4)->setLineStyle(QCPGraph::lsNone);
 customPlot->graph(4)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
 customPlot->graph(4)->setName("Some random data around\na quadratic function");

 // generate data, just playing with numbers, not much to learn here:
 QVector<double> x0(25), y0(25);
 QVector<double> x1(15), y1(15), y1err(15);
 QVector<double> x2(250), y2(250);
 QVector<double> x3(250), y3(250);
 QVector<double> x4(250), y4(250);
 for (int i=0; i<25; ++i) // data for graph 0
 {
  x0[i] = 3*i/25.0;
  y0[i] = qExp(-x0[i]*x0[i]*0.8)*(x0[i]*x0[i]+x0[i]);
 }
 for (int i=0; i<15; ++i) // data for graph 1
 {
  x1[i] = 3*i/15.0;;
  y1[i] = qExp(-x1[i]*x1[i])*(x1[i]*x1[i])*2.6;
  y1err[i] = y1[i]*0.25;
 }
 for (int i=0; i<250; ++i) // data for graphs 2, 3 and 4
 {
  x2[i] = i/250.0*3*M_PI;
  x3[i] = x2[i];
  x4[i] = i/250.0*100-50;
  y2[i] = qSin(x2[i]*12)*qCos(x2[i])*10;
  y3[i] = qCos(x3[i])*10;
  y4[i] = 0.01*x4[i]*x4[i] + 1.5*(rand()/(double)RAND_MAX-0.5) + 1.5*M_PI;
 }

 // pass data points to graphs:
 customPlot->graph(0)->setData(x0, y0);
 customPlot->graph(1)->setData(x1, y1);
 errorBars->setData(y1err);
 customPlot->graph(2)->setData(x2, y2);
 customPlot->graph(3)->setData(x3, y3);
 customPlot->graph(4)->setData(x4, y4);
 // activate top and right axes, which are invisible by default:
 customPlot->xAxis2->setVisible(true);
 customPlot->yAxis2->setVisible(true);
 // set ranges appropriate to show data:
 customPlot->xAxis->setRange(0, 2.7);
 customPlot->yAxis->setRange(0, 2.6);
 customPlot->xAxis2->setRange(0, 3.0*M_PI);
 customPlot->yAxis2->setRange(-70, 35);
 // set pi ticks on top axis:
 customPlot->xAxis2->setTicker(QSharedPointer<QCPAxisTickerPi>(new QCPAxisTickerPi));
 // add title layout element:
 customPlot->plotLayout()->insertRow(0);
 customPlot->plotLayout()->addElement(0, 0, new QCPTextElement(customPlot, "Way too many graphs in one plot", QFont("sans", 12, QFont::Bold)));
 // set labels:
 customPlot->xAxis->setLabel("Bottom axis with outward ticks");
 customPlot->yAxis->setLabel("Left axis label");
 customPlot->xAxis2->setLabel("Top axis label");
 customPlot->yAxis2->setLabel("Right axis label");
 // make ticks on bottom axis go outward:
 customPlot->xAxis->setTickLength(0, 5);
 customPlot->xAxis->setSubTickLength(0, 3);
 // make ticks on right axis go inward and outward:
 customPlot->yAxis2->setTickLength(3, 3);
 customPlot->yAxis2->setSubTickLength(1, 1);
}

void BCustomPlot::BCP_Tst_03(QCustomPlot *customPlot)
{
 // add two new graphs and set their look:
 customPlot->addGraph();
 customPlot->graph(0)->setPen(QPen(Qt::blue)); // line color blue for first graph
 customPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // first graph will be filled with translucent blue
 customPlot->addGraph();
 customPlot->graph(1)->setPen(QPen(Qt::red)); // line color red for second graph
 // generate some points of data (y0 for first, y1 for second graph):
 QVector<double> x(251), y0(251), y1(251);
 for (int i=0; i<251; ++i)
 {
   x[i] = i;
   y0[i] = qExp(-i/150.0)*qCos(i/10.0); // exponentially decaying cosine
   y1[i] = qExp(-i/150.0);              // exponential envelope
 }
 // configure right and top axis to show ticks but no labels:
 // (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
 customPlot->xAxis2->setVisible(true);
 customPlot->xAxis2->setTickLabels(false);
 customPlot->yAxis2->setVisible(true);
 customPlot->yAxis2->setTickLabels(false);
 // make left and bottom axes always transfer their ranges to right and top axes:
 connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
 connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));
 // pass data points to graphs:
 customPlot->graph(0)->setData(x, y0);
 customPlot->graph(1)->setData(x, y1);
 // let the ranges scale themselves so graph 0 fits perfectly in the visible area:
 customPlot->graph(0)->rescaleAxes();
 // same thing for graph 1, but only enlarge ranges (in case graph 1 is smaller than graph 0):
 customPlot->graph(1)->rescaleAxes(true);
 // Note: we could have also just called customPlot->rescaleAxes(); instead
 // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
 customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}
