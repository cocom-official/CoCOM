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
    void on_baudrateComboBox_currentIndexChanged(int index);
    void on_baudrateComboBox_currentTextChanged(const QString &text);

    void on_openAction_toggled(bool checked);

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
        int index;
    }Port_t;

    QList<Port_t> ports;
    Port_t currentPort;

    void setConfigToolBar();
    void setConfigTab();
    void setStatusBar();
    void enumPorts();
    void configPort(QSerialPort *port);
    QString getPortStr(Port_t *port);
};

