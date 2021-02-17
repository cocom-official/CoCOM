#pragma once

#include <QDebug>
#include <QObject>
#include <QList>
#include <QMutex>
#include <QByteArray>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "Common.h"

typedef enum
{
    HEX_TYPE,
    TEXT_TYPE,
} serialDataType;

class Serial : public QObject
{
    Q_OBJECT

    typedef struct
    {
        QSerialPort *port;
        QSerialPortInfo *info;
        int32_t index;
    }Port_t;

    QList<Port_t> ports;
    Port_t currentPort;

public:
    explicit Serial(QObject *parent);
    ~Serial();

    int32_t count();
    void setCurrentPort(int32_t index);
    int currentIndex();
    bool open();
    void close();
    bool isOpen(int32_t index);
    void sendRawData(QByteArray *bytes);
    void sendHexString(QString *bytes);
    void sendTextString(QString *bytes);
    bool config(uint baudrate, uint databits, uint parity, uint stopbits, uint flowControl);
    QString getPortStr(int index);
    void enumPorts();

private:
    QByteArray bytes;
    DataType sendDataType;
    QMutex mutex;

private slots:
    void currenPort_readyRead();

signals:
    void readyRead(int count, QByteArray *bytes);
    void bytesSend(int count);
};
