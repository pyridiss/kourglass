#include "mainview.h"
#include "ui_mainview.h"

#include <QList>
#include <KJob>
#include <QTableWidgetItem>
#include <QIcon>
#include <QItemDelegate>
#include <QDateTimeEdit>
#include <QDateTime>

#include <AkonadiCore/ChangeRecorder>
#include <AkonadiCore/EntityTreeModel>
#include <AkonadiCore/EntityMimeTypeFilterModel>

using namespace Akonadi;

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


MainView::MainView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainView)
{
    ui->setupUi(this);
    connect(ui->selectProject, SIGNAL(activated(const QString&)), this, SLOT(changeProject(const QString&)));
    connect(ui->treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(changeSelectedTask()));

    ChangeRecorder *changeRecorder = new ChangeRecorder(this);
    changeRecorder->setCollectionMonitored(Collection::root());
    changeRecorder->setMimeTypeMonitored("application/x-vnd.akonadi.calendar.todo", true);

    EntityTreeModel *model = new EntityTreeModel(changeRecorder, this);
    EntityMimeTypeFilterModel* filterModel = new EntityMimeTypeFilterModel(this);
    filterModel->setSourceModel(model);
    filterModel->addMimeTypeInclusionFilter(Collection::mimeType());
    filterModel->setHeaderGroup(EntityTreeModel::CollectionTreeHeaders);

    ui->listCalendars->setModel(filterModel);

    ui->radioButtonAllEvents->setChecked(true);
    QDate today = QDate::currentDate();
    ui->dateTo->setDate(today);

    connect(ui->listCalendars, SIGNAL(currentChanged(const Akonadi::Collection&)), this, SLOT(changeCalendar(const Akonadi::Collection&)));
    connect(ui->dateFrom, SIGNAL(dateChanged(const QDate&)), this, SLOT(changeDateFrom(const QDate&)));
    connect(ui->dateTo, SIGNAL(dateChanged(const QDate&)), this, SLOT(changeDateTo(const QDate&)));
    connect(ui->radioButtonAllEvents, SIGNAL(clicked()), this, SLOT(updateDatesFromTo()));
    connect(ui->radioButtonBetweenDates, SIGNAL(clicked()), this, SLOT(updateDatesFromTo()));
    connect(ui->checkBoxHideUnused, SIGNAL(clicked()), this, SLOT(updateHideUnused()));
    connect(ui->comboBoxHideUnused, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateHideUnused()));
    connect(ui->spinBoxHideUnused, SIGNAL(valueChanged(int)), this, SLOT(updateHideUnused()));

    ui->addEventButton->setIcon(QIcon::fromTheme("list-add"));
    ui->deleteEventButton->setIcon(QIcon::fromTheme("list-remove"));
    ui->deleteEventButton->setEnabled(false);
    connect(ui->addEventButton, SIGNAL(released()), this, SLOT(addEvent()));
    connect(ui->deleteEventButton, SIGNAL(released()), this, SLOT(deleteEvent()));
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

MainView::~MainView()
{
    delete ui;
    ui = nullptr;
}

void MainView::clearTreeWidget()
{
    ui->treeWidget->clear();
}

void MainView::addProject(QString& name, QTreeWidgetItem* item)
{
    ui->selectProject->addItem(name);
    ui->treeWidget->addTopLevelItem(item);
}

void MainView::changeProject(const QString& selectedProject)
{
    emit projectChanged(selectedProject);
}

void MainView::changeSelectedTask()
{
    emit taskChanged(ui->treeWidget->selectedItems().first());
}

void MainView::changeCalendar(const Akonadi::Collection& newCollection)
{
    ui->selectProject->clear();
    ui->selectProject->addItem(i18n("Show all projects"));
    emit calendarChanged(newCollection);
}

void MainView::changeDateFrom(const QDate &newDate)
{
    const QDate aLongTimeAgo;
    if (ui->radioButtonAllEvents->isChecked())
        emit dateFromChanged(aLongTimeAgo);
    else
        emit dateFromChanged(newDate);
}

void MainView::changeDateTo(const QDate &newDate)
{
    const QDate today = QDate::currentDate();
    if (ui->radioButtonAllEvents->isChecked())
        emit dateToChanged(today);
    else
        emit dateToChanged(newDate);
}

void MainView::updateDatesFromTo()
{
    changeDateFrom(ui->dateFrom->date());
    changeDateTo(ui->dateTo->date());
}

void MainView::updateHideUnused()
{
    emit hideUnusedChanged();
}


void MainView::anEventSelected()
{
    emit activateDeleteButton(true);
}

void MainView::noEventSelected()
{
    emit activateDeleteButton(false);
}

void MainView::setTask(Task* task)
{
    m_currentTask = task;
    ui->taskNameEdit->setText(task->m_name);
    updateTableEvents();
}

void MainView::updateTableEvents()
{
    //During update, updateTask() should not be called
    disconnect(ui->tableEvents, SIGNAL(cellChanged(int,int)), this, SLOT(updateTask()));

    int row = 0;
    ui->tableEvents->setSortingEnabled(false);
    ui->tableEvents->clearContents();
    while (ui->tableEvents->rowCount() > 0)
        ui->tableEvents->removeRow(0);

    if (m_currentTask == nullptr)
    {
        qDebug("Kourglass: MainView::updateTableEvents() called while m_currentTask is null; aborting.");
        return;
    }
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

    //Table updated; we can connect updateTask() again to save user's changes
    connect(ui->tableEvents, SIGNAL(cellChanged(int,int)), this, SLOT(updateTask()));
}

void MainView::resizeTableEventsColumns()
{
    ui->tableEvents->resizeColumnToContents(2);
    ui->tableEvents->resizeColumnToContents(3);
}

void MainView::updateTask()
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

void MainView::addEvent()
{
    updateTask();
    if (m_currentTask != nullptr)
    {
        m_currentTask->start();
        m_currentTask->stop();
    }
    else qDebug("Kouglass: MainView::addEvent called while m_currentTask is null; aborting.");
    updateTableEvents();
}

void MainView::deleteEvent()
{
    updateTask();
    QString uidToRemove = ui->tableEvents->item(ui->tableEvents->currentRow(), 0)->text();

    m_currentTask->removeEvent(uidToRemove);

    updateTableEvents();
}

void MainView::updateProgressValue(int value)
{
    ui->progressValue->setText(QVariant(value).toString() + "%");
}
