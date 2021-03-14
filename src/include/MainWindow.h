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
#include <QResizeEvent>
#include <QSettings>
#include <CommandSettings.h>
#include <QDir>
#include <QShortcut>

#ifdef Q_OS_WIN32
#include <QtWinExtras>
#endif

#include <QHotkey>

#include "ConfigDialog.h"
#include "Serial.h"
#include "TextBrowser.h"
#include "CommandsTab.h"
#include "GlobalSettings.h"
#include "CommandSettings.h"

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
    void findResultChanged(int cur, int all);

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

    void newMultiCommandTabButton_clicked();
    void multiCommandTabBar_clicked(int index);
    void multiCommandTabBar_doubleclicked(int index);
    void multiCommandTabWidget_currentChanged(int index);
    void globalSetting_onSaved();
    void restoreDefaultSettings();
    void onRestart();

    /* find toolbar */
    void findEdit_textChanged(const QString &text);
    void findCloseButton_clicked();
    void findNextButton_clicked();
    void findPrivButton_clicked();

    void statusLabel_mouseButtonEvent(QWidget *obj, QMouseEvent *event);
    void findToolbar_escapeKeyEvent(QWidget *obj, QKeyEvent *event);
    void findToolbar_enterKeyEvent(QWidget *obj, QKeyEvent *event);

    void showHotkey_activated();

    void sendText(QString text);

    void portSelectComboBox_currentIndexChanged(int index);
    void serial_readyRead(int count, QByteArray *bytes);
    void serial_bytesSend(int count);

private:
    Ui::MainWindow *ui;
    float dpiScaling;

    bool tabBarClicked;
    bool restoreSettings;
    QHotkey *showHideShortcut;
    QShortcut *clearOutputShortcut;
    QShortcut *scrollToEndShortcut;
    QTranslator *translator;

    QComboBox *portSelect;
    QToolBar *findToolBar;
    QLineEdit *findEdit;
    QLabel *findResultLabel;
    QPushButton *findNextButton;
    QPushButton *findPrivButton;
    QPushButton *findCloseButton;

    QTabWidget *multiCommandsTab;

    QLabel *statusInfoLabel;
    QLabel *statusTxLabel;
    QLabel *statusRxLabel;
    MouseButtonSignaler *statueLabelSignaler;
    EscapeKeySignaler *findToolbarEscapeSignaler;
    EnterKeySignaler *findToolbarEnterSignaler;

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

    QSystemTrayIcon *trayIcon;

    ConfigDialog *configDialog;

    Serial *serial;
    TextBrowser *textBrowser;
    GlobalSettings *globalSettings;
    CommandSettings *commandSettings;

    QTimer *timer;
    QTimer *periodicSendTimer;

    int lineBreakType;

    /* UI */
    void setupUI();
    void setLayout(double rate);
    void setInputTabWidget();
    void setDarkStyle(bool dark);
    void refreshDPI();
    void loadFont();
    void loadLanguage();
    void setToolBar();
    void setStatusBar();
    void updatePortSelectText();
    void updatePortsConfigComboBox();
    void moveFindToolBar(bool force = false, QPoint moveOffset = QPoint(0, 0));
    void sendToastMessage(QString msg, int level = InfoLevel, int index = 0);
    CommandsTab *addMultiCommandTab();

    /* no UI */
    void readSettings();
    void writeSettings();
    void setupSerialPort();
    void enumPorts();
    void updatePortConfig();
    void periodicSend();
    void addTxCount(int count);
    void addRxCount(int count);

    /* event */
    void moveEvent(QMoveEvent *event);
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
};
