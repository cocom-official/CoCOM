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
    void setEncoding(QString lEncoding);
    void insertData(QByteArray *data);
    void find(QString &text);
    void findFromCurrent(QString &text);
    void moveToNextFindResult();
    void moveToPrivFindResult();
    void moveToLastFindResult();
    void moveToFirstFindResult();
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
    QString encoding;
    char hexSeparator;
    bool hexUpperCase;
    QString findText;
    QList<int> findResultPos;
    int currentFindResultIndex;
    int findResultCount;

    HintFormat successHint;
    HintFormat warnHint;
    HintFormat errorHint;

    void lock();
    void unlock();
    void setFormat(int start, int end, QTextCharFormat *format);
    void setFindResultFormat(QTextCharFormat *format, int index = -1);
    void addHintFormat(HintFormat *hintFormat, const QString *key, QTextCharFormat *format);
    void initHintFormat();
    void processHintFormant(HintFormat *hint);
    int findHighlite(QString &text, int from = 0);
    void processAllHintFormants();
    void resetHintFormant(HintFormat *hint);

signals:
    void findResultChanged(int cur, int all);
};
