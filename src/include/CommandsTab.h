#pragma once

#include <QString>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

#include "Common.h"

#define COMMANDS_TAB_ITEM_COUNT 10
#define COMMANDS_TAB_GRID_COLUMN 2

class CommandsTab : public QWidget
{
    Q_OBJECT
public:
    explicit CommandsTab(QWidget *parent = nullptr);

private:
    MouseButtonSignaler *buttonSignaler;
    EnterKeySignaler *editSignaler;
    QHBoxLayout *hLavout[COMMANDS_TAB_ITEM_COUNT];
    QGridLayout *gLavout;
    QLineEdit *edit[COMMANDS_TAB_ITEM_COUNT];
    QPushButton *button[COMMANDS_TAB_ITEM_COUNT];

private slots:
    void buttonsEvent(QWidget *obj, QMouseEvent *event);
    void editEvent(QWidget *obj, QKeyEvent *event);

signals:
    void sendText(QString text);
};
