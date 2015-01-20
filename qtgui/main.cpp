#include "mainwindow.h"

#include <QCoreApplication>
#include <QApplication>
#include <ipcqueue.h>

#include <QMapControl/ImageManager.h>

#include <gdal/ogrsf_frmts.h>

#include <modelobjects/vesseldata.h>

int main (int argc, char *argv[])
{
    OGRRegisterAll();

    QCoreApplication::setOrganizationName("Studiofuga");
    QCoreApplication::setOrganizationDomain("studiofuga.com");
    QCoreApplication::setApplicationName("displace");

    // Cleanup the shm objets
    IpcQueue::forceCleanup();

    // Register here any qmetatype
    qRegisterMetaType<VesselStats>("VesselStats");

    QApplication app(argc, argv);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    QTranslator myappTranslator;
    myappTranslator.load("displace_" + QLocale::system().name());
    app.installTranslator(&myappTranslator);

    MainWindow mw;

    mw.show();

#ifndef WIN32
    QString path = QDir::homePath() + "/.displace/cache";
#else
    QString path = QDir::homePath() + "/.displace/cache";
#endif

    QDir cachepath(path);
    qmapcontrol::ImageManager::get().enablePersistentCache(std::chrono::minutes(24*60), cachepath);

    return app.exec();
}
