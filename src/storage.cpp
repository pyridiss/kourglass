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
    m_tasks.push_back(newProject);
    emit tasksChanged();
}

void Storage::addTask()
{
    addProject();
    emit tasksChanged();
}