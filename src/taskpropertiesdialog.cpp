#include "taskpropertiesdialog.h"
#include "ui_taskpropertiesdialog.h"

TaskPropertiesDialog::TaskPropertiesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TaskPropertiesDialog)
{
    ui->setupUi(this);
    m_currentTask = nullptr;
}

TaskPropertiesDialog::~TaskPropertiesDialog()
{
    delete ui;
}

void TaskPropertiesDialog::setTask(Task* task)
{
    m_currentTask = task;
}