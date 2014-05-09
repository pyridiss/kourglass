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
    newProject->m_name = name;
    return newProject->m_widgetItem;
}

QTreeWidgetItem* Storage::addTask(QString& project, Task* parent, QString& name)
{
    Task* newTask = new Task();
    m_tasks.insert(newTask->m_uid, newTask);
    newTask->m_widgetItem->setText(0, name);
    newTask->m_widgetItem->setText(1, "00:00");
    newTask->m_name = name;
    newTask->m_project = project;
    newTask->m_parent = parent;
    return newTask->m_widgetItem;
}

void Storage::updateDuration()
{
    for (auto& i : m_tasks)
    {
        if (i->running)
            i->addRunningTime();
    }
    for (auto& i : m_tasks)
    {
        i->m_widgetItem->setText(1, i->currentDuration.toString());
    }
}