#pragma once

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>

#include "MediaData.h"

namespace Ui
{
    class PlotConfigTabWIdget;
}

class PlotConfigTabWIdget
    : public QWidget
{
    Q_OBJECT

public:
    explicit PlotConfigTabWIdget(QWidget *parent = nullptr);
    ~PlotConfigTabWIdget();
    QList<int> getResultIndexes();
    void restoreResultIndexes(QList<int> m_indexes);
    int seriesIndexToRegs(int index);
    bool checkValid();

public slots:
    void setDataSource(MediaData *Data);
    void updateDataSource();

private slots:
    void on_seriesComboBox_currentIndexChanged(int index);
    void on_x0_ComboBox_currentIndexChanged(int index);
    void ysComboBox_currentIndexChanged(int index);

private:
    typedef struct
    {
        QLabel *label;
        QComboBox *combobox;
        QHBoxLayout *layout;
    } YAxisRow;

    Ui::PlotConfigTabWIdget *ui;
    QList<YAxisRow> rows;
    MediaData *mediaData;
    QList<int> indexes;

    void addYAxisRow();
    void setAxisRow(QStringList regNames);
    void updateResultIndexes();
};
