#include "PlotWindow.h"
#include "ui_plotWindow.h"

PlotWindow::PlotWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::PlotWindow)
{
    ui->setupUi(this);

    // newChart("", "");
    // appendData(0, 0, QPointF(1, 2));
    // appendData(0, 0, QPointF(2, 2));
    // appendData(0, 0, QPointF(3, 8));
    // appendData(0, 0, QPointF(4, 6));
    // appendData(0, 0, QPointF(5, 10));
    // appendData(0, 0, QPointF(6, 12));
    // appendData(0, 0, QPointF(7, 12));
    // appendData(0, 0, QPointF(8, 12));
    // appendData(0, 0, QPointF(9, 12));
    // appendData(0, 0, QPointF(10, 12));
    // appendData(0, 0, QPointF(11, 12));
    // appendData(0, 0, QPointF(12, 12));

    // newLine(0, "Line1");
    // appendData(0, 1, QPointF(1, 3));
    // appendData(0, 1, QPointF(2, 4));
    // appendData(0, 1, QPointF(3, 7));
    // appendData(0, 1, QPointF(4, 3));
    // appendData(0, 1, QPointF(5, 8));
    // appendData(0, 1, QPointF(6, 7));

    // connect(&timer, &QTimer::timeout, [this]() {
    //     static int x = 7;
    //     static int y = 8;
    //     appendData(0, 1, QPointF(x++, y++));
    // });
    // timer.setInterval(1000);
    // timer.start();

    // newChart("", "");
    // newChart("Chart3", "Line");
    // newChart("Chart4", "Line");

    // appendData(1, 0, QPointF(1, 3));
    // appendData(1, 0, QPointF(2, 4));
    // appendData(1, 0, QPointF(3, 7));
    // appendData(1, 0, QPointF(4, 3));
    // appendData(1, 0, QPointF(5, 8));
    // appendData(1, 0, QPointF(6, 7));

    // appendData(2, 0, QPointF(1, 3));
    // appendData(2, 0, QPointF(2, 4));
    // appendData(2, 0, QPointF(3, 7));
    // appendData(2, 0, QPointF(4, 3));
    // appendData(2, 0, QPointF(5, 8));
    // appendData(2, 0, QPointF(6, 7));

    // appendData(3, 0, QPointF(1, 3));
    // appendData(3, 0, QPointF(2, 4));
    // appendData(3, 0, QPointF(3, 7));
    // appendData(3, 0, QPointF(4, 3));
    // appendData(3, 0, QPointF(5, 8));
    // appendData(3, 0, QPointF(6, 7));
    // newLine(3, "Line1");
    // appendData(3, 1, QPointF(1, 6));
    // appendData(3, 1, QPointF(2, 6));
    // appendData(3, 1, QPointF(3, 7));
    // appendData(3, 1, QPointF(4, 32));
    // appendData(3, 1, QPointF(5, 7));
    // appendData(3, 1, QPointF(6, 9));

    // newChart("Chart5", "Line");

    rubberBandSelect(QChartView::RectangleRubberBand);
}

PlotWindow::~PlotWindow()
{
    delete ui;
}

int PlotWindow::newChart(QString title, QString lineName)
{
    int chartIndex = viewlist.count();

    Chart *chart = new Chart();
    if (!title.isEmpty())
    {
        chart->setTitle(title);
    }

    ChartView *view = new ChartView(chart, this);
    view->setRubberBand(getRubberBand());
    qobject_cast<QGridLayout *>(ui->centralwidget->layout())->addWidget(view, viewlist.count() / 2, viewlist.count() % 2);
    viewlist << view;

    newLine(chartIndex, lineName);

    chart->createDefaultAxes();

    if (viewlist.count() == 2)
    {
        resize(size().width() * 2, size().height());
    }
    else if (viewlist.count() == 3)
    {
        resize(size().width(), size().height() * 2);
    }

    return chartIndex;
}

int PlotWindow::newLine(int chart, QString name)
{
    LineSeries *line = new LineSeries();

    if (name.isEmpty())
    {
        name = QString("Line%1").arg(viewlist[chart]->chart()->seriesCount());
    }
    line->setName(name);

    viewlist[chart]->chart()->addSeries(line);

    viewlist[chart]->chart()->createDefaultAxes();

    return viewlist[chart]->chart()->seriesCount() - 1;
}

void PlotWindow::appendData(int chart, int line, QPointF data)
{
    viewlist[chart]->chart()->seriesAppend(line, data);
}

void PlotWindow::rubberBandSelect(QChartView::RubberBand band)
{
    for (auto &&i : viewlist)
    {
        i->setRubberBand(band);
    }

    ui->rectangleRubberAction->blockSignals(true);
    ui->horizontalRubberAction->blockSignals(true);
    ui->verticalRubberAction->blockSignals(true);

    switch (band)
    {
    case QChartView::RectangleRubberBand:
        ui->rectangleRubberAction->setChecked(true);
        ui->horizontalRubberAction->setChecked(false);
        ui->verticalRubberAction->setChecked(false);
        break;

    case QChartView::VerticalRubberBand:
        ui->verticalRubberAction->setChecked(true);
        ui->rectangleRubberAction->setChecked(false);
        ui->horizontalRubberAction->setChecked(false);
        break;

    case QChartView::HorizontalRubberBand:
        ui->horizontalRubberAction->setChecked(true);
        ui->rectangleRubberAction->setChecked(false);
        ui->verticalRubberAction->setChecked(false);
        break;

    default:
        break;
    }

    ui->rectangleRubberAction->blockSignals(false);
    ui->horizontalRubberAction->blockSignals(false);
    ui->verticalRubberAction->blockSignals(false);
}

QChartView::RubberBand PlotWindow::getRubberBand()
{
    if (ui->horizontalRubberAction->isChecked())
    {
        return QChartView::RectangleRubberBand;
    }
    else if (ui->verticalRubberAction->isChecked())
    {
        return QChartView::VerticalRubberBand;
    }
    else if (ui->horizontalRubberAction->isChecked())
    {
        return QChartView::HorizontalRubberBand;
    }
    else
    {
        return QChartView::NoRubberBand;
    }
}

void PlotWindow::on_rectangleRubberAction_toggled(bool checked)
{
    ui->rectangleRubberAction->blockSignals(true);
    if (checked)
    {
        rubberBandSelect(QChartView::RectangleRubberBand);
    }
    else
    {
        ui->rectangleRubberAction->setChecked(true);
    }
    ui->rectangleRubberAction->blockSignals(false);
}

void PlotWindow::on_horizontalRubberAction_toggled(bool checked)
{
    ui->horizontalRubberAction->blockSignals(true);
    if (checked)
    {
        rubberBandSelect(QChartView::HorizontalRubberBand);
    }
    else
    {
        ui->horizontalRubberAction->setChecked(true);
    }
    ui->horizontalRubberAction->blockSignals(false);
}

void PlotWindow::on_verticalRubberAction_toggled(bool checked)
{
    ui->verticalRubberAction->blockSignals(true);
    if (checked)
    {
        rubberBandSelect(QChartView::VerticalRubberBand);
    }
    else
    {
        ui->verticalRubberAction->setChecked(true);
    }
    ui->verticalRubberAction->blockSignals(false);
}

void PlotWindow::on_zoomResetAction_triggered(bool checked)
{
    Q_UNUSED(checked);

    for (auto &&i : viewlist)
    {
        i->chart()->zoomReset();
    }
}
