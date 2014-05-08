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

void Storage::addProject()
{
    Task* newProject = new Task();
    m_tasks.insert(newProject->m_uid, newProject);
    emit tasksChanged();
}

void Storage::addTask()
{
    addProject();
}