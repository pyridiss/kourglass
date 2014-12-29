#include <KJob>

#include <kcalcore/todo.h>

#include <akonadi/item.h>
#include <akonadi/itemcreatejob.h>
#include <akonadi/itemdeletejob.h>
#include <akonadi/itemfetchjob.h>

#include "task.h"

using namespace Akonadi;

Task::Task(QString& name, QString& uid, qint64 akonadiId, Task* parentTask, const Collection& collection, QObject *parent) :
    QObject(parent)
{
    m_name = name;
    m_widgetItem = new QTreeWidgetItem();
    m_currentDuration.reset();
    m_parent = parentTask;
    m_lastEvent = nullptr;
    m_running = false;
    m_collection = collection;

    if (uid == QString())
    {
        KCalCore::Todo *taskPointer = new KCalCore::Todo();
        QSharedPointer<KCalCore::Todo> task(taskPointer);
        task->setSummary(name);
        if (parentTask != nullptr) task->setRelatedTo(parentTask->m_uid);

        m_uid = task->uid();

        Item item;
        item.setMimeType("application/x-vnd.akonadi.calendar.todo");
        item.setPayload<QSharedPointer<KCalCore::Todo>>(task);

        ItemCreateJob *job = new ItemCreateJob(item, collection, this);
        connect(job, SIGNAL(result(KJob*)), this, SLOT(creationFinished(KJob*)));
    }
    else
    {
        m_uid = uid;
        m_akonadiId = akonadiId;
    }
}

Task::~Task()
{
    for (auto& i : m_events)
    {
        delete i;
        i = nullptr;
    }
    m_events.clear();
}

void Task::creationFinished(KJob *job)
{
    if (job->error()) return;

    ItemCreateJob *createJob = qobject_cast<ItemCreateJob*>(job);

    m_akonadiId = createJob->item().id();
}

void Task::start()
{
    m_runningTime.start();
    m_running = true;
    QString name = "Unnamed";
    QString uid = QString();
    Event* newEvent = new Event(name, uid, 0, m_uid, m_collection, this);
    m_lastEvent = newEvent;
    m_events.insert(newEvent->getUid(), newEvent);
    m_currentEvent = newEvent->getUid();
}

void Task::stop()
{
    m_running = false;
    addRunningTime();
    m_widgetItem->setText(1, m_currentDuration.toString());
    if (m_events.find(m_currentEvent) != m_events.end())
        m_events[m_currentEvent]->setEndTime();
}

void Task::addRunningTime(int msecs, bool addToParent)
{
    if (msecs == -1)
    {
        msecs = m_runningTime.elapsed();
        m_runningTime.restart();
        if (m_events.find(m_currentEvent) != m_events.end())
            m_events[m_currentEvent]->setEndTime();
    }
    m_currentDuration.add(msecs);
    if (m_parent != nullptr && addToParent)
        m_parent->addRunningTime(msecs);
}

void Task::addChild(Task* child)
{
    m_children.push_back(child);
}

void Task::computeDuration(QDate& from, QDate& to)
{
    m_currentDuration.reset();
    for (auto& child : m_children)
    {
        child->computeDuration(from, to);
    }
    for (auto& event : m_events)
    {
        if (event->m_startTime.date() >= from &&
            event->m_startTime.date() <= to)
            m_currentDuration.add(event->m_startTime.secsTo(event->m_endTime) * 1000);
    }
    if (m_parent != nullptr)
    {
        m_parent->addRunningTime(m_currentDuration.msecs(), false);
    }
}

void Task::removeEvent(QString& uidToRemove)
{
    if (m_events.find(uidToRemove) != m_events.end())
    {
        m_events[uidToRemove]->removeFromAkonadi();
        m_events[uidToRemove] = nullptr;
        m_events.remove(uidToRemove);
    }
}

void Task::removeFromAkonadi()
{
    ItemFetchJob *fetchJob = new ItemFetchJob(Item(m_akonadiId), this);
    connect(fetchJob, SIGNAL(result(KJob*)), SLOT(removeFromAkonadiItemFetched(KJob*)));
}

void Task::removeFromAkonadiItemFetched(KJob *job)
{
    if (job->error()) return;

    ItemFetchJob *fetchJob = qobject_cast<ItemFetchJob*>(job);

    const Item item = fetchJob->items().first();

    ItemDeleteJob *deleteJob = new ItemDeleteJob(item);
    connect(deleteJob, SIGNAL(result(KJob*)), this, SLOT(removeFromAkonadiItemRemoved(KJob*)));
}

void Task::removeFromAkonadiItemRemoved(KJob *job)
{
    if (job->error()) return;
//     delete this;
}

void Task::clearDatabase()
{
    for (auto& i : m_events)
    {
        i->removeFromAkonadi();
        i = nullptr;
    }
    m_events.clear();

    emit cleanDone(m_uid);
}
