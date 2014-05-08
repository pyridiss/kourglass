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

    connect(newProjectAction, SIGNAL(triggered(bool)), m_storage, SLOT(addProject()));
    connect(newTaskAction, SIGNAL(triggered(bool)), m_storage, SLOT(addTask()));

    connect(m_storage, SIGNAL(tasksChanged()), this, SLOT(updateView()));

    KStandardAction::quit(kapp, SLOT(quit()), actionCollection());

    setupGUI(Default, "timetrakui.rc");
}

void MainWindow::updateView()
{
}