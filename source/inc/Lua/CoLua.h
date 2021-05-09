#pragma once

#include <QObject>

#include "Common.h"
#include "fflua.h"
#include "IODeviceData.h"
#include "CoDevice.h"
using namespace std;

#define COCOM_LUA_BASE_EXTENSION "base.lua"

class CoLua
    : public QObject
{
    Q_OBJECT
public:
    explicit CoLua(QObject *parent = nullptr);
    ~CoLua();

    void setMedialData(IODeviceData *data);

    /* register to lua environment */
    int lua_print(string out);
    void lua_console(string out);
    void lua_log(int level, string out);
    void lua_sleep(uint64_t time);
    void lua_msleep(uint64_t time);
    string lua_raw2str(vector<uint8_t> data);
    vector<uint8_t> lua_str2raw(string data);
    int lua_dwrite(vector<uint8_t> data);
    int lua_dwrite_text(string data);
    vector<uint8_t> lua_dread(int len);
    vector<uint8_t> lua_dreadall();
    string lua_dread_text(int len);
    string lua_dread_line(int len);

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
    ff::fflua_t fflua;
    IODeviceData *ioDeviceData;
    CoDevice *coDevice;
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
