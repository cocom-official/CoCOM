#pragma once

#include <QObject>

#include "Common.h"
#include "fflua.h"

class CoLua
    : public QObject
{
    Q_OBJECT
public:
    explicit CoLua(QObject *parent = nullptr);

    /* register to lua */
    void __console(string out);
    void __log(string out, int level = 0);

    /* da lua */
    int loadFile(QString filePath);
    int runString(QString run);

private:
    ff::fflua_t fflua;

signals:
    void consoleOut(QString out);
    void logOut(QString out, MessageLevelType level = InfoLevel);
};
