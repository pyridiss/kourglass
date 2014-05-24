#include <KIcon>
#include <KMessageBox>
#include <KLocale>

#include <akonadi/collection.h>
#include <akonadi/itemfetchjob.h>
#include <akonadi/itemfetchscope.h>
#include <kcalcore/incidence.h>
#include <kcalcore/event.h>

#include "storage.h"

using namespace Akonadi;

Storage::Storage(QObject *parent) :
    QObject(parent)
{
    m_tasks.clear();
}

Storage::~Storage()
{
    for (auto& i : m_tasks)
    {
        delete i;
        i = nullptr;
    }
    m_tasks.clear();
}

void Storage::startTask(QString& task)
{
    if (m_tasks.find(task) != m_tasks.end())
    {
        m_tasks[task]->m_widgetItem->setIcon(0, KIcon("arrow-right"));
        m_tasks[task]->start();
    }
}

void Storage::stopTask(QString& task)
{
    if (m_tasks.find(task) != m_tasks.end())
    {
        if (!m_tasks[task]->m_running) return;
        m_tasks[task]->m_widgetItem->setIcon(0, KIcon("media-playback-pause"));
        m_tasks[task]->stop();
    }
}

void Storage::removeTask(QString task)
{
    //Be careful: task MUST NOT be a reference, as m_currentTask will change when the tree widget is updated

    int result = KMessageBox::questionYesNoCancel(
        0,
        i18n("This task and all sub-tasks will be deleted. Do you want to delete all related events?\nIf no, events will be reaffected to the parent task."),
        i18n("Question")
    );

    if (result == KMessageBox::Cancel) return;

    if (m_tasks.find(task) != m_tasks.end())
    {
        if (result == KMessageBox::Yes)
            removeTaskAndEvents(m_tasks[task]->m_uid);
        if (result == KMessageBox::No)
            removeTaskWithoutEvents(m_tasks[task]->m_uid);
    }
}

void Storage::removeTaskAndEvents(QString task)
{
    if (m_tasks.find(task) != m_tasks.end())
    {
        Task* toRemove = m_tasks[task];
        while (!toRemove->m_children.isEmpty())
            removeTaskAndEvents(toRemove->m_children.at(0)->m_uid);

        if (toRemove->m_parent != nullptr)
        {
            toRemove->m_parent->m_widgetItem->removeChild(toRemove->m_widgetItem);
            toRemove->m_parent->m_children.removeOne(toRemove);
        }
        connect(this, SIGNAL(beginEventsClean()), toRemove, SLOT(clearDatabase()));
        connect(toRemove, SIGNAL(cleanDone(QString)), this, SLOT(removeTaskEnd(QString)));
        emit beginEventsClean();
    }
}

void Storage::removeTaskEnd(QString task)
{
    if (m_tasks.find(task) != m_tasks.end())
    {
        m_tasks[task]->removeFromAkonadi();
        m_tasks[task] = nullptr;
        m_tasks.remove(task);
    }
}

void Storage::removeTaskWithoutEvents(QString task)
{
    if (m_tasks.find(task) != m_tasks.end())
    {
        Task* toRemove = m_tasks[task];
        while (!toRemove->m_children.isEmpty())
            removeTaskWithoutEvents(toRemove->m_children.at(0)->m_uid);

        if (toRemove->m_parent != nullptr)
        {
            for (auto& i : toRemove->m_events)
            {
                i->m_parentTask = toRemove->m_parent->m_uid;
                toRemove->m_parent->m_events.insert(i->m_uid, i);
                i->saveToAkonadi();
            }
            toRemove->m_events.clear();
            toRemove->m_parent->m_widgetItem->removeChild(toRemove->m_widgetItem);
            toRemove->m_parent->m_children.removeOne(toRemove);
        }
        removeTaskEnd(task);
    }
}

QTreeWidgetItem* Storage::addProject(QString& name, qint64 akonadiId, QString uid)
{
    Task* newProject = new Task(name, uid, akonadiId, nullptr, m_currentCollection, this);
    m_tasks.insert(uid, newProject);
    newProject->m_widgetItem->setText(0, name);
    newProject->m_widgetItem->setText(1, "00:00:00");
    newProject->m_name = name;
    return newProject->m_widgetItem;
}

QTreeWidgetItem* Storage::addTask(QString& project, Task* parent, QString& name, qint64 akonadiId, QString uid)
{
    Task* newTask = new Task(name, uid, akonadiId, parent, m_currentCollection, this);
    m_tasks.insert(newTask->m_uid, newTask);
    newTask->m_widgetItem->setText(0, name);
    newTask->m_widgetItem->setText(1, "00:00:00");
    newTask->m_project = project;
    parent->addChild(newTask);
    return newTask->m_widgetItem;
}

