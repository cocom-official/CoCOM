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

    void baudrateComboBox_currentTextChanged(const QString &text);
    void baudrateComboBox_currentIndexChanged(int index);
    void dataBitsComboBox_currentIndexChanged(int index);
    void parityComboBox_currentIndexChanged(int index);
    void stopBitsComboBox_currentIndexChanged(int index);
    void flowComboBox_currentIndexChanged(int index);

    void on_openAction_toggled(bool checked);
    void on_clearAction_triggered(bool checked);
    void on_actionPin_toggled(bool checked);

    void portSelectComboBox_currentIndexChanged(int index);
    void currenPort_readyRead();

private:
    Ui::MainWindow *ui;

    QComboBox *portSelect;
    QLabel  *statusInfoLabel;
    QLabel  *statusTxLabel;
    QLabel  *statusRxLabel;
    /* status bar combobox */
    QComboBox *baudrateComboBox;
    QComboBox *dataBitsComboBox;
    QComboBox *parityComboBox;
    QComboBox *stopBitsComboBox;
    QComboBox *flowComboBox;
    QComboBox *rxComboBox;
    QComboBox *txComboBox;

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
    void setStatusBar();
    void enumPorts();
    void configPort(QSerialPort *port);
    void updatePortConfig();
    QString getPortStr(Port_t *port);
    void addTxCount(int count);
    void addRxCount(int count);
};

