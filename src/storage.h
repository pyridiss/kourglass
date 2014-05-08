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

    private:
        QList<Task> m_tasks;

    public slots:
        void addProject();
        void addTask();
};

#endif // STORAGE_H
