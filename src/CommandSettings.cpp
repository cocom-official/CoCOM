#include "CommandSettings.h"
#include "Common.h"

CommandSettings::CommandSettings(QObject *parent)
    : QObject(parent)
{
    QFile portableFile(QCoreApplication::applicationDirPath() + "/" + QString(COCOM_PORTABLE_FILE_NAME));

    if (portableFile.open(QIODevice::ReadOnly))
    {
        settings = new QSettings(QCoreApplication::applicationDirPath() + "/CoCOM_Commands.ini", QSettings::IniFormat, this);
    }
    else
    {
        settings = new QSettings(QSettings::IniFormat,
                                 QSettings::UserScope,
                                 QString(COCOM_VENDER),
                                 QString(COCOM_APPLICATIONNAME) + QString("_Commands"), this);
    }

    settings->beginGroup("Info");
    tabCount = settings->value("TabCount", int(0)).toInt();
    settings->endGroup();
}

CommandSettings::~CommandSettings()
{
    settings->sync();
}

int CommandSettings::getTabCount()
{
    return tabCount;
}

void CommandSettings::setTabCount(int count)
{
    tabCount = count;
    settings->beginGroup("Info");
    settings->setValue("TabCount", count);
    settings->endGroup();
}

int CommandSettings::getCurrentTab()
{
    int index = 0;
    settings->beginGroup("Info");
    index = settings->value("CurrentTab", QVariant(0)).toInt();
    settings->endGroup();

    return index;
}

void CommandSettings::setCurrentTab(int tab)
{
    settings->beginGroup("Info");
    settings->setValue("CurrentTab", QVariant(tab));
    settings->endGroup();
}

void CommandSettings::beginTab(int i)
{
    settings->beginGroup(QString("Tab") + QString::number(i));
}

void CommandSettings::endTab()
{
    settings->endGroup();
}

void CommandSettings::setValue(int i, QString value)
{
    settings->setValue(QString("Edit") + QString::number(i), QVariant(value));
}

QString CommandSettings::getValue(int i)
{
    return settings->value(QString("Edit") + QString::number(i), QVariant("")).toString();
}
