#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QWidget>
#include <QStandardItemModel>

#include <KDChart/KDChartChart>
#include <KDChart/KDChartPolarDiagram>
#include <KDChart/KDChartDataValueAttributes>
#include <KDChart/KDChartBackgroundAttributes>


using namespace KDChart;

class ChartWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ChartWidget(QWidget *parent = 0);

signals:

public slots:

private:
    Chart m_chart;
    QStandardItemModel m_model;


};

#endif // CHARTWIDGET_H
