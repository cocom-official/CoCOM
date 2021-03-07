#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include <stdio.h>
#include "MainWindow.h"

#include "ConfigDialog.h"
#include "ui_configDialog.h"

ConfigDialog::ConfigDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::ConfigDialog),
      dpiScaling(1.0),
      aboutLabelsSignaler(new MouseButtonSignaler)
{
    setupUI();

    enumStyles();
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::setupUI()
{
    ui->setupUi(this);

    QListWidgetItem *common = new QListWidgetItem(ui->listWidget);
    common->setIcon(QIcon(":/assets/icons/sliders.svg"));
    common->setText(tr("Common"));
    ui->listWidget->addItem(common);

    QListWidgetItem *about = new QListWidgetItem(ui->listWidget);
    about->setIcon(QIcon(":/assets/logos/CoCOM.svg"));
    about->setText(tr("About"));
    ui->listWidget->addItem(about);

    ui->listWidget->setCurrentRow(0);

    ui->stackedWidget->setCurrentIndex(0);

    ui->versionLabel->setText(tr("Version") + QString(": v") + QString(COCOM_VERSION_STRING_WITH_SUFFIX));
    ui->commitLabel->setText(tr("Commit") + QString(": ") + QString(COCOM_SHORT_COMMIT_ID));
    ui->buildTimeLabel->setText(tr("Build Time") + QString(": ") + QString(__DATE__ " " __TIME__));
    ui->qtVersionLabel->setText(QString("Qt: ") + QString(QT_VERSION_STR));
    ui->luaVersionLabel->setText(QString("Lua: ") + QString(LUA_VERSION_MAJOR) + "." + QString(LUA_VERSION_MINOR) + "." + QString(LUA_VERSION_RELEASE));

    refreshDPI();

    aboutLabelsSignaler->installOn(ui->commitLabel);
    aboutLabelsSignaler->installOn(ui->qtVersionLabel);
    aboutLabelsSignaler->installOn(ui->luaVersionLabel);
    connect(aboutLabelsSignaler, &MouseButtonSignaler::mouseButtonEvent,
            this, &ConfigDialog::aboutLabels_mouseButtonEvent);
}

void ConfigDialog::refreshDPI()
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

    changeObjectSize(*this, objectRate);
    // resize(width() * objectRate, height() * objectRate);
}

void ConfigDialog::enumStyles()
{
    for (auto style : QStyleFactory::keys())
    {
        if (!style.startsWith("bb10"))
        {
            ui->styleComboBox->addItem(style);
        }
    }
}

void ConfigDialog::on_styleComboBox_currentIndexChanged(const QString &text)
{
    QApplication::setStyle(QStyleFactory::create(text));
}

void ConfigDialog::on_restoreButton_clicked()
{
    ((MainWindow *)parent())->restoreDefaultSettings();
}

void ConfigDialog::on_listWidget_currentRowChanged(int currentRow)
{
    ui->stackedWidget->setCurrentIndex(currentRow);
}

void ConfigDialog::on_okButton_clicked()
{
    close();
}

void ConfigDialog::on_cancelButton_clicked()
{
    close();
}

void ConfigDialog::aboutLabels_mouseButtonEvent(QWidget *obj, QMouseEvent *event)
{
    if (event->type() != QEvent::MouseButtonDblClick)
    {
        return;
    }

    QLabel *label = static_cast<QLabel *>(obj);

    if (label == ui->luaVersionLabel)
    {
        int old = dup(1);

        QTemporaryFile dup2_file;
        dup2_file.open();

        ff::fflua_t lua;
        try
        {
            if (-1 == dup2(dup2_file.handle(), fileno(stdout)))
            {
                qDebug() << "dup2 fail";
            }
            lua.load_file("scripts/hello_lua.lua");
            fflush(stdout);
            dup2(old, 1);
        }
        catch (exception &e)
        {
            qDebug() << "exception:" << e.what();
        }

        dup2_file.seek(0);
        ((MainWindow *)parent())->setStatusInfo(QString(dup2_file.readAll()));
    }
    else if (label == ui->qtVersionLabel)
    {
        QApplication::aboutQt();
    }
    else if (label == ui->commitLabel)
    {
        static int count = 0;
        count++;

        if (count % 2 == 0)
        {
            ui->commitLabel->setText(tr("Commit") + QString(": ") + QString(COCOM_SHORT_COMMIT_ID));
        }
        else
        {
            ui->commitLabel->setText(tr("Commit") + QString(": ") + QString(COCOM_LONG_COMMIT_ID));
        }
    }
}
