#ifndef STORAGE_H
#define STORAGE_H

#include <KXmlGuiWindow>
#include <QTreeWidget>
#include <QString>
#include <QMap>

#include <kcalcore/memorycalendar.h>

#include <akonadi/collection.h>
#include <akonadi/itemfetchjob.h>

#include "task.h"

using namespace Akonadi;

class Storage : public QObject
{
    Q_OBJECT

    public:
        explicit Storage(QObject *parent = 0);
        ~Storage();

    public:
        QMap<QString, Task*> m_tasks;
        Collection m_currentCollection;
        KCalCore::MemoryCalendar* m_memoryCalendar;

    public:
        void startTask(QString& task);
        void stopTask(QString& task);
        void removeTask(QString task);

    private:
        void findChildrenOf(QString parent, Akonadi::ItemFetchJob *fetchJob, QString project);
        void findEventsRelated(QString task, ItemFetchJob *fetchJob);
        void removeTaskAndEvents(QString task);
        void removeTaskWithoutEvents(QString task);
        void clearDatabase(QString task);

    signals:
        void tasksChanged();
        void projectLoaded(QString& name, QTreeWidgetItem* project);
        void beginEventsClean();

    public slots:
        QTreeWidgetItem* addProject(QString& name, qint64 akonadiId = 0, QString uid = QString());
        QTreeWidgetItem* addTask(Task* parent, QString& name, qint64 akonadiId = 0, QString uid = QString());
        void updateDuration();
        void computeAllDurations();
        void loadCalendar(const Collection& newCalendar);
        void newJobFromLoading(KJob *job);
        void removeTaskEnd(QString task);
};

#endif // STORAGE_H
