#ifndef STORAGE_H
#define STORAGE_H

#include <KXmlGuiWindow>
#include <QTreeWidget>
#include <QString>
#include <QMap>
#include <QDate>

#include <kcalcore/memorycalendar.h>

#include <AkonadiCore/Collection>
#include <AkonadiCore/ItemFetchJob>

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
        QDate m_dateFrom;
        QDate m_dateTo;

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
        void setDateFrom(const QDate &newDateFrom);
        void setDateTo(const QDate &newDateFrom);
        void hideUnusedTasks(QString& currentProject, bool hide, int intDuration, QString qStringDuration);
};

#endif // STORAGE_H
