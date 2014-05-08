#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QTreeWidgetItem>

class Task
{
    public:
        QString m_name;
        QTreeWidgetItem* m_widgetItem;
        bool m_shown = false;

    public:
        Task();
};

#endif //TASK_H