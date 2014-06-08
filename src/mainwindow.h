#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <KXmlGuiWindow>
#include <QString>

#include "storage.h"
#include "mainview.h"
#include "newprojectdialog.h"
#include "newtaskdialog.h"
#include "taskpropertiesdialog.h"
#include "neweventdialog.h"

class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent=0);
        virtual ~MainWindow();

    private:
        NewProjectDialog* m_addProjectDialog;
        NewTaskDialog* m_addTaskDialog;
        NewEventDialog* m_addEventDialog;
        TaskPropertiesDialog* m_taskPropertiesDialog;
        MainView* m_mainView;

        QString m_currentTask;
        Storage* m_storage;
        void setupActions();

    signals:
        void currentTaskIsRealTask(bool);
        void aCalendarIsSelected(bool);
        void aTaskIsSelected(bool);
        void aRunningTaskIsSelected(bool);
        void aNonRunningTaskIsSelected(bool);

    public slots:
        void changeCurrentProject(const QString& cur);
        void addProject(QString& name);
        void addProjectLoaded(QString& name, QTreeWidgetItem* project);
        void addTask(QString& name);
        void setCurrentTask(QTreeWidgetItem* cur);
        void startCurrentTask();
        void stopCurrentTask();
        void removeCurrentTask();
        void renameLastEvent(QString& name);
        void showTaskProperties();
        void setCurrentCalendar(const Collection& calendar);
};

#endif // MAINWINDOW_H
