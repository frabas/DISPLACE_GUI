#include "mainwindow.h"

#include <QCoreApplication>
#include <QApplication>

#include <QMapControl/ImageManager.h>

#include <gdal/ogrsf_frmts.h>

int main (int argc, char *argv[])
{
    OGRRegisterAll();

    QCoreApplication::setOrganizationName("Studiofuga");
    QCoreApplication::setOrganizationDomain("studiofuga.com");
    QCoreApplication::setApplicationName("displace");

    QApplication app(argc, argv);
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
