#pragma once

#include <QDialog>
#include <QDebug>
#include <QStyleFactory>
#include <QScreen>
#include <QWindow>
#include <QListWidgetItem>
#include <QFile>

#include "Common.h"
#include "lua.hpp"
#include "fflua.h"

namespace Ui
{
class ConfigDialog;
}

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget *parent = nullptr);
    ~ConfigDialog();

private slots:
    void on_styleComboBox_currentIndexChanged(const QString &text);
    void on_restoreButton_clicked();

    void on_listWidget_currentRowChanged(int currentRow);
    void on_okButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::ConfigDialog *ui;
    float dpiScaling;
    MouseButtonSignaler *aboutLabelsSignaler;

    void setupUI();
    void refreshDPI();

    void enumStyles();

    void aboutLabels_mouseButtonEvent(QWidget *obj, QMouseEvent *event);
};
