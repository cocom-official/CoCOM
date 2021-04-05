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

    ui->versionLabel->setText(QString("v" COCOM_VERSION_STRING));
    ui->commitLabel->setText(QString(COCOM_SHORT_COMMIT_ID));
    ui->buildTimeLabel->setText(QString(__DATE__));
    ui->qtVersionLabel->setText(QString(QT_VERSION_STR));
    ui->luaVersionLabel->setText(QString(LUA_VERSION_MAJOR "." LUA_VERSION_MINOR "." LUA_VERSION_RELEASE));

    refreshDPI();

    aboutLabelsSignaler->installOn(ui->versionLabel);
    aboutLabelsSignaler->installOn(ui->buildTimeLabel);
    aboutLabelsSignaler->installOn(ui->commitLabel);
    aboutLabelsSignaler->installOn(ui->qtVersionLabel);
    aboutLabelsSignaler->installOn(ui->luaVersionLabel);
    connect(aboutLabelsSignaler, &MouseButtonSignaler::mouseButtonEvent,
            this, &ConfigDialog::aboutLabels_mouseButtonEvent);

    refreshUI();
}

void ConfigDialog::refreshUI()
{
    /* language combobox */
    ui->languageComboBox->clear();
    int langIndex = 0;
    QString lang = settings->getValue("language").toString();
    for (int index = 0; index < (int)(sizeof(langs) / sizeof(langs[0])); index++)
    {
        ui->languageComboBox->addItem(langs[index]);
        if (lang == langs[index])
        {
            langIndex = index;
        }
    }
    ui->languageComboBox->setCurrentIndex(langIndex);

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

    if (settings->getValue("sendNotice").toBool())
    {
        ui->noticeCheckBox->setCheckState(Qt::Checked);
    }
    else
    {
        ui->noticeCheckBox->setCheckState(Qt::Unchecked);
    }

    if (settings->getValue("showAndHideEnable").toBool())
    {
        ui->showHideKeyCheckBox->setCheckState(Qt::Checked);
    }
    else
    {
        ui->showHideKeyCheckBox->setCheckState(Qt::Unchecked);
    }
    on_showHideKeyCheckBox_stateChanged(ui->showHideKeyCheckBox->checkState());

    if (settings->getValue("clearOutputEnable").toBool())
    {
        ui->clearOutputKeyCheckBox->setCheckState(Qt::Checked);
    }
    else
    {
        ui->clearOutputKeyCheckBox->setCheckState(Qt::Unchecked);
    }
    on_clearOutputKeyCheckBox_stateChanged(ui->clearOutputKeyCheckBox->checkState());

    if (settings->getValue("scrollToEndEnable").toBool())
    {
        ui->scrollToEndKeyCheckBox->setCheckState(Qt::Checked);
    }
    else
    {
        ui->scrollToEndKeyCheckBox->setCheckState(Qt::Unchecked);
    }
    on_scrollToEndKeyCheckBox_stateChanged(ui->scrollToEndKeyCheckBox->checkState());

    QString shortKey;
    shortKey = settings->getValue("showAndHideKey").toString();
    if (!shortKey.isEmpty())
    {
        ui->showHideKeySequenceEdit->blockSignals(true);
        ui->showHideKeySequenceEdit->setKeySequence(QKeySequence::fromString(shortKey));
        ui->showHideKeySequenceEdit->blockSignals(false);
    }

    shortKey = settings->getValue("clearOutputKey").toString();
    if (!shortKey.isEmpty())
    {
        ui->clearOutputKeySequenceEdit->blockSignals(true);
        ui->clearOutputKeySequenceEdit->setKeySequence(QKeySequence::fromString(shortKey));
        ui->clearOutputKeySequenceEdit->blockSignals(false);
    }

    shortKey = settings->getValue("scrollToEndKey").toString();
    if (!shortKey.isEmpty())
    {
        ui->scrollToEndKeySequenceEdit->blockSignals(true);
        ui->scrollToEndKeySequenceEdit->setKeySequence(QKeySequence::fromString(shortKey));
        ui->scrollToEndKeySequenceEdit->blockSignals(false);
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

void ConfigDialog::on_noticeCheckBox_stateChanged(int state)
{
    if (Qt::Checked == state)
    {
        settings->setUndoableValue("sendNotice", QVariant(true));
    }
    else
    {
        settings->setUndoableValue("sendNotice", QVariant(false));
    }
}

void ConfigDialog::on_showHideKeyCheckBox_stateChanged(int state)
{
    if (Qt::Checked == state)
    {
        ui->showHideKeySequenceEdit->setEnabled(true);
        settings->setUndoableValue("showAndHideEnable", QVariant(true));
    }
    else
    {
        ui->showHideKeySequenceEdit->setDisabled(true);
        settings->setUndoableValue("showAndHideEnable", QVariant(false));
    }
}

void ConfigDialog::on_clearOutputKeyCheckBox_stateChanged(int state)
{
    if (Qt::Checked == state)
    {
        ui->clearOutputKeySequenceEdit->setEnabled(true);
        settings->setUndoableValue("clearOutputEnable", QVariant(true));
    }
    else
    {
        ui->clearOutputKeySequenceEdit->setDisabled(true);
        settings->setUndoableValue("clearOutputEnable", QVariant(false));
    }
}

void ConfigDialog::on_scrollToEndKeyCheckBox_stateChanged(int state)
{
    if (Qt::Checked == state)
    {
        ui->scrollToEndKeySequenceEdit->setEnabled(true);
        settings->setUndoableValue("scrollToEndEnable", QVariant(true));
    }
    else
    {
        ui->scrollToEndKeySequenceEdit->setDisabled(true);
        settings->setUndoableValue("scrollToEndEnable", QVariant(false));
    }
}

void ConfigDialog::on_showHideKeySequenceEdit_keySequenceChanged(const QKeySequence &keySequence)
{
    settings->setUndoableValue("showAndHideKey", QVariant(keySequence.toString()));
}

void ConfigDialog::on_clearOutputKeySequenceEdit_keySequenceChanged(const QKeySequence &keySequence)
{
    settings->setUndoableValue("clearOutputKey", QVariant(keySequence.toString()));
}

void ConfigDialog::on_scrollToEndKeySequenceEdit_keySequenceChanged(const QKeySequence &keySequence)
{
    settings->setUndoableValue("scrollToEndKey", QVariant(keySequence.toString()));
}

void ConfigDialog::on_restoreButton_clicked()
{
    emit onRestore();
}

void ConfigDialog::on_okButton_clicked()
{
    settings->applyTrackChange();

    if (settings->checkNeedReStart() &&
        QMessageBox::Yes == QMessageBox::question(
                                this,
                                tr("Restart Application is Needed!"),
                                tr("Some settings need restart to take effect, restart application?"),
                                QMessageBox::Yes | QMessageBox::No,
                                QMessageBox::Yes))
    {
        emit onRestart();
    }

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
        QMessageBox *luaMsgBox = new QMessageBox(this);
        luaMsgBox->setWindowTitle("Hello Lua");
        luaMsgBox->setIconPixmap(QPixmap(":/assets/logos/lua.png").scaled(64, 64, Qt::KeepAspectRatio));
        connect(luaMsgBox, &QMessageBox::buttonClicked,
                [luaMsgBox](QAbstractButton *button) { Q_UNUSED(button); luaMsgBox->setParent(nullptr); luaMsgBox->deleteLater(); });

        CoLua lua;
        lua.setObjectName("Hello Lua");
        connect(&lua, &CoLua::consoleOut,
                [luaMsgBox](QString out) { luaMsgBox->setText(out); luaMsgBox->show(); });
        lua.loadFile(LUA_SCRIPTS_PATH + QString("/hello_lua.lua"));
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
            ui->commitLabel->setText(QString(COCOM_SHORT_COMMIT_ID));
        }
        else
        {
            ui->commitLabel->setText(QString(COCOM_LONG_COMMIT_ID));
        }
    }
    else if (label == ui->versionLabel)
    {
        static int count = 0;
        count++;

        if (count % 2 == 0)
        {
            ui->versionLabel->setText(QString(COCOM_VERSION_STRING));
        }
        else
        {
            ui->versionLabel->setText(QString(COCOM_VERSION_STRING_WITH_SUFFIX));
        }
    }
    else if (label == ui->buildTimeLabel)
    {
        static int count = 0;
        count++;

        if (count % 2 == 0)
        {
            ui->buildTimeLabel->setText(QString(__DATE__));
        }
        else
        {
            ui->buildTimeLabel->setText(QString(__DATE__ " " __TIME__));
        }
    }
}

void ConfigDialog::showEvent(QShowEvent *event)
{
    QFile file(README_FILE_PATH);
    file.open(QFile::ReadOnly | QFile::Text);
    ui->textBrowser->setMarkdown(file.readAll());

    refreshUI();
    settings->startTrackChange();

    event->accept();
}

void ConfigDialog::closeEvent(QCloseEvent *event)
{
    ui->textBrowser->clear();

    settings->undoChange();

    event->accept();
}
