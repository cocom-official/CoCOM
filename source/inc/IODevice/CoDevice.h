#pragma once

#include <QDebug>
#include <QObject>
#include <QList>
#include <QMutex>
#include <QByteArray>
#include <QTextCodec>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QIODevice>

#include "Common.h"

typedef enum
{
    HEX_TYPE,
    TEXT_TYPE,
} serialDataType;

class CoDevice : public QIODevice
{
    Q_OBJECT

    typedef struct
    {
        uint8_t type;
        QSerialPort *port;
        QSerialPortInfo *info;
        int32_t index;
    } Port_t;

    QList<Port_t> ports;
    Port_t currentPort;

public:
    explicit CoDevice();
    ~CoDevice();

    int32_t count();
    void setCurrentPort(int32_t index);
    int currentIndex();
    int open();
    void close();
    bool isOpen(int32_t index);
    bool config(PortConfig config);
    PortConfig getConfig();
    QString getDeviceStr(int index);
    QString getDeviceInfo(int index);
    void enumDevices();

    /* serial port config */
    bool setBaudRate(int32_t baudRate);
    bool setDataBits(int32_t dataBits);
    bool setParity(int32_t parity);
    bool setStopBits(int32_t stopBits);
    bool setFlowControl(int32_t flowControl);

public slots:
    void sendRawData(QByteArray bytes);
    void sendHexString(QString *bytes);
    void sendTextString(QString *bytes, QString encoding, LineBreakType linebreak);

protected:
    qint64 readData(char *data, qint64 maxSize);
    qint64 writeData(const char *data, qint64 maxSize);

private:
    DataType sendDataType;
    QByteArray deviceData;
    QMutex deviceDataLock;
    QMutex mutex;

private slots:
    void currenPort_readyRead();

signals:
    void bytesReadyRead(QByteArray bytes);
    void bytesSend(int count);
    void sendRawDataSignal(QByteArray bytes);
};
