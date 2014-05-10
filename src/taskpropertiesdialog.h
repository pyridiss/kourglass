#ifndef EVENTSLISTDIALOG_H
#define EVENTSLISTDIALOG_H

#include <QDialog>

namespace Ui {
class EventsListDialog;
}

class EventsListDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit EventsListDialog(QWidget *parent = 0);
    ~EventsListDialog();
    
private:
    Ui::EventsListDialog *ui;
};

#endif // EVENTSLISTDIALOG_H
