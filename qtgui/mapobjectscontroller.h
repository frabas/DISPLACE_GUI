// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2026 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------------------------------------------------------------

/* --------------------------------------------------------------------------
 * DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
 * AND EFFORT DISPLACEMENT
 * Copyright (c) 2012, 2013, 2014 Francois Bastardie <fba@aqua.dtu.dk>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */
#ifndef MAPOBJECTSCONTROLLER_H
#define MAPOBJECTSCONTROLLER_H

#include <modelobjects/nodedata.h>

#include <QObject>
#include <QList>
#include <QVector>

#include <memory>
#include <mainwindow.h>
#include <palettemanager.h>
#include <objecttreemodel.h>
#include <editorlayerinterface.h>
#include <mapobjects/mapobjectcontainer.h>

#include <ogrsf_frmts.h>

#include <QMapControl/Layer.h>
#include <QMapControl/Geometry.h>
#include <QMapControl/ESRIShapefile.h>
#include <QMapControl/LayerESRIShapefile.h>

namespace qmapcontrol {
class QMapControl;
class MapAdapter;
class LayerMapAdapter;
class LayerGeometry;
class GeometryWidget;
}

class DisplaceModel;
class MapObject;
class HarbourMapObject;
class NodeMapObject;
class VesselMapObject;
class ShipMapObject;
class FishfarmMapObject;
class WindmillMapObject;
class EdgeLayer;
class EdgeMapObject;
class HarbourData;
class MapsDataProvider;

QT_BEGIN_NAMESPACE
class QTextEdit;
QT_END_NAMESPACE

using qmapcontrol::Geometry;

class MapObjectsController : public QObject, public EditorLayerInterface
{
    Q_OBJECT

public:
    class LayerList {
    public:
        virtual ~LayerList() noexcept = default;

        virtual int getCount() const = 0;
        virtual QString getName(int idx) const = 0;
    };

    class WidgetAncillaryData : public qmapcontrol::Geometry::AncillaryData {
    private:
        MapObject *mObject;
    public:
        explicit WidgetAncillaryData(MapObject *object)
            : mObject(object) {
        }

        MapObject *object() const { return mObject; }
    };

private:
    static const int MaxLayers = 32;

    class LayerListImpl : public LayerList {
    public:
        LayerListImpl()
            : LayerListImpl(MaxLayers) {}

        explicit LayerListImpl (int sz) {
            Q_UNUSED(sz);
        }

        virtual int getCount() const { return layers.size(); }
        virtual QString getName(int idx) const { return layers[idx] != 0 ? QString::fromStdString(layers[idx]->getName()) : QString(""); }
        virtual bool isVisible(int idx) const { return layers[idx] != 0 && layers[idx]->isVisible(); }

        virtual void setVisible(int idx, bool v) { visibility[idx] = v; layers[idx]->setVisible(v); }

        virtual void setLayer (int idx, std::shared_ptr<qmapcontrol::Layer> layer, bool shown = true) {
            while (layers.size() <= idx)
                layers.push_back(0);
            while (visibility.size() <= idx)
                visibility.push_back(false);
            layers[idx] = layer;
            visibility[idx] = shown;
        }

        virtual void updateVisibility(bool show) {
            for (int i = 0; i < layers.size(); ++i) {
                layers[i]->setVisible(show ? visibility[i] : false);
            }
        }

        std::shared_ptr<qmapcontrol::Layer> layer(int idx) const { return layers[idx]; }
    protected:
        QVector<std::shared_ptr<qmapcontrol::Layer> > layers;
        QVector<bool> visibility;
    };

    template <typename L>
    class LayerVarListImpl : public LayerList {
    public:
        LayerVarListImpl() {}

        QList<std::shared_ptr<L> > layers;
        QVector<bool> visibility;
        QStringList fullpath;

        virtual int getCount() const { return layers.size(); }
        virtual QString getName(int idx) const { return QString::fromStdString(layers[idx]->getName()); }
        virtual QString getFullPath(int idx) const { return fullpath.at(idx); }
        virtual bool isVisible(int idx) const { return layers[idx] != 0 && layers[idx]->isVisible(); }

        virtual void setVisible(int idx, bool v) { visibility[idx] = v; layers[idx]->setVisible(v); }

        virtual void updateVisibility(bool show) {
            for (int i = 0; i < layers.size(); ++i) {
                layers[i]->setVisible(show ? visibility[i] : false);
            }
        }

        virtual bool add(std::shared_ptr<L> layer, QString _fullpath, bool show = true) {
            layers.push_back(layer);
            visibility.push_back(show);
            fullpath.push_back(_fullpath);

            return true;
        }
    };


public:
    enum LayerIds {
        LayerMain = 0, LayerSeamarks = 1,
        LayerEntities = 2, LayerGraph = 3,
        LayerEdges,

