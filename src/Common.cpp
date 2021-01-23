#include <QSerialPort>

#include "Common.h"

const int indexToBaudRate[5] = {9600,
                                38400,
                                57600,
                                115200,
                                921600};

const QSerialPort::DataBits indexToDataBits[4] = {QSerialPort::DataBits::Data5,
                                                 QSerialPort::DataBits::Data6,
                                                 QSerialPort::DataBits::Data7,
                                                 QSerialPort::DataBits::Data8};

const QSerialPort::Parity indexToParity[5] = {QSerialPort::Parity::NoParity,
                                             QSerialPort::Parity::EvenParity,
                                             QSerialPort::Parity::OddParity,
                                             QSerialPort::Parity::SpaceParity,
                                             QSerialPort::Parity::MarkParity};

const QSerialPort::StopBits indexToStopBits[3] = {QSerialPort::StopBits::OneStop,
                                                 QSerialPort::StopBits::OneAndHalfStop,
                                                 QSerialPort::StopBits::TwoStop};

const QSerialPort::FlowControl indexToFlowControl[3] = {QSerialPort::FlowControl::NoFlowControl,
                                                       QSerialPort::FlowControl::HardwareControl,
                                                       QSerialPort::FlowControl::SoftwareControl};

const int defaultSerialConfig[5] = {3, 3, 0, 0, 0};
