#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <KXmlGuiWindow>
#include <QString>
#include <QLabel>

#include "storage.h"
#include "mainview.h"
#include "newprojectdialog.h"
#include "newtaskdialog.h"
#include "neweventdialog.h"

class KStatusNotifierItem;

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
        MainView* m_mainView;
        KStatusNotifierItem* systemTray;

        QString m_currentProject;
        QString m_currentTask;
        Storage* m_storage;

        QLabel* m_statusBarTotalTime;
        QLabel* m_statusBarToday;
        QLabel* m_statusBarWeek;

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
        void updateStatusBar();
        void hideUnusedChanged();
};

#endif // MAINWINDOW_H
