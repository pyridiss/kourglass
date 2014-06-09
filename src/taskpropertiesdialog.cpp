#include "taskpropertiesdialog.h"
#include "ui_taskpropertiesdialog.h"

#include <QTableWidgetItem>
#include <KIcon>
#include <QItemDelegate>
#include <QDateTimeEdit>
#include <QDateTime>

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

class DateItem : public QTableWidgetItem
{
    //This class will help sorting events by real date instead of QString
    public:
        QDateTime m_time;

        DateItem() : QTableWidgetItem()
        {
        }

        virtual bool operator<(const QTableWidgetItem &other) const
        {
            const DateItem *right = dynamic_cast<const DateItem*>(&other);
            if (right != nullptr)
            {
                return (m_time < right->m_time);
            }
            return (QTableWidgetItem::operator<(other));
        }
};

TaskPropertiesDialog::TaskPropertiesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TaskPropertiesDialog)
{
    ui->setupUi(this);
    ui->addEventButton->setIcon(KIcon("list-add"));
    ui->deleteEventButton->setIcon(KIcon("list-remove"));
    ui->deleteEventButton->setEnabled(false);
    connect(ui->addEventButton, SIGNAL(released()), this, SLOT(addEvent()));
    connect(ui->deleteEventButton, SIGNAL(released()), this, SLOT(deleteEvent()));
    connect(this, SIGNAL(accepted()), this, SLOT(updateTask()));
    connect(this, SIGNAL(accepted()), this, SLOT(noEventSelected()));
    connect(this, SIGNAL(rejected()), this, SLOT(noEventSelected()));
    connect(ui->tableEvents, SIGNAL(itemActivated(QTableWidgetItem*)), this, SLOT(anEventSelected()));
    connect(this, SIGNAL(activateDeleteButton(bool)), ui->deleteEventButton, SLOT(setEnabled(bool)));

    connect(ui->progressSlider, SIGNAL(valueChanged(int)), this, SLOT(updateProgressValue(int)));

    m_currentTask = nullptr;

    TaskPropertiesDateTimeEditDelegate *widgetDelegate = new TaskPropertiesDateTimeEditDelegate(ui->tableEvents);
    ui->tableEvents->setItemDelegateForColumn(2, widgetDelegate);
    ui->tableEvents->setItemDelegateForColumn(3, widgetDelegate);

    ui->tableEvents->setColumnHidden(0, true);
    ui->tableEvents->setSortingEnabled(true);
    connect(ui->tableEvents, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(resizeTableEventsColumns()));
}

TaskPropertiesDialog::~TaskPropertiesDialog()
{
    delete ui;
}

void TaskPropertiesDialog::anEventSelected()
{
    emit activateDeleteButton(true);
}

void TaskPropertiesDialog::noEventSelected()
{
    emit activateDeleteButton(false);
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
    ui->tableEvents->setSortingEnabled(false);
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

        // Columns 2 and 3 are DateItem's instead of QTableWidgetItem's to be correctly sorted

        DateItem *start = new DateItem();
        start->setText(i->m_startTime.toString());
        start->m_time = i->m_startTime;
        ui->tableEvents->setItem(row, 2, start);

        DateItem *end = new DateItem();
        end->setText(i->m_endTime.toString());
        end->m_time = i->m_endTime;
        ui->tableEvents->setItem(row, 3, end);

        ++row;
    }
    resizeTableEventsColumns();

    //Default is: new events first (by starting time)
    ui->tableEvents->sortItems(2, Qt::DescendingOrder);
    ui->tableEvents->setSortingEnabled(true);
}

void TaskPropertiesDialog::resizeTableEventsColumns()
{
    ui->tableEvents->resizeColumnToContents(1);
    ui->tableEvents->resizeColumnToContents(2);
    ui->tableEvents->resizeColumnToContents(3);
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
            event->saveToAkonadi();
        }
    }
    emit allDurationsChanged();
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
    updateTask();
    QString uidToRemove = ui->tableEvents->item(ui->tableEvents->currentRow(), 0)->text();

    m_currentTask->removeEvent(uidToRemove);

    updateTableEvents();
}

void TaskPropertiesDialog::updateProgressValue(int value)
{
    ui->progressValue->setText(QVariant(value).toString() + "%");
}