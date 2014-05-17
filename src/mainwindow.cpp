#include "mainwindow.h"

#include <KApplication>
#include <KAction>
#include <KLocale>
#include <KActionCollection>
#include <KStandardAction>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) : KXmlGuiWindow(parent)
{
    m_storage = new Storage();
    connect(m_storage, SIGNAL(projectLoaded(QString&, QTreeWidgetItem*)), this, SLOT(addProjectLoaded(QString&, QTreeWidgetItem*)));

    m_mainView = new MainView(this);
    connect(m_mainView, SIGNAL(projectChanged(const QString&)), this, SLOT(changeCurrentProject(const QString&)));
    connect(m_mainView, SIGNAL(taskChanged(QTreeWidgetItem*)), this, SLOT(setCurrentTask(QTreeWidgetItem*)));
    connect(m_mainView, SIGNAL(calendarChanged(const Collection&)), m_storage, SLOT(loadCalendar(const Collection&)));
    setCentralWidget(m_mainView);

    m_taskPropertiesDialog = new TaskPropertiesDialog(this);
    connect(m_taskPropertiesDialog, SIGNAL(allDurationsChanged()), m_storage, SLOT(computeAllDurations()));

    m_addProjectDialog = new NewProjectDialog();
    connect(m_addProjectDialog, SIGNAL(projectAccepted(QString&)), this, SLOT(addProject(QString&)));

    m_addTaskDialog = new NewTaskDialog();
    connect(m_addTaskDialog, SIGNAL(taskAccepted(QString&)), this, SLOT(addTask(QString&)));

    m_addEventDialog = new NewEventDialog();
    connect(m_addEventDialog, SIGNAL(eventAccepted(QString&)), this, SLOT(renameLastEvent(QString&)));

    QTimer* durationUpdater = new QTimer(this);
    connect(durationUpdater, SIGNAL(timeout()), m_storage, SLOT(updateDuration()));
    durationUpdater->start(1000);

    setupActions();
}

MainWindow::~MainWindow()
{
    delete m_storage;
    m_storage = nullptr;
}

void MainWindow::setupActions()
{
    KAction* newProjectAction = new KAction(this);
    newProjectAction->setText(i18n("&New project"));
    newProjectAction->setIcon(KIcon("document-new"));
    newProjectAction->setShortcut(Qt::Key_N);

    KAction* newTaskAction = new KAction(this);
    newTaskAction->setText(i18n("&New sub-task"));
    newTaskAction->setIcon(KIcon("go-next-view-page"));
    newTaskAction->setShortcut(Qt::CTRL + Qt::Key_N);

    KAction* startTaskAction = new KAction(this);
    startTaskAction->setText(i18n("&Start"));
    startTaskAction->setIcon(KIcon("media-playback-start"));
    startTaskAction->setShortcut(Qt::Key_R);

    KAction* stopTaskAction = new KAction(this);
    stopTaskAction->setText(i18n("&Stop"));
    stopTaskAction->setIcon(KIcon("media-playback-stop"));
    stopTaskAction->setShortcut(Qt::Key_S);

    KAction* removeTaskAction = new KAction(this);
    removeTaskAction->setText(i18n("&Remove task"));
    removeTaskAction->setIcon(KIcon("edit-delete"));
    removeTaskAction->setShortcut(Qt::Key_D);

    KAction* deleteTaskEventsAction = new KAction(this);
    deleteTaskEventsAction->setText(i18n("&Delete task and related events"));
    deleteTaskEventsAction->setIcon(KIcon("edit-clear-list"));
    deleteTaskEventsAction->setShortcut(Qt::CTRL + Qt::Key_D);

    KAction* taskPropertiesAction = new KAction(this);
    taskPropertiesAction->setText(i18n("&Task properties"));
    taskPropertiesAction->setIcon(KIcon("configure"));
    taskPropertiesAction->setShortcut(Qt::Key_P);

    actionCollection()->addAction("new-project", newProjectAction);
    actionCollection()->addAction("new-task", newTaskAction);
    actionCollection()->addAction("start-task", startTaskAction);
    actionCollection()->addAction("stop-task", stopTaskAction);
    actionCollection()->addAction("remove-task", removeTaskAction);
    actionCollection()->addAction("delete-task-events", deleteTaskEventsAction);
    actionCollection()->addAction("task-properties", taskPropertiesAction);

    connect(newProjectAction, SIGNAL(triggered(bool)), m_addProjectDialog, SLOT(show()));
    connect(newTaskAction, SIGNAL(triggered(bool)), m_addTaskDialog, SLOT(show()));
    connect(startTaskAction, SIGNAL(triggered(bool)), this, SLOT(startCurrentTask()));
    connect(stopTaskAction, SIGNAL(triggered(bool)), this, SLOT(stopCurrentTask()));
    connect(stopTaskAction, SIGNAL(triggered(bool)), m_addEventDialog, SLOT(show()));
    connect(removeTaskAction, SIGNAL(triggered(bool)), this, SLOT(removeCurrentTask()));
    connect(deleteTaskEventsAction, SIGNAL(triggered(bool)), this, SLOT(removeCurrentTask()));
    connect(taskPropertiesAction, SIGNAL(triggered(bool)), this, SLOT(showTaskProperties()));

    connect(this, SIGNAL(currentTaskIsRealTask(bool)), removeTaskAction, SLOT(setEnabled(bool)));

    KStandardAction::quit(kapp, SLOT(quit()), actionCollection());

    setupGUI(Default, "timetrakui.rc");
}

