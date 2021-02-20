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

#ifdef Q_OS_WIN32
#include <QtWinExtras>
#endif

#include "Serial.h"
#include "TextBrowser.h"

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
    void encodingBox_currentIndexChanged(int index);

    void on_openAction_toggled(bool checked);
    void on_clearAction_triggered(bool checked);
    void on_actionPin_toggled(bool checked);

    void on_textSendButton_pressed();
    void on_commandLineSendButton_pressed();

    void portSelectComboBox_currentIndexChanged(int index);
    void serial_readyRead(int count, QByteArray *bytes);
    void serial_bytesSend(int count);

private:
    Ui::MainWindow *ui;
    float dpiScaling;

    QComboBox *portSelect;
    QLabel *statusInfoLabel;
    QLabel *statusTxLabel;
    QLabel *statusRxLabel;
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

    QList<int> inputTabWidgetHeight;

    Serial *serial;
    TextBrowser *textBrowser;

    QTimer *timer;

    void setupUI();
    void setLayout(double rate);
    void setupSerialPort();
    void refreshDPI();
    void loadFont();
    void setConfigToolBar();
    void setStatusBar();
    void updatePortSelectText();
    void enumPorts();
    void updatePortConfig();
    void addTxCount(int count);
    void addRxCount(int count);
    void setStatusInfo(QString text);

    /* event */
    void moveEvent(QMoveEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
};
