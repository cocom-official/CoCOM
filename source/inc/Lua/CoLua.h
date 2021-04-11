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
    int __print(string out);
    void __console(string out);
    void __log(int level, string out);

    /* do lua */
    int loadFile(QString filePath);
    void runString(QString run);
    void REPL(QString run);

public slots:
    void resetInstance();

private:
    ff::fflua_t fflua;
    void initInstance();

signals:
    void consoleOut(QString out);
    void logOut(QString out, MessageLevelType level = InfoLevel);
    void replOut(const QString &out);
    void replNewComplateLine();
    void replNewIncomplateLine();
};
