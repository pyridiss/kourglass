#include "mainwindow.h"

#include <KApplication>
#include <KAction>
#include <KLocale>
#include <KActionCollection>
#include <KStandardAction>

MainWindow::MainWindow(QWidget *parent) : KXmlGuiWindow(parent)
{
    m_mainTree = new QTreeWidget();
    m_storage = new Storage();
    setCentralWidget(m_mainTree);
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

    actionCollection()->addAction("new-project", newProjectAction);
    actionCollection()->addAction("new-task", newTaskAction);

    connect(newProjectAction, SIGNAL(triggered(bool)), this, SLOT(addProject()));
    connect(newTaskAction, SIGNAL(triggered(bool)), this, SLOT(addTask()));

    connect(m_mainTree, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(setCurrentTask(QTreeWidgetItem*)));

    KStandardAction::quit(kapp, SLOT(quit()), actionCollection());

    setupGUI(Default, "timetrakui.rc");
}

void MainWindow::addProject()
{
    QTreeWidgetItem* project = m_storage->addProject();
    m_mainTree->addTopLevelItem(project);
}

void MainWindow::addTask()
{
    if (m_storage->m_tasks.find(m_currentTask) != m_storage->m_tasks.end())
    {
        QTreeWidgetItem* task = m_storage->addTask(m_currentTask);
        m_storage->m_tasks[m_currentTask]->m_widgetItem->addChild(task);
        m_mainTree->expandItem(m_storage->m_tasks[m_currentTask]->m_widgetItem);
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