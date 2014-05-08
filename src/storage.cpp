#include "storage.h"

Storage::Storage()
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

QTreeWidgetItem* Storage::addProject()
{
    Task* newProject = new Task();
    m_tasks.insert(newProject->m_uid, newProject);
    return newProject->m_widgetItem;
}

QTreeWidgetItem* Storage::addTask(QString& parent)
{
    Task* newTask = new Task();
    m_tasks.insert(newTask->m_uid, newTask);
    newTask->m_relatedTo = parent;
    return newTask->m_widgetItem;
}