#pragma once

#include <QDebug>
#include <QLayout>
#include <QMainWindow>
#include <QTranslator>
#include <QLocale>
#include <QString>
#include <QTabWidget>
#include <QComboBox>
#include <QAbstractItemView>
#include <QLabel>
#include <QPushButton>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QTextDocument>
#include <QTextCursor>
#include <QScrollBar>
#include <QTimer>
#include <QMessageBox>
#include <QScreen>
#include <QWindow>
#include <QFileDialog>
#include <QFile>
#include <QIODevice>
#include <QDateTime>

#ifdef Q_OS_WIN32
#include <QtWinExtras>
#endif

#include <QHotkey>

#include "ConfigDialog.h"
#include "Serial.h"
#include "TextBrowser.h"
#include "CommandsTab.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setStatusInfo(QString text);

private slots:
    void on_inputTabWidget_currentChanged(int index);

    void on_outputTextBrowser_cursorPositionChanged();

    void baudrateComboBox_currentTextChanged(const QString &text);
    void baudrateComboBox_currentIndexChanged(int index);
    void dataBitsComboBox_currentIndexChanged(int index);
    void parityComboBox_currentIndexChanged(int index);
    void stopBitsComboBox_currentIndexChanged(int index);
    void flowComboBox_currentIndexChanged(int index);
    void rxTypeComboBox_currentIndexChanged(int index);
    void txTypeComboBox_currentIndexChanged(int index);
    void encodingBox_currentIndexChanged(int index);

    void on_openAction_toggled(bool checked);
    void on_clearAction_triggered(bool checked);
    void on_goDownAction_triggered(bool checked);
    //
    void on_saveToFileAction_triggered(bool checked);
    void on_pinAction_toggled(bool checked);
    void on_configAction_triggered(bool checked);

    void on_textSendButton_pressed();
    void on_commandLineSendButton_pressed();

    void on_periodicSendCheckBox_stateChanged(int state);
    void on_timerPeriodSpinBox_valueChanged(int value);

    void statusLabel_mouseButtonEvent(QWidget *obj, QMouseEvent *event);

    void showHotkey_activated();

    void sendText(QString text);

    void portSelectComboBox_currentIndexChanged(int index);
    void serial_readyRead(int count, QByteArray *bytes);
    void serial_bytesSend(int count);


private:
    Ui::MainWindow *ui;
    float dpiScaling;

    QComboBox *portSelect;

    QTabWidget *multiCommandsTab;

    QLabel *statusInfoLabel;
    QLabel *statusTxLabel;
    QLabel *statusRxLabel;
    MouseButtonSignaler *statueLabelSignaler;

    /* status bar combobox */
    QComboBox *baudrateComboBox;
    QComboBox *dataBitsComboBox;
    QComboBox *parityComboBox;
    QComboBox *stopBitsComboBox;
    QComboBox *flowComboBox;
    QComboBox *rxTypeComboBox;
    QComboBox *txTypeComboBox;
    QComboBox *lineBreakBox;
    QComboBox *encodingBox;

    ConfigDialog *configDialog;

    Serial *serial;
    TextBrowser *textBrowser;

    QTimer *timer;
    QTimer *periodicSendTimer;

    int  lineBreakType;

    void setupUI();
    void setLayout(double rate);
    void setupSerialPort();
    void setInputTabWidget();
    void refreshDPI();
    void loadFont();
    void setConfigToolBar();
    void setStatusBar();
    void updatePortSelectText();
    void updatePortsConfigComboBox();
    void enumPorts();
    void updatePortConfig();
    void periodicSend();
    void addTxCount(int count);
    void addRxCount(int count);

    /* event */
    void moveEvent(QMoveEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
};
