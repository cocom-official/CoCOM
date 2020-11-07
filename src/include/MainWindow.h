#pragma once

#include <QDebug>
#include <QLayout>
#include <QMainWindow>
#include <QTranslator>
#include <QLocale>
#include <QString>
#include <QTabWidget>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QSplitter>
#include <QWidget>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTextDocument>
#include <QTextCursor>
#include <QScrollBar>

#ifdef Q_OS_WIN32
#include <QtWinExtras>
#endif

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_inputTabWidget_currentChanged(int index);

    void on_outputTextBrowser_cursorPositionChanged();

    void on_baudrateComboBox_currentTextChanged(const QString &text);
    void on_baudrateComboBox_currentIndexChanged(int index);
    void on_dataBitsComboBox_currentIndexChanged(int index);
    void on_parityComboBox_currentIndexChanged(int index);
    void on_stopBitsComboBox_currentIndexChanged(int index);
    void on_flowComboBox_currentIndexChanged(int index);

    void on_openAction_toggled(bool checked);
    void on_clearAction_toggled(bool checked);

    void on_portSelectComboBox_currentIndexChanged(int index);
    void on_currenPort_readyRead();

private:
    Ui::MainWindow *ui;

    QComboBox *portSelect;
    QLabel  *statusInfoLeftLabel;
    QLabel  *statusTxLabel;
    QLabel  *statusRxLabel;
    QLabel  *statusInfoRightLabel;

    QList<int> inputTabWidgetHeight;

    typedef struct
    {
        QSerialPort *port;
        QSerialPortInfo *info;
        QTextDocument *text;
        int index;
    }Port_t;

    QList<Port_t> ports;
    Port_t currentPort;

    void setConfigToolBar();
    void setConfigTab();
    void setStatusBar();
    void enumPorts();
    void configPort(QSerialPort *port);
    void updatePortConfig();
    QString getPortStr(Port_t *port);
};

