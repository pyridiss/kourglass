#ifndef TASKPROPERTIESDIALOG_H
#define TASKPROPERTIESDIALOG_H

#include <QDialog>

#include "task.h"

namespace Ui {
class TaskPropertiesDialog;
}

class TaskPropertiesDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit TaskPropertiesDialog(QWidget *parent = 0);
    ~TaskPropertiesDialog();
    void setTask(Task* task);
    
private:
    Ui::TaskPropertiesDialog *ui;
    Task* m_currentTask;
    
private:
    void updateTableEvents();

signals:
    void allDurationsChanged();

public slots:
    void updateTask();
    void addEvent();
    void deleteEvent();
};

#endif // TASKPROPERTIESDIALOG_H
