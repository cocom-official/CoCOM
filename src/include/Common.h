#pragma once

#include <QString>
#include <QtWidgets>
#include <QSerialPort>
#include <QTextCodec>

/* dummy definitions */
#ifndef COCOM_APPLICATIONNAME
#define COCOM_APPLICATIONNAME ""
#define COCOM_VENDER ""
#define COCOM_HOMEPAGE ""
#define COCOM_VERSION_STRING_WITH_SUFFIX ""
#define COCOM_COMMIT_ID ""
#endif

#define DEFAULT_ENCODING "Local"


#ifdef _WIN32
#define DEFAULT_LINEBREAK_INDEX 2
#elif __APPLE__
#define DEFAULT_LINEBREAK_INDEX 1
#elif __linux__
#define DEFAULT_LINEBREAK_INDEX 1
#elif __unix__
#define DEFAULT_LINEBREAK_INDEX 1
#else
#error "un-supported plantform"
#endif
enum CoCOM_Return
{
    OK = 0,
    E_Null,
    E_Busy,
    E_Invalid,
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

template<class T>
struct ComboBoxConfig
{
    T param;
    QString str;
};

extern const ComboBoxConfig<int> configBaudRate[5];
extern const ComboBoxConfig<QSerialPort::DataBits> configDataBits[4];
extern const ComboBoxConfig<QSerialPort::Parity> configParity[5];
extern const ComboBoxConfig<QSerialPort::StopBits> configStopBits[3];
extern const ComboBoxConfig<QSerialPort::FlowControl> configFlowControl[3];

typedef struct
{
    int baudRate;
    int dataBits;
    int parity;
    int stopBits;
    int flowControl;
} PortConfig;

extern const PortConfig defaultSerialConfig;

extern const QString successKey[4];
extern const QString warnKey[2];
extern const QString errorKey[3];

extern void changeObjectSize(const QObject &o, double objectRate);
extern QTextCodec *getEncodingCodecFromString(QString encoding);

class MouseButtonSignaler : public QObject
{
    Q_OBJECT

private:
    bool eventFilter(QObject *obj, QEvent *ev) Q_DECL_OVERRIDE
    {
        if ((ev->type() == QEvent::MouseButtonPress ||
             ev->type() == QEvent::MouseButtonRelease ||
             ev->type() == QEvent::MouseButtonDblClick) &&
            obj->isWidgetType())
            emit mouseButtonEvent(static_cast<QWidget *>(obj),
                                  static_cast<QMouseEvent *>(ev));
        return false;
    };

public:
    explicit MouseButtonSignaler(QObject *parent = 0) : QObject(parent){};
    ~MouseButtonSignaler(){};

    void installOn(QWidget *widget)
    {
        widget->installEventFilter(this);
    };

signals:
    void mouseButtonEvent(QWidget *, QMouseEvent *);
};
