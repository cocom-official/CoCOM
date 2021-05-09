#pragma once

#include <QString>
#include <QtWidgets>
#include <QSerialPort>
#include <QTextCodec>
#include <QTextCharFormat>

/* dummy definitions */
#ifndef COCOM_APPLICATIONNAME
#define COCOM_APPLICATIONNAME ""
#define COCOM_VENDER ""
#define COCOM_HOMEPAGE ""
#define COCOM_VERSION_STRING ""
#define COCOM_VERSION_STRING_WITH_SUFFIX ""
#define COCOM_SHORT_COMMIT_ID ""
#define COCOM_LONG_COMMIT_ID ""
#define COCOM_PORTABLE_FILE_NAME ""
#define COCOM_SOURCE_DIR ""
#endif

#ifdef DEBUG
#define INI_NAME_SUFFIX "_Debug"
#define README_FILE_PATH QString(COCOM_SOURCE_DIR "/README.md")
#define LUA_SCRIPTS_PATH QString(COCOM_SOURCE_DIR "/scripts")
#else
#define INI_NAME_SUFFIX ""
#define README_FILE_PATH QString(QCoreApplication::applicationDirPath() + QString("/README.md"))
#define LUA_SCRIPTS_PATH QString(QCoreApplication::applicationDirPath() + QString("/scripts"))
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

enum DeviceType
{
    SerialDevice = 0,
    USBDevice,
    ADCDevices,
};

enum LineBreakType
{
    LineBreakOFF = 0,
    LineBreakLF,
    LineBreakCRLF,
};

enum MessageLevelType
{
    InfoLevel = 0,
    WarningLevel,
    CriticalLevel,
};

template <class T>
struct ComboBoxConfig
{
    T param;
    QString str;
};

extern const QString langs[2];

extern const int restartExitCode;

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

/* format */
extern QTextCharFormat *getcharFormatHighlite();
extern QTextCharFormat *getCharFormatHighliteSelect();

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
        {
            emit mouseButtonEvent(static_cast<QWidget *>(obj),
                                  static_cast<QMouseEvent *>(ev));
        }
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

class EnterKeySignaler : public QObject
{
    Q_OBJECT

private:
    bool eventFilter(QObject *obj, QEvent *ev) Q_DECL_OVERRIDE
    {
        if ((ev->type() == QEvent::KeyPress &&
             ((static_cast<QKeyEvent *>(ev))->key() == Qt::Key_Enter ||
              (static_cast<QKeyEvent *>(ev))->key() == Qt::Key_Return)) &&
            obj->isWidgetType())
        {
            emit enterKeyEvent(static_cast<QWidget *>(obj),
                               static_cast<QKeyEvent *>(ev));
        }
        return false;
    };

public:
    explicit EnterKeySignaler(QObject *parent = 0) : QObject(parent){};
    ~EnterKeySignaler(){};

    void installOn(QWidget *widget)
    {
        widget->installEventFilter(this);
    };

signals:
    void enterKeyEvent(QWidget *, QKeyEvent *);
};

class EscapeKeySignaler : public QObject
{
    Q_OBJECT

private:
    bool eventFilter(QObject *obj, QEvent *ev) Q_DECL_OVERRIDE
    {
        if ((ev->type() == QEvent::KeyPress &&
             (static_cast<QKeyEvent *>(ev))->key() == Qt::Key_Escape) &&
            obj->isWidgetType())
        {
            emit escapeKeyEvent(static_cast<QWidget *>(obj),
                                static_cast<QKeyEvent *>(ev));
        }
        return false;
    };

public:
    explicit EscapeKeySignaler(QObject *parent = 0) : QObject(parent){};
    ~EscapeKeySignaler(){};

    void installOn(QWidget *widget)
    {
        widget->installEventFilter(this);
    };

signals:
    void escapeKeyEvent(QWidget *, QKeyEvent *);
};
