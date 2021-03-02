#include "CommandsTab.h"

CommandsTab::CommandsTab(QWidget *parent)
    : QWidget(parent),
      buttonSignaler(new MouseButtonSignaler(this)),
      editSignaler(new EnterKeySignaler(this))
{
    gLavout = new QGridLayout(this);
    gLavout->setMargin(0);

    for (size_t index = 0; index < COMMANDS_TAB_ITEM_COUNT; index++)
    {
        hLavout[index] = new QHBoxLayout;

        edit[index] = new QLineEdit(this);

        editSignaler->installOn(edit[index]);

        button[index] = new QPushButton(this);

        buttonSignaler->installOn(button[index]);
        button[index]->setText(tr("Send") + QString("[%1]").arg(index));
        button[index]->setToolTip(tr("Send") + QString(" Ctrl + %1").arg(index));

        hLavout[index]->addWidget(edit[index]);
        hLavout[index]->addWidget(button[index]);
        hLavout[index]->setMargin(0);

        gLavout->addLayout(hLavout[index], index / COMMANDS_TAB_GRID_COLUMN, index % COMMANDS_TAB_GRID_COLUMN);
    }

    connect(buttonSignaler, &MouseButtonSignaler::mouseButtonEvent,
            this, &CommandsTab::buttonsEvent);
    connect(editSignaler, &EnterKeySignaler::enterKeyEvent,
            this, &CommandsTab::editEvent);
}

void CommandsTab::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat())
    {
        return;
    }

    if (event->modifiers() == Qt::ControlModifier &&
        (event->key() >= Qt::Key_0 &&  event->key() <= Qt::Key_9))
    {
        emit sendText(edit[event->key() - Qt::Key_0]->text());
    }
}

void CommandsTab::buttonsEvent(QWidget *obj, QMouseEvent *event)
{
    if (event->type() != QEvent::MouseButtonPress &&
        event->type() != QEvent::MouseButtonDblClick)
    {
        return;
    }

    int index = 0;
    for (index = 0; index < COMMANDS_TAB_ITEM_COUNT; index++)
    {
        if (static_cast<QWidget *>(button[index]) == obj)
        {
            break;
        }
    }

    emit sendText(edit[index]->text());
}

void CommandsTab::editEvent(QWidget *obj, QKeyEvent *event)
{
    Q_UNUSED(event);

    for (int index = 0; index < COMMANDS_TAB_ITEM_COUNT; index++)
    {
        if (static_cast<QWidget *>(edit[index]) == obj)
        {
            emit sendText(edit[index]->text());
        }
    }
}
