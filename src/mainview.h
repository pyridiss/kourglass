#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QWidget>
#include <akonadi/collection.h>

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
    void changeTreeView(Task*oldRoot, Task* newRoot);

private:
    Ui::MainView *ui;

signals:
    void projectChanged(const QString& selectedProject);
    void taskChanged(QTreeWidgetItem* selectedTask);
    void calendarChanged(const Collection& newCollection);

public slots:
    void changeProject(const QString& selectedProject);
    void changeSelectedTask();
    void changeCalendar(const Akonadi::Collection& newCollection);
};

#endif // MAINVIEW_H
