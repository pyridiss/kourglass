#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocale>

#include "mainwindow.h"

int main (int argc, char *argv[])
{
    KAboutData aboutData( "timetrak",
                          0,
                          ki18n("TimeTrak"),
                          "0.0.1",
                          ki18n("Tracks your time"),
                          KAboutData::License_GPL,
                          ki18n("Copyright © 2014 Quentin Henriet"),
                          ki18n("Some text..."), // no text in the About box for the moment
                          0, // No website for the moment
                          "quentin.henriet@free.fr" );
    KCmdLineArgs::init( argc, argv, &aboutData );

    KApplication app;

    MainWindow* window = new MainWindow();
    window->show();

    int ret = app.exec();

    delete window;
    return ret;
}