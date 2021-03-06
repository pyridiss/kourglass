#ifndef EVENT_H
#define EVENT_H

#include <QDateTime>

#include <AkonadiCore/Collection>

using namespace Akonadi;

class KJob;

class Event : public QObject
{
    Q_OBJECT

    public:
        Event(QString& name, QString& uid, qint64 akonadiId, QString& parentTask, const Collection& collection, QObject *parent);
        ~Event();
        QString& getUid();
        void start(QDateTime& start);
        void setEndTime();
        void saveToAkonadi();
        void removeFromAkonadi();

    public:
        QString m_name;
        QDateTime m_startTime;
        QDateTime m_endTime;
        QString m_uid;
        qint64 m_akonadiId;
        QString m_parentTask;

    public slots:
        void creationFinished(KJob* job);
        void saveToAkonadiItemFetched(KJob* job);
        void saveToAkonadiItemSaved(KJob *job);
        void removeFromAkonadiItemFetched(KJob *job);
        void removeFromAkonadiItemRemoved(KJob *job);
};

#endif //EVENT_H
