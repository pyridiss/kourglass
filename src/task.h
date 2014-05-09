#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QTreeWidgetItem>

class Task
{
    public:
        QString m_name;
        QString m_uid;
        QString m_parent;
        QString m_project;
        QTreeWidgetItem* m_widgetItem;

    public:
        Task();

    private:
        void setUid();
};

#endif //TASK_H