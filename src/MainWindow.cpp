#include "MainWindow.h"
#include "ui_mainWindow.h"
#include "Common.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      dpiScaling(1.0),
      portSelect(new QComboBox(this)),
      statusInfoLabel(new QLabel(this)),
      statusTxLabel(new QLabel(this)),
      statusRxLabel(new QLabel(this)),
      statueLabelSignaler(new MouseButtonSignaler(this)),
      baudrateComboBox(new QComboBox(this)),
      dataBitsComboBox(new QComboBox(this)),
      parityComboBox(new QComboBox(this)),
      stopBitsComboBox(new QComboBox(this)),
      flowComboBox(new QComboBox(this)),
      rxTypeComboBox(new QComboBox(this)),
      txTypeComboBox(new QComboBox(this)),
      lineBreakBox(new QComboBox(this)),
      encodingBox(new QComboBox(this)),
      configDialog(nullptr),
      serial(new Serial(this)),
      timer(new QTimer(this)),
      periodicSendTimer(new QTimer(this)),
      lineBreakType(LineBreakOFF)
{
    QCoreApplication::setApplicationName(QString(COCOM_APPLICATIONNAME));
    QCoreApplication::setOrganizationName(QString(COCOM_VENDER));
    QCoreApplication::setOrganizationDomain(QString(COCOM_HOMEPAGE));

    setupUI();

    setupSerialPort();

    QHotkey *hotkey = new QHotkey(QKeySequence("Ctrl+Alt+Z"), true);
    qDebug() << "hotkey Is Registered: " << hotkey->isRegistered();

    connect(hotkey, &QHotkey::activated,
            this, &MainWindow::showHotkey_activated);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    ui->setupUi(this);
    textBrowser = new TextBrowser(this, ui->outputTextBrowser);

    setWindowTitle(QString(COCOM_APPLICATIONNAME) + " -- " + tr("Serial Port Utility"));

    setConfigToolBar();

    setStatusBar();

    loadFont();

    connect(portSelect, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &MainWindow::portSelectComboBox_currentIndexChanged);

    connect(periodicSendTimer, &QTimer::timeout, this, &MainWindow::periodicSend);

    inputTabWidgetHeight
        << ui->sendTab->minimumSizeHint().height()
        << ui->commandLineTab->minimumSizeHint().height()
        << ui->multiCommandTab->minimumSizeHint().height();

    ui->commandLineTab->setMaximumHeight(inputTabWidgetHeight[0]);
    ui->multiCommandTab->setMaximumHeight(inputTabWidgetHeight[0]);

    refreshDPI();

    on_openAction_toggled(false);

    /* current disable */
    ui->multiCommandTab->setEnabled(false);
    ui->multiCommandTab->setToolTip(tr("Not Implement Current!"));

    setStatusInfo(tr("Ready"));
}

void MainWindow::setLayout(double rate)
{
    int defaultMargin = 4;

    ui->centralVerticalLayout->setMargin(defaultMargin * rate);
    ui->sendTabHorizontalLayout->setMargin(defaultMargin * rate);
    ui->commandLineTabHorizontalLayout->setMargin(defaultMargin * rate);
    ui->multiCommandTabGridLayout->setMargin(defaultMargin * rate);
    ui->sendTabVerticalLayout->setMargin(0);

    ui->centralVerticalLayout->setSpacing(defaultMargin * rate);
    ui->sendTabHorizontalLayout->setSpacing(defaultMargin * rate);
    ui->commandLineTabHorizontalLayout->setSpacing(defaultMargin * rate);
    ui->multiCommandTabGridLayout->setSpacing(defaultMargin * rate);
    ui->sendTabVerticalLayout->setSpacing(defaultMargin * rate);
}

void MainWindow::setupSerialPort()
{
    connect(serial, &Serial::readyRead, this, &MainWindow::serial_readyRead);
    connect(serial, &Serial::bytesSend, this, &MainWindow::serial_bytesSend);
    enumPorts();
    connect(timer, &QTimer::timeout, this, &MainWindow::enumPorts);
    timer->start(1000);
}

