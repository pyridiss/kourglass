#include "taskpropertiesdialog.h"
#include "ui_taskpropertiesdialog.h"

#include <QTableWidgetItem>
#include <KIcon>

TaskPropertiesDialog::TaskPropertiesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TaskPropertiesDialog)
{
    ui->setupUi(this);
    ui->addEventButton->setIcon(KIcon("list-add"));
    ui->deleteEventButton->setIcon(KIcon("list-remove"));
    m_currentTask = nullptr;
}

TaskPropertiesDialog::~TaskPropertiesDialog()
{
    delete ui;
}

void TaskPropertiesDialog::setTask(Task* task)
{
    m_currentTask = task;
    ui->taskNameEdit->setText(task->m_name);
    ui->radioButtonToday->click();
    ui->radioButtonOneTask->click();
    updateTableEvents();
}

void TaskPropertiesDialog::updateTableEvents()
{
    int row = 0;
    for (auto& i : m_currentTask->m_events)
    {
        ui->tableEvents->insertRow(row);
        QTableWidgetItem *name = new QTableWidgetItem();
        name->setText(i->m_name);
        ui->tableEvents->setItem(row, 0, name);
        QTableWidgetItem *start = new QTableWidgetItem();
        start->setText(i->m_startTime.toString());
        ui->tableEvents->setItem(row, 1, start);
        QTableWidgetItem *end = new QTableWidgetItem();
        end->setText(i->m_endTime.toString());
        ui->tableEvents->setItem(row, 2, end);
        ++row;
    }
}

void TaskPropertiesDialog::updateTask()
{
    
}