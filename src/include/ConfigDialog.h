#pragma once

#include <QDialog>
#include <QDebug>
#include <QStyleFactory>
#include <QScreen>
#include <QWindow>
#include <QListWidgetItem>
#include <QFile>

#include "GlobalSettings.h"
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
    explicit ConfigDialog(GlobalSettings *settings, QWidget *parent = nullptr);
    ~ConfigDialog();

private slots:
    void on_languageComboBox_currentIndexChanged(const QString &text);
    void on_styleComboBox_currentIndexChanged(const QString &text);
    void on_darkModeCheckBox_stateChanged(int state);
    void on_keepWindowsSizeCheckBox_stateChanged(int state);
    void on_keepWindowsPosCheckBox_stateChanged(int state);
    void on_noticeCheckBox_stateChanged(int state);
    void on_restoreButton_clicked();

    void on_okButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::ConfigDialog *ui;
    float dpiScaling;
    MouseButtonSignaler *aboutLabelsSignaler;
    GlobalSettings *settings;

    void setupUI();
    void refreshDPI();
    void refreshUI();

    void aboutLabels_mouseButtonEvent(QWidget *obj, QMouseEvent *event);

    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);

signals:
    void onRestore();
    void onRestart();
};