void MainWindow::refreshDPI()
{
    QScreen *screen = nullptr;
    static QScreen *lastScreen = nullptr;
    QWindow *window = this->window()->windowHandle();

    if (window != nullptr)
    {
        screen = window->screen();
    }
    else
    {
        screen = QApplication::primaryScreen();
    }

    if (screen == nullptr || lastScreen == screen)
    {
        return;
    }

    lastScreen = screen;

    qreal dpi = screen->logicalDotsPerInch();

    double objectRate = dpi / 96.0 / dpiScaling;
    dpiScaling = dpi / 96.0;

    ui->buttosToolBar->setIconSize(ui->buttosToolBar->iconSize() * objectRate);
    setLayout(dpiScaling);

    // changeObjectSize(*this, objectRate);
    // resize(width() * objectRate, height() * objectRate);
}

void MainWindow::loadFont()
{
    QFile fontFile(":/assets/fonts/CascadiaCodePL.ttf");
    if (!fontFile.open(QIODevice::ReadOnly))
    {
        qDebug("open fontFile fail");
        return;
    }

    int fontId = QFontDatabase::addApplicationFontFromData(fontFile.readAll());
    if (fontId == -1)
    {
        qDebug("load fontFile fail");
        return;
    }

    QStringList fontFamily = QFontDatabase::applicationFontFamilies(fontId);
    if (fontFamily.empty())
    {
        qDebug("fontFile empty");
        return;
    }

    QFont font(fontFamily.at(0));
    ui->outputTextBrowser->setFont(font);
    ui->sendTextEdit->setFont(font);
    ui->commandLineSendComboBox->setFont(font);
    statusInfoLabel->setFont(font);
}

void MainWindow::setConfigToolBar()
{
    portSelect->setMinimumContentsLength(20);
    portSelect->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToMinimumContentsLengthWithIcon);
    ui->configToolBar->addWidget(portSelect);
}

void MainWindow::setStatusBar()
{
    statusTxLabel->setToolTip(tr("Double Click to Clear Count!"));
    statusRxLabel->setToolTip(tr("Double Click to Clear Count!"));

    addTxCount(-1);
    addRxCount(-1);

    statueLabelSignaler->installOn(statusTxLabel);
    statueLabelSignaler->installOn(statusRxLabel);
    statueLabelSignaler->installOn(statusInfoLabel);
    connect(statueLabelSignaler, &MouseButtonSignaler::mouseButtonEvent,
            this, &MainWindow::statusLabel_mouseButtonEvent);

    for (size_t i = 0; i < sizeof(configBaudRate) / sizeof(configBaudRate[0]); i++)
    {
        baudrateComboBox->addItem(configBaudRate[i].str);
    }
    baudrateComboBox->addItem("Custom");
    baudrateComboBox->setCurrentIndex(defaultSerialConfig.baudRate);
    baudrateComboBox->setToolTip(tr("Baudrate"));

    for (size_t i = 0; i < sizeof(configDataBits) / sizeof(configDataBits[0]); i++)
    {
        dataBitsComboBox->addItem(configDataBits[i].str);
    }
    dataBitsComboBox->setCurrentIndex(defaultSerialConfig.dataBits);
    dataBitsComboBox->setToolTip(tr("Data Bits"));

    for (size_t i = 0; i < sizeof(configParity) / sizeof(configParity[0]); i++)
    {
        parityComboBox->addItem(configParity[i].str);
    }
    parityComboBox->setCurrentIndex(defaultSerialConfig.parity);
    parityComboBox->setToolTip(tr("Parity"));

    for (size_t i = 0; i < sizeof(configStopBits) / sizeof(configStopBits[0]); i++)
    {
        stopBitsComboBox->addItem(configStopBits[i].str);
    }
    stopBitsComboBox->setCurrentIndex(defaultSerialConfig.stopBits);
    stopBitsComboBox->setToolTip(tr("Stop Bits"));

    for (size_t i = 0; i < sizeof(configFlowControl) / sizeof(configFlowControl[0]); i++)
    {
        flowComboBox->addItem(configFlowControl[i].str);
    }
    flowComboBox->setCurrentIndex(defaultSerialConfig.flowControl);
    flowComboBox->setToolTip(tr("Flow Control"));
    flowComboBox->setItemData(0, tr("Flow Control OFF"), Qt::ToolTipRole);
    flowComboBox->setItemData(1, tr("Hardware Flow Control"), Qt::ToolTipRole);
    flowComboBox->setItemData(2, tr("Software Flow Control"), Qt::ToolTipRole);

    rxTypeComboBox->addItem("Text");
    rxTypeComboBox->addItem("Hex");
    rxTypeComboBox->setCurrentIndex(0);
    rxTypeComboBox->setToolTip(tr("Rx Type"));

    txTypeComboBox->addItem("Text");
    txTypeComboBox->addItem("Hex");
    txTypeComboBox->setCurrentIndex(0);
    txTypeComboBox->setToolTip(tr("Tx Type"));

    lineBreakBox->addItem("OFF");
    lineBreakBox->addItem("LF");
    lineBreakBox->addItem("CRLF");
    lineBreakBox->setCurrentIndex(DEFAULT_LINEBREAK_INDEX);
    lineBreakBox->setToolTip(tr("Line Break"));

    encodingBox->addItem("Local");
    encodingBox->addItem("UTF-8");
    encodingBox->addItem("Latin1");
    encodingBox->setCurrentIndex(0);
    encodingBox->setToolTip(tr("Encoding"));

    QWidget *statusWidget = new QWidget(this);
    QHBoxLayout *statusLayout = new QHBoxLayout(this);
    statusLayout->setMargin(0);
    statusLayout->addWidget(baudrateComboBox);
    statusLayout->addWidget(dataBitsComboBox);
    statusLayout->addWidget(parityComboBox);
    statusLayout->addWidget(stopBitsComboBox);
    statusLayout->addWidget(flowComboBox);
    statusLayout->addWidget(rxTypeComboBox);
    statusLayout->addWidget(txTypeComboBox);
    statusLayout->addWidget(lineBreakBox);
    statusLayout->addWidget(encodingBox);
    statusWidget->setLayout(statusLayout);

    ui->statusbar->addWidget(statusWidget, 0);
    ui->statusbar->addWidget(statusRxLabel, 1);
    ui->statusbar->addWidget(statusTxLabel, 1);
    ui->statusbar->addWidget(statusInfoLabel, 8);

    connect(baudrateComboBox, &QComboBox::currentTextChanged,
            this, &MainWindow::baudrateComboBox_currentTextChanged);
    connect(baudrateComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &MainWindow::baudrateComboBox_currentIndexChanged);
    connect(dataBitsComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &MainWindow::dataBitsComboBox_currentIndexChanged);
    connect(parityComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &MainWindow::parityComboBox_currentIndexChanged);
    connect(stopBitsComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &MainWindow::stopBitsComboBox_currentIndexChanged);
    connect(flowComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &MainWindow::flowComboBox_currentIndexChanged);
    connect(rxTypeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &MainWindow::rxTypeComboBox_currentIndexChanged);
    connect(txTypeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &MainWindow::txTypeComboBox_currentIndexChanged);
    connect(encodingBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &MainWindow::encodingBox_currentIndexChanged);
}

