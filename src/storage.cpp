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

QTreeWidgetItem* Storage::addProject(QString& name)
{
    Task* newProject = new Task();
    m_tasks.insert(newProject->m_uid, newProject);
    newProject->m_widgetItem->setText(0, name);
    newProject->m_widgetItem->setText(1, "00:00");
    return newProject->m_widgetItem;
}

QTreeWidgetItem* Storage::addTask(QString& parent, QString& name)
{
    Task* newTask = new Task();
    m_tasks.insert(newTask->m_uid, newTask);
    newTask->m_widgetItem->setText(0, name);
    newTask->m_widgetItem->setText(1, "00:00");
    newTask->m_relatedTo = parent;
    return newTask->m_widgetItem;
}