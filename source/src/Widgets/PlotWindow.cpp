#include "PlotWindow.h"
#include "ui_plotWindow.h"

PlotWindow::PlotWindow(IODeviceData *data, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::PlotWindow),
      configDialog(new PlotConfigDialog(data, this)),
      ioDeviceData(data),
      columuCount(2)
{
    ui->setupUi(this);

    connect(ioDeviceData, &IODeviceData::regExpNewData,
            this, &PlotWindow::dataAppend);
    connect(configDialog, &PlotConfigDialog::acceptClicked,
            this, &PlotWindow::configDialogAccepted);
    configDialog->open();

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

    QGridLayout *layout = qobject_cast<QGridLayout *>(ui->centralwidget->layout());
    layout->addWidget(view, viewlist.count() / columuCount, viewlist.count() % columuCount);
    layout->setRowStretch(viewlist.count() / columuCount, 1);
    layout->setColumnStretch(viewlist.count() % columuCount, 1);

    viewlist << view;

    newLine(chartIndex, lineName);

    chart->createDefaultAxes();

    rubberBandSelect(rubberBand);

    return chartIndex;
}

int PlotWindow::newLine(int chart, QString name)
{
    LineSeries *line = new LineSeries();

    Q_ASSERT(chart < viewlist.count());

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
    rubberBand = band;

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

void PlotWindow::loadPlotConfig()
{
    clearAll();
    for (int chartIndex = 0; chartIndex < config.count(); chartIndex++)
    {
        newChart(ioDeviceData->getRegExpTitle(config[chartIndex].series),
                 ioDeviceData->getRegExpNames(config[chartIndex].series)[config[chartIndex].ysIndex[0]]);

        if (config[chartIndex].ysIndex.count() > 1)
        {
            for (int lineIndex = 1; lineIndex < config[chartIndex].ysIndex.count(); lineIndex++)
            {
                newLine(chartIndex,
                        ioDeviceData->getRegExpNames(config[chartIndex].series)[config[chartIndex].ysIndex[lineIndex]]);
            }
        }

        for (auto &&i : ioDeviceData->getRegExpValues(config[chartIndex].series))
        {
            for (int lineValueIndex = 0; lineValueIndex < config[chartIndex].ysIndex.count(); lineValueIndex++)
            {
                appendData(chartIndex,
                           lineValueIndex,
                           QPointF(i[config[chartIndex].xIndex],
                                   i[config[chartIndex].ysIndex[lineValueIndex]]));
            }
        }
    }
}

void PlotWindow::clearAll()
{
    QGridLayout *layout = qobject_cast<QGridLayout *>(ui->centralwidget->layout());
    int index = 0;
    for (auto &&i : viewlist)
    {
        Chart *m_chart = i->chart();
        m_chart->removeAllSeries();
        i->setParent(nullptr);
        delete i;

        layout->setRowStretch(index / columuCount, 0);
        layout->setColumnStretch(index % columuCount, 0);
        index++;
    }
    viewlist.clear();
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

void PlotWindow::on_configAction_triggered(bool checked)
{
    Q_UNUSED(checked);

    configDialog->open();
}

void PlotWindow::configDialogAccepted()
{
    QList<PlotConfig> m_config = configDialog->getConfig();
    if (config != m_config)
    {
        config = m_config;
        loadPlotConfig();
    }
}

void PlotWindow::dataAppend(int exp, QList<qreal> data)
{
    for (int chartIndex = 0; chartIndex < config.count(); chartIndex++)
    {
        if (config[chartIndex].series != exp)
        {
            continue;
        }

        for (int lineValueIndex = 0; lineValueIndex < config[chartIndex].ysIndex.count(); lineValueIndex++)
        {
            appendData(chartIndex,
                       lineValueIndex,
                       QPointF(data[config[chartIndex].xIndex],
                               data[config[chartIndex].ysIndex[lineValueIndex]]));
        }
    }
}
