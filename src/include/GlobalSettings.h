#pragma once

#include <QSettings>
#include <QObject>

#include "Common.h"

class GlobalSettings : public QObject
{
    Q_OBJECT
public:
    GlobalSettings(QObject *parent = nullptr);
    ~GlobalSettings();

    bool setValue(QString name, QVariant value);
    QVariant getValue(QString name);
    bool setUndoableValue(QString name, QVariant value);
    void startTrackChange();
    void applyTrackChange();
    void undoChange();
    void restoreDefault();
    void exportToFile();
    void importFromFile();
    bool checkNeedReStart();

private:
    typedef struct
    {
        QString group;
        QString name;
        QVariant defaultValue;
        bool needReStart;
    } SettingEntry;

    const SettingEntry settingsEntry[20] = {
        {"MainWindow", "keepWindowSize", QVariant(true), false},
        {"MainWindow", "keepWindowPos", QVariant(false), false},
        {"MainWindow", "size", QVariant(QSize(1000, 800)), false},
        {"MainWindow", "pos", QVariant(QPoint(400, 300)), false},
        {"Interface", "language", QVariant(""), true},
        {"Interface", "windosStyle", QVariant(""), false},
        {"Interface", "darkMode", QVariant(false), false},
        {"Interface", "sendNotice", QVariant(true), false},
        {"HotKey", "showAndHideKey", QVariant(QString("Ctrl+Alt+Z")), false},
        {"HotKey", "clearOutputKey", QVariant(0), false},
        {"HotKey", "scrollToEndKey", QVariant(0), false},
    };

    typedef struct
    {
        QString name;
        QVariant value;
    } SettingValue;

    QSettings *settings;
    bool track;
    bool needReStart;
    QList<SettingValue> trackingSettings;

signals:
    void onSaved();
};
