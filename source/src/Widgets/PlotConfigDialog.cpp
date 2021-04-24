#include "PlotConfigDialog.h"
#include "ui_plotConfigDialog.h"

PlotConfigDialog::PlotConfigDialog(MediaData *data, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::PlotConfigDialog),
      mediaData(data)
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
    tabWidgetMutex.lock();
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
    tabWidgetMutex.unlock();

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

    removeTab(index);
    tabWidgetMutex.lock();
    tabsWidgetConfig.removeAt(index);
    tabWidgetMutex.unlock();
}

void PlotConfigDialog::on_buttonBox_accepted()
{
    tabWidgetMutex.lock();
    for (auto &&i : tabsWidgetConfig)
    {
        i.indexes = i.widget->getResultIndexes();
    }
    tabWidgetMutex.unlock();
    emit acceptClicked();
}

void PlotConfigDialog::addNewTab()
{
    int tabCount = ui->tabWidget->count();
    PlotConfigTabWIdget *tab = new PlotConfigTabWIdget(this);
    ConfigTabWidget config;
    config.widget = tab;
    tab->setDataSource(mediaData);
    ui->tabWidget->insertTab(tabCount - 1, tab, tr("Plot") + " " + QString::number(tabCount - 1));
    ui->tabWidget->setCurrentIndex(tabCount - 1);

    ui->tabWidget->tabBar()->setTabToolTip(
        tabCount - 1,
        tr("Plot") + " " + QString::number(tabCount - 1) + ", " + tr("Double Click to Close"));

    tabWidgetMutex.lock();
    tabsWidgetConfig << config;
    tabWidgetMutex.unlock();
}

void PlotConfigDialog::removeTab(int index)
{
    if (ui->tabWidget->count() == index + 1)
    {
        return;
    }

    ui->tabWidget->removeTab(index);

    for (int i = index; i < ui->tabWidget->count() - 1; i++)
    {
        ui->tabWidget->setTabText(i, tr("Plot") + " " + QString::number(i));
        ui->tabWidget->tabBar()->setTabToolTip(
            i, tr("Plot") + " " + QString::number(i) + ", " + tr("Double Click to Close"));
    }

    ui->tabWidget->setCurrentIndex(index - 1);
}

void PlotConfigDialog::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    QList<int> removeList;

    tabWidgetMutex.lock();
    for (int i = 0; i < tabsWidgetConfig.count(); i++)
    {
        if (i > 0 && !tabsWidgetConfig[i].widget->checkValid())
        {
            removeTab(i - removeList.count());
            removeList << i;
        }

        tabsWidgetConfig[i].widget->updateDataSource();
        tabsWidgetConfig[i].widget->restoreResultIndexes(tabsWidgetConfig[i].indexes);
    }

    while (!removeList.isEmpty())
    {
        tabsWidgetConfig.removeAt(removeList.last());
        removeList.removeLast();
    }
    tabWidgetMutex.unlock();
}
