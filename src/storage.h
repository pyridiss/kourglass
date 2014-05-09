#ifndef STORAGE_H
#define STORAGE_H

#include <KXmlGuiWindow>
#include <QTreeWidget>
#include <QString>
#include <QMap>

#include "task.h"

class Storage : public QObject
{
    Q_OBJECT

    public:
        explicit Storage();
        ~Storage();

    public:
        QMap<QString, Task*> m_tasks;

    signals:
        void tasksChanged();

    public slots:
        QTreeWidgetItem* addProject(QString& name);
        QTreeWidgetItem* addTask(QString& project, QString& parent, QString& name);
};

#endif // STORAGE_H