void MainWindow::updatePortSelectText()
{
    for (int i = 0; i < portSelect->count(); i++)
    {
        QString portStr = serial->getPortStr(i);
        portSelect->setItemText(i, portStr);
        portSelect->setItemData(i, portStr.mid(2), Qt::ToolTipRole);
    }
    portSelect->setToolTip(serial->getPortStr(0).mid(2));

    if (ui->openAction->isChecked() && serial->isOpen(serial->currentIndex()) != true)
    {
        ui->openAction->setChecked(false);
    }
}

void MainWindow::updatePortsConfigComboBox()
{
    PortConfig config = serial->getConfig();

    portSelect->blockSignals(true);
    baudrateComboBox->blockSignals(true);
    dataBitsComboBox->blockSignals(true);
    parityComboBox->blockSignals(true);
    stopBitsComboBox->blockSignals(true);
    flowComboBox->blockSignals(true);

    if (config.baudRate < (int)(sizeof(configBaudRate) / sizeof(configBaudRate[0])))
    {
        baudrateComboBox->setCurrentIndex(config.baudRate);
    }
    else
    {
        baudrateComboBox->setCurrentIndex(sizeof(configBaudRate) / sizeof(configBaudRate[0]));
        baudrateComboBox->setCurrentText(QString::number(config.baudRate));
    }

    dataBitsComboBox->setCurrentIndex(config.dataBits);
    parityComboBox->setCurrentIndex(config.parity);
    stopBitsComboBox->setCurrentIndex(config.stopBits);
    flowComboBox->setCurrentIndex(config.flowControl);

    baudrateComboBox->blockSignals(false);
    dataBitsComboBox->blockSignals(false);
    parityComboBox->blockSignals(false);
    stopBitsComboBox->blockSignals(false);
    flowComboBox->blockSignals(false);
    portSelect->blockSignals(false);
}

