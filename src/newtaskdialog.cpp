#include "newtaskdialog.h"
#include "ui_newtaskdialog.h"

NewTaskDialog::NewTaskDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewTaskDialog)
{
    ui->setupUi(this);
    ui->lineEdit->setFocus();
}

NewTaskDialog::~NewTaskDialog()
{
    delete ui;
}

void NewTaskDialog::accept()
{
    QString nameOfTask = ui->lineEdit->text();
    ui->lineEdit->clear();
    hide();
    emit taskAccepted(nameOfTask);
}

void NewTaskDialog::reject()
{
    ui->lineEdit->clear();
    hide();
    emit taskCancelled();
}