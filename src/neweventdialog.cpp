#include "neweventdialog.h"
#include "ui_neweventdialog.h"

NewEventDialog::NewEventDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewEventDialog)
{
    ui->setupUi(this);
}

NewEventDialog::~NewEventDialog()
{
    delete ui;
}
