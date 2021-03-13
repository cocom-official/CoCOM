#pragma once

#include <QString>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

#include "Common.h"
#include "CommandSettings.h"

#define COMMANDS_TAB_ITEM_COUNT 10
#define COMMANDS_TAB_GRID_COLUMN 2

class CommandsTab : public QWidget
{
    Q_OBJECT
public:
    explicit CommandsTab(QWidget *parent = nullptr);
    void readSettings(CommandSettings *setting, int tab);
    void writeSettings(CommandSettings *setting, int tab);
    void clear();

private:
    MouseButtonSignaler *buttonSignaler;
    EnterKeySignaler *editSignaler;
    QHBoxLayout *hLavout[COMMANDS_TAB_ITEM_COUNT];
    QGridLayout *gLavout;
    QLineEdit *edit[COMMANDS_TAB_ITEM_COUNT];
    QPushButton *button[COMMANDS_TAB_ITEM_COUNT];

    void keyPressEvent(QKeyEvent *event);

private slots:
    void buttonsEvent(QWidget *obj, QMouseEvent *event);
    void editEvent(QWidget *obj, QKeyEvent *event);

signals:
    void sendText(QString text);
};
