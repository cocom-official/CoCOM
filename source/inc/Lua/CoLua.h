#pragma once

#include <QObject>

#include "Common.h"
#include "fflua.h"
#include "SerialData.h"

#define COCOM_LUA_BASE_EXTENSION "base.lua"

class CoLua
    : public QObject
{
    Q_OBJECT
public:
    explicit CoLua(QObject *parent = nullptr);
    ~CoLua();

    void setSerialData(SerialData *data);

    /* register to lua environment */
    int lua_print(string out);
    void lua_console(string out);
    void lua_log(int level, string out);
    void lua_sleep(uint64_t time);
    void lua_msleep(uint64_t time);

public slots:
    /* do lua */
    int loadFileSync(QString filePath);
    int loadFileAsync(QString filePath);
    int runStringSync(QString run);
    int runStringAsync(QString run);
    int replSync(QString run);
    int replAsync(QString run);

    void resetInstance();

private:
    typedef void (*WorkerSlot)(QString);

    ff::fflua_t fflua;
    SerialData *serialData;
    QThread workerThread;

    void initInstance();

private slots:
    void workFinish(int result);

signals:
    void consoleOut(QString out);
    void logOut(QString out, MessageLevelType level = InfoLevel);
    void stdOut(const QString &out);
    void replNewComplateLine();
    void replNewIncomplateLine();
    void startWorkerThread(const QString run);
};
