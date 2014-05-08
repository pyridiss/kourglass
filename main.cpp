#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KMessageBox>
#include <KLocale>

int main (int argc, char *argv[])
{
    KAboutData aboutData( "timetrak",
                          0,
                          ki18n("TimeTrak"),
                          "0.0.1",
                          ki18n("Tracks your time"),
                          KAboutData::License_GPL,
                          ki18n("Quentin Henriet © 2014"),
                          ki18n("Some text..."), // no text in the About box for the moment
                          0, // No website for the moment
                          "quentin.henriet@free.fr" );
 
    KCmdLineArgs::init( argc, argv, &aboutData );
    KApplication app;
    KGuiItem yesButton( i18n( "Hello" ), QString(),
                        i18n( "This is a tooltip" ),
                        i18n( "This is a WhatsThis help text." ) );
    return 
        KMessageBox ::questionYesNo 
        (0, i18n( "Hello World" ), i18n( "Hello" ), yesButton ) 
        == KMessageBox ::Yes? EXIT_SUCCESS: EXIT_FAILURE;
}