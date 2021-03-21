#include <QWidget>
#include <QSerialPort>

#include "Common.h"

extern const QString langs[2] = {
    "zh_CN",
    "en",
};

const int restartExitCode = 777;

const ComboBoxConfig<int> configBaudRate[] = {
    {9600, "9600"},
    {38400, "38400"},
    {57600, "57600"},
    {115200, "115200"},
    {921600, "921600"},
};

const ComboBoxConfig<QSerialPort::DataBits> configDataBits[] = {
    {QSerialPort::DataBits::Data5, "5"},
    {QSerialPort::DataBits::Data6, "6"},
    {QSerialPort::DataBits::Data7, "7"},
    {QSerialPort::DataBits::Data8, "8"},
};

const ComboBoxConfig<QSerialPort::Parity> configParity[] = {
    {QSerialPort::Parity::NoParity, "None"},
    {QSerialPort::Parity::EvenParity, "Even"},
    {QSerialPort::Parity::OddParity, "Odd"},
    {QSerialPort::Parity::SpaceParity, "Space"},
    {QSerialPort::Parity::MarkParity, "Mark"},
};

const ComboBoxConfig<QSerialPort::StopBits> configStopBits[] = {
    {QSerialPort::StopBits::OneStop, "1"},
    {QSerialPort::StopBits::OneAndHalfStop, "1.5"},
    {QSerialPort::StopBits::TwoStop, "2"},
};

const ComboBoxConfig<QSerialPort::FlowControl> configFlowControl[] = {
    {QSerialPort::FlowControl::NoFlowControl, "OFF"},
    {QSerialPort::FlowControl::HardwareControl, "HFC"},
    {QSerialPort::FlowControl::SoftwareControl, "SFC"},
};

const PortConfig defaultSerialConfig = {3, 3, 0, 0, 0};

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
            qDebug() << "Encoding: " << encoding << "NOT Found!"
                     << "use Local";
            codec = QTextCodec::codecForLocale();
        }
    }

    return codec;
}

QTextCharFormat *getcharFormatHighlite()
{
    QTextCharFormat *charFormatHighlite = nullptr;

    if (charFormatHighlite == nullptr)
    {
        charFormatHighlite = new QTextCharFormat();
        charFormatHighlite->setBackground(QBrush(QColor(237, 184, 154)));
    }

    return charFormatHighlite;
}

QTextCharFormat *getCharFormatHighliteSelect()
{
    static QTextCharFormat *charFormatHighliteSelect = nullptr;

    if (charFormatHighliteSelect == nullptr)
    {
        charFormatHighliteSelect = new QTextCharFormat();

        charFormatHighliteSelect->setBackground(QBrush(Qt::yellow));
        charFormatHighliteSelect->setFontUnderline(true);
        charFormatHighliteSelect->setFontWeight(charFormatHighliteSelect->fontWeight() * 2);
    }

    return charFormatHighliteSelect;
}
