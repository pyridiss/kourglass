#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <KXmlGuiWindow>
#include <QTreeWidget>

#include "storage.h"

class MainWindow : public KXmlGuiWindow
{
    public:
        MainWindow(QWidget *parent=0);

    private:
        QTreeWidget* m_mainTree;
        Storage* m_storage;
        void setupActions();

};

#endif // MAINWINDOW_H
