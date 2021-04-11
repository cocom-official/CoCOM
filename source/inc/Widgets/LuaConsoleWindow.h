#pragma once

#include <QMainWindow>

#include "Console.h"
#include "CoLua.h"

class Console;

namespace Ui
{
    class LuaConsoleWindow;
}

class LuaConsoleWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LuaConsoleWindow(QWidget *parent = nullptr);
    ~LuaConsoleWindow();

private:
    Ui::LuaConsoleWindow *ui;
    Console *console;
    CoLua lua;

    void loadFont();

private slots:
    void doREPL(const QString &input);
};
