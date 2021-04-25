#include "CoLua.h"
#include "lua_repl.h"
#include "CoLuaWorker.h"

static void lua_reg(lua_State *ls)
{
    ff::fflua_register_t<QObject, ctor(QObject *)>(ls, "QObject");

    ff::fflua_register_t<CoLua, ctor(QObject *)>(ls, "CoLua")
        .def(&CoLua::lua_print, "print")
        .def(&CoLua::lua_console, "console")
        .def(&CoLua::lua_log, "log")
        .def(&CoLua::lua_sleep, "sleep")
        .def(&CoLua::lua_msleep, "msleep");
}

int CoLua::lua_print(string out)
{
    QString console = QString(out.c_str());
    qDebug() << QString("%1:print").arg(objectName()) << console;

    emit consoleOut(console);

    if (!console.trimmed().isEmpty())
    {
        emit stdOut(console);
    }

    return 0;
}

void CoLua::lua_console(string out)
{
    QString console = QString(out.c_str());
    qDebug() << QString("%1:console").arg(objectName()) << console;

    emit consoleOut(console);

    if (!console.trimmed().isEmpty())
    {
        emit stdOut(console);
    }
}

void CoLua::lua_log(int level, string out)
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
        emit stdOut(log);
    }
}

void CoLua::lua_sleep(uint64_t time)
{
    workerThread.sleep(time);
}

void CoLua::lua_msleep(uint64_t time)
{
    workerThread.msleep(time);
}

CoLua::CoLua(QObject *parent)
    : QObject(parent)
{
    setObjectName("CoLua");
    initInstance();
}

CoLua::~CoLua()
{
    workerThread.quit();
    workerThread.wait();
}

void CoLua::setMedialData(IODeviceData *data)
{
    ioDeviceData = data;
}

int CoLua::loadFileSync(QString filePath)
{
    return fflua.load_file(filePath.toStdString());
}

int CoLua::loadFileAsync(QString filePath)
{
    if (workerThread.isRunning())
    {
        return -1;
    }

    CoLuaWorker *worker = new CoLuaWorker(this);
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &CoLua::startWorkerThread, worker, &CoLuaWorker::doLoadFile);
    connect(worker, &CoLuaWorker::workFinish, this, &CoLua::workFinish);
    workerThread.start();
    emit startWorkerThread(filePath);

    return 0;
}

int CoLua::runStringSync(QString run)
{
    return fflua.run_string(run.toStdString());
}

int CoLua::runStringAsync(QString run)
{
    if (workerThread.isRunning())
    {
        return -1;
    }

    CoLuaWorker *worker = new CoLuaWorker(this);
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &CoLua::startWorkerThread, worker, &CoLuaWorker::doRunString);
    connect(worker, &CoLuaWorker::workFinish, this, &CoLua::workFinish);
    workerThread.start();
    emit startWorkerThread(run);

    return 0;
}

int CoLua::replSync(QString run)
{
    QString repl = QString(fflua.do_REPL(run.toStdString()).c_str());

    if (!repl.isEmpty())
    {
        if (!repl.trimmed().isEmpty())
        {
            emit stdOut(repl);
        }
        emit replNewComplateLine();
    }
    else
    {
        emit replNewIncomplateLine();
    }

    return 0;
}

int CoLua::replAsync(QString run)
{
    if (workerThread.isRunning())
    {
        return -1;
    }

    CoLuaWorker *worker = new CoLuaWorker(this);
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &CoLua::startWorkerThread, worker, &CoLuaWorker::doREPL);
    connect(worker, &CoLuaWorker::workFinish, this, &CoLua::workFinish);
    workerThread.start();
    emit startWorkerThread(run);

    return 0;
}

void CoLua::initInstance()
{
    fflua.add_package_path(LUA_SCRIPTS_PATH.toStdString());
    fflua.setModFuncFlag(true);
    fflua.reg(lua_reg);
    fflua.set_global_variable("coLua", this);

    loadFileSync(LUA_SCRIPTS_PATH + QString("/" COCOM_LUA_BASE_EXTENSION));
}

void CoLua::resetInstance()
{
    if (workerThread.isRunning())
    {
        workerThread.quit();
    }

    fflua.reset();
    initInstance();
}

void CoLua::workFinish(int result)
{
    Q_UNUSED(result);
    workerThread.quit();
}
