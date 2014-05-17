#include "task.h"

Task::Task(QObject *parent) :
    QObject(parent)
{
    m_name = "Task";
    m_widgetItem = new QTreeWidgetItem();
    m_currentDuration.setHMS(0, 0, 0);
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
    delete m_widgetItem;
    m_widgetItem = nullptr;
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
    m_currentDuration = m_currentDuration.addMSecs(msecs);
    if (m_parent != nullptr && addToParent)
        m_parent->addRunningTime(msecs);
}

void Task::addChild(Task* child)
{
    m_children.push_back(child);
}

void Task::computeDuration()
{
    m_currentDuration.setHMS(0, 0, 0, 0);
    for (auto& child : m_children)
    {
        child->computeDuration();
    }
    for (auto& event : m_events)
    {
        m_currentDuration = m_currentDuration.addSecs(event->m_startTime.secsTo(event->m_endTime));
    }
    if (m_parent != nullptr)
    {
        qint64 msecs = ((m_currentDuration.hour() * 60 + m_currentDuration.minute()) * 60 + m_currentDuration.second()) * 1000;
        m_parent->addRunningTime(msecs, false);
    }
}