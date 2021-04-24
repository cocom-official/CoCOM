#pragma once

#include <QObject>

#include "CoLua.h"

class CoLuaWorker : public QObject
{
    Q_OBJECT
public:
    explicit CoLuaWorker(CoLua *_lua)
        : lua(_lua){};

public slots:
    void doLoadFile(const QString file)
    {
        emit workFinish(lua->loadFileSync(file));
    }

    void doRunString(const QString file)
    {
        emit workFinish(lua->runStringSync(file));
    }

    void doREPL(const QString repl)
    {
        emit workFinish(lua->replSync(repl));
    }

private:
    CoLua *lua;

signals:
    void workFinish(int result);
};
