#include "SourceEditWindow.h"
#include "ui_sourceEditWindow.h"

SourceEditWindow::SourceEditWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SourceEditWindow)
{
    ui->setupUi(this);
}

SourceEditWindow::~SourceEditWindow()
{
    delete ui;
}
