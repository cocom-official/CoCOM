#include "CoDevice.h"
#include "Common.h"

CoDevice::CoDevice()
    : sendDataType(TextType)
{
    currentPort.index = -1;
    currentPort.info = nullptr;
    currentPort.port = nullptr;

    connect(this, &CoDevice::sendRawDataSignal, this, &CoDevice::sendRawData, Qt::QueuedConnection);

    enumDevices();
}

CoDevice::~CoDevice()
{
    for (auto &&i : ports)
    {
        if (i.info == nullptr)
        {
            continue;
        }

        delete (i.info);
        i.info = nullptr;
    }
}

int32_t CoDevice::count()
{
    return ports.count();
}

void CoDevice::setCurrentPort(int32_t index)
{
    if (currentPort.index == index || index < 0 || index > ports.count())
    {
        return;
    }

    CoDevice::Port_t lastPort = currentPort;

    mutex.lock();

    currentPort = ports[index];

    if (lastPort.port == nullptr || lastPort.info == nullptr ||
        currentPort.port == nullptr || currentPort.info == nullptr)
    {
        return;
    }

    if (lastPort.port != nullptr && lastPort.port->isOpen())
    {
        if (lastPort.port != nullptr)
        {
            lastPort.port->close();
        }
        open();
    }

    mutex.unlock();
}

int CoDevice::currentIndex()
{
    return currentPort.index;
}

int CoDevice::open()
{
    if (currentPort.port == nullptr || currentPort.info == nullptr || currentPort.info->isNull())
    {
        return E_Null;
    }

    if (currentPort.info->isBusy())
    {
        return E_Busy;
    }

    bool ret = currentPort.port->open(ReadWrite);

    if (ret)
    {
        connect(currentPort.port, &QSerialPort::readyRead,
                this, &CoDevice::currenPort_readyRead, Qt::QueuedConnection);
    }

    QIODevice::open(ReadWrite);
    return OK;
}

void CoDevice::close()
{
    if (currentPort.port != nullptr)
    {
        disconnect(currentPort.port);
        currentPort.port->close();
    }

    deviceDataLock.lock();
    deviceData.clear();
    deviceDataLock.unlock();

    QIODevice::close();
}

bool CoDevice::isOpen(int32_t index)
{
    if (index < 0 || index > ports.count())
    {
        return false;
    }

    Port_t port = ports[index];

    if (port.port == nullptr || port.info == nullptr)
    {
        return false;
    }

    return port.port->isOpen();
}

void CoDevice::sendRawData(QByteArray bytes)
{
    currentPort.port->write(bytes);
}

void CoDevice::sendHexString(QString *string)
{
    QByteArray bytes = QByteArray().fromHex(string->toLatin1());
    emit bytesSend(bytes.size());
    currentPort.port->write(bytes);
}

void CoDevice::sendTextString(QString *string, QString encoding, LineBreakType linebreak)
{
    if (string->length() == 0)
    {
        return;
    }

    QByteArray bytes = getEncodingCodecFromString(encoding)->fromUnicode(*string);

    switch (linebreak)
    {
    case LineBreakLF:
        bytes.append('\n');
        break;

    case LineBreakCRLF:
        bytes.append('\r');
        bytes.append('\n');
        break;

    default:
        break;
    }

    emit bytesSend(bytes.size());
    currentPort.port->write(bytes);
}

bool CoDevice::config(PortConfig config)
{
    bool ret = false;

    if (config.dataBits >= (int)sizeof(configDataBits) ||
        config.parity >= (int)sizeof(configParity) ||
        config.stopBits >= (int)sizeof(configStopBits) ||
        config.flowControl >= (int)sizeof(configFlowControl))
    {
        return false;
    }

    if (currentPort.port == nullptr)
    {
        qDebug("Serial::config nullptr");
        return false;
    }

    ret = currentPort.port->setBaudRate(config.baudRate);
    if (!ret)
    {
        qWarning() << "Serial::config "
                   << "setBaudRate "
                   << "fail";
        return ret;
    }
    ret = currentPort.port->setDataBits(configDataBits[config.dataBits].param);
    if (!ret)
    {
        qWarning() << "Serial::config "
                   << "setDataBits "
                   << "fail";
        return ret;
    }
    ret = currentPort.port->setParity(configParity[config.parity].param);
    if (!ret)
    {
        qWarning() << "Serial::config "
                   << "setParity "
                   << "fail";
        return ret;
    }
    ret = currentPort.port->setStopBits(configStopBits[config.stopBits].param);
    if (!ret)
    {
        qWarning() << "Serial::config "
                   << "setStopBits "
                   << "fail";
        return ret;
    }
    ret = currentPort.port->setFlowControl(configFlowControl[config.flowControl].param);
    if (!ret)
    {
        qWarning() << "Serial::config "
                   << "setFlowControl "
                   << "fail";
        return ret;
    }

    return true;
}

