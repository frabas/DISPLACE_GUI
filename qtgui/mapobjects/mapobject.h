#ifndef MAPOBJECT_H
#define MAPOBJECT_H

#include <memory>

namespace qmapcontrol {
class Geometry;
}

class MapObject
{
public:
    MapObject();
    virtual ~MapObject();


    virtual std::shared_ptr<qmapcontrol::Geometry> getGeometryEntity() const = 0;
    virtual bool showProperties();
    virtual void updateProperties();

    virtual void toggleSelection() final;
    virtual void setSelection(bool selection);

    /** Called when the selection has changed. Override this if you want specific action to be performed */
    virtual void onSelectionChanged();
};

#endif // MAPOBJECT_H