void MainWindow::enumPorts()
{
    QAbstractItemView *view = portSelect->view();
    if (view != nullptr && view->isVisible())
    {
        return;
    }

    serial->enumPorts();

    portSelect->blockSignals(true);

    while (portSelect->count() != 0)
    {
        portSelect->removeItem(portSelect->count() - 1);
    }

    for (int i = 0; i < serial->count(); i++)
    {
        portSelect->addItem(serial->getPortStr(i));
    }

    portSelect->blockSignals(false);

    portSelect->setCurrentIndex(serial->currentIndex());
}

void MainWindow::updatePortConfig()
{
    PortConfig config;
    config.baudRate = baudrateComboBox->currentText().toInt();
    config.dataBits = dataBitsComboBox->currentIndex();
    config.parity = parityComboBox->currentIndex();
    config.stopBits = stopBitsComboBox->currentIndex();
    config.flowControl = flowComboBox->currentIndex();
    bool ret = serial->config(config);
    if (!ret)
    {
        qDebug() << "port config fail";
    }
}

void MainWindow::periodicSend()
{
    on_textSendButton_pressed();
}

void MainWindow::addTxCount(int count)
{
    static int txCount = 0;

    if (count < 0)
    {
        txCount = 0;
    }
    else
    {
        txCount += count;
    }

    statusTxLabel->setText(QString(" Tx: %1 ").arg(QString::number(txCount)));
}

void MainWindow::addRxCount(int count)
{
    static int rxCount = 0;

    if (count < 0)
    {
        rxCount = 0;
    }
    else
    {
        rxCount += count;
    }

    statusRxLabel->setText(QString(" Rx: %1 ").arg(QString::number(rxCount)));
}

void MainWindow::setStatusInfo(QString text)
{
    text = "->" + text;

    QFontMetrics fontWidth(statusInfoLabel->font());
    QString elideText = fontWidth.elidedText(text, Qt::ElideRight, statusInfoLabel->width());

    if (elideText.isEmpty())
    {
        elideText = text;
    }

    statusInfoLabel->setToolTip(text);
    statusInfoLabel->setText(elideText);
}

