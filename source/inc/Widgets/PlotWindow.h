#pragma once

#include <QTimer>
#include <QMainWindow>

#include "Chart.h"
#include "ChartView.h"
#include "LineSeries.h"
#include "MediaData.h"
#include "PlotConfigDialog.h"

namespace Ui
{
    class PlotWindow;
}

class PlotWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlotWindow(MediaData *data, QWidget *parent = nullptr);
    ~PlotWindow();

public slots:
    int newChart(QString title, QString lineName);
    int newLine(int chart, QString name);
    void appendData(int chart, int line, QPointF data);

private:
    Ui::PlotWindow *ui;
    PlotConfigDialog *configDialog;
    QList<ChartView *> viewlist;
    MediaData *mediaData;
    QList<PlotConfig> config;
    int columuCount;
    QChartView::RubberBand rubberBand;

    void rubberBandSelect(QChartView::RubberBand band);
    QChartView::RubberBand getRubberBand();
    void loadPlotConfig();
    void clearAll();

private slots:
    void on_rectangleRubberAction_toggled(bool checked);
    void on_horizontalRubberAction_toggled(bool checked);
    void on_verticalRubberAction_toggled(bool checked);
    void on_zoomResetAction_triggered(bool checked);
    void on_configAction_triggered(bool checked);

    void configDialogAccepted();
    void dataAppend(int exp, QList<qreal> data);
};
