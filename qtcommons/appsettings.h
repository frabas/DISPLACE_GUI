#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QString>
#include <QSettings>

namespace displace {

class AppSettings
{
    QSettings settings;
public:
    AppSettings();

    QPointF getMapCenterPoint() const;
    void setMapCenterPoint(const QPointF &);

    int getMapZoom() const;
    void setMapZoom (int zoomlevel);

    static void initialize();

    static const QString MapCenterPoint;
    static const QString MapZoom;

    static QString getSimulatorAppName();
};

}


#endif // APPSETTINGS_H
