#include "MainWindow.h"

#include <QLocale>
#include <QString>
#include <QApplication>
#include <QTranslator>

#include "Common.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow windows;
    windows.show();

    int retCode = app.exec();

    if (retCode == restartExitCode)
    {
        QProcess::startDetached(qApp->applicationFilePath(), QStringList());
        return 0;
    }

    return retCode;
}
