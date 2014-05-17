#include "task.h"

Task::Task(QObject *parent) :
    QObject(parent)
{
    m_name = "Task";
    m_widgetItem = new QTreeWidgetItem();
    m_currentDuration.reset();
    m_parent = nullptr;
    m_lastEvent = nullptr;
    m_running = false;
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
    if (msecs == 0)
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