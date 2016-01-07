#include "mainwindow.h"

#include "ui_mainview.h"

#include <KApplication>
#include <KStatusNotifierItem>
#include <KStatusBar>
#include <QAction>
#include <KLocale>
#include <KActionCollection>
#include <KStandardAction>
#include <QLabel>
#include <QIcon>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) : KXmlGuiWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, false);

    systemTray = new KStatusNotifierItem("kourglass", this);
    systemTray->setStatus(KStatusNotifierItem::Active);
    systemTray->setIconByName("clock");
    systemTray->setTitle("Kourglass");
    systemTray->setToolTip("clock", "Kourglass", i18n("Track your time!"));

    QLabel* textStatusBar1 = new QLabel(i18n("Total calendar time:"), statusBar());
    m_statusBarTotalTime = new QLabel(i18n("00:00:00"), statusBar());
    QLabel* textStatusBar3 = new QLabel(i18n("Today:"), statusBar());
    m_statusBarToday = new QLabel(i18n("00:00:00"), statusBar());
    QLabel* textStatusBar5 = new QLabel(i18n("This week:"), statusBar());
    m_statusBarWeek = new QLabel(i18n("00:00:00"), statusBar());

    statusBar()->insertPermanentWidget(1, textStatusBar1);
    statusBar()->insertPermanentWidget(2, m_statusBarTotalTime);
    statusBar()->insertPermanentWidget(3, textStatusBar3);
    statusBar()->insertPermanentWidget(4, m_statusBarToday);
    statusBar()->insertPermanentWidget(5, textStatusBar5);
    statusBar()->insertPermanentWidget(5, m_statusBarWeek);


    m_storage = new Storage(this);
    connect(m_storage, SIGNAL(projectLoaded(QString&, QTreeWidgetItem*)), this, SLOT(addProjectLoaded(QString&, QTreeWidgetItem*)));

    m_mainView = new MainView(this);
    connect(m_mainView, SIGNAL(projectChanged(const QString&)), this, SLOT(changeCurrentProject(const QString&)));
    connect(m_mainView, SIGNAL(taskChanged(QTreeWidgetItem*)), this, SLOT(setCurrentTask(QTreeWidgetItem*)));
    connect(m_mainView, SIGNAL(calendarChanged(const Collection&)), this, SLOT(setCurrentCalendar(const Collection&)));
    connect(m_mainView, SIGNAL(dateFromChanged(const QDate&)), m_storage, SLOT(setDateFrom(const QDate&)));
    connect(m_mainView, SIGNAL(dateToChanged(const QDate&)), m_storage, SLOT(setDateTo(const QDate&)));
    connect(m_mainView, SIGNAL(hideUnusedChanged()), this, SLOT(hideUnusedChanged()));
    setCentralWidget(m_mainView);

    connect(m_mainView, SIGNAL(allDurationsChanged()), m_storage, SLOT(computeAllDurations()));

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
    QAction* newProjectAction = new QAction(this);
    newProjectAction->setText(i18n("&New project"));
    newProjectAction->setIcon(QIcon::fromTheme("document-new"));
    newProjectAction->setShortcut(Qt::Key_N);
    newProjectAction->setEnabled(false);

    QAction* newTaskAction = new QAction(this);
    newTaskAction->setText(i18n("&New sub-task"));
    newTaskAction->setIcon(QIcon::fromTheme("go-next-view-page"));
    newTaskAction->setShortcut(Qt::CTRL + Qt::Key_N);
    newTaskAction->setEnabled(false);

    QAction* startTaskAction = new QAction(this);
    startTaskAction->setText(i18n("&Start"));
    startTaskAction->setIcon(QIcon::fromTheme("media-playback-start"));
    startTaskAction->setShortcut(Qt::Key_R);
    startTaskAction->setEnabled(false);

    QAction* stopTaskAction = new QAction(this);
    stopTaskAction->setText(i18n("&Stop"));
    stopTaskAction->setIcon(QIcon::fromTheme("media-playback-stop"));
    stopTaskAction->setShortcut(Qt::Key_S);
    stopTaskAction->setEnabled(false);

    QAction* removeTaskAction = new QAction(this);
    removeTaskAction->setText(i18n("&Remove task"));
    removeTaskAction->setIcon(QIcon::fromTheme("edit-delete"));
    removeTaskAction->setShortcut(Qt::Key_D);
    removeTaskAction->setEnabled(false);

    actionCollection()->addAction("new-project", newProjectAction);
    actionCollection()->addAction("new-task", newTaskAction);
    actionCollection()->addAction("start-task", startTaskAction);
    actionCollection()->addAction("stop-task", stopTaskAction);
    actionCollection()->addAction("remove-task", removeTaskAction);

    connect(newProjectAction, SIGNAL(triggered(bool)), m_addProjectDialog, SLOT(show()));
    connect(newTaskAction, SIGNAL(triggered(bool)), m_addTaskDialog, SLOT(show()));
    connect(startTaskAction, SIGNAL(triggered(bool)), this, SLOT(startCurrentTask()));
    connect(stopTaskAction, SIGNAL(triggered(bool)), this, SLOT(stopCurrentTask()));
    connect(stopTaskAction, SIGNAL(triggered(bool)), m_addEventDialog, SLOT(show()));
    connect(removeTaskAction, SIGNAL(triggered(bool)), this, SLOT(removeCurrentTask()));

    connect(this, SIGNAL(currentTaskIsRealTask(bool)), removeTaskAction, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(aCalendarIsSelected(bool)), newProjectAction, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(aTaskIsSelected(bool)), newTaskAction, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(aNonRunningTaskIsSelected(bool)), startTaskAction, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(aRunningTaskIsSelected(bool)), stopTaskAction, SLOT(setEnabled(bool)));

    KStandardAction::quit(qApp, SLOT(quit()), actionCollection());

    setupGUI(Default, "kourglassui.rc");
}

void MainWindow::changeCurrentProject(const QString& cur)
{
    m_currentProject = cur;
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
    showTaskProperties();
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
        m_mainView->setTask(m_storage->m_tasks[m_currentTask]);
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

    m_statusBarTotalTime->setText(total.toString());
    m_statusBarToday->setText(today.toString());
    m_statusBarWeek->setText(thisWeek.toString());
}

void MainWindow::hideUnusedChanged()
{
    bool hide = m_mainView->ui->checkBoxHideUnused->isChecked();
    int intDuration = m_mainView->ui->spinBoxHideUnused->value();
    QString qStringDuration = m_mainView->ui->comboBoxHideUnused->currentText();

    m_storage->hideUnusedTasks(m_currentProject, hide, intDuration, qStringDuration);
}
