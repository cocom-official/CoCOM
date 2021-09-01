#ifndef SOURCEEDITWINDOW_H
#define SOURCEEDITWINDOW_H

#include <QMainWindow>

namespace Ui {
class SourceEditWindow;
}

class SourceEditWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SourceEditWindow(QWidget *parent = nullptr);
    ~SourceEditWindow();

private:
    Ui::SourceEditWindow *ui;
};

#endif // SOURCEEDITWINDOW_H
