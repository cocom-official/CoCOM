#include "PlotConfigDialog.h"
#include "ui_plotConfigDialog.h"

PlotConfigDialog::PlotConfigDialog(SerialData *data, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::PlotConfigDialog),
      serialData(data)
{
    ui->setupUi(this);
    setWindowModality(Qt::WindowModal);

    ui->tabWidget->clear();
    ui->tabWidget->addTab(new QLabel(), QString());
    ui->tabWidget->setTabText(0, "+");
    ui->tabWidget->setTabToolTip(0, tr("Click '+' to add a new tab"));
    addNewTab();
}

PlotConfigDialog::~PlotConfigDialog()
{
    delete ui;
}

QList<PlotConfig> PlotConfigDialog::getConfig()
{
    QList<PlotConfig> configs;
    for (auto &&i : tabsWidgetConfig)
    {
        PlotConfig config;

        if (i.indexes.count() < 2)
        {
            continue;
        }

        config.series = i.widget->seriesIndexToRegs(i.indexes[0]);
        config.xIndex = i.indexes[1];

        config.ysIndex.clear();
        for (int j = 2; j < i.indexes.count(); j++)
        {
            if (i.indexes[j] >= 0)
            {
                config.ysIndex << i.indexes[j];
            }
        }
        configs << config;
    }

    return configs;
}

void PlotConfigDialog::on_tabWidget_currentChanged(int index)
{
    if (index == ui->tabWidget->count() - 1)
    {
        if (tabBarClicked == true)
        {
            addNewTab();
            tabBarClicked = false;
        }
        else
        {
            ui->tabWidget->setCurrentIndex(index - 1);
        }
    }
}

void PlotConfigDialog::on_tabWidget_tabBarClicked(int index)
{
    if (index == ui->tabWidget->count() - 1)
    {
        tabBarClicked = true;
    }
}

void PlotConfigDialog::on_tabWidget_tabBarDoubleClicked(int index)
{
    if (ui->tabWidget->count() <= 2)
    {
        return;
    }

    ui->tabWidget->removeTab(index);
    tabsWidgetConfig.removeAt(index);

    for (int i = index; i < ui->tabWidget->count() - 1; i++)
    {
        ui->tabWidget->setTabText(i, tr("Plot") + " " + QString::number(i));
        ui->tabWidget->tabBar()->setTabToolTip(
            i, tr("Plot") + " " + QString::number(i) + ", " + tr("Double Click to Close"));
    }

    ui->tabWidget->setCurrentIndex(index - 1);
}

void PlotConfigDialog::on_buttonBox_accepted()
{
    for (auto &&i : tabsWidgetConfig)
    {
        i.indexes = i.widget->getResultIndexes();
    }
    emit acceptClicked();
}

void PlotConfigDialog::addNewTab()
{
    int tabCount = ui->tabWidget->count();
    PlotConfigTabWIdget *tab = new PlotConfigTabWIdget(this);
    ConfigTabWidget config;
    config.widget = tab;
    tabsWidgetConfig << config;
    tab->setDataSource(serialData);
    ui->tabWidget->insertTab(tabCount - 1, tab, tr("Plot") + " " + QString::number(tabCount - 1));
    ui->tabWidget->setCurrentIndex(tabCount - 1);

    ui->tabWidget->tabBar()->setTabToolTip(
        tabCount - 1,
        tr("Plot") + " " + QString::number(tabCount - 1) + ", " + tr("Double Click to Close"));
}

void PlotConfigDialog::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);

    int i = 0;

rerun:
    for (; i < tabsWidgetConfig.count(); i++)
    {
        if (tabsWidgetConfig.count() > 1 && !tabsWidgetConfig[i].widget->checkValid())
        {
            on_tabWidget_tabBarDoubleClicked(i);
            goto rerun;
        }

        tabsWidgetConfig[i].widget->updateDataSource();
        tabsWidgetConfig[i].widget->restoreResultIndexes(tabsWidgetConfig[i].indexes);
    }
}
