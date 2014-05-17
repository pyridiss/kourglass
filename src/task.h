#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QTreeWidgetItem>
#include <QTime>
#include <QList>
#include <QMap>

#include "event.h"

class Task : public QObject
{
    Q_OBJECT

    public:
        QString m_name;
        QString m_uid;
        Task* m_parent;
        QList<Task*> m_children;
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
        explicit Task(QObject *parent = 0);
        ~Task();
        void start();
        void stop();
        void addRunningTime(int msecs = 0, bool addToParent = true);
        void addChild(Task* child);
        void computeDuration();
};

#endif //TASK_H