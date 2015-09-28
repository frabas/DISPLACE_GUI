#include <QCoreApplication>
#include <QApplication>
#include <QLibraryInfo>
#include <QTranslator>
#include <vesseleditorwindow.h>

int main (int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("DTUAqua");
    QCoreApplication::setOrganizationDomain("dtu.aqua.dk");
    QCoreApplication::setApplicationName("vesseleditor");

    QApplication app(argc, argv);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    myappTranslator.load("vesseleditor_" + QLocale::system().name());
    app.installTranslator(&myappTranslator);

    VesselEditorWindow w;
    w.show();

    return app.exec();
}

