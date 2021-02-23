#pragma once

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
