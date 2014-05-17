#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QTreeWidgetItem>
#include <QTime>
#include <QList>
#include <QMap>

#include "event.h"

class Duration
{
    unsigned long long m_msecs;

public:
    QString toString()
    {
        QString str;
        unsigned long long hours = m_msecs / 1000 / 60 / 60;
        if (hours < 10) str += "0";
        str += QVariant(hours).toString();
        str += ":";
        unsigned long long minutes = m_msecs / 1000 / 60 - 60 * hours;
        if (minutes < 10) str += "0";
        str += QVariant(minutes).toString();
        str += ":";
        unsigned long long seconds = m_msecs / 1000 - 60 * 60 * hours - 60 * minutes;
        if (seconds < 10) str += "0";
        str += QVariant(seconds).toString();
        return str;
    }

    void reset()
    {
        m_msecs = 0;
    }
    
    void add(unsigned long long delta)
    {
        m_msecs += delta;
    }

    unsigned long long int msecs()
    {
        return m_msecs;
    }
};

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
        Duration m_currentDuration;
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