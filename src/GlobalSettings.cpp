#include "GlobalSettings.h"

GlobalSettings::GlobalSettings(QObject *parent)
    : QObject(parent),
      settings(nullptr),
      track(false),
      needReStart(false)
{
    QFile portableFile(QCoreApplication::applicationDirPath() + "/" + QString(COCOM_PORTABLE_FILE_NAME));

    if (portableFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "CoCOM Portable Mode";
        settings = new QSettings(QCoreApplication::applicationDirPath() + "/CoCOM.ini", QSettings::IniFormat, this);
    }
    else
    {
        settings = new QSettings(QSettings::IniFormat,
                                 QSettings::UserScope,
                                 QString(COCOM_VENDER),
                                 QString(COCOM_APPLICATIONNAME), this);
    }
}

GlobalSettings::~GlobalSettings()
{
    settings->sync();
}

bool GlobalSettings::setValue(QString name, QVariant value)
{
    QString group;
    bool ret = false;

    for (auto &&i : settingsEntry)
    {
        if (i.name == name)
        {
            group = i.group;
            ret = true;

            if (i.needReStart)
            {
                needReStart = true;
            }

            break;
        }
    }

    if (ret)
    {
        settings->beginGroup(group);
        settings->setValue(name, value);
        settings->endGroup();
    }

    return ret;
}

QVariant GlobalSettings::getValue(QString name)
{
    QString group;
    QVariant setValue;
    QVariant defaultValue;
    bool ret = false;

    for (auto &&i : settingsEntry)
    {
        if (i.name == name)
        {
            group = i.group;
            defaultValue = i.defaultValue;
            ret = true;
            break;
        }
    }

    if (ret)
    {
        settings->beginGroup(group);
        setValue = settings->value(name, defaultValue);
        settings->endGroup();
    }

    return setValue;
}

bool GlobalSettings::setUndoableValue(QString name, QVariant value)
{
    QString group;
    bool ret = false;

    for (auto &&i : settingsEntry)
    {
        if (i.name == name)
        {
            group = i.group;
            ret = true;
            break;
        }
    }

    if (ret && track)
    {
        SettingValue setting = {name, value};
        trackingSettings << setting;
    }

    return ret && track;
}

void GlobalSettings::startTrackChange()
{
    track = true;
    needReStart = false;
}

void GlobalSettings::applyTrackChange()
{
    for (auto &&i : trackingSettings)
    {
        setValue(i.name, i.value);
    }
    trackingSettings.clear();
    track = false;
    emit onSaved();
}

void GlobalSettings::undoChange()
{
    trackingSettings.clear();
    track = false;
    needReStart = false;
}

void GlobalSettings::restoreDefault()
{
    settings->clear();
    track = false;
    needReStart = false;
}

void GlobalSettings::exportToFile()
{
}

void GlobalSettings::importFromFile()
{
}

bool GlobalSettings::checkNeedReStart()
{
    return needReStart;
}
