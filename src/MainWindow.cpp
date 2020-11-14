#include "MainWindow.h"
#include "ui_mainWindow.h"

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
      rxComboBox(new QComboBox(this)),
      txComboBox(new QComboBox(this))
{
    ui->setupUi(this);

    setConfigToolBar();

    setStatusBar();

    connect(portSelect, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &MainWindow::on_portSelectComboBox_currentIndexChanged);

    inputTabWidgetHeight
        << ui->basicSendTab->minimumSizeHint().height()
        << ui->multiLineSendTab->minimumSizeHint().height()
        << ui->commandTab->minimumSizeHint().height();

    ui->multiLineSendTab->setMaximumHeight(inputTabWidgetHeight[0]);
    ui->commandTab->setMaximumHeight(inputTabWidgetHeight[0]);
    ui->horizontalSplitter->setSizes(QList<int>() << 700 << 1);

    enumPorts();

#ifdef Q_OS_WIN32
    bool color;
    qDebug() << QtWin::colorizationColor(&color);
    qDebug() << color;
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
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
    baudrateComboBox->addItem("19200");
    baudrateComboBox->addItem("38400");
    baudrateComboBox->addItem("57600");
    baudrateComboBox->addItem("115200");
    baudrateComboBox->addItem("921600");
    baudrateComboBox->addItem("Custom");
    baudrateComboBox->setCurrentIndex(4);
    baudrateComboBox->setToolTip(tr("baudrate"));

    dataBitsComboBox->addItem("5");
    dataBitsComboBox->addItem("6");
    dataBitsComboBox->addItem("7");
    dataBitsComboBox->addItem("8");
    dataBitsComboBox->setCurrentIndex(3);
    dataBitsComboBox->setToolTip(tr("data bits"));

    parityComboBox->addItem("None");
    parityComboBox->addItem("Even");
    parityComboBox->addItem("Odd");
    parityComboBox->addItem("Space");
    parityComboBox->addItem("Mark");
    parityComboBox->setCurrentIndex(0);
    parityComboBox->setToolTip(tr("parity"));

    stopBitsComboBox->addItem("1");
    stopBitsComboBox->addItem("1.5");
    stopBitsComboBox->addItem("2");
    stopBitsComboBox->setCurrentIndex(0);
    stopBitsComboBox->setToolTip(tr("stop bits"));

    flowComboBox->addItem("OFF");
    flowComboBox->addItem("RTX/CTX");
    flowComboBox->addItem("XON/XOFF");
    flowComboBox->setCurrentIndex(0);
    flowComboBox->setToolTip(tr("flow control"));

    rxComboBox->addItem("Rx: Text");
    rxComboBox->addItem("Rx: HEX");
    rxComboBox->setCurrentIndex(0);
    rxComboBox->setToolTip(tr("Rx"));

    txComboBox->addItem("Tx: Text");
    txComboBox->addItem("Tx: HEX");
    txComboBox->setCurrentIndex(0);
    txComboBox->setToolTip(tr("Tx"));

    QWidget *statusWidget = new QWidget(this);
    QHBoxLayout *statusLayout = new QHBoxLayout(this);
    statusLayout->setMargin(0);
    statusLayout->addWidget(baudrateComboBox);
    statusLayout->addWidget(dataBitsComboBox);
    statusLayout->addWidget(parityComboBox);
    statusLayout->addWidget(stopBitsComboBox);
    statusLayout->addWidget(flowComboBox);
    statusLayout->addWidget(rxComboBox);
    statusLayout->addWidget(txComboBox);
    statusWidget->setLayout(statusLayout);

    ui->statusbar->addWidget(statusWidget, 0);
    ui->statusbar->addWidget(statusTxLabel, 1);
    ui->statusbar->addWidget(statusRxLabel, 1);
    ui->statusbar->addWidget(statusInfoLabel, 8);

    connect(baudrateComboBox, &QComboBox::currentTextChanged,
            this, baudrateComboBox_currentTextChanged);
    connect(baudrateComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, baudrateComboBox_currentIndexChanged);
    connect(dataBitsComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, dataBitsComboBox_currentIndexChanged);
    connect(parityComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, parityComboBox_currentIndexChanged);
    connect(stopBitsComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, stopBitsComboBox_currentIndexChanged);
    connect(flowComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, flowComboBox_currentIndexChanged);
    // connect(rxComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
    //         this, nullptr);
    // connect(txComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
    //         this, nullptr);
}

