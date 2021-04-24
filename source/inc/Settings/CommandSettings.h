#pragma once

#include <QSettings>
#include <QObject>
#include <QFile>

class CommandSettings : public QObject
{
    Q_OBJECT
public:
    CommandSettings(QObject *parent = nullptr);
    ~CommandSettings();
    int getTabCount();
    void setTabCount(int count);
    int getCurrentTab();
    void setCurrentTab(int tab);
    void beginTab(int i);
    void endTab();
    void setValue(int i, QString value);
    QString getValue(int i);
private:
    int tabCount;
    QSettings *settings;
};
