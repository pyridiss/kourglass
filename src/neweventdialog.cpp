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

void NewEventDialog::accept()
{
    QString nameOfProject = ui->lineEdit->text();
    ui->lineEdit->clear();
    ui->lineEdit->setFocus();
    hide();
    emit eventAccepted(nameOfProject);
}

void NewEventDialog::reject()
{
    ui->lineEdit->clear();
    hide();
    emit eventCancelled();
}
