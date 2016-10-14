#include "appsettings.h"

#include <QPointF>
#include <QCoreApplication>

using namespace displace;

const QString AppSettings::MapCenterPoint = "MapCentralPoint";
const QString AppSettings::MapZoom = "MapZoom";

AppSettings::AppSettings()
{

}

QPointF AppSettings::getMapCenterPoint() const
{
    return settings.value(MapCenterPoint, QVariant(QPointF(13.7507,43.7282))).toPointF();
}

void AppSettings::setMapCenterPoint(const QPointF &point)
{
    settings.setValue(MapCenterPoint, point);
}

int AppSettings::getMapZoom() const
{
    return settings.value(MapZoom, 7).toInt();
}

void AppSettings::setMapZoom(int zoomlevel)
{
    settings.setValue(MapZoom, zoomlevel);
}

void AppSettings::initialize()
{
    QCoreApplication::setOrganizationName("DTUAqua");
    QCoreApplication::setOrganizationDomain("dtu.aqua.dk");
    QCoreApplication::setApplicationName("vesseleditor");
}

QString AppSettings::getSimulatorAppName()
{
    QString appname;
#if defined(Q_OS_DARWIN) || defined (Q_OS_LINUX)
    appname = "displace";
#elif defined (Q_OS_WIN)
    appname = "displace.exe";
#else
    throw std::runtime_error("Platform simulator not supported.");
#endif

    return appname;
}