PortConfig CoDevice::getConfig()
{
    if (!currentPort.port->isOpen())
    {
        return defaultSerialConfig;
    }

    PortConfig config;
    size_t i = 0;

    for (i = 0; i < sizeof(configBaudRate) / sizeof(configBaudRate[0]); i++)
    {
        if (configBaudRate[i].param == currentPort.port->baudRate())
        {
            config.baudRate = i;
            break;
        }
    }
    if (i == (int)(sizeof(configBaudRate) / sizeof(configBaudRate[0])))
    {
        config.baudRate = currentPort.port->baudRate();
    }

    for (i = 0; i < sizeof(configDataBits) / sizeof(configDataBits[0]); i++)
    {
        if (configDataBits[i].param == currentPort.port->dataBits())
        {
            config.dataBits = i;
            break;
        }
    }

    for (i = 0; i < sizeof(configParity) / sizeof(configParity[0]); i++)
    {
        if (configParity[i].param == currentPort.port->parity())
        {
            config.parity = i;
            break;
        }
    }

    for (i = 0; i < sizeof(configStopBits) / sizeof(configStopBits[0]); i++)
    {
        if (configStopBits[i].param == currentPort.port->stopBits())
        {
            config.stopBits = i;
            break;
        }
    }

    for (i = 0; i < sizeof(configFlowControl) / sizeof(configFlowControl[0]); i++)
    {
        if (configFlowControl[i].param == currentPort.port->flowControl())
        {
            config.flowControl = i;
            break;
        }
    }

    return config;
}

QString CoDevice::getDeviceStr(int index)
{
    if (index >= ports.count() ||
        index < 0 ||
        ports.isEmpty())
    {
        return QString("x");
    }

    CoDevice::Port_t port = ports[index];

    if (port.port == nullptr || port.info == nullptr)
    {
        return QString("x");
    }

    auto statusStr = [=]()
    {
        if (port.port->isOpen())
        {
            return QString("●");
        }
        else
        {
            return QString("○");
        }
    };

    QString portStr = QString("%1 %2|%4");

    portStr = portStr.arg(
        statusStr(),
        port.port->portName(),
        port.info->description());

    return portStr;
}

QString CoDevice::getDeviceInfo(int index)
{
    if (index >= ports.count())
    {
        return QString();
    }

    CoDevice::Port_t port = ports[index];

    if (port.port == nullptr || port.info == nullptr)
    {
        return QString();
    }

    QString info = QString("SerialPort");
    bool hasIdentifier = false;

    if (port.info->hasVendorIdentifier())
    {
        hasIdentifier = true;
        info += "-";
        info += QString::number(port.info->vendorIdentifier(), 16).toUpper();
    }

    if (port.info->productIdentifier())
    {
        hasIdentifier = true;
        info += "-";
        info += QString::number(port.info->productIdentifier(), 16).toUpper();
    }

    if (!hasIdentifier)
    {
        info += "-";
        info += port.info->portName();
    }

    return info;
}

void CoDevice::enumDevices()
{
    int index = 0;
    int currentIndex = 0;
    bool portExit = false;
    CoDevice::Port_t lPort;
    QList<Port_t> newPorts;

    mutex.lock();

    for (auto &&comInfo : QSerialPortInfo::availablePorts())
    {
        portExit = false;
        for (auto &&i : ports)
        {
            if (i.port != nullptr && i.port->portName() == comInfo.portName())
            {
                lPort = i;
                portExit = true;
            }
        }

        if (!portExit)
        {
            lPort.port = new QSerialPort(comInfo.portName(), this);
            lPort.info = new QSerialPortInfo(*lPort.port);

            lPort.port->setBaudRate(configBaudRate[defaultSerialConfig.baudRate].param);
        }

        lPort.index = index++;

        if (currentPort.port != nullptr && currentPort.port->portName() == comInfo.portName())
        {
            currentIndex = lPort.index;
        }

        newPorts << lPort;
    }

    /* find no need port and delete */
    for (auto &&i : ports)
    {
        bool delPort = true;
        for (auto &&j : newPorts)
        {
            if (i.port != nullptr && j.port != nullptr &&
                i.port->portName() == j.port->portName())
            {
                delPort = false;
            }
        }

        if (delPort && i.port != nullptr && i.info != nullptr)
        {
            delete (i.info);
            i.port->setParent(nullptr);
            delete (i.port);
        }
    }

    ports = newPorts;

    if (!ports.isEmpty())
    {
        currentPort = ports[currentIndex];
    }

    mutex.unlock();
}

bool CoDevice::setBaudRate(int32_t baudRate)
{
    return currentPort.port->setBaudRate(baudRate);
}

bool CoDevice::setDataBits(int32_t dataBits)
{
    return currentPort.port->setDataBits(configDataBits[dataBits].param);
}

bool CoDevice::setParity(int32_t parity)
{
    return currentPort.port->setParity(configParity[parity].param);
}

bool CoDevice::setStopBits(int32_t stopBits)
{
    return currentPort.port->setStopBits(configStopBits[stopBits].param);
}

bool CoDevice::setFlowControl(int32_t flowControl)
{
    return currentPort.port->setFlowControl(configFlowControl[flowControl].param);
}

qint64 CoDevice::readData(char *data, qint64 maxSize)
{
    deviceDataLock.lock();
    qint64 size = deviceData.size();
    if (maxSize >= size)
    {
        memcpy(data, deviceData.data(), size);
        deviceData.clear();
    }
    else
    {
        memcpy(data, deviceData.data(), maxSize);
        deviceData = QByteArray(deviceData.data() + maxSize, size - maxSize);
        size = maxSize;
    }
    deviceDataLock.unlock();

    return size;
}

qint64 CoDevice::writeData(const char *data, qint64 maxSize)
{
    qint64 size = 0;

    if (currentPort.port != nullptr && currentPort.port->isOpen())
    {
        emit sendRawDataSignal(QByteArray(data, maxSize));
        size = maxSize;
    }

    return size;
}

void CoDevice::currenPort_readyRead()
{
    QByteArray bytes = currentPort.port->read(currentPort.port->bytesAvailable());

    deviceDataLock.lock();
    deviceData.append(bytes);
    deviceDataLock.unlock();

    emit bytesReadyRead(bytes);
}
