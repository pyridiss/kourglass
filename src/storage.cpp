#include <KIcon>

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

void Storage::removeTask(QString& task)
{
    if (m_tasks.find(task) != m_tasks.end())
    {
        if (m_tasks[task]->m_parent != nullptr)
        {
            m_tasks[task]->m_parent->m_widgetItem->removeChild(m_tasks[task]->m_widgetItem);
            m_tasks.remove(task);
        }
    }
}


QTreeWidgetItem* Storage::addProject(QString& name)
{
    Task* newProject = new Task();
    m_tasks.insert(newProject->m_uid, newProject);
    newProject->m_widgetItem->setText(0, name);
    newProject->m_widgetItem->setText(1, "00:00:00");
    newProject->m_name = name;
    return newProject->m_widgetItem;
}

QTreeWidgetItem* Storage::addTask(QString& project, Task* parent, QString& name)
{
    Task* newTask = new Task();
    m_tasks.insert(newTask->m_uid, newTask);
    newTask->m_widgetItem->setText(0, name);
    newTask->m_widgetItem->setText(1, "00:00:00");
    newTask->m_name = name;
    newTask->m_project = project;
    newTask->m_parent = parent;
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