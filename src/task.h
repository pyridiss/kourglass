#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QTreeWidgetItem>
#include <QDateTime>
#include <QTime>

class Task
{
    public:
        QString m_name;
        QString m_uid;
        Task* m_parent;
        QString m_project;
        QTreeWidgetItem* m_widgetItem;
        QTime currentDuration;
        bool running;

    private:
        QDateTime startTime;
        QTime runningTime;

    public:
        Task();
        void start();
        void stop();
        void addRunningTime(int msecs = 0);

    private:
        void setUid();

};

#endif //TASK_H