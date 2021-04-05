#include "PlotConfigTabWIdget.h"
#include "ui_plotConfigTabWIdget.h"

PlotConfigTabWIdget::PlotConfigTabWIdget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::PlotConfigTabWIdget),
      serialData(nullptr)
{
    ui->setupUi(this);
}

PlotConfigTabWIdget::~PlotConfigTabWIdget()
{
    delete ui;
}

QList<int> PlotConfigTabWIdget::getResultIndexes()
{
    return indexes;
}

void PlotConfigTabWIdget::restoreResultIndexes(QList<int> m_indexes)
{
    if (!m_indexes.isEmpty() && m_indexes[0] < ui->seriesComboBox->count())
    {
        ui->seriesComboBox->setCurrentIndex(m_indexes[0]);
        m_indexes.removeFirst();
    }
    else
    {
        return;
    }

    if (!m_indexes.isEmpty() && m_indexes[0] < ui->x0_ComboBox->count())
    {
        ui->x0_ComboBox->setCurrentIndex(m_indexes[0]);
        m_indexes.removeFirst();
    }
    else
    {
        return;
    }

    for (int i = 0; i < rows.count() - m_indexes.count(); i++)
    {
        m_indexes.push_back(-1);
    }

    for (int i = 0; i < rows.count(); i++)
    {
        int index = m_indexes[i] + 1;
        if (index < rows[i].combobox->count())
        {
            rows[i].combobox->setCurrentIndex(index);
        }
        else
        {
            rows[i].combobox->setCurrentIndex(0);
        }
    }
}

int PlotConfigTabWIdget::seriesIndexToRegs(int index)
{
    if (index < ui->seriesComboBox->count())
    {
        return ui->seriesComboBox->itemData(index).toInt();
    }
    else
    {
        return -1;
    }
}

bool PlotConfigTabWIdget::checkValid()
{
    int validCount = 0;
    for (auto &&i : rows)
    {
        int index = i.combobox->currentIndex();
        if (index > 0)
        {
            validCount++;
        }
    }

    if (validCount > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void PlotConfigTabWIdget::setDataSource(SerialData *data)
{
    if (data == nullptr)
    {
        return;
    }
    serialData = data;

    for (auto &&i : serialData->getRegExpList())
    {
        ui->seriesComboBox->addItem(serialData->getRegExpTitle(i));
        ui->seriesComboBox->setItemData(ui->seriesComboBox->count() - 1, QVariant(i));
    }
}

void PlotConfigTabWIdget::updateDataSource()
{
    if (serialData == nullptr)
    {
        return;
    }

    ui->seriesComboBox->clear();
    for (auto &&i : serialData->getRegExpList())
    {
        ui->seriesComboBox->addItem(serialData->getRegExpTitle(i));
        ui->seriesComboBox->setItemData(ui->seriesComboBox->count() - 1, QVariant(i));
    }
}

void PlotConfigTabWIdget::on_seriesComboBox_currentIndexChanged(int index)
{
    QStringList regNames = serialData->getRegExpNames(ui->seriesComboBox->itemData(index).toInt());
    setAxisRow(regNames);
}

void PlotConfigTabWIdget::on_x0_ComboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    updateResultIndexes();
}

void PlotConfigTabWIdget::ysComboBox_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    updateResultIndexes();
}

void PlotConfigTabWIdget::addYAxisRow()
{
    YAxisRow row;
    row.label = new QLabel(this);
    row.combobox = new QComboBox(this);
    row.layout = new QHBoxLayout;

    row.label->setText(QString("Y%1:").arg(rows.count()));

    connect(row.combobox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &PlotConfigTabWIdget::ysComboBox_currentIndexChanged);
    row.combobox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    row.combobox->addItem(tr("No Select"));
    row.layout->addWidget(row.label);
    row.layout->addWidget(row.combobox);
    ui->rowsVerticalLayout->addLayout(row.layout);

    rows << row;
}

void PlotConfigTabWIdget::setAxisRow(QStringList regNames)
{
    int index = 2;
    for (auto &&i : rows)
    {
        i.combobox->clear();
        i.combobox->setParent(nullptr);
        i.label->setParent(nullptr);
        delete i.combobox;
        delete i.label;
        delete ui->rowsVerticalLayout->takeAt(index++);
        rows.removeAt(0);
    }

    for (int i = 0; i < regNames.count() - 1; i++)
    {
        addYAxisRow();
    }

    ui->x0_ComboBox->clear();
    for (auto &&i : regNames)
    {
        ui->x0_ComboBox->addItem(i);
        for (auto &&j : rows)
        {
            j.combobox->addItem(i);
        }
    }
}

void PlotConfigTabWIdget::updateResultIndexes()
{
    int validCount = 0;
    QList<int> result;
    QList<int> yAxisIndexes;

    result << ui->seriesComboBox->currentIndex();
    result << ui->x0_ComboBox->currentIndex();

    for (auto &&i : rows)
    {
        int index = i.combobox->currentIndex();

        if ((index != 0 && yAxisIndexes.indexOf(index) != -1) ||
            index == result[1] + 1)
        {
            index = 0;
        }
        else
        {
            yAxisIndexes << index;
        }

        result << index - 1;
        if (index > 0)
        {
            validCount++;
        }
    }

    if (validCount == 0)
    {
        result.clear();
    }

    indexes = result;
}
