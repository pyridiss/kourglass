#include "taskpropertiesdialog.h"
#include "ui_taskpropertiesdialog.h"

#include <QTableWidgetItem>
#include <KIcon>
#include <QItemDelegate>
#include <QDateTimeEdit>

class TaskPropertiesDateTimeEditDelegate : public QItemDelegate
{
    public:
        TaskPropertiesDateTimeEditDelegate(QObject *parent = 0) : QItemDelegate(parent)
        {
        }

        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem&, const QModelIndex&) const
        {
            QDateTimeEdit *editor = new QDateTimeEdit(parent);
            return editor;
        }

        void setEditorData(QWidget *editor, const QModelIndex &index) const
        {
            QDateTime dateTime = QDateTime::fromString(index.model()->data(index, Qt::DisplayRole).toString());
            QDateTimeEdit *dateTimeWidget = dynamic_cast<QDateTimeEdit*>(editor);
            dateTimeWidget->setDateTime(dateTime);
        }

        void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
        {
            QDateTimeEdit *dateTimeWidget = dynamic_cast<QDateTimeEdit*>(editor);
            QDateTime dateTime = dateTimeWidget->dateTime();
            model->setData(index, dateTime.toString(), Qt::EditRole);
        }

        void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex&) const
        {
            editor->setGeometry(option.rect);
        }
};

TaskPropertiesDialog::TaskPropertiesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TaskPropertiesDialog)
{
    ui->setupUi(this);
    ui->addEventButton->setIcon(KIcon("list-add"));
    ui->deleteEventButton->setIcon(KIcon("list-remove"));
    connect(ui->addEventButton, SIGNAL(released()), this, SLOT(addEvent()));
    connect(ui->deleteEventButton, SIGNAL(released()), this, SLOT(deleteEvent()));

    m_currentTask = nullptr;

    TaskPropertiesDateTimeEditDelegate *widgetDelegate = new TaskPropertiesDateTimeEditDelegate(ui->tableEvents);
    ui->tableEvents->setItemDelegateForColumn(1, widgetDelegate);
    ui->tableEvents->setItemDelegateForColumn(2, widgetDelegate);
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
    ui->tableEvents->clearContents();
    while (ui->tableEvents->rowCount() > 0)
        ui->tableEvents->removeRow(0);

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
    ui->tableEvents->resizeColumnsToContents();
}

void TaskPropertiesDialog::updateTask()
{
    
}

void TaskPropertiesDialog::addEvent()
{
    m_currentTask->start();
    m_currentTask->stop();
    updateTableEvents();
}

void TaskPropertiesDialog::deleteEvent()
{
    
}