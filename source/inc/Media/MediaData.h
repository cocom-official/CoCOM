#pragma once

#include <QObject>
#include <QByteArray>
#include <QMutex>
#include <QRegularExpression>

#include "Common.h"

class MediaData
    : public QObject
{
    Q_OBJECT
public:
    explicit MediaData(QObject *parent = nullptr);
    void setEncoding(QString lEncoding);
    void clear();

    /* Regular Expression */
    int addRegExp(QString pattern, QString title = QString());
    int setRegExp(int exp, QString pattern);
    bool isRegExpEnable(int exp);
    QString getRegExpPattern(int exp);
    QString getRegExpTitle(int exp);
    void setRegExpTitle(int exp, QString title);
    QStringList getRegExpNames(int exp);
    QList<QList<qreal>> getRegExpValues(int exp);
    QList<int> getRegExpList();
    void removeRegExp(int exp);

public slots:
    void appendBytes(QByteArray *bytes);

private:
    typedef struct
    {
        bool enable;
        QString title;
        QRegularExpression regExp;
        QStringList names;
        QList<QList<qreal>> values;
    } RegExp_t;

    QMutex clearMutex;
    QByteArray serialData;
    QString encoding;
    QList<RegExp_t> regs;

    void findNewLine(QString line);

signals:
    void newTextLine(QString line);
    void regExpNewData(int exp, QList<qreal> data);
};
