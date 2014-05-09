#include "mainwindow.h"

#include <KApplication>
#include <KAction>
#include <KLocale>
#include <KActionCollection>
#include <KStandardAction>

MainWindow::MainWindow(QWidget *parent) : KXmlGuiWindow(parent)
{
    m_mainView = new MainView();
    m_storage = new Storage();
    setCentralWidget(m_mainView);

    m_addProjectDialog = new NewProjectDialog();
    connect(m_addProjectDialog, SIGNAL(projectAccepted(QString&)), this, SLOT(addProject(QString&)));

    m_addTaskDialog = new NewTaskDialog();
    connect(m_addTaskDialog, SIGNAL(taskAccepted(QString&)), this, SLOT(addTask(QString&)));

    setupActions();
}

void MainWindow::setupActions()
{
    KAction* newProjectAction = new KAction(this);
    newProjectAction->setText(i18n("&New project"));
    newProjectAction->setIcon(KIcon("document-new"));
    newProjectAction->setShortcut(Qt::Key_N);

    KAction* newTaskAction = new KAction(this);
    newTaskAction->setText(i18n("&New task"));
    newTaskAction->setIcon(KIcon("go-next-view-page"));
    newTaskAction->setShortcut(Qt::CTRL + Qt::Key_N);

    KAction* startTaskAction = new KAction(this);
    startTaskAction->setText(i18n("&Run"));
    startTaskAction->setIcon(KIcon("media-playback-start"));
    startTaskAction->setShortcut(Qt::Key_R);

    KAction* stopTaskAction = new KAction(this);
    stopTaskAction->setText(i18n("&Stop"));
    stopTaskAction->setIcon(KIcon("media-playback-stop"));
    stopTaskAction->setShortcut(Qt::Key_S);

    actionCollection()->addAction("new-project", newProjectAction);
    actionCollection()->addAction("new-task", newTaskAction);
    actionCollection()->addAction("start-task", startTaskAction);
    actionCollection()->addAction("stop-task", stopTaskAction);

    connect(newProjectAction, SIGNAL(triggered(bool)), m_addProjectDialog, SLOT(show()));
    connect(newTaskAction, SIGNAL(triggered(bool)), m_addTaskDialog, SLOT(show()));
    connect(startTaskAction, SIGNAL(triggered(bool)), this, SLOT(startCurrentTask()));
    connect(stopTaskAction, SIGNAL(triggered(bool)), this, SLOT(stopCurrentTask()));

    connect(m_mainView, SIGNAL(projectChanged(const QString&)), this, SLOT(changeCurrentProject(const QString&)));
    connect(m_mainView, SIGNAL(taskChanged(QTreeWidgetItem*)), this, SLOT(setCurrentTask(QTreeWidgetItem*)));

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

void MainWindow::addTask(QString& name)
{
    if (m_storage->m_tasks.find(m_currentTask) != m_storage->m_tasks.end())
    {
        QTreeWidgetItem* task = m_storage->addTask(m_currentProject, m_currentTask, name);
        m_storage->m_tasks[m_currentTask]->m_widgetItem->addChild(task);
        m_storage->m_tasks[m_currentTask]->m_widgetItem->setExpanded(true);
    }
}

void MainWindow::setCurrentTask(QTreeWidgetItem* cur)
{
    for (auto& i : m_storage->m_tasks)
    {
        if (i->m_widgetItem == cur)
            m_currentTask = i->m_uid;
    }
}

void MainWindow::startCurrentTask()
{
    if (m_storage->m_tasks.find(m_currentTask) != m_storage->m_tasks.end())
    {
        m_storage->m_tasks[m_currentTask]->m_widgetItem->setIcon(0, KIcon("arrow-right"));
    }
}

void MainWindow::stopCurrentTask()
{
    if (m_storage->m_tasks.find(m_currentTask) != m_storage->m_tasks.end())
    {
        m_storage->m_tasks[m_currentTask]->m_widgetItem->setIcon(0, KIcon("media-playback-pause"));
    }
}
