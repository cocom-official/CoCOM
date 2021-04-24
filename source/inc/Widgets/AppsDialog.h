#pragma once

#include <QDebug>
#include <QDialog>
#include <QRegularExpression>

#include "MediaData.h"

namespace Ui {
class AppsDialog;
}

class AppsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AppsDialog(MediaData *data, QWidget *parent = nullptr);
    ~AppsDialog();

public slots:
    void newLineAdded(QString line);

private slots:
    void on_regExpEdit_textChanged();
    void on_regsTable_itemSelectionChanged();
    void on_okButton_pressed();
    void on_removeButton_pressed();
    void on_okDialogButton_pressed();
    void on_cancelDialogButton_pressed();

private:
    MediaData *mediaData;
    Ui::AppsDialog *ui;

    QRegularExpression preViewRegExp;
    QString lastLine;

    void showRow(int row);
    void setRegsRow(int row, int id, QString title, QString regexp, QString valuesName, int count);
    void setDataTable(QStringList valuesName, QList<QList<qreal>> values);
    void showRegsTable();
    int getRegsTableSelectedRow();

    void showEvent(QShowEvent *event);
};
