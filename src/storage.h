#ifndef STORAGE_H
#define STORAGE_H

#include <KXmlGuiWindow>
#include <QTreeWidget>
#include <QString>
#include <QMap>

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

    public:
        void startTask(QString& task);
        void stopTask(QString& task);
        void removeTask(QString task);

    private:
        void findChildrenOf(QString parent, Akonadi::ItemFetchJob *fetchJob, QString project);
        void findEventsRelated(QString task, ItemFetchJob *fetchJob);

    signals:
        void tasksChanged();
        void projectLoaded(QString& name, QTreeWidgetItem* project);

    public slots:
        QTreeWidgetItem* addProject(QString& name, QString uid = QString());
        QTreeWidgetItem* addTask(QString& project, Task* parent, QString& name, QString uid = QString());
        void updateDuration();
        void computeAllDurations();
        void loadCalendar(const Collection& newCalendar);
        void newJobFromLoading(KJob *job);
};

#endif // STORAGE_H
