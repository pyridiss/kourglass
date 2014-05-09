#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <KXmlGuiWindow>
#include <QString>
#include <QTreeWidget>

#include "storage.h"
#include "newprojectdialog.h"
#include "newtaskdialog.h"

class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent=0);

    private:
        NewProjectDialog* m_addProjectDialog;
        NewTaskDialog* m_addTaskDialog;
        QTreeWidget* m_mainTree;
        QString m_currentTask;
        Storage* m_storage;
        void setupActions();

    public slots:
        void addProject(QString& name);
        void addTask(QString& name);
        void setCurrentTask(QTreeWidgetItem* cur);
};

#endif // MAINWINDOW_H