void MainWindow::changeCurrentProject(const QString& cur)
{
    QString old = m_currentProject;
    for (auto& i : m_storage->m_tasks)
    {
        if (i->m_name == cur)
            m_currentProject = i->m_uid;
    }
    if (m_storage->m_tasks.find(m_currentProject) != m_storage->m_tasks.end())
        if (m_storage->m_tasks.find(old) != m_storage->m_tasks.end())
            m_mainView->changeTreeView(m_storage->m_tasks[old], m_storage->m_tasks[m_currentProject]);
}

void MainWindow::addProject(QString& name)
{
    QTreeWidgetItem* project = m_storage->addProject(name);
    m_mainView->addProject(name, project);
    changeCurrentProject(name);
}

void MainWindow::addProjectLoaded(QString& name, QTreeWidgetItem* project)
{
    m_mainView->addProject(name, project);
    changeCurrentProject(name);
}

void MainWindow::addTask(QString& name)
{
    if (m_storage->m_tasks.find(m_currentTask) != m_storage->m_tasks.end())
    {
        QTreeWidgetItem* task = m_storage->addTask(m_currentProject, m_storage->m_tasks[m_currentTask], name);
        m_storage->m_tasks[m_currentTask]->m_widgetItem->addChild(task);
        m_storage->m_tasks[m_currentTask]->m_widgetItem->setExpanded(true);
    }
}

void MainWindow::setCurrentTask(QTreeWidgetItem* cur)
{
    for (auto& i : m_storage->m_tasks)
    {
        if (i->m_widgetItem == cur)
        {
            m_currentTask = i->m_uid;
            emit currentTaskIsRealTask((i->m_parent != nullptr));
        }
    }
}

void MainWindow::startCurrentTask()
{
    m_storage->startTask(m_currentTask);
}

void MainWindow::stopCurrentTask()
{
    m_storage->stopTask(m_currentTask);
}

void MainWindow::removeCurrentTask()
{
    m_storage->removeTask(m_currentTask);
}

void MainWindow::renameLastEvent(QString& name)
{
    m_storage->m_tasks[m_currentTask]->m_lastEvent->m_name = name;
}

void MainWindow::showTaskProperties()
{
    if (m_storage->m_tasks.find(m_currentTask) != m_storage->m_tasks.end())
    {
        m_taskPropertiesDialog->setTask(m_storage->m_tasks[m_currentTask]);
        m_taskPropertiesDialog->show();
    }
}