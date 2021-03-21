#include "CoLua.h"

static void lua_reg(lua_State *ls)
{
    ff::fflua_register_t<QObject, ctor(QObject *)>(ls, "QObject");

    ff::fflua_register_t<CoLua, ctor(QObject *)>(ls, "CoLua")
        .def(&CoLua::__console, "console")
        .def(&CoLua::__log, "log");
}

static const QString lua_printf =
    "print = function(s,...)"
    "            return coLua:console(s:format(...))"
    "        end -- function";

CoLua::CoLua(QObject *parent)
    : QObject(parent)
{
    setObjectName("CoLua");

    fflua.setModFuncFlag(true);
    fflua.reg(lua_reg);
    fflua.set_global_variable("coLua", this);

    runString(lua_printf);
}

void CoLua::__console(string out)
{
    QString console = QString(out.c_str());
    qDebug() << QString("%1:console").arg(objectName()) << console;

    emit consoleOut(console);
}

void CoLua::__log(string out, int level)
{
    QString log = QString(out.c_str());
    MessageLevelType l = static_cast<MessageLevelType>(level);
    switch (l)
    {
    case InfoLevel:
        qInfo() << log;
        break;

    case WarningLevel:
        qWarning() << log;
        break;

    case CriticalLevel:
        qCritical() << log;
        break;

    default:
        break;
    }

    emit logOut(log, l);
}

int CoLua::loadFile(QString filePath)
{
    return fflua.load_file(filePath.toStdString());
}

void CoLua::runString(QString run)
{
    fflua.run_string(run.toStdString());
}
