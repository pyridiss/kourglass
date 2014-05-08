#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QTreeWidgetItem>

class Task
{
    public:
        QString m_name;
        QString m_uid;
        QString m_relatedTo;
        QTreeWidgetItem* m_widgetItem;
        bool m_shown = false;

    public:
        Task();

    private:
        void setUid();
};

#endif //TASK_H