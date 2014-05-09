#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"

NewProjectDialog::NewProjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProjectDialog)
{
    ui->setupUi(this);
    ui->lineEdit->setFocus();
}

NewProjectDialog::~NewProjectDialog()
{
    delete ui;
}

void NewProjectDialog::accept()
{
    QString nameOfProject = ui->lineEdit->text();
    ui->lineEdit->clear();
    ui->lineEdit->setFocus();
    hide();
    emit projectAccepted(nameOfProject);
}

void NewProjectDialog::reject()
{
    ui->lineEdit->clear();
    hide();
    emit projectCancelled();
}