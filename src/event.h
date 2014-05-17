#ifndef EVENT_H
#define EVENT_H

#include <KDateTime>

class Event
{
    public:
        Event();
        Event(KDateTime& start);
        ~Event();
        QString& getUid();
        void end(KDateTime& end);
        void end();

    public:
        QString m_name;
        KDateTime m_startTime;
        KDateTime m_endTime;
        QString m_uid;
};

#endif //EVENT_H