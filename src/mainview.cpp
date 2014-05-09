#include "mainview.h"
#include "ui_mainview.h"

MainView::MainView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainView)
{
    ui->setupUi(this);
    connect(ui->selectProject, SIGNAL(activated(const QString&)), this, SLOT(changeProject(const QString&)));
    connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(changeSelectedTask(QTreeWidgetItem*)));
}

MainView::~MainView()
{
    delete ui;
}

void MainView::addProject(QString& name, QTreeWidgetItem* item)
{
    ui->selectProject->addItem(name);
    ui->selectProject->setCurrentIndex(ui->selectProject->count()-1);
    ui->treeWidget->addTopLevelItem(item);
}

void MainView::changeTreeView(Task* oldRoot, Task* newRoot)
{
    ui->treeWidget->setItemHidden(oldRoot->m_widgetItem, true);
    ui->treeWidget->setItemHidden(newRoot->m_widgetItem, false);
}

void MainView::changeProject(const QString& selectedProject)
{
    emit projectChanged(selectedProject);
}

void MainView::changeSelectedTask(QTreeWidgetItem* selectedTask)
{
    emit taskChanged(selectedTask);
}