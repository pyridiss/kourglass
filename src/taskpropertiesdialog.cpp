#include "eventslistdialog.h"
#include "ui_eventslistdialog.h"

EventsListDialog::EventsListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EventsListDialog)
{
    ui->setupUi(this);
}

EventsListDialog::~EventsListDialog()
{
    delete ui;
}
