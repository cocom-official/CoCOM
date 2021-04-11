#include "CoLua.h"
#include "lua_repl.h"

static void lua_reg(lua_State *ls)
{
    ff::fflua_register_t<QObject, ctor(QObject *)>(ls, "QObject");

    ff::fflua_register_t<CoLua, ctor(QObject *)>(ls, "CoLua")
        .def(&CoLua::__print, "print")
        .def(&CoLua::__console, "console")
        .def(&CoLua::__log, "log");
}

CoLua::CoLua(QObject *parent)
    : QObject(parent)
{
    setObjectName("CoLua");
    initInstance();
}

int CoLua::__print(string out)
{
    QString console = QString(out.c_str());
    qDebug() << QString("%1:print").arg(objectName()) << console;

    emit consoleOut(console);

    if (!console.trimmed().isEmpty())
    {
        emit replOut(console);
    }

    return 0;
}

void CoLua::__console(string out)
{
    QString console = QString(out.c_str());
    qDebug() << QString("%1:console").arg(objectName()) << console;

    emit consoleOut(console);

    if (!console.trimmed().isEmpty())
    {
        emit replOut(console);
    }
}

void CoLua::__log(int level, string out)
{
    QString levelStr;
    QString log = QString(out.c_str());
    MessageLevelType l = static_cast<MessageLevelType>(level);
    switch (l)
    {
    case InfoLevel:
        levelStr = "Info:";
        qInfo() << QString("%1:log").arg(objectName()) << log;
        break;

    case WarningLevel:
        levelStr = "Warning:";
        qWarning() << QString("%1:log").arg(objectName()) << log;
        break;

    case CriticalLevel:
        levelStr = "Critical:";
        qCritical() << QString("%1:log").arg(objectName()) << log;
        break;

    default:
        break;
    }
    log = levelStr + log;

    emit logOut(log, l);

    if (!log.trimmed().isEmpty())
    {
        emit replOut(log);
    }
}

int CoLua::loadFile(QString filePath)
{
    return fflua.load_file(filePath.toStdString());
}

void CoLua::runString(QString run)
{
    fflua.run_string(run.toStdString());
}

void CoLua::REPL(QString run)
{
    QString repl = QString(fflua.do_REPL(run.toStdString()).c_str());

    if (!repl.isEmpty())
    {
        if (!repl.trimmed().isEmpty())
        {
            emit replOut(repl);
        }
        emit replNewComplateLine();
    }
    else
    {
        emit replNewIncomplateLine();
    }
}

void CoLua::initInstance()
{
    fflua.add_package_path(LUA_SCRIPTS_PATH.toStdString());
    fflua.setModFuncFlag(true);
    fflua.reg(lua_reg);
    fflua.set_global_variable("coLua", this);

    loadFile(LUA_SCRIPTS_PATH + QString("/cocom_ext.lua"));
}

void CoLua::resetInstance()
{
    fflua.reset();
    initInstance();
}
