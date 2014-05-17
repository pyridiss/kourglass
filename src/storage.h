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

    public:
        void startTask(QString& task);
        void stopTask(QString& task);
        void removeTask(QString task);
        static QString getNewUid();

    private:
        void findChildrenOf(QString parent, Akonadi::ItemFetchJob *fetchJob, QString project);

    signals:
        void tasksChanged();
        void projectLoaded(QString& name, QTreeWidgetItem* project);

    public slots:
        QTreeWidgetItem* addProject(QString& name, QString uid = getNewUid());
        QTreeWidgetItem* addTask(QString& project, Task* parent, QString& name, QString uid = getNewUid());
        void updateDuration();
        void computeAllDurations();
        void loadCalendar(const Collection& newCalendar);
        void newJobFromLoading(KJob *job);
};

#endif // STORAGE_H
