#include "task.h"

#include <QVariant>

Task::Task()
{
    m_name = "Task";
    m_widgetItem = new QTreeWidgetItem();
    setUid();
    currentDuration.setHMS(0, 0, 0);
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
    currentDuration = currentDuration.addMSecs(runningTime.elapsed());
    m_widgetItem->setText(1, currentDuration.toString());
}