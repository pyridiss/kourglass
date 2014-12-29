#include "mainwindow.h"

#include <KApplication>
#include <KStatusNotifierItem>
#include <KStatusBar>
#include <KAction>
#include <KLocale>
#include <KActionCollection>
#include <KStandardAction>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) : KXmlGuiWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, false);

    systemTray = new KStatusNotifierItem("kourglass", this);
    systemTray->setStatus(KStatusNotifierItem::Active);
    systemTray->setIconByName("clock");
    systemTray->setTitle("Kourglass");
    systemTray->setToolTip("clock", "Kourglass", i18n("Track your time!"));

    statusBar()->insertPermanentItem(i18n("Total calendar time:"), 1);
    statusBar()->insertPermanentItem("00:00:00", 2);
    statusBar()->insertPermanentItem("Today:", 3);
    statusBar()->insertPermanentItem("00:00:00", 4);
    statusBar()->insertPermanentItem("This week:", 5);
    statusBar()->insertPermanentItem("00:00:00", 6);

    m_storage = new Storage(this);
    connect(m_storage, SIGNAL(projectLoaded(QString&, QTreeWidgetItem*)), this, SLOT(addProjectLoaded(QString&, QTreeWidgetItem*)));

    m_mainView = new MainView(this);
    connect(m_mainView, SIGNAL(projectChanged(const QString&)), this, SLOT(changeCurrentProject(const QString&)));
    connect(m_mainView, SIGNAL(taskChanged(QTreeWidgetItem*)), this, SLOT(setCurrentTask(QTreeWidgetItem*)));
    connect(m_mainView, SIGNAL(calendarChanged(const Collection&)), this, SLOT(setCurrentCalendar(const Collection&)));
    connect(m_mainView, SIGNAL(dateFromChanged(const QDate&)), m_storage, SLOT(setDateFrom(const QDate&)));
    connect(m_mainView, SIGNAL(dateToChanged(const QDate&)), m_storage, SLOT(setDateTo(const QDate&)));
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
    connect(durationUpdater, SIGNAL(timeout()), this, SLOT(updateStatusBar()));
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
    connect(this, SIGNAL(aNonRunningTaskIsSelected(bool)), startTaskAction, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(aRunningTaskIsSelected(bool)), stopTaskAction, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(aTaskIsSelected(bool)), taskPropertiesAction, SLOT(setEnabled(bool)));

    KStandardAction::quit(kapp, SLOT(quit()), actionCollection());

    setupGUI(Default, "kourglassui.rc");
}

void MainWindow::changeCurrentProject(const QString& cur)
{
    for (auto& i : m_storage->m_tasks)
    {
        if (i->m_parent == nullptr) //Only projects are concerned
        {
            if (i->m_name == cur || cur == i18n("Show all projects")) i->m_widgetItem->setHidden(false);
            else i->m_widgetItem->setHidden(true);
        }
    }

    emit aTaskIsSelected(false);
    emit aRunningTaskIsSelected(false);
    emit aNonRunningTaskIsSelected(false);
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
        QTreeWidgetItem* task = m_storage->addTask(m_storage->m_tasks[m_currentTask], name);
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
            if (i->m_running)
            {
                emit aRunningTaskIsSelected(true);
                emit aNonRunningTaskIsSelected(false);
            }
            else
            {
                emit aRunningTaskIsSelected(false);
                emit aNonRunningTaskIsSelected(true);
            }
            m_currentTask = i->m_uid;
            emit currentTaskIsRealTask((i->m_parent != nullptr));
        }
    }
}

void MainWindow::startCurrentTask()
{
    m_storage->startTask(m_currentTask);
    emit aRunningTaskIsSelected(true);
    emit aNonRunningTaskIsSelected(false);
}

void MainWindow::stopCurrentTask()
{
    m_storage->stopTask(m_currentTask);
    emit aRunningTaskIsSelected(false);
    emit aNonRunningTaskIsSelected(true);
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
    emit aRunningTaskIsSelected(false);
    emit aNonRunningTaskIsSelected(false);
    emit currentTaskIsRealTask(false);
}

void MainWindow::updateStatusBar()
{
    Duration total, today, thisWeek;

    QDateTime now = QDateTime::currentDateTime();
    QDate todayDate = now.date();
    int todayWeekNumber = now.date().weekNumber();

    for (auto& i : m_storage->m_tasks)
    {
        //Projects (without parents) have already total duration calculated
        if (i->m_parent == nullptr)
            total += i->m_currentDuration;

        //To calculate 'today' and 'thisWeek', we need to check all events
        for (auto& j : i->m_events)
        {
            if (j->m_endTime.date() == todayDate)
            {
                //There is a working time today
                today.add(j->m_startTime.secsTo(j->m_endTime) * 1000);
            }
            if (j->m_endTime.date().weekNumber() == todayWeekNumber)
            {
                //There is a working time this week
                thisWeek.add(j->m_startTime.secsTo(j->m_endTime) * 1000);
            }
        }
    }

    statusBar()->changeItem(total.toString(), 2);
    statusBar()->changeItem(today.toString(), 4);
    statusBar()->changeItem(thisWeek.toString(), 6);
}
