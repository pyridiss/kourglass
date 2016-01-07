#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QWidget>
#include <QDialog>

#include <AkonadiCore/Collection>

#include "task.h"

using namespace Akonadi;

class KJob;

namespace Ui {
class MainView;
}

class MainView : public QWidget
{
    Q_OBJECT

public:
    explicit MainView(QWidget *parent = 0);
    ~MainView();

public:
    void addProject(QString& name, QTreeWidgetItem* item);
    void setTask(Task* task);

private:
    Ui::MainView *ui;
    Task* m_currentTask;

private:
    void updateTableEvents();

signals:
    void projectChanged(const QString& selectedProject);
    void taskChanged(QTreeWidgetItem* selectedTask);
    void calendarChanged(const Collection& newCollection);
    void dateFromChanged(const QDate& newDate);
    void dateToChanged(const QDate& newDate);
    void hideUnusedChanged();
    void allDurationsChanged();
    void activateDeleteButton(bool);

public slots:
    void changeProject(const QString& selectedProject);
    void changeSelectedTask();
    void changeCalendar(const Akonadi::Collection& newCollection);
    void changeDateFrom(const QDate& newDate);
    void changeDateTo(const QDate& newDate);
    void updateDatesFromTo();
    void updateHideUnused();
    void clearTreeWidget();
    void updateTask();
    void addEvent();
    void deleteEvent();
    void anEventSelected();
    void noEventSelected();
    void resizeTableEventsColumns();
    void updateProgressValue(int value);

    //MainWindow really needs to access ui...
    friend class MainWindow;
};

#endif // MAINVIEW_H

