#include "AppsDialog.h"
#include "ui_appsDialog.h"

AppsDialog::AppsDialog(MediaData *data, QWidget *parent)
    : QDialog(parent),
      mediaData(data),
      ui(new Ui::AppsDialog)
{
    ui->setupUi(this);

    ui->removeButton->setDisabled(true);

    QStringList regsHeader;
    regsHeader << tr("ID")
               << tr("Title")
               << tr("RegExp")
               << tr("Values Name")
               << tr("Item Count");
    ui->regsTable->setColumnCount(regsHeader.count());
    ui->regsTable->setHorizontalHeaderLabels(regsHeader);
    ui->regsTable->setColumnHidden(0, true);

    showRegsTable();

    ui->regExpEdit->setText("<(?<time>\\d+.\\d+)> value1:(?<value1>\\d) value2:(?<value2>\\d)");
}

AppsDialog::~AppsDialog()
{
    delete ui;
}

void AppsDialog::newLineAdded(QString line)
{
    if (!isVisible() || getRegsTableSelectedRow() >= 0)
    {
        return;
    }

    lastLine = line;
    if (preViewRegExp.isValid())
    {
        QRegularExpressionMatch match = preViewRegExp.match(line);

        if (!match.hasMatch())
        {
            return;
        }

        QStringList regsHeader;
        int index = 0;
        ui->dataTable->setRowCount(1);
        ui->dataTable->setColumnCount(preViewRegExp.namedCaptureGroups().size() - 1);
        for (auto &&i : preViewRegExp.namedCaptureGroups())
        {
            if (!i.isEmpty())
            {
                regsHeader << i;
                if (ui->dataTable->item(0, index) != nullptr)
                {
                    ui->dataTable->item(0, index++)->setText(match.captured(i));
                }
                else
                {
                    QTableWidgetItem *item = new QTableWidgetItem(match.captured(i));
                    ui->dataTable->setItem(0, index++, item);
                }
            }
        }
        ui->dataTable->setHorizontalHeaderLabels(regsHeader);
    }
}

void AppsDialog::showRow(int row)
{
    int id = ui->regsTable->item(row, 0)->text().toInt();

    ui->regExpTitleEdit->setText(mediaData->getRegExpTitle(id));
    ui->regExpEdit->setText(mediaData->getRegExpPattern(id));

    setDataTable(mediaData->getRegExpNames(id), mediaData->getRegExpValues(id));
}

void AppsDialog::setRegsRow(int row, int id, QString title, QString regexp, QString valuesName, int count)
{
    auto setItem = [=, this](int colum, QString text) {
        if (ui->regsTable->item(row, colum) != nullptr)
        {
            ui->regsTable->item(row, colum)->setText(text);
        }
        else
        {
            if (ui->regsTable->rowCount() < row + 1)
            {
                ui->regsTable->setRowCount(row + 1);
            }

            QTableWidgetItem *item = new QTableWidgetItem(text);
            ui->regsTable->setItem(row, colum, item);
        }
    };

    setItem(0, QString::number(id));
    setItem(1, title);
    setItem(2, regexp);
    setItem(3, valuesName);
    setItem(4, QString::number(count));
}

void AppsDialog::setDataTable(QStringList valuesName, QList<QList<qreal>> values)
{
    if (values.count() < 1 || values[0].count() < 1)
    {
        return;
    }

    int row = values.count();
    int column = values[0].count();

    while (ui->dataTable->rowCount() > row)
    {
        for (int i = 0; i < ui->dataTable->columnCount(); i++)
        {
            delete ui->dataTable->item(ui->dataTable->rowCount() - 1, i);
        }
        ui->dataTable->setRowCount(ui->dataTable->rowCount() - 1);
    }

    while (ui->dataTable->columnCount() > column)
    {
        for (int i = 0; i < ui->dataTable->columnCount(); i++)
        {
            delete ui->dataTable->item(i, ui->dataTable->columnCount() - 1);
        }
        ui->dataTable->setColumnCount(ui->dataTable->columnCount() - 1);
    }

    ui->dataTable->setRowCount(row);
    ui->dataTable->setColumnCount(column);
    ui->dataTable->setHorizontalHeaderLabels(valuesName);

    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column; j++)
        {
            if (ui->dataTable->item(i, j) != nullptr)
            {
                ui->dataTable->item(i, j)->setText(QString::number(values[i][j]));
            }
            else
            {
                QTableWidgetItem *item = new QTableWidgetItem(QString::number(values[i][j]));
                ui->dataTable->setItem(i, j, item);
            }
        }
    }
}

