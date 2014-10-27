#ifndef VESSELMAPOBJECT_H
#define VESSELMAPOBJECT_H

#include <QObject>

#include <mapobjects/mapobject.h>
#include <modelobjects/vesseldata.h>
#include <mapobjects/nodedetailswidget.h>

#include <QMapControl/GeometryPointShapeScaled.h>

#include <QBrush>

namespace qmapcontrol {
    class RectWorldCoord;
}

class MapObjectsController;

class VesselMapObject : public QObject, public MapObject
{
    Q_OBJECT

    class VesselGraphics : public qmapcontrol::GeometryPointShapeScaled {
        static QBrush *color;
        static QBrush *statHarbour, *statSteaming, *statFishing;

        VesselData *mVessel;
    public:
        VesselGraphics (VesselData *vessel);

        void updated();
    protected:
        virtual void drawShape(QPainter &painter, const qmapcontrol::RectWorldPx &rect);
    };

public:
    VesselMapObject(MapObjectsController *controller, VesselData *vessel);

    std::shared_ptr<qmapcontrol::Geometry> getGeometryEntity() const {
        return mGeometry;
    }

    virtual bool showProperties();
    virtual void updateProperties();

    void vesselUpdated();

private slots:
    void widgetClosed();

private:
    MapObjectsController *mController;

    VesselData *mVessel;
    std::shared_ptr<VesselGraphics> mGeometry;

    NodeDetailsWidget *mWidget;
};

#endif // VESSELMAPOBJECT_H
