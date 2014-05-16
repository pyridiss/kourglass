#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QWidget>

#include "task.h"

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
    void changeTreeView(Task*oldRoot, Task* newRoot);

private:
    Ui::MainView *ui;

signals:
    void projectChanged(const QString& selectedProject);
    void taskChanged(QTreeWidgetItem* selectedTask);

public slots:
    void changeProject(const QString& selectedProject);
    void changeSelectedTask();
    void updateCalendarsList(KJob *job);
};

#endif // MAINVIEW_H
