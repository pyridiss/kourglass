#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QTreeWidgetItem>
#include <QTime>
#include <QMap>

#include "event.h"

class Task
{
    public:
        QString m_name;
        QString m_uid;
        Task* m_parent;
        QString m_project;
        QTreeWidgetItem* m_widgetItem;
        QTime m_currentDuration;
        bool m_running;
        QMap<QString, Event*> m_events;
        Event* m_lastEvent;

    private:
        QString m_currentEvent;
        QTime m_runningTime;

    public:
        Task();
        ~Task();
        void start();
        void stop();
        void addRunningTime(int msecs = 0);

    private:
        void setUid();

};

#endif //TASK_H