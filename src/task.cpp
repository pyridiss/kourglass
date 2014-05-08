#include "task.h"

#include <QVariant>

Task::Task()
{
    m_name = "Task";
    m_widgetItem = new QTreeWidgetItem();
    setUid();
}

void Task::setUid()
{
    static unsigned int uid = 0;
    m_uid = QVariant(uid).toString();
    ++uid;
}