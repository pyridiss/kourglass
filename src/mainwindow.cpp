#include "mainwindow.h"

#include <KApplication>
#include <KAction>
#include <KLocale>
#include <KActionCollection>
#include <KStandardAction>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) : KXmlGuiWindow(parent)
{
    m_storage = new Storage(this);
    connect(m_storage, SIGNAL(projectLoaded(QString&, QTreeWidgetItem*)), this, SLOT(addProjectLoaded(QString&, QTreeWidgetItem*)));

    m_mainView = new MainView(this);
    connect(m_mainView, SIGNAL(projectChanged(const QString&)), this, SLOT(changeCurrentProject(const QString&)));
    connect(m_mainView, SIGNAL(taskChanged(QTreeWidgetItem*)), this, SLOT(setCurrentTask(QTreeWidgetItem*)));
    connect(m_mainView, SIGNAL(calendarChanged(const Collection&)), this, SLOT(setCurrentCalendar(const Collection&)));
    setCentralWidget(m_mainView);

    m_taskPropertiesDialog = new TaskPropertiesDialog(this);
    connect(m_taskPropertiesDialog, SIGNAL(allDurationsChanged()), m_storage, SLOT(computeAllDurations()));

    m_addProjectDialog = new NewProjectDialog(this);
    connect(m_addProjectDialog, SIGNAL(projectAccepted(QString&)), this, SLOT(addProject(QString&)));

    m_addTaskDialog = new NewTaskDialog(this);
    connect(m_addTaskDialog, SIGNAL(taskAccepted(QString&)), this, SLOT(addTask(QString&)));

    m_addEventDialog = new NewEventDialog(this);
    connect(m_addEventDialog, SIGNAL(eventAccepted(QString&)), this, SLOT(renameLastEvent(QString&)));

    QTimer* durationUpdater = new QTimer(this);
    connect(durationUpdater, SIGNAL(timeout()), m_storage, SLOT(updateDuration()));
    durationUpdater->start(1000);

    setupActions();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupActions()
{
    KAction* newProjectAction = new KAction(this);
    newProjectAction->setText(i18n("&New project"));
    newProjectAction->setIcon(KIcon("document-new"));
    newProjectAction->setShortcut(Qt::Key_N);
    newProjectAction->setEnabled(false);

    KAction* newTaskAction = new KAction(this);
    newTaskAction->setText(i18n("&New sub-task"));
    newTaskAction->setIcon(KIcon("go-next-view-page"));
    newTaskAction->setShortcut(Qt::CTRL + Qt::Key_N);
    newTaskAction->setEnabled(false);

    KAction* startTaskAction = new KAction(this);
    startTaskAction->setText(i18n("&Start"));
    startTaskAction->setIcon(KIcon("media-playback-start"));
    startTaskAction->setShortcut(Qt::Key_R);
    startTaskAction->setEnabled(false);

    KAction* stopTaskAction = new KAction(this);
    stopTaskAction->setText(i18n("&Stop"));
    stopTaskAction->setIcon(KIcon("media-playback-stop"));
    stopTaskAction->setShortcut(Qt::Key_S);
    stopTaskAction->setEnabled(false);

    KAction* removeTaskAction = new KAction(this);
    removeTaskAction->setText(i18n("&Remove task"));
    removeTaskAction->setIcon(KIcon("edit-delete"));
    removeTaskAction->setShortcut(Qt::Key_D);
    removeTaskAction->setEnabled(false);

    KAction* taskPropertiesAction = new KAction(this);
    taskPropertiesAction->setText(i18n("&Task properties"));
    taskPropertiesAction->setIcon(KIcon("configure"));
    taskPropertiesAction->setShortcut(Qt::Key_P);
    taskPropertiesAction->setEnabled(false);

    actionCollection()->addAction("new-project", newProjectAction);
    actionCollection()->addAction("new-task", newTaskAction);
    actionCollection()->addAction("start-task", startTaskAction);
    actionCollection()->addAction("stop-task", stopTaskAction);
    actionCollection()->addAction("remove-task", removeTaskAction);
    actionCollection()->addAction("task-properties", taskPropertiesAction);

    connect(newProjectAction, SIGNAL(triggered(bool)), m_addProjectDialog, SLOT(show()));
    connect(newTaskAction, SIGNAL(triggered(bool)), m_addTaskDialog, SLOT(show()));
    connect(startTaskAction, SIGNAL(triggered(bool)), this, SLOT(startCurrentTask()));
    connect(stopTaskAction, SIGNAL(triggered(bool)), this, SLOT(stopCurrentTask()));
    connect(stopTaskAction, SIGNAL(triggered(bool)), m_addEventDialog, SLOT(show()));
    connect(removeTaskAction, SIGNAL(triggered(bool)), this, SLOT(removeCurrentTask()));
    connect(taskPropertiesAction, SIGNAL(triggered(bool)), this, SLOT(showTaskProperties()));

    connect(this, SIGNAL(currentTaskIsRealTask(bool)), removeTaskAction, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(aCalendarIsSelected(bool)), newProjectAction, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(aTaskIsSelected(bool)), newTaskAction, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(aTaskIsSelected(bool)), startTaskAction, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(aTaskIsSelected(bool)), stopTaskAction, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(aTaskIsSelected(bool)), taskPropertiesAction, SLOT(setEnabled(bool)));

    KStandardAction::quit(kapp, SLOT(quit()), actionCollection());

    setupGUI(Default, "kourglassui.rc");
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
    emit aTaskIsSelected(false);
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
            emit aTaskIsSelected(true);
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
    m_storage->m_tasks[m_currentTask]->m_lastEvent->saveToAkonadi();
}

void MainWindow::showTaskProperties()
{
    if (m_storage->m_tasks.find(m_currentTask) != m_storage->m_tasks.end())
    {
        m_taskPropertiesDialog->setTask(m_storage->m_tasks[m_currentTask]);
        m_taskPropertiesDialog->show();
    }
}

void MainWindow::setCurrentCalendar(const Collection& calendar)
{
    m_storage->loadCalendar(calendar);
    m_mainView->clearTreeWidget();
    emit aCalendarIsSelected(true);
    emit aTaskIsSelected(false);
    emit currentTaskIsRealTask(false);
}