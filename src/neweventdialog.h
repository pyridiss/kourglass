#ifndef NEWEVENTDIALOG_H
#define NEWEVENTDIALOG_H

#include <QDialog>

namespace Ui {
class NewEventDialog;
}

class NewEventDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewEventDialog(QWidget *parent = 0);
    ~NewEventDialog();
    
private:
    Ui::NewEventDialog *ui;

signals:
    void eventAccepted(QString&);
    void eventCancelled();

public slots:
    void accept();
    void reject();
};

#endif // NEWEVENTDIALOG_H
