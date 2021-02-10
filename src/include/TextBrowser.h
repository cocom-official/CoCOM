#include <QObject>
#include <QMutex>
#include <QByteArray>
#include <QTextBrowser>
#include <QScrollBar>
#include <QTextCharFormat>
#include <QDebug>
#include "Common.h"

class TextBrowser : public QObject
{
    Q_OBJECT

public:
    explicit TextBrowser(QObject *parent, QTextBrowser *browser);
    ~TextBrowser();

    void setDataType(DataType type);
    void insertData(QByteArray *data);

public slots:

private:
    QTextBrowser *browser;
    QMutex mutex;
    DataType dataType;
    QTextCursor cursor;
    int scroll;
    int maxScroll;

    void lock();
    void unlock();
    void setFormat(int from, int end, QTextCharFormat format);
};
