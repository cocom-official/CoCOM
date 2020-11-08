#include "MainWindow.h"
#include "ui_mainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      portSelect(new QComboBox(this)),
      statusInfoLeftLabel(new QLabel(this)),
      statusTxLabel(new QLabel(this)),
      statusRxLabel(new QLabel(this)),
      statusInfoRightLabel(new QLabel(this))
{
    ui->setupUi(this);

    setConfigToolBar();

    setConfigTab();

    setStatusBar();

    connect(portSelect, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &MainWindow::on_portSelectComboBox_currentIndexChanged);

    ui->verticalSplitter->setSizes(QList<int>() << 2000 << 1);

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

void MainWindow::setConfigTab()
{
    ui->baudrateComboBox->addItem("9600");
    ui->baudrateComboBox->addItem("19200");
    ui->baudrateComboBox->addItem("38400");
    ui->baudrateComboBox->addItem("57600");
    ui->baudrateComboBox->addItem("115200");
    ui->baudrateComboBox->addItem("921600");
    ui->baudrateComboBox->addItem(tr("Custom"));
    ui->baudrateComboBox->setCurrentIndex(0);
    ui->baudrateComboBox->setEditable(false);

    ui->dataBitsComboBox->addItem("5");
    ui->dataBitsComboBox->addItem("6");
    ui->dataBitsComboBox->addItem("7");
    ui->dataBitsComboBox->addItem("8");
    ui->dataBitsComboBox->setCurrentIndex(3);

    ui->parityComboBox->addItem("None");
    ui->parityComboBox->addItem("Even");
    ui->parityComboBox->addItem("Odd");
    ui->parityComboBox->addItem("Space");
    ui->parityComboBox->addItem("Mark");
    ui->parityComboBox->setCurrentIndex(0);

    ui->stopBitsComboBox->addItem("1");
    ui->stopBitsComboBox->addItem("1.5");
    ui->stopBitsComboBox->addItem("2");
    ui->stopBitsComboBox->setCurrentIndex(0);

    ui->flowComboBox->addItem("OFF");
    ui->flowComboBox->addItem("RTX/CTX");
    ui->flowComboBox->addItem("XON/XOFF");
    ui->flowComboBox->setCurrentIndex(0);

    int minWidth = ui->flowComboBox->width();

    ui->baudrateComboBox->setFixedWidth(minWidth);
    ui->dataBitsComboBox->setFixedWidth(minWidth);
    ui->parityComboBox->setFixedWidth(minWidth);
    ui->stopBitsComboBox->setFixedWidth(minWidth);
    ui->flowComboBox->setFixedWidth(minWidth);
}

void MainWindow::setStatusBar()
{
    statusTxLabel->setText("Tx: 0");
    statusRxLabel->setText("Rx: 0");

    ui->statusbar->addWidget(statusInfoLeftLabel, 2);
    ui->statusbar->addWidget(statusTxLabel, 1);
    ui->statusbar->addWidget(statusRxLabel, 1);
    ui->statusbar->addWidget(statusInfoRightLabel, 3);
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

    port->setBaudRate(ui->baudrateComboBox->currentText().toInt());
    port->setDataBits(indexToDataBits[ui->dataBitsComboBox->currentIndex()]);
    port->setParity(indexToParity[ui->parityComboBox->currentIndex()]);
    port->setStopBits(indexToStopBits[ui->stopBitsComboBox->currentIndex()]);
    port->setFlowControl(indexToFlowControl[ui->flowComboBox->currentIndex()]);
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

void MainWindow::on_baudrateComboBox_currentIndexChanged(int index)
{
    if (index == ui->baudrateComboBox->count() - 1)
    {
        ui->baudrateComboBox->setEditable(true);
        ui->baudrateComboBox->setEditText("");
    }
    else
    {
        ui->baudrateComboBox->setEditable(false);
    }
}

void MainWindow::on_baudrateComboBox_currentTextChanged(const QString &text)
{
    Q_UNUSED(text);
    updatePortConfig();
}

void MainWindow::on_dataBitsComboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    updatePortConfig();
}

void MainWindow::on_parityComboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    updatePortConfig();
}

void MainWindow::on_stopBitsComboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    updatePortConfig();
}

void MainWindow::on_flowComboBox_currentIndexChanged(int index)
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