void MainWindow::enumPorts()
{
    if (ports.count())
    {
        qDebug() << "reenum Ports";

        for (int i = 0; i < ports.count(); i++)
        {
            portSelect->setItemText(i, getPortStr(&ports[i]));
        }

        return;
    }

    MainWindow::Port_t *port = new MainWindow::Port_t();
    int index = 0;

    for (auto &&comInfo : QSerialPortInfo::availablePorts())
    {
        port->port = new QSerialPort(comInfo.portName(), this);
        port->info = new QSerialPortInfo(*port->port);
        port->text = new QTextDocument(this);
        port->index = index++;

        configPort(port->port);
        portSelect->addItem(getPortStr(port));

        ports << *port;
    }

    on_portSelectComboBox_currentIndexChanged(portSelect->currentIndex());
}

void MainWindow::configPort(QSerialPort *port)
{
    QSerialPort::DataBits indexToDataBits[] = {QSerialPort::DataBits::Data5,
                                               QSerialPort::DataBits::Data6,
                                               QSerialPort::DataBits::Data7,
                                               QSerialPort::DataBits::Data8};

    QSerialPort::Parity indexToParity[] = {QSerialPort::Parity::NoParity,
                                           QSerialPort::Parity::EvenParity,
                                           QSerialPort::Parity::OddParity,
                                           QSerialPort::Parity::SpaceParity,
                                           QSerialPort::Parity::MarkParity};

    QSerialPort::StopBits indexToStopBits[] = {QSerialPort::StopBits::OneStop,
                                               QSerialPort::StopBits::OneAndHalfStop,
                                               QSerialPort::StopBits::TwoStop};

    QSerialPort::FlowControl indexToFlowControl[] = {QSerialPort::FlowControl::NoFlowControl,
                                                     QSerialPort::FlowControl::HardwareControl,
                                                     QSerialPort::FlowControl::SoftwareControl};

    port->setBaudRate(baudrateComboBox->currentText().toInt());
    port->setDataBits(indexToDataBits[dataBitsComboBox->currentIndex()]);
    port->setParity(indexToParity[parityComboBox->currentIndex()]);
    port->setStopBits(indexToStopBits[stopBitsComboBox->currentIndex()]);
    port->setFlowControl(indexToFlowControl[flowComboBox->currentIndex()]);
}

void MainWindow::updatePortConfig()
{
    if (!ports.isEmpty() &&
        currentPort.port)
    {
        configPort(currentPort.port);
    }
}

QString MainWindow::getPortStr(MainWindow::Port_t *port)
{
    auto statusStr = [=]() {
        if (port->port->isOpen())
        {
            return "●";
        }
        else
        {
            return "○";
        }
    };

    QString portStr = QString("%1 %2|%4");

    portStr = portStr.arg(
        statusStr(),
        port->port->portName(),
        port->info->description());

    return portStr;
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
    qDebug() << "on_outputTextBrowser_cursorPositionChanged";
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

void MainWindow::on_openAction_toggled(bool checked)
{
    if (checked)
    {
        currentPort.port->open(QIODevice::OpenModeFlag::ReadWrite);
        connect(currentPort.port, &QSerialPort::readyRead,
                this, &MainWindow::on_currenPort_readyRead);
    }
    else
    {
        currentPort.port->close();
        currentPort.text->clear();
    }

    enumPorts();
}

void MainWindow::on_clearAction_toggled(bool checked)
{
    qDebug() << "clear";
    Q_UNUSED(checked);
    ui->outputTextBrowser->clear();
}

void MainWindow::on_portSelectComboBox_currentIndexChanged(int index)
{
    if (index >= ports.count()|| index < 0)
    {
        return;
    }

    currentPort = ports[index];
}

void MainWindow::on_currenPort_readyRead()
{
    QTextCursor cursor(currentPort.text);
    cursor.movePosition(QTextCursor::End);
    cursor.beginEditBlock();
    cursor.insertText(QString(currentPort.port->read(
        currentPort.port->bytesAvailable())));
    cursor.endEditBlock();

    qDebug() << ui->outputTextBrowser->cursor();

    int scrollBarValue = ui->outputTextBrowser->verticalScrollBar()->value();
    Q_UNUSED(scrollBarValue);
    ui->outputTextBrowser->setPlainText(currentPort.text->toPlainText());
    ui->outputTextBrowser->moveCursor(QTextCursor::End);
}
