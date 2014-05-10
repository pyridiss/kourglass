#include "event.h"

#include <QVariant>

Event::Event()
{
    m_startTime = QDateTime::currentDateTime();
    setUid();
}

Event::Event(QDateTime& start)
{
    m_startTime = start;
    setUid();
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
    QDateTime now = QDateTime::currentDateTime();
    end(now);
}

QString& Event::getUid()
{
    return m_uid;
}

void Event::setUid()
{
    static unsigned int uid = 100;
    m_uid = QVariant(uid).toString();
    ++uid;
}