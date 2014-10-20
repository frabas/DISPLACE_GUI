#ifndef HARBOURMAPOBJECT_H
#define HARBOURMAPOBJECT_H

#include <QObject>

#include <mapobjects/mapobject.h>
#include <QMapControl/GeometryPointImageScaled.h>
#include <modelobjects/harbourdata.h>
#include <mapobjects/nodedetailswidget.h>

#include <QPixmap>

class MapObjectsController;
class DisplaceModel;

class HarbourMapObject : public QObject, public MapObject
{
    Q_OBJECT
public:
    HarbourMapObject(MapObjectsController *controller, DisplaceModel *model, HarbourData *harbour);

    std::shared_ptr<qmapcontrol::Geometry> getGeometryEntity() const {
        return mGeometry;
    }

    virtual bool clicked();
    virtual void update();

private slots:
    void widgetClosed();

private:
    MapObjectsController *mController;
    DisplaceModel *mModel;

    HarbourData *mHarbour;
    std::shared_ptr<qmapcontrol::GeometryPointImageScaled> mGeometry;

    static QPixmap *symbol;
    NodeDetailsWidget *mWidget;

};

#endif // HARBOURMAPOBJECT_H
