#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <KXmlGuiWindow>
#include <QTreeWidget>

#include "storage.h"

class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent=0);

    private:
        QTreeWidget* m_mainTree;
        Storage* m_storage;
        void setupActions();

    public slots:
        void updateView();
};

#endif // MAINWINDOW_H
