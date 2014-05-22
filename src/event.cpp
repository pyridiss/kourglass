#include "event.h"

#include <KJob>
#include <KDateTime>

#include <kcalcore/event.h>

#include <akonadi/item.h>
#include <akonadi/itemcreatejob.h>
#include <akonadi/itemmodifyjob.h>
#include <akonadi/itemfetchjob.h>
#include <akonadi/itemfetchscope.h>

Event::Event(QString& name, QString& uid, int akonadiId, QString& parentTask, const Collection& collection, QObject *parent) :
    QObject(parent)
{
    m_startTime = QDateTime::currentDateTime();
    m_name = name;

    if (uid == QString())
    {
        KCalCore::Event *eventPointer = new KCalCore::Event();
        QSharedPointer<KCalCore::Event> event(eventPointer);
        event->setSummary(name);
        event->setRelatedTo(parentTask);
        KDateTime start = KDateTime(m_startTime);
        event->setDtStart(start);
        event->setDtEnd(start);

        m_uid = event->uid();

        Item item;
        item.setMimeType("application/x-vnd.akonadi.calendar.event");
        item.setPayload<QSharedPointer<KCalCore::Event>>(event);

        ItemCreateJob *job = new ItemCreateJob(item, collection, this);
        connect(job, SIGNAL(result(KJob*)), this, SLOT(creationFinished(KJob*)));
    }
    else
    {
        m_uid = uid;
        m_akonadiId = akonadiId;
    }
}

Event::~Event()
{
}

void Event::creationFinished(KJob* job)
{
    if (job->error()) return;

    ItemCreateJob *createJob = qobject_cast<ItemCreateJob*>(job);

    m_akonadiId = createJob->item().id();
}

void Event::start(QDateTime& start)
{
    m_startTime = start;
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

void Event::saveToAkonadi()
{
    ItemFetchJob *fetchJob = new ItemFetchJob(Item(m_akonadiId));
    connect(fetchJob, SIGNAL(result(KJob*)), SLOT(saveToAkonadiItemFetched(KJob*)));
    fetchJob->fetchScope().fetchFullPayload();
}

void Event::saveToAkonadiItemFetched(KJob* job)
{
    if (job->error()) return;

    ItemFetchJob *fetchJob = qobject_cast<ItemFetchJob*>(job);

    const Item item = fetchJob->items().first();

    if (item.mimeType() == QString("application/x-vnd.akonadi.calendar.event"))
        if (item.hasPayload<QSharedPointer<KCalCore::Incidence>>())
        {
            QSharedPointer<KCalCore::Incidence> incidence = item.payload<QSharedPointer<KCalCore::Incidence>>();
            QSharedPointer<KCalCore::Event> event = incidence.dynamicCast<KCalCore::Event>();

            event->setSummary(m_name);

            KDateTime start = KDateTime(m_startTime);
            event->setDtStart(start);

            KDateTime end = KDateTime(m_endTime);
            event->setDtEnd(end);

            ItemModifyJob *modifyJob = new ItemModifyJob(item);
            connect(modifyJob, SIGNAL(result(KJob*)), SLOT(saveToAkonadiItemSaved(KJob*)));
        }
}

void Event::saveToAkonadiItemSaved(KJob *job)
{
    if (job->error()) return;
}
