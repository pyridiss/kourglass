#include "event.h"

#include <QVariant>

Event::Event()
{
    m_startTime = KDateTime(QDateTime::currentDateTime());
}

Event::Event(KDateTime& start)
{
    m_startTime = start;
}

Event::~Event()
{
}

void Event::end(KDateTime& end)
{
    m_endTime = end;
}

void Event::end()
{
    KDateTime now = KDateTime(QDateTime::currentDateTime());
    end(now);
}

QString& Event::getUid()
{
    return m_uid;
}
