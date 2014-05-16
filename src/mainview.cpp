#include "mainview.h"
#include "ui_mainview.h"

#include <QList>
#include <KJob>

#include <akonadi/collectionfetchjob.h>

using namespace Akonadi;

MainView::MainView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainView)
{
    ui->setupUi(this);
    connect(ui->selectProject, SIGNAL(activated(const QString&)), this, SLOT(changeProject(const QString&)));
    connect(ui->treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(changeSelectedTask()));
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

void MainView::changeSelectedTask()
{
    emit taskChanged(ui->treeWidget->selectedItems().first());
}

void MainView::updateCalendarsList(KJob *job)
{
    if (job->error()) return;

    CollectionFetchJob *fetchJob = qobject_cast<CollectionFetchJob*>(job);

    const Collection::List collections = fetchJob->collections();
    for (auto& collection : collections)
    {
        if (collection.contentMimeTypes().contains("application/x-vnd.akonadi.calendar.todo"))
        {
            ui->listCalendars->addItem(collection.name());
        }
    }
}