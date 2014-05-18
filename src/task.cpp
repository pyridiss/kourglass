#include <KJob>

#include <kcalcore/todo.h>

#include <akonadi/item.h>
#include <akonadi/itemcreatejob.h>

#include "task.h"

using namespace Akonadi;

Task::Task(QString& name, QString& uid, Task* parentTask, const Collection& collection, QObject *parent) :
    QObject(parent)
{
    m_name = name;
    m_widgetItem = new QTreeWidgetItem();
    m_currentDuration.reset();
    m_parent = parentTask;
    m_lastEvent = nullptr;
    m_running = false;

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
    if (job->error())
        qDebug() << "Error occurred";
    else
        qDebug() << "Task created successfully";
}

void Task::start()
{
    m_runningTime.start();
    m_running = true;
    Event* newEvent = new Event();
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
        m_events[m_currentEvent]->end();
}

void Task::addRunningTime(int msecs, bool addToParent)
{
    if (msecs == -1)
    {
        msecs = m_runningTime.elapsed();
        m_runningTime.restart();
    }
    m_currentDuration.add(msecs);
    if (m_parent != nullptr && addToParent)
        m_parent->addRunningTime(msecs);
}

void Task::addChild(Task* child)
{
    m_children.push_back(child);
}

void Task::computeDuration()
{
    m_currentDuration.reset();
    for (auto& child : m_children)
    {
        child->computeDuration();
    }
    for (auto& event : m_events)
    {
        m_currentDuration.add(event->m_startTime.secsTo(event->m_endTime) * 1000);
    }
    if (m_parent != nullptr)
    {
        m_parent->addRunningTime(m_currentDuration.msecs(), false);
    }
}