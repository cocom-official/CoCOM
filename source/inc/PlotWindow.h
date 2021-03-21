#pragma once

#include <QTimer>
#include <QMainWindow>

#include "Chart.h"
#include "ChartView.h"
#include "LineSeries.h"

namespace Ui
{
    class PlotWindow;
}

class PlotWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlotWindow(QWidget *parent = nullptr);
    ~PlotWindow();

public slots:
    int newChart(QString title, QString lineName);
    int newLine(int chart, QString name);
    void appendData(int chart, int line, QPointF data);

private:
    Ui::PlotWindow *ui;
    QList<ChartView *> viewlist;
    QList<LineSeries *> linelist;

    QTimer timer;

    void rubberBandSelect(QChartView::RubberBand band);
    QChartView::RubberBand getRubberBand();

private slots:
    void on_rectangleRubberAction_toggled(bool checked);
    void on_horizontalRubberAction_toggled(bool checked);
    void on_verticalRubberAction_toggled(bool checked);
    void on_zoomResetAction_triggered(bool checked);
};
