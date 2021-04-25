#include "IODeviceData.h"

IODeviceData::IODeviceData(QObject *parent)
    : QObject(parent),
      encoding("Local")
{
}

void IODeviceData::appendBytes(QByteArray *bytes)
{
    QMutex mutex;
    serialData += *bytes;
    static QByteArray remain;
    QByteArray inBytes = *bytes;
    int rIndexPos = 0;
    int nIndexPos = 0;

    mutex.lock();

    remain.append(inBytes);

    while (remain.size() > 0 &&
           (remain[0] == '\r' || remain[0] == '\n'))
    {
        remain.remove(0, 1);
    }

    rIndexPos = remain.indexOf('\r');
    nIndexPos = remain.indexOf('\n');
    while (rIndexPos > 0 || nIndexPos > 0)
    {
        int pos = 0;
        if (rIndexPos > 0 && ((nIndexPos > 0 && rIndexPos < nIndexPos) || nIndexPos < 0))
        {
            pos = rIndexPos;
        }
        else if (nIndexPos > 0 && ((rIndexPos > 0 && nIndexPos < rIndexPos) || rIndexPos < 0))
        {
            pos = nIndexPos;
        }
        else
        {
            /* should not run to here */
            Q_ASSERT(false);
        }

        findNewLine(getEncodingCodecFromString(encoding)->toUnicode(remain.left(pos)));
        remain.remove(0, pos);

        while (remain.size() > 0 &&
               (remain[0] == '\r' || remain[0] == '\n'))
        {
            remain.remove(0, 1);
        }

        rIndexPos = remain.indexOf('\r');
        nIndexPos = remain.indexOf('\n');
    }

    mutex.unlock();
}

void IODeviceData::findNewLine(QString line)
{
    emit newTextLine(line);

    int exp = 0;
    clearMutex.lock();
    for (auto &&i : regs)
    {
        if (i.enable && i.regExp.isValid())
        {
            QRegularExpressionMatch match = i.regExp.match(line);
            if (match.hasMatch())
            {
                QList<qreal> data;
                for (auto &&j : i.names)
                {
                    data << match.captured(j).toDouble();
                }
                i.values << data;
                emit regExpNewData(exp, data);
            }
        }
        exp++;
    }
    clearMutex.unlock();
}

void IODeviceData::setEncoding(QString lEncoding)
{
    encoding = lEncoding;
}

void IODeviceData::clear()
{
    clearMutex.lock();
    regs.clear();
    clearMutex.unlock();
}

int IODeviceData::addRegExp(QString pattern, QString title)
{
    int ret = regs.count();

    if (title.isEmpty())
    {
        title = QString(tr("RegExp")) + QString::number(ret);
    }

    regs << RegExp_t{true,
                     title,
                     QRegularExpression(pattern),
                     QStringList(),
                     QList<QList<qreal>>()};

    if (!regs[ret].regExp.isValid())
    {
        return -1;
    }

    regs[ret].names.clear();
    QStringList names = regs[ret].regExp.namedCaptureGroups();

    for (auto &&i : names)
    {
        if (!i.isEmpty())
        {
            regs[ret].names << i;
        }
    }

    return ret;
}

int IODeviceData::setRegExp(int exp, QString pattern)
{
    if (exp >= regs.count())
    {
        return -1;
    }

    QString lastPattern = regs[exp].regExp.pattern();
    regs[exp].regExp.setPattern(pattern);

    if (!regs[exp].regExp.isValid())
    {
        regs[exp].regExp.setPattern(lastPattern);
        return -1;
    }

    regs[exp].values.clear();
    regs[exp].names.clear();
    QStringList names = regs[exp].regExp.namedCaptureGroups();

    for (auto &&i : names)
    {
        if (!i.isEmpty())
        {
            regs[exp].names << i;
        }
    }

    return exp;
}

bool IODeviceData::isRegExpEnable(int exp)
{
    if (exp >= regs.count())
    {
        return false;
    }

    return regs[exp].enable;
}

QString IODeviceData::getRegExpPattern(int exp)
{
    if (exp >= regs.count())
    {
        return QString();
    }

    return regs[exp].regExp.pattern();
}

QString IODeviceData::getRegExpTitle(int exp)
{
    if (exp >= regs.count())
    {
        return QString();
    }

    return regs[exp].title;
}

void IODeviceData::setRegExpTitle(int exp, QString title)
{
    if (exp >= regs.count())
    {
        return;
    }

    regs[exp].title = title;
}

QStringList IODeviceData::getRegExpNames(int exp)
{
    if (exp >= regs.count())
    {
        return QStringList();
    }

    return regs[exp].names;
}

QList<QList<qreal>> IODeviceData::getRegExpValues(int exp)
{
    if (exp >= regs.count())
    {
        return QList<QList<qreal>>();
    }

    return regs[exp].values;
}

QList<int> IODeviceData::getRegExpList()
{
    QList<int> ret;
    clearMutex.lock();
    int exp = 0;
    for (auto &&i : regs)
    {
        if (i.enable && i.regExp.isValid())
        {
            ret << exp;
        }
        exp++;
    }
    clearMutex.unlock();

    return ret;
}

void IODeviceData::removeRegExp(int exp)
{
    if (exp >= regs.count())
    {
        return;
    }

    regs[exp].enable = false;
    regs[exp].names.clear();
    regs[exp].values.clear();
}