void MainWindow::moveEvent(QMoveEvent *event)
{
    Q_UNUSED(event);
    refreshDPI();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return;

    if (event->key() == Qt::Key_Enter ||
        event->key() == Qt::Key_Return)
    {
        if (ui->sendTextEdit->hasFocus())
        {
            on_textSendButton_pressed();
        }
        else if (ui->commandLineSendComboBox->hasFocus() && ui->openAction->isChecked())
        {
            on_commandLineSendButton_pressed();
            ui->commandLineSendComboBox->setCurrentText("");
        }
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
        return;
}

void MainWindow::on_inputTabWidget_currentChanged(int index)
{
    int maxHeight = inputTabWidgetHeight[index];

    for (int i = 0; i < ui->inputTabWidget->count(); i++)
    {
        if (ui->inputTabWidget->widget(i) != ui->inputTabWidget->currentWidget())
        {
            ui->inputTabWidget->widget(i)->setMaximumHeight(maxHeight);
        }
        else
        {
            ui->inputTabWidget->currentWidget()->setMaximumHeight(0xFFFFFF);
        }
    }
}

void MainWindow::on_outputTextBrowser_cursorPositionChanged()
{
}

void MainWindow::baudrateComboBox_currentIndexChanged(int index)
{
    static int baudrateCount = 0;

    if (0 == baudrateCount)
    {
        baudrateCount = baudrateComboBox->count();
    }

    if (index == baudrateCount)
    {
        QString baudrate = baudrateComboBox->currentText();
        baudrateComboBox->removeItem(baudrateCount);
        baudrateComboBox->setCurrentIndex(baudrateCount - 1);
        baudrateComboBox->setCurrentText(baudrate);
        return;
    }

    if (index == baudrateCount - 1)
    {
        baudrateComboBox->setEditable(true);
        baudrateComboBox->setEditText("");
    }
    else
    {
        baudrateComboBox->setEditable(false);
    }
}

void MainWindow::baudrateComboBox_currentTextChanged(const QString &text)
{
    static QString lastText;
    if (text == "Custom")
    {
        baudrateComboBox->setCurrentText("");
    }
    else
    {
        if (!serial->setBaudRate(text.toInt()))
        {
            setStatusInfo(tr("set") + " " + tr("BaudRate") + " " + tr("fail") + "!");
            baudrateComboBox->setCurrentText(lastText);
        }
        else
        {
            lastText = text;
        }
    }
}

void MainWindow::dataBitsComboBox_currentIndexChanged(int index)
{
    static int lastIndex = 0;
    if (!serial->setDataBits(index))
    {
        setStatusInfo(tr("set") + " " + tr("DataBits") + " " + tr("fail") + "!");
        dataBitsComboBox->setCurrentIndex(lastIndex);
    }
    else
    {
        lastIndex = index;
    }
}

void MainWindow::parityComboBox_currentIndexChanged(int index)
{
    static int lastIndex = 0;
    if (!serial->setParity(index))
    {
        setStatusInfo(tr("set") + " " + tr("Parity") + " " + tr("fail") + "!");
        parityComboBox->setCurrentIndex(lastIndex);
    }
    else
    {
        lastIndex = index;
    }
}

void MainWindow::stopBitsComboBox_currentIndexChanged(int index)
{
    static int lastIndex = 0;
    if (!serial->setStopBits(index))
    {
        setStatusInfo(tr("set") + " " + tr("StopBits") + " " + tr("fail") + "!");
        stopBitsComboBox->setCurrentIndex(lastIndex);
    }
    else
    {
        lastIndex = index;
    }
}

void MainWindow::flowComboBox_currentIndexChanged(int index)
{
    static int lastIndex = 0;
    if (!serial->setFlowControl(index))
    {
        setStatusInfo(tr("set") + " " + tr("FlowControl") + " " + tr("fail") + "!");
        flowComboBox->setCurrentIndex(lastIndex);
    }
    else
    {
        lastIndex = index;
    }
}

void MainWindow::rxTypeComboBox_currentIndexChanged(int index)
{
    textBrowser->setDataType((DataType)index);
}

void MainWindow::txTypeComboBox_currentIndexChanged(int index)
{
    if (index == HexType)
    {
        lineBreakType = lineBreakBox->currentIndex();
        lineBreakBox->setDisabled(true);
        lineBreakBox->setCurrentIndex(LineBreakOFF);
    }
    else
    {
        lineBreakBox->setCurrentIndex(lineBreakType);
        lineBreakBox->setDisabled(false);
    }
}

void MainWindow::encodingBox_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    textBrowser->setEncoding(encodingBox->currentText());
}

void MainWindow::on_openAction_toggled(bool checked)
{
    QString portName = serial->getPortStr(serial->currentIndex()).split("|")[0].mid(2);
    QString portDesc = serial->getPortStr(serial->currentIndex()).split("|")[1];
    if (checked)
    {
        int ret = serial->open();

        if (ret != OK)
        {
            QString messageBoxTitle = tr("Open Failed!");
            QString comma = tr(",");
            ui->openAction->setChecked(false);

            switch (ret)
            {
            case E_Busy:
                QMessageBox::critical(this, messageBoxTitle, portName + " " + portDesc + " " + tr("Busy") + comma + messageBoxTitle);
                break;

            case E_Null:
                QMessageBox::critical(this, messageBoxTitle, portName + " " + portDesc + " " + tr("Null") + comma + messageBoxTitle);
                break;

            default:
                QMessageBox::critical(this, messageBoxTitle, portName + " " + portDesc + " " + tr("Unknow Reason") + comma + messageBoxTitle);
                break;
            }

            return;
        }

        setStatusInfo(tr("Open") + " " + portName + " " + portDesc);

        ui->openAction->setIcon(QIcon(":/assets/icons/pause.svg"));
        ui->textSendButton->setEnabled(true);
        ui->commandLineSendButton->setEnabled(true);
        ui->periodicSendCheckBox->setEnabled(true);
    }
    else
    {
        serial->close();

        setStatusInfo(tr("Close") + " " + portName + " " + portDesc);

        ui->openAction->setIcon(QIcon(":/assets/icons/play.svg"));
        ui->textSendButton->setEnabled(false);
        ui->commandLineSendButton->setEnabled(false);
        ui->periodicSendCheckBox->setEnabled(false);
        ui->periodicSendCheckBox->setChecked(false);
    }

    updatePortSelectText();
}

