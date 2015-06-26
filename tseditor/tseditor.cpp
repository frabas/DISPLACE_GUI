#include <QCoreApplication>
#include <QApplication>
#include <QLibraryInfo>
#include <QTranslator>
#include <tseditorwindow.h>

int main (int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Studiofuga");
    QCoreApplication::setOrganizationDomain("studiofuga.com");
    QCoreApplication::setApplicationName("tseditor");

    QApplication app(argc, argv);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    myappTranslator.load("dteditor_" + QLocale::system().name());
    app.installTranslator(&myappTranslator);

    TsEditorWindow w;
    w.show();

    return app.exec();
}

