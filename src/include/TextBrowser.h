#include <QObject>
#include <QMutex>
#include <QString>
#include <QList>
#include <QByteArray>
#include <QTextCodec>
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
    void setEncoding(EncodingType lEncoding);
    void insertData(QByteArray *data);
    void clear();

public slots:

private:
    typedef struct
    {
        const QString *key;
        int cursorPos;
    } HintKey;

    typedef struct
    {
        QList<HintKey*> key;
        QTextCharFormat *format;
    } HintFormat;

    QTextBrowser *browser;
    QMutex mutex;
    DataType dataType;
    QTextCursor lockCursor;
    int scroll;
    int maxScroll;
    EncodingType encoding;

    HintFormat successHint;
    HintFormat warnHint;
    HintFormat errorHint;

    void lock();
    void unlock();
    void setFormat(int start, int end, QTextCharFormat *format);
    void addHintFormat(HintFormat *hintFormat, const QString *key, QTextCharFormat *format);
    void initHintFormat();
    void processHintFormant(HintFormat *hint);
    void processAllHintFormants();
    void resetHintFormant(HintFormat *hint);
};
