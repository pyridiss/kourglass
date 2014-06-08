#include "mainview.h"
#include "ui_mainview.h"

#include <QList>
#include <KJob>

#include <akonadi/changerecorder.h>
#include <akonadi/entitytreemodel.h>
#include <akonadi/entitymimetypefiltermodel.h>

using namespace Akonadi;

MainView::MainView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainView)
{
    ui->setupUi(this);
    connect(ui->selectProject, SIGNAL(activated(const QString&)), this, SLOT(changeProject(const QString&)));
    connect(ui->treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(changeSelectedTask()));

    ChangeRecorder *changeRecorder = new ChangeRecorder(this);
    changeRecorder->setCollectionMonitored(Collection::root());
    changeRecorder->setMimeTypeMonitored("application/x-vnd.akonadi.calendar.todo", true);

    EntityTreeModel *model = new EntityTreeModel(changeRecorder, this);
    EntityMimeTypeFilterModel* filterModel = new EntityMimeTypeFilterModel(this);
    filterModel->setSourceModel(model);
    filterModel->addMimeTypeInclusionFilter(Collection::mimeType());
    filterModel->setHeaderGroup(EntityTreeModel::CollectionTreeHeaders);

    ui->listCalendars->setModel(filterModel);

    connect(ui->listCalendars, SIGNAL(currentChanged(const Akonadi::Collection&)), this, SLOT(changeCalendar(const Akonadi::Collection&)));
    connect(ui->treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(resizeColumn()));
}

MainView::~MainView()
{
    delete ui;
    ui = nullptr;
}

void MainView::clearTreeWidget()
{
    ui->treeWidget->clear();
}

void MainView::addProject(QString& name, QTreeWidgetItem* item)
{
    ui->selectProject->addItem(name);
    ui->selectProject->setCurrentIndex(ui->selectProject->count()-1);
    ui->treeWidget->addTopLevelItem(item);
}

void MainView::resizeColumn()
{
    ui->treeWidget->resizeColumnToContents(0);
}

void MainView::changeProject(const QString& selectedProject)
{
    emit projectChanged(selectedProject);
}

void MainView::changeSelectedTask()
{
    emit taskChanged(ui->treeWidget->selectedItems().first());
}

void MainView::changeCalendar(const Akonadi::Collection& newCollection)
{
    ui->selectProject->clear();
    ui->selectProject->addItem(i18n("Show all projects"));
    emit calendarChanged(newCollection);
}
