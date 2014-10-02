#ifndef MAPOBJECTSCONTROLLER_H
#define MAPOBJECTSCONTROLLER_H

#include <QList>
#include <QVector>

#include <memory>
#include <mainwindow.h>

#include <QMapControl/Layer.h>

namespace qmapcontrol {
class QMapControl;
class MapAdapter;
class LayerMapAdapter;
class LayerGeometry;
}

class DisplaceModel;
class HarbourMapObject;
class NodeMapObject;
class VesselMapObject;

class MapObjectsController
{
public:
    class LayerList {
    public:
        virtual int getCount() const = 0;
        virtual QString getName(int idx) const = 0;
    };

private:
    static const int MaxLayers = 32;

    class LayerListImpl : public LayerList {
    public:
        LayerListImpl()
            : LayerListImpl(MaxLayers) {}

        explicit LayerListImpl (int sz) {
            layers.fill(std::shared_ptr<qmapcontrol::Layer>(), sz);
            visibility.fill(false, sz);
        }

        QVector<std::shared_ptr<qmapcontrol::Layer> > layers;
        QVector<bool> visibility;

        virtual int getCount() const { return layers.size(); }
        virtual QString getName(int idx) const { return QString::fromStdString(layers[idx]->getName()); }
        virtual bool isVisible(int idx) const { return layers[idx]->isVisible(); }

        virtual void setVisible(int idx, bool v) { layers[idx]->setVisible(v); }
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

    LayerList *getStandardLayerList(int model) {
        return &mLayers[model];
    }
    LayerList *getOutputLayerList(int model) {
        return &mOutputLayers[model];
    }

    enum Visibility { Visible, Invisible };
    /** \brief Sets the visibility of the model objects
     * \param model index of the model
     * \param visibility visibility status
     * */
    void setModelVisibility(int model, Visibility visibility);
    void setLayerVisibility (int model, LayerIds layer, bool visibility);
    bool isLayerVisible (int model, LayerIds layer);

    bool isModelActive (int model) const;

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

    QVector<bool> mModelVisibility;
    QVector<LayerListImpl> mLayers;
    QVector<LayerListImpl> mOutputLayers;
};

#endif // MAPOBJECTSCONTROLLER_H
