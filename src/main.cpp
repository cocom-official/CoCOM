#include "MainWindow.h"

#include <QLocale>
#include <QString>
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QTranslator translator;
    QString locale = QLocale::system().name();
    qDebug() << locale;
    qDebug() << translator.load(":/translations/CoCOM_" + locale);
    app.installTranslator(&translator);
    MainWindow windows;
    windows.show();
        return app.exec();
}
