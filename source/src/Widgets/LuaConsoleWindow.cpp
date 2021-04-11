#include "LuaConsoleWindow.h"
#include "ui_luaConsoleWindow.h"

LuaConsoleWindow::LuaConsoleWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::LuaConsoleWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/assets/logos/lua.png"));
    console = new Console(QString(LUA_COPYRIGHT), this);
    console->setEnabled(true);
    setCentralWidget(console);
    loadFont();

    connect(&lua, &CoLua::replOut, console, &Console::putOut);
    connect(&lua, &CoLua::replNewComplateLine, console, &Console::completeNewLine);
    connect(&lua, &CoLua::replNewIncomplateLine, console, &Console::incompleteNewLine);
    connect(console, &Console::newLineInput, this, &LuaConsoleWindow::doREPL);
}

LuaConsoleWindow::~LuaConsoleWindow()
{
    delete ui;
}

void LuaConsoleWindow::loadFont()
{
    QFile fontFile(":/assets/fonts/CascadiaCodePL.ttf");
    if (!fontFile.open(QIODevice::ReadOnly))
    {
        qDebug("open fontFile fail");
        return;
    }

    int fontId = QFontDatabase::addApplicationFontFromData(fontFile.readAll());
    if (fontId == -1)
    {
        qDebug("load fontFile fail");
        return;
    }

    QStringList fontFamily = QFontDatabase::applicationFontFamilies(fontId);
    if (fontFamily.empty())
    {
        qDebug("fontFile empty");
        return;
    }

    QFont font(fontFamily.at(0));
    console->setFont(font);
}

void LuaConsoleWindow::doREPL(const QString &input)
{
    lua.REPL(input);
}
