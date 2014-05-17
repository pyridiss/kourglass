#include "event.h"

#include <QVariant>

Event::Event()
{
    m_startTime = QDateTime::currentDateTime();
}

Event::Event(QDateTime& start)
{
    m_startTime = start;
}

Event::~Event()
{
}

void Event::end(QDateTime& end)
{
    m_endTime = end;
}

void Event::end()
{
    QDateTime now = QDateTime(QDateTime::currentDateTime());
    end(now);
}

QString& Event::getUid()
{
    return m_uid;
}
