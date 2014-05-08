#ifndef STORAGE_H
#define STORAGE_H

#include <KXmlGuiWindow>
#include <QTreeWidget>
#include <QList>

#include "task.h"

class Storage : public QObject
{
    Q_OBJECT

    public:
        explicit Storage();
        ~Storage();

    public:
        QList<Task*> m_tasks;

    signals:
        void tasksChanged();

    public slots:
        void addProject();
        void addTask();
};

#endif // STORAGE_H