void MainWindow::on_clearAction_triggered(bool checked)
{
    Q_UNUSED(checked);
    textBrowser->clear();
    addRxCount(-1);
    addTxCount(-1);
}

void MainWindow::on_goDownAction_triggered(bool checked)
{
    Q_UNUSED(checked);
    ui->outputTextBrowser->verticalScrollBar()->setValue(
        ui->outputTextBrowser->verticalScrollBar()->maximum());
}

void MainWindow::on_saveToFileAction_triggered(bool checked)
{
    Q_UNUSED(checked);

    QDateTime dateTime(QDateTime::currentDateTime());
    QString selectedFile = dateTime.toString("yyyy_MM_dd_hhmm");

    QString path = QFileDialog::getSaveFileName(this,
                                                tr("Save Output To File"),
                                                selectedFile + ".txt",
                                                tr("Text files (*.txt)"));

    if (path.isEmpty())
    {
        return;
    }

    QFile file(path);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        file.write(getEncodingCodecFromString(
                       encodingBox->currentText())
                       ->fromUnicode(
                           ui->outputTextBrowser->document()->toPlainText()));
        file.close();
    }
}

void MainWindow::on_pinAction_toggled(bool checked)
{
    if (checked)
    {
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        ui->pinAction->setIcon(QIcon(":/assets/icons/pushpin-ed.svg"));
    }
    else
    {
        setWindowFlags(windowFlags() ^ Qt::WindowStaysOnTopHint);
        ui->pinAction->setIcon(QIcon(":/assets/icons/pushpin.svg"));
    }

    show();
}

void MainWindow::on_configAction_triggered(bool checked)
{
    Q_UNUSED(checked);

    if (nullptr == configDialog)
    {
        configDialog = new ConfigDialog(this);
    }

    configDialog->show();
}

void MainWindow::on_commandLineSendButton_pressed()
{
    if (!ui->openAction->isChecked())
    {
        return;
    }

    QString string = ui->commandLineSendComboBox->currentText();

    if (txTypeComboBox->currentIndex() == TextType)
    {
        serial->sendTextString(&string, encodingBox->currentText(), (LineBreakType)lineBreakBox->currentIndex());
    }
    else
    {
        serial->sendHexString(&string);
    }
}

void MainWindow::on_textSendButton_pressed()
{
    if (!ui->openAction->isChecked())
    {
        return;
    }

    QString string = ui->sendTextEdit->toPlainText();

    if (txTypeComboBox->currentIndex() == TextType)
    {
        serial->sendTextString(&string, encodingBox->currentText(), (LineBreakType)lineBreakBox->currentIndex());
    }
    else
    {
        serial->sendHexString(&string);
    }
}

void MainWindow::on_periodicSendCheckBox_stateChanged(int state)
{
    if (state == Qt::Checked)
    {
        periodicSendTimer->start(ui->timerPeriodSpinBox->value());
    }
    else
    {
        periodicSendTimer->stop();
    }
}

void MainWindow::on_timerPeriodSpinBox_valueChanged(int value)
{
    if (value > 0)
    {
        periodicSendTimer->setInterval(value);
    }
    else
    {
        periodicSendTimer->stop();
    }
}

void MainWindow::portSelectComboBox_currentIndexChanged(int index)
{
    serial->setCurrentPort(index);
    updatePortSelectText();
    updatePortsConfigComboBox();
}

void MainWindow::statusLabel_mouseButtonEvent(QWidget *obj, QMouseEvent *event)
{
    obj = static_cast<QLabel *>(obj);
    if (event->type() == QEvent::MouseButtonDblClick)
    {
        if (obj == statusTxLabel)
        {
            addTxCount(-1);
        }
        else if (obj == statusRxLabel)
        {
            addRxCount(-1);
        }
        else if (obj == statusInfoLabel)
        {
            setStatusInfo(tr("Ready"));
        }
    }
}

void MainWindow::showHotkey_activated()
{
    if (isActiveWindow())
    {
        showMinimized();
    }
    else
    {
        showNormal();
        raise();
        activateWindow();
        raise();
        QApplication::setActiveWindow(this);
        raise();
    }
}

void MainWindow::serial_readyRead(int count, QByteArray *bytes)
{
    addRxCount(count);
    textBrowser->insertData(bytes);
}

void MainWindow::serial_bytesSend(int count)
{
    addTxCount(count);
}
