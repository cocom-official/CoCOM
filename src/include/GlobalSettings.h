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

private:
    typedef struct
    {
        QString group;
        QString name;
        QVariant defaultValue;
    } SettingEntry;

    const SettingEntry settingsEntry[20] = {
        {"MainWindow", "keepWindowSize", QVariant(true)},
        {"MainWindow", "keepWindowPos", QVariant(false)},
        {"MainWindow", "size", QVariant(QSize(1000, 800))},
        {"MainWindow", "pos", QVariant(QPoint(400, 300))},
        {"General", "language", QVariant(0)},
        {"General", "windosStyle", QVariant(0)},
        {"General", "darkMode", QVariant(false)},
        {"HotKey", "showAndHideKey", QVariant(QString("Ctrl+Alt+Z"))},
        {"HotKey", "clearOutputKey", QVariant(0)},
        {"HotKey", "scrollToEndKey", QVariant(0)},
    };

    typedef struct
    {
        QString name;
        QVariant value;
    } SettingValue;

    QSettings *settings;
    bool track;
    QList<SettingValue> trackingSettings;

signals:
    void onSaved();
};
