#pragma once

#include <QDialog>

#include "SerialData.h"
#include "PlotConfigTabWIdget.h"

namespace Ui
{
    class PlotConfigDialog;
}

typedef struct PlotConfig_s
{
    int series;
    int xIndex;
    QList<int> ysIndex;

    bool operator==(const PlotConfig_s b) const
    {
        return (this->series == b.series) && (this->xIndex == b.xIndex) && (this->ysIndex == b.ysIndex);
    }

    bool operator!=(const PlotConfig_s b) const
    {
        return (this->series != b.series) || (this->xIndex != b.xIndex) || (this->ysIndex != b.ysIndex);
    }
} PlotConfig;

class PlotConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlotConfigDialog(SerialData *data, QWidget *parent = nullptr);
    ~PlotConfigDialog();
    QList<PlotConfig> getConfig();

private slots:
    void on_tabWidget_currentChanged(int index);
    void on_tabWidget_tabBarDoubleClicked(int index);
    void on_tabWidget_tabBarClicked(int index);
    void on_buttonBox_accepted();

private:
    typedef struct
    {
        PlotConfigTabWIdget *widget;
        QList<int> indexes;
    } ConfigTabWidget;

    Ui::PlotConfigDialog *ui;
    SerialData *serialData;
    QList<ConfigTabWidget> tabsWidgetConfig;
    QMutex tabWidgetMutex;

    bool tabBarClicked;

    void addNewTab();
    void removeTab(int index);

    void showEvent(QShowEvent *event);

signals:
    void acceptClicked();
};