void AppsDialog::showRegsTable()
{
    QList<int> regs = mediaData->getRegExpList();

    while (ui->regsTable->rowCount() > regs.count())
    {
        for (int i = 0; i < ui->regsTable->columnCount(); i++)
        {
            delete ui->regsTable->takeItem(ui->regsTable->rowCount() - 1, i);
        }
        ui->regsTable->setRowCount(ui->regsTable->rowCount() - 1);
    }

    int index = 0;
    for (auto &&i : regs)
    {
        if (!mediaData->isRegExpEnable(i))
        {
            continue;
        }
        setRegsRow(index++, i, mediaData->getRegExpTitle(i), mediaData->getRegExpPattern(i), mediaData->getRegExpNames(i).join(","), mediaData->getRegExpValues(i).count());
    }
}

int AppsDialog::getRegsTableSelectedRow()
{
    auto ranges = ui->regsTable->selectedRanges();
    if (ranges.size() == 1 && ranges[0].rowCount() == 1 && ranges[0].columnCount() == ui->regsTable->columnCount())
    {
        return ranges[0].topRow();
    }
    else
    {
        return -1;
    }
}

void AppsDialog::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    showRegsTable();
}

void AppsDialog::on_regExpEdit_textChanged()
{
    preViewRegExp.setPattern(ui->regExpEdit->document()->toPlainText());
    if (ui->regExpEdit->document()->toPlainText().isEmpty())
    {
        ui->regExpEdit->setStyleSheet("");
        ui->okButton->setDisabled(true);
        return;
    }

    if (preViewRegExp.isValid())
    {
        ui->okButton->setDisabled(false);
        ui->regExpEdit->setStyleSheet("QTextEdit { background: rgb(150, 255, 150); }");
    }
    else
    {
        ui->okButton->setDisabled(true);
        ui->regExpEdit->setStyleSheet("QTextEdit { background: rgb(255, 130, 130); }");
    }
}

void AppsDialog::on_regsTable_itemSelectionChanged()
{
    static bool hasLast = false;
    static QString lastTitle;
    static QString lastPattern;
    int row = getRegsTableSelectedRow();

    if (row >= 0)
    {
        if (!hasLast)
        {
            hasLast = true;
            lastTitle = ui->regExpTitleEdit->text();
            lastPattern = ui->regExpEdit->document()->toPlainText();
        }

        showRow(row);
        ui->removeButton->setDisabled(false);
    }
    else
    {
        if (hasLast)
        {
            ui->regExpTitleEdit->setText(lastTitle);
            ui->regExpEdit->setText(lastPattern);
            hasLast = false;
        }

        ui->removeButton->setDisabled(true);
    }
}

void AppsDialog::on_okButton_pressed()
{
    int row = getRegsTableSelectedRow();

    if (row >= 0)
    {
        int id = ui->regsTable->item(row, 0)->text().toInt();
        mediaData->setRegExpTitle(id, ui->regExpTitleEdit->text());
        mediaData->setRegExp(id, ui->regExpEdit->document()->toPlainText());
        showRegsTable();
    }
    else
    {
        mediaData->addRegExp(ui->regExpEdit->document()->toPlainText(), ui->regExpTitleEdit->text());
        showRegsTable();
    }
}

void AppsDialog::on_removeButton_pressed()
{
    int row = getRegsTableSelectedRow();
    if (row >= 0)
    {
        int id = ui->regsTable->item(row, 0)->text().toInt();
        mediaData->removeRegExp(id);
        showRegsTable();
    }
}

void AppsDialog::on_okDialogButton_pressed()
{
    close();
}

void AppsDialog::on_cancelDialogButton_pressed()
{
    close();
}
