#include "geometrydb.h"

#include <gdal/ogrsf_frmts.h>

#include <vector>
#include <mutex>
#include <set>

#include <QRectF>
#include <QDebug>

using namespace qmapcontrol;

namespace qmapcontrol {
namespace impl {
class GeometryDBImpl {
    static std::mutex globMutex;
    static bool initialized;

    mutable std::mutex mutex;
    OGRSFDriver *drv;
    OGRDataSource *ds;
    OGRLayer *layer;
    OGRSpatialReference mSpatialReference;
    int nIdField;

    std::vector<GeometryDB::GeometryPtr> mObjects;
    std::vector<int> mEmptyObjects;
public:
    GeometryDBImpl() {
        std::unique_lock<std::mutex> lock(globMutex);
        if (!initialized) {
            OGRRegisterAll();
            initialized = true;
        }

        OGRSFDriverRegistrar *registrar =  OGRSFDriverRegistrar::GetRegistrar();
        mSpatialReference.SetWellKnownGeogCS( "WGS84" );

        drv = registrar->GetDriverByName("memory");
        ds = drv->CreateDataSource("memory", nullptr );
        layer = ds->CreateLayer("layer", &mSpatialReference, wkbPoint, nullptr);

        const char *FieldName = "objectId";
        OGRFieldDefn fld(FieldName, OFTInteger);
        layer->CreateField(&fld);
        nIdField = layer->FindFieldIndex(FieldName, true);
    }
    void query(std::set<GeometryDB::GeometryPtr>& return_points, const RectWorldCoord& range) const {
        std::unique_lock<std::mutex> lock(mutex);

        QRectF r= range.rawRect();
        layer->ResetReading();
        layer->SetSpatialFilterRect(r.left(), r.top(), r.right(), r.bottom());

        OGRFeature *feature;
        while ((feature = layer->GetNextFeature()) != nullptr) {
            int id = feature->GetFieldAsInteger(nIdField);
            return_points.insert(mObjects[id]);
        }
        layer->ResetReading();
    }
    bool insert(const PointWorldCoord& point, const GeometryDB::GeometryPtr& object) {
        std::unique_lock<std::mutex> lock(mutex);

        int pos;
        if (mEmptyObjects.empty()) {
            pos = mObjects.size();
            mObjects.push_back(object);
        } else {
            pos = mEmptyObjects.back();
            mEmptyObjects.pop_back();
        }

        OGRPoint pt(point.longitude(),point.latitude());
        auto f = OGRFeature::CreateFeature(layer->GetLayerDefn());
        f->SetGeometry(&pt);
        f->SetField(nIdField, pos);

        layer->CreateFeature(f);
        OGRFeature::DestroyFeature(f);

        return true;
    }

    void move(const PointWorldCoord& point, const GeometryDB::GeometryPtr& object, const PointWorldCoord& newpoint) {
        std::unique_lock<std::mutex> lock(mutex);

        QRect r(point.longitude() - 0.1, point.latitude() - 0.1, 0.2, 0.2);
        layer->ResetReading();
        layer->SetSpatialFilterRect(r.left(), r.top(), r.right(), r.bottom());

        OGRFeature *feature;
        while ((feature = layer->GetNextFeature()) != nullptr) {
            int id = feature->GetFieldAsInteger(nIdField);
            if (mObjects.at(id) == object) {
                auto pt = feature->GetGeometryRef();
                if( pt != NULL && wkbFlatten(pt->getGeometryType()) == wkbPoint )
                {
                    OGRPoint *point = (OGRPoint *) pt;
                    point->setX(newpoint.longitude());
                    point->setY(newpoint.latitude());
                }
            }
        }

    }

    void erase(const PointWorldCoord& point, const GeometryDB::GeometryPtr& object) {
        std::unique_lock<std::mutex> lock(mutex);

        QRectF r(point.longitude() - 0.01, point.latitude() - 0.01, 0.02, 0.02);
        layer->ResetReading();
        layer->SetSpatialFilterRect(r.left(), r.top(), r.right(), r.bottom());

        OGRFeature *feature;
        while ((feature = layer->GetNextFeature()) != nullptr) {
            int id = feature->GetFieldAsInteger(nIdField);
            if (mObjects.at(id) == object) {
                mEmptyObjects.push_back(id);
                mObjects.at(id) = nullptr;
                auto r = layer->DeleteFeature(feature->GetFID());
                Q_ASSERT(r == OGRERR_NONE);
            }
        }

    }

    void clear() {
        std::unique_lock<std::mutex> lock(mutex);
    }
};

std::mutex GeometryDBImpl::globMutex;
bool GeometryDBImpl::initialized = false;
}
}



GeometryDB::GeometryDB()
    : impl(std::make_shared<impl::GeometryDBImpl>())
{

}

void GeometryDB::query(std::set<GeometryPtr>& return_points, const RectWorldCoord& range_coord) const
{
    return impl->query(return_points, range_coord);
}

bool GeometryDB::insert(const PointWorldCoord& point_coord, const GeometryPtr& object)
{
    return impl->insert(point_coord,object);
}

void GeometryDB::erase(const PointWorldCoord& point_coord, const GeometryPtr& object)
{
    impl->erase(point_coord, object);
}

void GeometryDB::clear()
{
    impl->clear();
}
