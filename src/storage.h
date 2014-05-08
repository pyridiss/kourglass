#ifndef STORAGE_H
#define STORAGE_H

#include <KXmlGuiWindow>
#include <QTreeWidget>

class Storage : public QObject
{
    Q_OBJECT

    public slots:
        void addProject();
        void addTask();
};

#endif // STORAGE_H
