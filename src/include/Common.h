#pragma once

#include <QSerialPort>

enum SerialConfigIndex
{
    IndexBaudRate = 0,
    IndexDataBits,
    IndexParity,
    IndexStopBits,
    IndexFlowControl,
};

enum DataType
{
    TextType = 0,
    HexType,
};

extern const int indexToBaudRate[5];
extern const QSerialPort::DataBits indexToDataBits[4];
extern const QSerialPort::Parity indexToParity[5];
extern const QSerialPort::StopBits indexToStopBits[3];
extern const QSerialPort::FlowControl indexToFlowControl[3];
extern const int defaultSerialConfig[5];

extern const QString successKey[4];
extern const QString warnKey[2];
extern const QString errorKey[3];
