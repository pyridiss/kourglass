#include <QApplication>
#include <QCommandLineParser>
#include <QIcon>
#include <KAboutData>
#include <KLocalizedString>
#include <QtCore/QLoggingCategory>

#include "mainwindow.h"

Q_DECLARE_LOGGING_CATEGORY(KF5)
Q_LOGGING_CATEGORY(KF5, "kf5")

int main (int argc, char *argv[])
{
    QApplication app(argc, argv);

    KLocalizedString::setApplicationDomain("kourglass");

    KAboutData aboutData( QStringLiteral("kourglass"),
                          i18n("kourglass"),
                          QStringLiteral("0.1.0"),
                          i18n("Tracks your time"),
                          KAboutLicense::GPL,
                          i18n("Copyright © 2014, 2015 Quentin Henriet <quentin.henriet@free.fr"));

    aboutData.addAuthor(i18n("Quentin Henriet"),i18n("Author"), QStringLiteral("quentin.henriet@free.fr"));

    app.setWindowIcon(QIcon::fromTheme("kourglass"));

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);
    KAboutData::setApplicationData(aboutData);

    MainWindow* window = new MainWindow();
    window->show();

    int ret = app.exec();

    delete window;
    return ret;
}
