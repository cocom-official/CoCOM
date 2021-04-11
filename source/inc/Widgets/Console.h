#pragma once

#include <QMenu>
#include <QEvent>
#include <QString>
#include <QStringList>
#include <QPlainTextEdit>

class Console : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit Console(QWidget *parent = nullptr);
    explicit Console(const QString &copyright, QWidget *parent = nullptr);
    void setPS1(const QString &ps1);
    void setPS2(const QString &ps2);
    void setBlockEnd(const QString &end);

public slots:
    void putOut(const QString &out);
    void setLocalEchoEnabled(bool set);
    void setSelfNewLineJudge(bool set);
    void completeNewLine();
    void incompleteNewLine();

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void contextMenuEvent(QContextMenuEvent *e);

private:
    bool localEchoEnabled;
    bool selfNewLineJudge;
    QString PS1;
    QString PS2;
    QString blockEnd;

    void newLine();
    void newBlockLine();
    int getCurrentLine();

signals:
    void newInput(const QString &input);
    void newLineInput(const QString &input);
};
