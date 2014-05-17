#ifndef STORAGE_H
#define STORAGE_H

#include <KXmlGuiWindow>
#include <QTreeWidget>
#include <QString>
#include <QMap>

#include <akonadi/collection.h>

#include "task.h"

using namespace Akonadi;

class Storage : public QObject
{
    Q_OBJECT

    public:
        explicit Storage();
        ~Storage();

    public:
        QMap<QString, Task*> m_tasks;

    public:
        void startTask(QString& task);
        void stopTask(QString& task);
        void removeTask(QString task);

    signals:
        void tasksChanged();
        void projectLoaded(QString& project);

    public slots:
        QTreeWidgetItem* addProject(QString& name);
        QTreeWidgetItem* addTask(QString& project, Task* parent, QString& name);
        void updateDuration();
        void computeAllDurations();
        void loadCalendar(const Collection& newCalendar);
};

#endif // STORAGE_H
