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
    void on_listWidget_currentRowChanged(int currentRow);
    void on_okButton_pressed();
    void on_cancelButton_pressed();

private:
    Ui::ConfigDialog *ui;
    float dpiScaling;
    MouseButtonSignaler *luaLabelSignaler;

    void setupUI();
    void refreshDPI();

    void enumStyles();

    void luaLabel_mouseButtonEvent(QWidget *obj, QMouseEvent *event);
};
