#ifndef EVENT_H
#define EVENT_H

#include <QDateTime>

class Event
{
    public:
        Event();
        Event(QDateTime& start);
        ~Event();
        QString& getUid();
        void end(QDateTime& end);
        void end();

    public:
        QString m_name;
        QDateTime m_startTime;
        QDateTime m_endTime;
        QString m_uid;
};

#endif //EVENT_H