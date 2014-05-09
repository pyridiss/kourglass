#include "task.h"

#include <QVariant>

Task::Task()
{
    m_name = "Task";
    m_widgetItem = new QTreeWidgetItem();
    setUid();
    currentDuration.setHMS(0, 0, 0);
    m_parent = nullptr;
}

void Task::setUid()
{
    static unsigned int uid = 0;
    m_uid = QVariant(uid).toString();
    ++uid;
}

void Task::start()
{
    startTime = QDateTime::currentDateTime();
    runningTime.start();
    running = true;
}

void Task::stop()
{
    running = false;
    addRunningTime();
    m_widgetItem->setText(1, currentDuration.toString());
}

void Task::addRunningTime(int msecs)
{
    if (msecs == 0)
    {
        msecs = runningTime.elapsed();
        runningTime.restart();
    }
    currentDuration = currentDuration.addMSecs(msecs);
    if (m_parent != nullptr)
        m_parent->addRunningTime(msecs);
}