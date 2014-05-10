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
            editor->setDisplayFormat(QString("dd/MM/yyyy  HH:mm:ss"));
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
    connect(this, SIGNAL(accepted()), this, SLOT(updateTask()));

    m_currentTask = nullptr;

    TaskPropertiesDateTimeEditDelegate *widgetDelegate = new TaskPropertiesDateTimeEditDelegate(ui->tableEvents);
    ui->tableEvents->setItemDelegateForColumn(2, widgetDelegate);
    ui->tableEvents->setItemDelegateForColumn(3, widgetDelegate);

    ui->tableEvents->setColumnHidden(0, true);
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

        QTableWidgetItem *uid = new QTableWidgetItem();
        uid->setText(i->getUid());
        ui->tableEvents->setItem(row, 0, uid);

        QTableWidgetItem *name = new QTableWidgetItem();
        name->setText(i->m_name);
        ui->tableEvents->setItem(row, 1, name);

        QTableWidgetItem *start = new QTableWidgetItem();
        start->setText(i->m_startTime.toString());
        ui->tableEvents->setItem(row, 2, start);

        QTableWidgetItem *end = new QTableWidgetItem();
        end->setText(i->m_endTime.toString());
        ui->tableEvents->setItem(row, 3, end);

        ++row;
    }
    ui->tableEvents->resizeColumnsToContents();
}

void TaskPropertiesDialog::updateTask()
{
    QTableWidgetItem* item = nullptr;
    Event* event = nullptr;
    for (int i = 0 ; i < ui->tableEvents->rowCount() ; ++i)
    {
        item = ui->tableEvents->item(i, 0);
        if (m_currentTask->m_events.find(item->text()) != m_currentTask->m_events.end())
        {
            event = m_currentTask->m_events[item->text()];
            item = ui->tableEvents->item(i, 1);
            event->m_name = item->text();
            item = ui->tableEvents->item(i, 2);
            event->m_startTime = QDateTime::fromString(item->text());
            item = ui->tableEvents->item(i, 3);
            event->m_endTime = QDateTime::fromString(item->text());
        }
    }
}

void TaskPropertiesDialog::addEvent()
{
    updateTask();
    m_currentTask->start();
    m_currentTask->stop();
    updateTableEvents();
}

void TaskPropertiesDialog::deleteEvent()
{
    
}