#include "task.h"

#include <QVariant>

Task::Task()
{
    m_name = "Task";
    m_widgetItem = new QTreeWidgetItem();
    setUid();
    m_currentDuration.setHMS(0, 0, 0);
    m_parent = nullptr;
    m_lastEvent = nullptr;
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

void Task::setUid()
{
    static unsigned int uid = 0;
    m_uid = QVariant(uid).toString();
    ++uid;
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

void Task::addRunningTime(int msecs)
{
    if (msecs == 0)
    {
        msecs = m_runningTime.elapsed();
        m_runningTime.restart();
    }
    m_currentDuration = m_currentDuration.addMSecs(msecs);
    if (m_parent != nullptr)
        m_parent->addRunningTime(msecs);
}