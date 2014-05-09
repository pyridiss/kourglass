#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>

namespace Ui {
class NewProjectDialog;
}

class NewProjectDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewProjectDialog(QWidget *parent = 0);
    ~NewProjectDialog();
    
private:
    Ui::NewProjectDialog *ui;

signals:
    void projectAccepted(QString&);
    void projectCancelled();

public slots:
    void accept();
    void reject();
};

#endif // NEWPROJECTDIALOG_H
