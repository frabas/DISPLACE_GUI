#ifndef MAPOBJECTSCONTROLLER_H
#define MAPOBJECTSCONTROLLER_H

#include <QList>
#include <QVector>

#include <memory>
#include <mainwindow.h>

namespace qmapcontrol {
class QMapControl;
class MapAdapter;
class Layer;
class LayerMapAdapter;
class LayerGeometry;
}

class DisplaceModel;
class HarbourMapObject;
class NodeMapObject;
class VesselMapObject;

class MapObjectsController
{
    static const int MaxLayers = 32;

    class LayerList {
    public:
        LayerList()
            : LayerList(MaxLayers) {}

        explicit LayerList (int sz) {
            layers.fill(std::shared_ptr<qmapcontrol::Layer>(), sz);
            visibility.fill(false, sz);
        }

        QVector<std::shared_ptr<qmapcontrol::Layer> > layers;
        QVector<bool> visibility;
    };

public:
    enum LayerIds {
        LayerMain = 0, LayerSeamarks = 1,
        LayerEntities = 2, LayerGraph = 3,

        LayerMax
    };
    enum OutLayerIds {
        OutLayerPops = 0,

        OutLayerMax
    };

    MapObjectsController(qmapcontrol::QMapControl *map);

    void createMapObjectsFromModel(int model_n, DisplaceModel *model);
    void updateMapObjectsFromModel(int model_n, DisplaceModel *model);

    void updateVesselPosition (int model, int idx);

    void updateNodes(int model);

    const LayerList &getStandardLayerList(int model) const {
        return mLayers[model];
    }
    const LayerList &getOutputLayerList(int model) const {
        return mOutputLayers[model];
    }

    enum Visibility { Visible, Invisible };
    /** \brief Sets the visibility of the model objects
     * \param model index of the model
     * \param visibility visibility status
     * */
    void setModelVisibility(int model, Visibility visibility);

protected:
    void addStandardLayer(int model, LayerIds id, std::shared_ptr<Layer> layer);
    void addOutputLayer(int model, LayerIds id, std::shared_ptr<Layer> layer);

private:
    qmapcontrol::QMapControl *mMap;
    QList<HarbourMapObject *> mHarbourObjects[MAX_MODELS];
    QList<NodeMapObject *> mNodeObjects[MAX_MODELS];
    QList<VesselMapObject *> mVesselObjects[MAX_MODELS];

    std::shared_ptr<qmapcontrol::MapAdapter> mMainMapAdapter;
    std::shared_ptr<qmapcontrol::MapAdapter> mSeamarkAdapter;
    std::shared_ptr<qmapcontrol::LayerMapAdapter> mMainLayer;
    std::shared_ptr<qmapcontrol::LayerMapAdapter> mSeamarkLayer;

    QVector<LayerList> mLayers;
    QVector<LayerList> mOutputLayers;
};

#endif // MAPOBJECTSCONTROLLER_H
