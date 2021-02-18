#include "MainWindow.h"
#include "ui_mainWindow.h"
#include "Common.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      portSelect(new QComboBox(this)),
      statusInfoLabel(new QLabel(this)),
      statusTxLabel(new QLabel(this)),
      statusRxLabel(new QLabel(this)),
      baudrateComboBox(new QComboBox(this)),
      dataBitsComboBox(new QComboBox(this)),
      parityComboBox(new QComboBox(this)),
      stopBitsComboBox(new QComboBox(this)),
      flowComboBox(new QComboBox(this)),
      rxTypeComboBox(new QComboBox(this)),
      txTypeComboBox(new QComboBox(this)),
      lineBreakBox(new QComboBox(this)),
      encodingBox(new QComboBox(this)),
      serial(new Serial(this)),
      timer(new QTimer(this))
{
    ui->setupUi(this);
    textBrowser = new TextBrowser(this, ui->outputTextBrowser);

    setConfigToolBar();

    setStatusBar();

    loadFont();

    connect(portSelect, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &MainWindow::portSelectComboBox_currentIndexChanged);

    inputTabWidgetHeight
        << ui->basicSendTab->minimumSizeHint().height()
        << ui->multiLineSendTab->minimumSizeHint().height()
        << ui->commandTab->minimumSizeHint().height();

    ui->multiLineSendTab->setMaximumHeight(inputTabWidgetHeight[0]);
    ui->commandTab->setMaximumHeight(inputTabWidgetHeight[0]);
    ui->horizontalSplitter->setSizes(QList<int>() << 700 << 1);

    connect(serial, &Serial::readyRead, this, &MainWindow::serial_readyRead);
    connect(serial, &Serial::bytesSend, this, &MainWindow::serial_bytesSend);

    enumPorts();
    connect(timer, &QTimer::timeout, this, &MainWindow::enumPorts);
    timer->start(1000);

// #ifdef Q_OS_WIN32
#if 0
    bool color;
    qDebug() << QtWin::colorizationColor(&color);
    qDebug() << color;
#endif
    setStatusInfo(tr("Ready"));
}

MainWindow::~MainWindow()
{
    delete ui;
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
}

void MainWindow::setConfigToolBar()
{
    portSelect->setMinimumContentsLength(20);
    portSelect->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToMinimumContentsLengthWithIcon);
    ui->configToolBar->addWidget(portSelect);
}

void MainWindow::setStatusBar()
{
    statusTxLabel->setText("Tx: 0");
    statusRxLabel->setText("Rx: 0");

    baudrateComboBox->addItem("9600");
    baudrateComboBox->addItem("38400");
    baudrateComboBox->addItem("57600");
    baudrateComboBox->addItem("115200");
    baudrateComboBox->addItem("921600");
    baudrateComboBox->addItem("Custom");
    baudrateComboBox->setCurrentIndex(defaultSerialConfig[IndexBaudRate]);
    baudrateComboBox->setToolTip(tr("Baudrate"));

    dataBitsComboBox->addItem("5");
    dataBitsComboBox->addItem("6");
    dataBitsComboBox->addItem("7");
    dataBitsComboBox->addItem("8");
    dataBitsComboBox->setCurrentIndex(defaultSerialConfig[IndexDataBits]);
    dataBitsComboBox->setToolTip(tr("Data Bits"));

    parityComboBox->addItem("None");
    parityComboBox->addItem("Even");
    parityComboBox->addItem("Odd");
    parityComboBox->addItem("Space");
    parityComboBox->addItem("Mark");
    parityComboBox->setCurrentIndex(defaultSerialConfig[IndexParity]);
    parityComboBox->setToolTip(tr("Parity"));

    stopBitsComboBox->addItem("1");
    stopBitsComboBox->addItem("1.5");
    stopBitsComboBox->addItem("2");
    stopBitsComboBox->setCurrentIndex(defaultSerialConfig[IndexStopBits]);
    stopBitsComboBox->setToolTip(tr("Stop Bits"));

    flowComboBox->addItem("OFF");
    flowComboBox->addItem("RTX/CTX");
    flowComboBox->addItem("XON/XOFF");
    flowComboBox->setCurrentIndex(defaultSerialConfig[IndexFlowControl]);
    flowComboBox->setToolTip(tr("Flow Control"));

    rxTypeComboBox->addItem("Text");
    rxTypeComboBox->addItem("HEX");
    rxTypeComboBox->setCurrentIndex(0);
    rxTypeComboBox->setToolTip(tr("Rx Type"));

    txTypeComboBox->addItem("Text");
    txTypeComboBox->addItem("HEX");
    txTypeComboBox->setCurrentIndex(0);
    txTypeComboBox->setToolTip(tr("Tx Type"));

    lineBreakBox->addItem("OFF");
    lineBreakBox->addItem("LF");
    lineBreakBox->addItem("CRLF");
    lineBreakBox->setCurrentIndex(2);
    lineBreakBox->setToolTip(tr("Line Break"));

    encodingBox->addItem("Local");
    encodingBox->addItem("UTF-8");
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
    ui->statusbar->addWidget(statusTxLabel, 1);
    ui->statusbar->addWidget(statusRxLabel, 1);
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
    connect(encodingBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &MainWindow::encodingBox_currentIndexChanged);
}