        LayerMax
    };
    enum OutLayerIds {
        OutLayerPopStats = 0,
        OutLayerBiomass,
        OutLayerPopImpact,
        OutLayerPopCumcatches,
        OutLayerBenthosBiomass,
        OutLayerBenthosNumber,
        OutLayerBenthosMeanweight,
        OutLayerCumFTime,
        OutLayerCumSweptArea,
        OutLayerCumSubsurfaceSweptArea,
        OutLayerCumCatches,
        OutLayerCumCatchesWithThreshold,
        OutLayerCumdiscards,
        OutLayerCumdiscardsRatio,
        OutLayerNbChoked,

        OutLayerMax

    };

    enum TariffLayerIds {
        TariffLayerTariffMet0,
        TariffLayerTariffMet1,
        TariffLayerTariffMet2,

     TariffLayerMax
    };

    enum EnvLayerIds {
        EnvLayerWind,
        EnvLayerSST,
        EnvLayerSalinity,
        EnvLayerNitrogen,
        EnvLayerPhosphorus,
        EnvLayerOxygen,
        EnvLayerDissolvedCarbon,
        EnvLayerBathymetry,
        EnvLayerShippingdensity,
        EnvLayerSiltfraction,
        EnvLayerIcesrectanglecode,

     EnvLayerMax
    };

    enum EditorModes {
        NoEditorMode,
        EdgeEditorMode, NodeEditorMode
    };

    MapObjectsController(qmapcontrol::QMapControl *map);
    virtual ~MapObjectsController() noexcept = default;

    qmapcontrol::QMapControl *mapWidget() const { return mMap; }

    void setModel(int model_n, std::shared_ptr<DisplaceModel> model);
    DisplaceModel &getModel (int model_n);
    void removeModel(int model_n);
    void createMapObjectsFromModel(int model_n, DisplaceModel *model);
    void updateMapObjectsFromModel(int model_n, DisplaceModel *model);

    void updateVesselPosition (int model, int idx);
    void updateShipPosition (int model, int idx);
    void updateFishfarmPosition (int model, int idx);
    void updateWindmillPosition (int model, int idx);

    void updateNodes(int model);

    LayerList *getLayerList(int model, ObjectTreeModel::Category type) {
        switch (type) {
        case ObjectTreeModel::Layers: return &mLayers[model];
        case ObjectTreeModel::ShapefileLayers: return &mShapefileLayers[model];
        case ObjectTreeModel::EnvLayers: return &mEnvLayers[model];
        case ObjectTreeModel::OutputLayers: return &mOutputLayers[model];
        case ObjectTreeModel::TariffsLayers: return &mTariffsLayers[model];
        default:
            break;
        }
        return 0;
    }

    enum Visibility { Visible, Invisible };
    /** \brief Sets the visibility of the model objects
     * \param model index of the model
     * \param visibility visibility status
     * */
    void setModelVisibility(int model, Visibility visibility);
    void setLayerVisibility (int model, ObjectTreeModel::Category type, int layer, bool visibility);
    bool isLayerVisible (int model, ObjectTreeModel::Category type, int layer);

    bool isModelActive (int model) const;

    const Palette &getPalette(int model, PaletteRole n) const {
        Q_UNUSED(model);
        return PaletteManager::instance()->palette(n);
    }
    void setPalette (int model, PaletteRole n, const Palette &palette);

    void forceRedraw();

    void showDetailsWidget(const PointWorldCoord &point, QWidget *widget);

    /* Editor functions */

    bool importShapefile(int model_idx, QString path, QString layername);
    QStringList getShapefilesList(int model_idx) const;
    std::shared_ptr<qmapcontrol::ESRIShapefile> getShapefile(int model_idx, int idx);
    std::shared_ptr<GDALDataset> cloneShapefileDatasource(int model_idx, const QString &name);

    void setEditorMode (EditorModes mode);
    EditorModes getEditorMode() const { return mEditorMode; }

    void clearNodeSelection(int model);
    void selectNodes(int model, QList<types::NodeId> nodes);
    void delSelected(int model);

    QSet<EdgeMapObject *> edgeSelection(int model) const { return mEdgeSelection[model]; }


    void clearAllNodes(int model_n);
    void addNode(int model_n, std::shared_ptr<NodeData> nd, bool disable_redraw = false);
    void addHarbour(int model_n, std::shared_ptr<HarbourData> nd, bool disable_redraw = false);
    void addEdge (int model_n, std::shared_ptr<NodeData::Edge> edge, bool disable_redraw);

    void clearEditorLayer();
    void addEditorLayerGeometry (std::shared_ptr<qmapcontrol::Geometry> geometry);

    MapsDataProvider &getMapDataProvider(int model);

