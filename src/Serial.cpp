#include "Serial.h"
#include "Common.h"

Serial::Serial(QObject *parent)
    : QObject(parent),
      sendDataType(TextType)
{
    enumPorts();
}

int32_t Serial::count()
{
    return ports.size();
}

void Serial::setCurrentPort(int32_t index)
{
    if (index < 0 || index > ports.size())
    {
        return;
    }

    currentPort = ports[index];
}

int Serial::currentIndex()
{
    return currentPort.index;
}

bool Serial::open()
{
    bool ret = currentPort.port->open(QIODevice::OpenModeFlag::ReadWrite);
    connect(currentPort.port, &QSerialPort::readyRead,
            this, &Serial::currenPort_readyRead, Qt::QueuedConnection);

    return ret;
}

void Serial::close()
{
    currentPort.port->close();
}

void Serial::sendRawData(QByteArray *bytes)
{
    currentPort.port->write(*bytes);
}

void Serial::sendHexString(QString *string)
{
}

void Serial::sendTextString(QString *string)
{
    if (string->length() == 0)
    {
        return;
    }

    QByteArray bytes = string->toUtf8();
    emit bytesSend(bytes.size());
    bytes.append('\r');
    bytes.append('\n');
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
    Serial::Port_t *port = new Serial::Port_t();
    int index = 0;

    for (auto &&comInfo : QSerialPortInfo::availablePorts())
    {
        port->port = new QSerialPort(comInfo.portName(), this);
        port->info = new QSerialPortInfo(*port->port);
        port->index = index++;

        port->port->setBaudRate(indexToBaudRate[defaultSerialConfig[IndexBaudRate]]);

        ports << *port;
    }
}

void Serial::currenPort_readyRead()
{
    int rxCount = currentPort.port->bytesAvailable();
    QByteArray bytes = currentPort.port->read(rxCount);

    emit readyRead(rxCount, &bytes);
}