void MainWindow::updatePortSelectText()
{
    for (int i = 0; i < portSelect->count(); i++)
    {
        portSelect->setItemText(i, serial->getPortStr(i));
    }

    if (ui->openAction->isChecked() && serial->isOpen(serial->currentIndex()) != true)
    {
        ui->openAction->setChecked(false);
    }
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
    bool ret = serial->config(baudrateComboBox->currentText().toInt(),
                              dataBitsComboBox->currentIndex(),
                              parityComboBox->currentIndex(),
                              stopBitsComboBox->currentIndex(),
                              flowComboBox->currentIndex());
    if (!ret)
    {
        qDebug() << "port config fail";
    }
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

    statusTxLabel->setText(QString("Tx: %1").arg(QString::number(txCount)));
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

    statusTxLabel->setText(QString("Tx: %1").arg(QString::number(rxCount)));
}

void MainWindow::setStatusInfo(QString text)
{
    statusInfoLabel->setText(text);
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
    if (text == "Custom")
    {
        baudrateComboBox->setCurrentText("");
    }
    updatePortConfig();
}

void MainWindow::dataBitsComboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    updatePortConfig();
}

void MainWindow::parityComboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    updatePortConfig();
}

void MainWindow::stopBitsComboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    updatePortConfig();
}

void MainWindow::flowComboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    updatePortConfig();
}

void MainWindow::rxTypeComboBox_currentIndexChanged(int index)
{
    textBrowser->setDataType((DataType)index);
}

void MainWindow::encodingBox_currentIndexChanged(int index)
{
    textBrowser->setEncoding((EncodingType)index);
}

void MainWindow::on_openAction_toggled(bool checked)
{
    if (checked)
    {
        if (!serial->open())
        {
            ui->openAction->setChecked(false);
            QMessageBox::critical(this, tr("Open Failed!"), tr("Serial Port [%1] Open Failed!").arg(serial->getPortStr(serial->currentIndex())));
        }
    }
    else
    {
        serial->close();
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

void MainWindow::on_actionPin_toggled(bool checked)
{
    if (checked)
    {
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        ui->actionPin->setIcon(QIcon(":/assets/icons/pushpin-ed.svg"));
    }
    else
    {
        setWindowFlags(windowFlags() ^ Qt::WindowStaysOnTopHint);
        ui->actionPin->setIcon(QIcon(":/assets/icons/pushpin.svg"));
    }

    show();
}

void MainWindow::on_basicSendButton_pressed()
{
    QString string = ui->sendComboBox->currentText();

    if (txTypeComboBox->currentIndex() == TextType)
    {
        serial->sendTextString(&string, (EncodingType)encodingBox->currentIndex(), (LineBreakType)lineBreakBox->currentIndex());
    }
    else
    {
        serial->sendHexString(&string);
    }
}

void MainWindow::on_multiSendButton_pressed()
{
    QString string = ui->sendTextEdit->toPlainText();

    if (txTypeComboBox->currentIndex() == TextType)
    {
        serial->sendTextString(&string, (EncodingType)encodingBox->currentIndex(), (LineBreakType)lineBreakBox->currentIndex());
    }
    else
    {
        serial->sendHexString(&string);
    }
}

void MainWindow::portSelectComboBox_currentIndexChanged(int index)
{
    serial->setCurrentPort(index);
    updatePortSelectText();
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