    void delSelectedNodes(int model);

protected:
    void addStandardLayer(int model, LayerIds id, std::shared_ptr<Layer> layer, bool visibility);
    void addOutputLayer(int model, OutLayerIds id, std::shared_ptr<Layer> layer, bool visibility);
    void addTariffLayer(int model, int id, std::shared_ptr<Layer> layer, bool visibility);
    void addEnvLayer(int model, int id, std::shared_ptr<Layer> layer, bool visibility);
    void addShapefileLayer(int model, QString name, std::shared_ptr<GDALDataset> datasource, std::shared_ptr<LayerESRIShapefile> layer, bool show = true);

    void delSelectedEdges(int model);

protected slots:
    void geometryClicked(const Geometry *);
    void widgetClosed(QObject *);

public slots:
    void signalAppIsClosing();
    void removeAllWidgets();

    /* Selection slots */
    void edgeSelectionHasChanged (EdgeMapObject *object);
    void nodeSelectionHasChanged (NodeMapObject *node);

    void redraw();

signals:
    int edgeSelectionChanged (int num);
    int nodeSelectionChanged (int num);

private:
    qmapcontrol::QMapControl *mMap;
    std::shared_ptr<DisplaceModel> mModels[MainWindow::MAX_MODELS];

    MapObjectContainer<HarbourMapObject, types::NodeId> mHarbourObjects[MainWindow::MAX_MODELS];
    MapObjectContainer<NodeMapObject, types::NodeId> mNodeObjects[MainWindow::MAX_MODELS];
    MapObjectContainer<VesselMapObject, int> mVesselObjects[MainWindow::MAX_MODELS];
    MapObjectContainer<ShipMapObject, int> mShipObjects[MainWindow::MAX_MODELS];
    MapObjectContainer<FishfarmMapObject, int> mFishfarmObjects[MainWindow::MAX_MODELS];
    MapObjectContainer<WindmillMapObject, int> mWindmillObjects[MainWindow::MAX_MODELS];
    MapObjectContainer<EdgeMapObject, int> mEdgeObjects[MainWindow::MAX_MODELS];

//    std::shared_ptr<PaletteManager> mPaletteManager[MainWindow::MAX_MODELS];

    /* Layers and adapters commons to all models */
    std::shared_ptr<qmapcontrol::MapAdapter> mMainMapAdapter;
    std::shared_ptr<qmapcontrol::MapAdapter> mSeamarkAdapter;
    std::shared_ptr<qmapcontrol::LayerMapAdapter> mMainLayer;
    std::shared_ptr<qmapcontrol::LayerMapAdapter> mSeamarkLayer;
    std::shared_ptr<qmapcontrol::LayerGeometry> mWidgetLayer;
    std::shared_ptr<qmapcontrol::LayerGeometry> mEditorLayer;   /* Layer to show temporary geometries */

    /* Layers specific to every model */
    std::shared_ptr<qmapcontrol::LayerGeometry> mEntityLayer[MainWindow::MAX_MODELS];
    std::shared_ptr<EdgeLayer> mEdgesLayer[MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mGraphLayer[MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerPop[MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerBiomass[MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerImpact [MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerCumcatchesPerPop [MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerBenthosBiomass [MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerBenthosNumber [MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerBenthosMeanweight [MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerTariffs [MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerEnv [MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerCumftime[MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerCumsweptarea[MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerCumsubsurfacesweptarea[MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerCumcatches[MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerCumcatchesWithThreshold[MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerCumdiscards[MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerCumdiscardsratio[MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerNbchoked[MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerTariffMet0[MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerTariffMet1[MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerTariffMet2[MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerWind[MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerSST[MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerSalinity[MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerNitrogen[MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerPhosphorus[MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerOxygen[MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerDissolvedCarbon[MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerBathymetry[MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerShippingdensity[MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerSiltfraction[MainWindow::MAX_MODELS];
    std::shared_ptr<qmapcontrol::LayerGeometry> mStatsLayerIcesrectanglecode[MainWindow::MAX_MODELS];

    QVector<bool> mModelVisibility;

    QVector<LayerListImpl> mLayers;
    QVector<LayerListImpl> mEnvLayers;
    QVector<LayerListImpl> mOutputLayers;
    QVector<LayerListImpl> mTariffsLayers;
    QVector<QList<std::shared_ptr<GDALDataset> > > mShapefiles;
    QVector<LayerVarListImpl<qmapcontrol::LayerESRIShapefile> > mShapefileLayers;

    EditorModes mEditorMode;
    bool mClosing;

    /* Selection handling */

    QHash<QWidget*, std::shared_ptr<qmapcontrol::GeometryWidget>> mWidgetMap;
    QSet<EdgeMapObject *> mEdgeSelection[MainWindow::MAX_MODELS];
    QSet<NodeMapObject *> mNodeSelection[MainWindow::MAX_MODELS];
};

#endif // MAPOBJECTSCONTROLLER_H
