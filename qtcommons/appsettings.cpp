#include "appsettings.h"

#include <QPointF>
#include <QCoreApplication>

using namespace displace;

const QString AppSettings::MapCenterPoint = "MapCentralPoint";

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

void AppSettings::initialize()
{
    QCoreApplication::setOrganizationName("DTUAqua");
    QCoreApplication::setOrganizationDomain("dtu.aqua.dk");
    QCoreApplication::setApplicationName("vesseleditor");
}
