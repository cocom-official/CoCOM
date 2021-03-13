#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include <stdio.h>
#include "MainWindow.h"

#include "ConfigDialog.h"
#include "ui_configDialog.h"

ConfigDialog::ConfigDialog(GlobalSettings *_settings, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::ConfigDialog),
      dpiScaling(1.0),
      aboutLabelsSignaler(new MouseButtonSignaler),
      settings(_settings)
{
    setupUI();
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::setupUI()
{
    ui->setupUi(this);

    ui->iconLabel->setPixmap(QPixmap::fromImage(
                                 QImage(":/assets/logos/CoCOM_128.png"))
                                 .scaled(ui->iconLabel->size(), Qt::KeepAspectRatio));

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

    refreshUI();
}

void ConfigDialog::refreshUI()
{
    /* styleComboBox */
    ui->styleComboBox->clear();
    int styleIndex = 0;
    QString style = settings->getValue("windosStyle").toString();
    QStringList styleList = QStyleFactory::keys();
    for (int index = 0; index < styleList.size(); index++)
    {
        if (!styleList[index].startsWith("bb10"))
        {
            ui->styleComboBox->addItem(styleList[index]);
            if (style == styleList[index])
            {
                styleIndex = index;
            }
        }
    }
    ui->styleComboBox->setCurrentIndex(styleIndex);

    if (settings->getValue("darkMode").toBool())
    {
        ui->darkModeCheckBox->setCheckState(Qt::Checked);
    }
    else
    {
        ui->darkModeCheckBox->setCheckState(Qt::Unchecked);
    }

    if (settings->getValue("keepWindowSize").toBool())
    {
        ui->keepWindowsSizeCheckBox->setCheckState(Qt::Checked);
    }
    else
    {
        ui->keepWindowsSizeCheckBox->setCheckState(Qt::Unchecked);
    }

    if (settings->getValue("keepWindowPos").toBool())
    {
        ui->keepWindowsPosCheckBox->setCheckState(Qt::Checked);
    }
    else
    {
        ui->keepWindowsPosCheckBox->setCheckState(Qt::Unchecked);
    }
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

void ConfigDialog::on_languageComboBox_currentIndexChanged(const QString &text)
{
    settings->setUndoableValue("language", QVariant(text));
}

void ConfigDialog::on_styleComboBox_currentIndexChanged(const QString &text)
{
    settings->setUndoableValue("windosStyle", QVariant(text));
}

void ConfigDialog::on_darkModeCheckBox_stateChanged(int state)
{
    static QString style;
    if (Qt::Checked == state)
    {
        style = ui->styleComboBox->currentText();
        for (int i = 0; i < ui->styleComboBox->count(); i++)
        {
            if (ui->styleComboBox->itemText(i) == "Fusion")
            {
                ui->styleComboBox->setCurrentIndex(i);
                ui->styleComboBox->setDisabled(true);
                break;
            }
        }
        settings->setUndoableValue("darkMode", QVariant(true));
    }
    else
    {
        for (int i = 0; i < ui->styleComboBox->count(); i++)
        {
            if (ui->styleComboBox->itemText(i) == style)
            {
                ui->styleComboBox->setCurrentIndex(i);
                ui->styleComboBox->setDisabled(false);
                break;
            }
        }
        settings->setUndoableValue("darkMode", QVariant(false));
    }
}

void ConfigDialog::on_keepWindowsSizeCheckBox_stateChanged(int state)
{
    if (Qt::Checked == state)
    {
        settings->setUndoableValue("keepWindowSize", QVariant(true));
    }
    else
    {
        settings->setUndoableValue("keepWindowSize", QVariant(false));
    }
}

void ConfigDialog::on_keepWindowsPosCheckBox_stateChanged(int state)
{
    if (Qt::Checked == state)
    {
        settings->setUndoableValue("keepWindowPos", QVariant(true));
    }
    else
    {
        settings->setUndoableValue("keepWindowPos", QVariant(false));
    }
}

void ConfigDialog::on_restoreButton_clicked()
{
    emit onRestore();
}

void ConfigDialog::on_okButton_clicked()
{
    settings->applyTrackChange();
    close();
}

void ConfigDialog::on_cancelButton_clicked()
{
    settings->undoChange();
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

void ConfigDialog::showEvent(QShowEvent *event)
{
    refreshUI();
    settings->startTrackChange();

    event->accept();
}

void ConfigDialog::closeEvent(QCloseEvent *event)
{
    settings->undoChange();

    event->accept();
}
