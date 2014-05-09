#ifndef NEWTASKDIALOG_H
#define NEWTASKDIALOG_H

#include <QDialog>

namespace Ui {
class NewTaskDialog;
}

class NewTaskDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewTaskDialog(QWidget *parent = 0);
    ~NewTaskDialog();
    
private:
    Ui::NewTaskDialog *ui;

signals:
    void taskAccepted(QString&);
    void taskCancelled();

public slots:
    void accept();
    void reject();
};

#endif // NEWTASKDIALOG_H
