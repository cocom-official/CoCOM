#include "Serial.h"
#include "Common.h"

Serial::Serial(QObject *parent)
    : QObject(parent),
      sendDataType(TextType)
{
    currentPort.index = -1;
    currentPort.info = nullptr;
    currentPort.port = nullptr;

    enumPorts();
}

Serial::~Serial()
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

int32_t Serial::count()
{
    return ports.count();
}

void Serial::setCurrentPort(int32_t index)
{
    if (currentPort.index == index || index < 0 || index > ports.count())
    {
        return;
    }

    Serial::Port_t lastPort = currentPort;

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

int Serial::currentIndex()
{
    return currentPort.index;
}

bool Serial::open()
{
    if (currentPort.port == nullptr || currentPort.info == nullptr ||
        currentPort.info->isBusy() || currentPort.info->isNull())
    {
        qDebug() << "serial port open fail";
        return false;
    }

    bool ret = currentPort.port->open(QIODevice::OpenModeFlag::ReadWrite);

    if (ret)
    {
        connect(currentPort.port, &QSerialPort::readyRead,
                this, &Serial::currenPort_readyRead, Qt::QueuedConnection);
    }

    return ret;
}

void Serial::close()
{
    disconnect(currentPort.port);
    currentPort.port->close();
}

bool Serial::isOpen(int32_t index)
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

void Serial::sendRawData(QByteArray *bytes)
{
    currentPort.port->write(*bytes);
}

void Serial::sendHexString(QString *string)
{
    QByteArray bytes = QByteArray().fromHex(string->toLatin1());
    emit bytesSend(bytes.size());
    currentPort.port->write(bytes);
}

void Serial::sendTextString(QString *string, EncodingType encodingType, LineBreakType linebreak)
{
    if (string->length() == 0)
    {
        return;
    }

    QTextCodec *codec = nullptr;

    switch (encodingType)
    {
    case UTF_8:
        codec = QTextCodec::codecForName("UTF-8");
        break;

    default:
        codec = QTextCodec::codecForLocale();
        break;
    }

    QByteArray bytes = codec->fromUnicode(*string);

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

bool Serial::config(uint baudrate, uint databits, uint parity, uint stopbits, uint flowControl)
{
    if (databits >= sizeof(indexToDataBits) ||
        parity >= sizeof(indexToParity) ||
        stopbits >= sizeof(indexToStopBits) ||
        flowControl >= sizeof(indexToFlowControl))
    {
        return false;
    }

    if (currentPort.port == nullptr)
    {
        qDebug("Serial::config nullptr");
        return false;
    }

    currentPort.port->setBaudRate(baudrate);
    currentPort.port->setDataBits(indexToDataBits[databits]);
    currentPort.port->setParity(indexToParity[parity]);
    currentPort.port->setStopBits(indexToStopBits[stopbits]);
    currentPort.port->setFlowControl(indexToFlowControl[flowControl]);

    return true;
}

QString Serial::getPortStr(int index)
{
    Serial::Port_t port = ports[index];

    if (port.port == nullptr || port.info == nullptr)
    {
        return "x";
    }

    auto statusStr = [=]() {
        if (port.port->isOpen())
        {
            return "●";
        }
        else
        {
            return "○";
        }
    };

    QString portStr = QString("%1 %2|%4");

    portStr = portStr.arg(
        statusStr(),
        port.port->portName(),
        port.info->description());

    return portStr;
}

void Serial::enumPorts()
{
    int index = 0;
    int currentIndex = 0;
    bool portExit = false;
    Serial::Port_t lPort;
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

            lPort.port->setBaudRate(indexToBaudRate[defaultSerialConfig[IndexBaudRate]]);
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

    currentPort = ports[currentIndex];

    mutex.unlock();
}

void Serial::currenPort_readyRead()
{
    int rxCount = currentPort.port->bytesAvailable();
    QByteArray bytes = currentPort.port->read(rxCount);

    emit readyRead(rxCount, &bytes);
}
