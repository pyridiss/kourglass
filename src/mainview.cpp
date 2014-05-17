#include "mainview.h"
#include "ui_mainview.h"

#include <QList>
#include <KJob>

#include <akonadi/collectionmodel.h>
#include <akonadi/collectionfilterproxymodel.h>

using namespace Akonadi;

MainView::MainView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainView)
{
    ui->setupUi(this);
    connect(ui->selectProject, SIGNAL(activated(const QString&)), this, SLOT(changeProject(const QString&)));
    connect(ui->treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(changeSelectedTask()));

    CollectionModel *model = new CollectionModel(this);
    CollectionFilterProxyModel *proxy = new CollectionFilterProxyModel();
    proxy->addMimeTypeFilter("application/x-vnd.akonadi.calendar.todo");
    proxy->setSourceModel(model);
    ui->listCalendars->setModel(proxy);
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