void Storage::updateDuration()
{
    for (auto& i : m_tasks)
    {
        if (i->m_running)
            i->addRunningTime();
    }
    for (auto& i : m_tasks)
    {
        i->m_widgetItem->setText(1, i->m_currentDuration.toString());
    }
}

void Storage::computeAllDurations()
{
    for (auto& i : m_tasks)
    {
        if (i->m_parent == nullptr)
            i->computeDuration();
    }
}

void Storage::clearDatabase(QString task)
{
    if (m_tasks.find(task) != m_tasks.end())
    {
        Task* toRemove = m_tasks[task];
        while (!toRemove->m_children.isEmpty())
            clearDatabase(toRemove->m_children.at(0)->m_uid);

        if (toRemove->m_parent != nullptr)
        {
            toRemove->m_parent->m_widgetItem->removeChild(toRemove->m_widgetItem);
            toRemove->m_parent->m_children.removeOne(toRemove);
        }
        delete m_tasks[task];
        m_tasks[task] = nullptr;
        m_tasks.remove(task);
    }
}

void Storage::loadCalendar(const Collection& newCalendar)
{
    while (!m_tasks.empty())
    {
        clearDatabase(m_tasks.begin().key());
    }

    ItemFetchJob *job = new ItemFetchJob(newCalendar, this);
    connect(job, SIGNAL(result(KJob*)), this, SLOT(newJobFromLoading(KJob*)));
    job->fetchScope().fetchFullPayload();
    m_currentCollection = newCalendar;
}

void Storage::newJobFromLoading(KJob *job)
{
    if (job->error()) return;

    ItemFetchJob *fetchJob = qobject_cast<ItemFetchJob*>(job);

    const Item::List items = fetchJob->items();
    for (const Item &item : items)
    {
        if (item.mimeType() == QString("application/x-vnd.akonadi.calendar.todo"))
            if (item.hasPayload<QSharedPointer<KCalCore::Incidence>>())
            {
                QSharedPointer<KCalCore::Incidence> incidence = item.payload<QSharedPointer<KCalCore::Incidence>>();
                if (incidence->relatedTo() == QString(""))
                {
                    QString name = incidence->summary();
                    qint64 akonadiId = item.id();
                    QTreeWidgetItem* project = addProject(name, akonadiId, incidence->uid());
                    emit projectLoaded(name, project);
                    findChildrenOf(incidence->uid(), fetchJob, incidence->uid());
                    findEventsRelated(incidence->uid(), fetchJob);
                }
            }
    }
    computeAllDurations();
}

void Storage::findChildrenOf(QString parent, ItemFetchJob *fetchJob, QString project)
{
    const Item::List items = fetchJob->items();
    for (const Item &item : items)
    {
        if (item.mimeType() == QString("application/x-vnd.akonadi.calendar.todo"))
            if (item.hasPayload<QSharedPointer<KCalCore::Incidence>>())
            {
                QSharedPointer<KCalCore::Incidence> incidence = item.payload<QSharedPointer<KCalCore::Incidence>>();
                if (incidence->relatedTo() == parent)
                {
                    QString name = incidence->summary();
                    qint64 akonadiId = item.id();
                    QTreeWidgetItem* task = addTask(project, m_tasks[parent], name, akonadiId, incidence->uid());
                    m_tasks[parent]->m_widgetItem->addChild(task);
                    m_tasks[parent]->m_widgetItem->setExpanded(true);
                    findChildrenOf(incidence->uid(), fetchJob, project);
                    findEventsRelated(incidence->uid(), fetchJob);
                }
            }
    }
}

void Storage::findEventsRelated(QString task, ItemFetchJob *fetchJob)
{
    const Item::List items = fetchJob->items();
    for (const Item &item : items)
    {
        if (item.mimeType() == QString("application/x-vnd.akonadi.calendar.event"))
            if (item.hasPayload<QSharedPointer<KCalCore::Incidence>>())
            {
                QSharedPointer<KCalCore::Incidence> incidence = item.payload<QSharedPointer<KCalCore::Incidence>>();
                QSharedPointer<KCalCore::Event> event = incidence.dynamicCast<KCalCore::Event>();
                if (incidence->relatedTo() == task)
                {
                    QString name = incidence->summary();
                    QString uid = incidence->uid();
                    qint64 akonadiId = item.id();
                    Event *newEvent = new Event(name, uid, akonadiId, task, m_currentCollection, this);
                    QString dtStart = event->dtStart().toString("\%Y-\%m-\%d \%H:\%M:\%S");
                    newEvent->m_startTime = QDateTime::fromString(dtStart, QString("yyyy-MM-dd HH:mm:ss"));
                    QString dtEnd = event->dtEnd().toString("\%Y-\%m-\%d \%H:\%M:\%S");
                    newEvent->m_endTime = QDateTime::fromString(dtEnd, QString("yyyy-MM-dd HH:mm:ss"));
                    m_tasks[task]->m_events.insert(incidence->uid(), newEvent);
                }
            }
    }
}