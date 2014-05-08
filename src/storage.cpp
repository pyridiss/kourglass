#include "storage.h"

Storage::Storage()
{
    m_tasks.clear();
}

Storage::~Storage()
{
    m_tasks.clear();
}

void Storage::addProject()
{
    Task newProject;
    m_tasks.push_back(newProject);
}

void Storage::addTask()
{
    addProject();
}