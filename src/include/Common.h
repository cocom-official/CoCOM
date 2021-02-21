#pragma once

#include <QSerialPort>

/* dummy definitions */
#ifndef COCOM_APPLICATIONNAME
#define COCOM_APPLICATIONNAME ""
#define COCOM_VENDER ""
#define COCOM_HOMEPAGE ""
#define COCOM_VERSION_STRING_WITH_SUFFIX ""
#endif

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

enum LineBreakType
{
    LineBreakOFF = 0,
    LineBreakLF,
    LineBreakCRLF,
};

enum EncodingType
{
    LocalEnc = 0,
    UTF_8,
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

extern void changeObjectSize(const QObject &o, double objectRate);
