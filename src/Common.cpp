#include <QWidget>
#include <QSerialPort>

#include "Common.h"

const int indexToBaudRate[5] = {
    9600,
    38400,
    57600,
    115200,
    921600,
};

const QSerialPort::DataBits indexToDataBits[4] = {
    QSerialPort::DataBits::Data5,
    QSerialPort::DataBits::Data6,
    QSerialPort::DataBits::Data7,
    QSerialPort::DataBits::Data8,
};

const QSerialPort::Parity indexToParity[5] = {
    QSerialPort::Parity::NoParity,
    QSerialPort::Parity::EvenParity,
    QSerialPort::Parity::OddParity,
    QSerialPort::Parity::SpaceParity,
    QSerialPort::Parity::MarkParity,
};

const QSerialPort::StopBits indexToStopBits[3] = {
    QSerialPort::StopBits::OneStop,
    QSerialPort::StopBits::OneAndHalfStop,
    QSerialPort::StopBits::TwoStop,
};

const QSerialPort::FlowControl indexToFlowControl[3] = {
    QSerialPort::FlowControl::NoFlowControl,
    QSerialPort::FlowControl::HardwareControl,
    QSerialPort::FlowControl::SoftwareControl,
};

const int defaultSerialConfig[5] = {3, 3, 0, 0, 0};

const QString successKey[4] = {
    "success",
    "ok",
    "pass",
    "successfully",
};

const QString warnKey[2] = {
    "warn",
    "warning",
};

const QString errorKey[3] = {
    "error",
    "fail",
    "failed",
};

void changeObjectSize(const QObject &o, double objectRate)
{
    for (int i = 0; i < o.children().size(); ++i)
    {
        QWidget *pWidget = qobject_cast<QWidget *>(o.children().at(i));
        if (pWidget != nullptr)
        {
            pWidget->setGeometry(pWidget->x() * objectRate, pWidget->y() * objectRate,
                                 pWidget->width() * objectRate, pWidget->height() * objectRate);
            changeObjectSize(*(o.children().at(i)), objectRate);
        }
    }
}

QTextCodec *getEncodingCodecFromString(QString encoding)
{
    QTextCodec *codec = nullptr;

    if (encoding == DEFAULT_ENCODING)
    {
        codec = QTextCodec::codecForLocale();
    }
    else
    {
        codec = QTextCodec::codecForName(encoding.toUtf8());

        if (codec == 0)
        {
            qDebug() << "Encoding: " << encoding << "NOT Found!" << "use Local";
            codec = QTextCodec::codecForLocale();
        }
    }

    return codec;
}
