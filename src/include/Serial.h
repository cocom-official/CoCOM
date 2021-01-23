#pragma once

#include <QDebug>
#include <QObject>
#include <QList>
#include <QByteArray>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QComboBox>

typedef enum
{
    HEX_TYPE,
    TEXT_TYPE,
} serialDataType;

class Serial : public QObject
{
    typedef struct
    {
        QSerialPort *port;
        QSerialPortInfo *info;
        int32_t index;
    }Port_t;

    QList<Port_t> ports;
    Port_t currentPort;

    Q_OBJECT
public:
    explicit Serial(QObject *parent);
    int32_t count();
    void setCurrentPort(int32_t index);
    int currentIndex();
    bool open();
    void close();
    bool config(uint baudrate, uint databits, uint parity, uint stopbits, uint flowControl);
    QString getPortStr(int index);
    void enumPorts();

private:
    QByteArray bytes;


private slots:
    void currenPort_readyRead();

signals:
    void readyRead(int count, QByteArray *bytes);
};
