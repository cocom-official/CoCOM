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
        .def(&CoLua::lua_msleep, "msleep")
        .def(&CoLua::lua_raw2str, "raw2str")
        .def(&CoLua::lua_str2raw, "str2raw")
        .def(&CoLua::lua_dwrite, "dwrite")
        .def(&CoLua::lua_dwrite_text, "dwrite_text")
        .def(&CoLua::lua_dread, "dread")
        .def(&CoLua::lua_dreadall, "dreadall")
        .def(&CoLua::lua_dread_text, "dread_text")
        .def(&CoLua::lua_dread_line, "dread_line");
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

string CoLua::lua_raw2str(vector<uint8_t> data)
{
    QByteArray bytes;

    for (auto &&i : data)
    {
        bytes.append(QChar(i).toLatin1());
    }

    return bytes.toStdString();
}

vector<uint8_t> CoLua::lua_str2raw(string data)
{
    vector<uint8_t> bytes;

    for (auto &&i : data)
    {
        bytes.push_back(i);
    }

    return bytes;
}

int CoLua::lua_dwrite(vector<uint8_t> data)
{
    if (coDevice == nullptr)
    {
        return -1;
    }

    return coDevice->write((const char *)data.data(), data.size());
}

int CoLua::lua_dwrite_text(string data)
{
    if (coDevice == nullptr)
    {
        return -1;
    }

    return coDevice->write((const char *)data.c_str(), data.length());
}

vector<uint8_t> CoLua::lua_dread(int len)
{
    vector<uint8_t> data;
    if (coDevice == nullptr)
    {
        return data;
    }

    QByteArray retData = coDevice->read(len);
    for (auto &&i : retData)
    {
        data.push_back(i);
    }

    return data;
}

vector<uint8_t> CoLua::lua_dreadall()
{
    vector<uint8_t> data;
    if (coDevice == nullptr)
    {
        return data;
    }

    QByteArray retData = coDevice->readAll();
    for (auto &&i : retData)
    {
        data.push_back(i);
    }

    return data;
}

string CoLua::lua_dread_text(int len)
{
    if (coDevice == nullptr)
    {
        return string();
    }

    return coDevice->read(len).toStdString();
}

string CoLua::lua_dread_line(int len)
{
    if (coDevice == nullptr)
    {
        return string();
    }

    return coDevice->readLine(len).toStdString();
}

CoLua::CoLua(QObject *parent)
    : QObject(parent),
      ioDeviceData(nullptr),
      coDevice(nullptr)
{
    setObjectName("CoLua");
    coDevice = qApp->property("coDevice").value<CoDevice *>();
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
