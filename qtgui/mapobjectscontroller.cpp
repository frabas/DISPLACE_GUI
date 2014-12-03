#include "mapobjectscontroller.h"

#include <displacemodel.h>
#include <mapobjects/harbourmapobject.h>
#include <mapobjects/nodemapobject.h>
#include <mapobjects/vesselmapobject.h>
#include <mapobjects/edgemapobject.h>
#include <mapobjects/edgelayer.h>

#include <QMapControl/QMapControl.h>
#include <QMapControl/MapAdapterOSM.h>
#include <QMapControl/Layer.h>
#include <QMapControl/MapAdapterOpenSeaMap.h>
#include <QMapControl/LayerGeometry.h>
#include <QMapControl/GeometryPointCircle.h>
#include <QMapControl/LayerMapAdapter.h>
#include <QMapControl/ImageManager.h>
#include <QMapControl/GeometryWidget.h>
#include <QMapControl/LayerESRIShapefile.h>

#include <QTextEdit>

/* If defined, CONFIG_ALLOW_MULTIPLE_DETAILS_WINDOWS allows opening more than one Details window.
 */
#undef CONFIG_ALLOW_MULTIPLE_DETAILS_WINDOWS

MapObjectsController::MapObjectsController(qmapcontrol::QMapControl *map)
    : mMap(map),
      mModelVisibility(MAX_MODELS, false),
      mLayers(MAX_MODELS, LayerListImpl(LayerMax)),
      mOutputLayers(MAX_MODELS, LayerListImpl(OutLayerMax)),
      mShapefiles(MAX_MODELS, QList<std::shared_ptr<OGRDataSource> >()),
      mShapefileLayers(MAX_MODELS, LayerVarListImpl<qmapcontrol::LayerESRIShapefile>()),
      mEditorMode(NoEditorMode),
      mClosing(false)
{
    // create mapadapter, for mainlayer and overlay
    mMainMapAdapter = std::shared_ptr<qmapcontrol::MapAdapter> (new qmapcontrol::MapAdapterOSM());
    mSeamarkAdapter = std::shared_ptr<qmapcontrol::MapAdapter> (new qmapcontrol::MapAdapterOpenSeaMap());

    // create a layer with the mapadapter and type MapLayer
    mMainLayer = std::shared_ptr<qmapcontrol::LayerMapAdapter>(new qmapcontrol::LayerMapAdapter("OpenStreetMap", mMainMapAdapter));
    mSeamarkLayer = std::shared_ptr<qmapcontrol::LayerMapAdapter>(new qmapcontrol::LayerMapAdapter("Seamark", mSeamarkAdapter));
    mWidgetLayer = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry("Details"));
    mEditorLayer = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry("Editor"));
    mEditorLayer->setVisible(true);

    mMap->addLayer(mMainLayer);
    mMap->addLayer(mSeamarkLayer);
    mMap->addLayer(mWidgetLayer);
    mMap->addLayer(mEditorLayer);

    mMap->setMapFocusPoint(qmapcontrol::PointWorldCoord(11.54105,54.49299));
    mMap->setZoom(10);

    connect (mMap, SIGNAL(geometryClicked(const Geometry*)), this, SLOT(geometryClicked(const Geometry*)));
}

void MapObjectsController::setModel(int model_n, std::shared_ptr<DisplaceModel> model)
{
    mModels[model_n] = model;
}

void MapObjectsController::removeModel(int model_n)
{
    mModels[model_n] = std::shared_ptr<DisplaceModel>();
    // maybe we should reload everything...?
}

void MapObjectsController::createMapObjectsFromModel(int model_n, DisplaceModel *model)
{
    addStandardLayer(model_n, LayerMain, mMainLayer);
    addStandardLayer(model_n, LayerSeamarks, mSeamarkLayer);

    mEntityLayer[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Entities")).arg(model_n).toStdString()));
    mGraphLayer[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Graph")).arg(model_n).toStdString()));
    mEdgesLayer[model_n] = std::shared_ptr<EdgeLayer>(new EdgeLayer(this, QString(tr("#%1#Graph Edges")).arg(model_n)));
    mEdgesLayer[model_n]->setVisible(false);

    addStandardLayer(model_n, LayerEntities, mEntityLayer[model_n]);
    addStandardLayer(model_n, LayerGraph, mGraphLayer[model_n]);
    addStandardLayer(model_n, LayerEdges, mEdgesLayer[model_n]->layer());

    mStatsLayerPop[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Abundance")).arg(model_n).toStdString()));
    addOutputLayer(model_n, OutLayerPopStats, mStatsLayerPop[model_n]);

    mStatsLayerBiomass[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Biomass")).arg(model_n).toStdString()));
    addOutputLayer(model_n, OutLayerBiomass, mStatsLayerBiomass[model_n]);

    mStatsLayerImpact[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Impact")).arg(model_n).toStdString()));
    addOutputLayer(model_n, OutLayerPopImpact, mStatsLayerImpact[model_n] );

    mStatsLayerCumftime[model_n] = std::shared_ptr<qmapcontrol::LayerGeometry>(new qmapcontrol::LayerGeometry(QString(tr("#%1#Fishing Effort")).arg(model_n).toStdString()));
    addOutputLayer(model_n, OutLayerCumFTime, mStatsLayerCumftime[model_n]);

    const QList<std::shared_ptr<HarbourData> > &harbours = model->getHarboursList();
    foreach (std::shared_ptr<HarbourData> h, harbours) {
        addHarbour(model_n, h, true);
    }

    const QList<std::shared_ptr<NodeData> > &nodes = model->getNodesList();
    foreach (std::shared_ptr<NodeData> nd, nodes) {
        if (nd->get_harbour())
            continue;
        addNode(model_n, nd, true);
    }

    const QList<std::shared_ptr<VesselData> > &vessels = model->getVesselList();
    foreach (std::shared_ptr<VesselData> vsl, vessels) {
        VesselMapObject *obj = new VesselMapObject(this,vsl.get());
        mVesselObjects[model_n].append(obj);

        mEntityLayer[model_n]->addGeometry(obj->getGeometryEntity());
    }
}

void MapObjectsController::updateMapObjectsFromModel(int model_n, DisplaceModel *model)
{
    const QList<std::shared_ptr<VesselData> > &vessels = model->getVesselList();
    foreach (std::shared_ptr<VesselData> vsl, vessels) {
        updateVesselPosition(model_n, vsl->mVessel->get_idx());
    }

}

void MapObjectsController::updateVesselPosition(int model, int idx)
{
    mVesselObjects[model].at(idx)->vesselUpdated();
}

void MapObjectsController::updateNodes(int model)
{
    foreach (NodeMapObject *obj, mNodeObjects[model]) {
        obj->updateProperties();
        obj->getGeometryEntity()->requestRedraw();
    }
}

void MapObjectsController::setModelVisibility(int model, MapObjectsController::Visibility visibility)
{
    bool visible = (visibility == Visible);
    mModelVisibility[model] = visible;

    foreach (HarbourMapObject *h, mHarbourObjects[model]) {
        h->getGeometryEntity()->setVisible(visible);
    }
    foreach (NodeMapObject *n, mNodeObjects[model]) {
        n->getGeometryEntity()->setVisible(visible);
    }
    foreach (VesselMapObject *v, mVesselObjects[model]) {
        v->getGeometryEntity()->setVisible(visible);
    }
    foreach (EdgeMapObject *e, mEdgeObjects[model]) {
        e->getGeometryEntity()->setVisible(visible);
    }
}

void MapObjectsController::setLayerVisibility(int model, ObjectTreeModel::Category type, int layer, bool visibility)
{
    switch (type) {
    case ObjectTreeModel::Layers:
        mLayers[model].setVisible(layer, visibility);
        if (isModelActive(model))
            mLayers[model].layer(layer)->setVisible(visibility);
        break;
    case ObjectTreeModel::OutputLayers:
        mOutputLayers[model].setVisible(layer, visibility);
        if (isModelActive(model))
            mOutputLayers[model].layer(layer)->setVisible(visibility);
        break;
    case ObjectTreeModel::ShapefileLayers:
        mShapefileLayers[model].setVisible(layer, visibility);
        if (isModelActive(model))
            mShapefileLayers[model].layers[layer]->setVisible(visibility);
        break;
    default:
        throw std::runtime_error("Unhandled type in setLayerVisibility");
    }
}

bool MapObjectsController::isLayerVisible(int model, ObjectTreeModel::Category type, int layer)
{
    if (!mModelVisibility[model])
        return false;

    switch (type) {
    case ObjectTreeModel::Layers:
        return mLayers[model].isVisible(layer);
    case ObjectTreeModel::OutputLayers:
        return mOutputLayers[model].isVisible(layer);
    case ObjectTreeModel::ShapefileLayers:
        return mShapefileLayers[model].isVisible(layer);
    default:
        break;
    }

    return false;
}

bool MapObjectsController::isModelActive(int model) const
{
    return mModelVisibility[model];
}

void MapObjectsController::setPalette(int model, PaletteRole n, const Palette &palette)
{
    Q_UNUSED(model);
    PaletteManager::instance()->setPalette(n, palette);
}

void MapObjectsController::forceRedraw()
{
    mMap->requestRedraw();
}

void MapObjectsController::showDetailsWidget(const PointWorldCoord &point, QWidget *widget)
{
#ifndef CONFIG_ALLOW_MULTIPLE_DETAILS_WINDOWS
    removeAllWidgets();
//    foreach (const auto &g, mWidgetLayer->getGeometryWidgets()) {
//        mWidgetLayer->removeGeometry(g);
//    }
#endif

    std::shared_ptr<qmapcontrol::GeometryWidget>  mDetailsWidgetContainer = std::shared_ptr<qmapcontrol::GeometryWidget>(new qmapcontrol::GeometryWidget(point, widget));
    mDetailsWidgetContainer->setAlignmentType(GeometryPoint::AlignmentType::BottomLeft);
    mDetailsWidgetContainer->setVisible(true);

    widget->setUserData(0, new WidgetUserData(mDetailsWidgetContainer));
    widget->setAttribute(Qt::WA_DeleteOnClose);
    connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(widgetClosed(QObject*)));

    mWidgetLayer->addGeometry(mDetailsWidgetContainer);
}

bool MapObjectsController::importShapefile(int model_idx, QString path, QString layername)
{
    std::shared_ptr<OGRDataSource> src(OGRSFDriverRegistrar::Open(path.toStdString().c_str(), FALSE));
    if (!src.get()) {
        return false;
    }

    std::shared_ptr<ESRIShapefile> file (new ESRIShapefile(src.get(), layername.toStdString()));

    file->setPenPolygon(QPen(Qt::red));
    QColor col (Qt::yellow);
    col.setAlpha(64);
    file->setBrushPolygon(QBrush(col));

    QFileInfo info(path);
    QString label = QString("Shapefile %1").arg(info.fileName());

    if (!layername.isEmpty())
        label += QString (" (%1)").arg(layername);

    std::shared_ptr<qmapcontrol::LayerESRIShapefile> newlayer(new qmapcontrol::LayerESRIShapefile(label.toStdString()));
    newlayer->addESRIShapefile(file);
    addShapefileLayer(model_idx, src, newlayer);

    return true;
}

QStringList MapObjectsController::getShapefilesList(int model_idx) const
{
    QStringList list;
    int n = mShapefileLayers[model_idx].getCount();
    for (int i = 0; i < n; ++i) {
        list << mShapefileLayers[model_idx].getName(i);
    }
    return list;
}

std::shared_ptr<ESRIShapefile> MapObjectsController::getShapefile(int model_idx, int idx)
{
    return mShapefileLayers[model_idx].layers[idx]->getShapefile(0);
}

std::shared_ptr<OGRDataSource> MapObjectsController::getShapefileDatasource(int model_idx, const QString &name)
{
    for (int i = 0; i < mShapefileLayers[model_idx].getCount(); ++i) {
        if (mShapefileLayers[model_idx].getName(i) == name) {
            return mShapefiles[model_idx].at(i);
        }
    }

    return std::shared_ptr<OGRDataSource>();
}

void MapObjectsController::setEditorMode(MapObjectsController::EditorModes mode)
{
    mEditorMode = mode;
}

void MapObjectsController::delSelected(int model)
{
    if (mEditorMode == NoEditorMode)
        return;

    switch (mEditorMode) {
    case EdgeEditorMode:
        delSelectedEdges(model);
        break;
    default:
        break;
    }
}

void MapObjectsController::clearAllNodes(int model_n)
{
    mGraphLayer[model_n]->clearGeometries();
    mStatsLayerBiomass[model_n]->clearGeometries();
    mStatsLayerCumftime[model_n]->clearGeometries();
    mStatsLayerImpact[model_n]->clearGeometries();
    mStatsLayerPop[model_n]->clearGeometries();
    mEdgesLayer[model_n]->clear();
    mEntityLayer[model_n]->clearGeometries();

    mHarbourObjects[model_n].clear();
    mNodeObjects[model_n].clear();
    mEdgeObjects[model_n].clear();
}

void MapObjectsController::addStandardLayer(int model, LayerIds id, std::shared_ptr<Layer> layer)
{
    if (layer != mMainLayer && layer != mSeamarkLayer)
        mMap->addLayer(layer);
    mLayers[model].setLayer(id, layer);
}

void MapObjectsController::addOutputLayer(int model, OutLayerIds id, std::shared_ptr<Layer> layer)
{
    mMap->addLayer(layer);
    mOutputLayers[model].setLayer(id,layer);
}

void MapObjectsController::addShapefileLayer(int model, std::shared_ptr<OGRDataSource> datasource, std::shared_ptr<qmapcontrol::LayerESRIShapefile> layer, bool show)
{
    mMap->addLayer(layer);
    mShapefileLayers[model].add(layer, show);
    mShapefiles[model].append(datasource);
}

void MapObjectsController::addNode(int model_n, std::shared_ptr<NodeData> nd, bool disable_redraw)
{
    if (nd->isDeleted())
        return;

    NodeMapObject *obj = new NodeMapObject(this, model_n, NodeMapObject::GraphNodeRole, nd);
    connect(obj, SIGNAL(nodeSelectionHasChanged(NodeMapObject*)), this, SLOT(nodeSelectionHasChanged(NodeMapObject*)));
    mNodeObjects[model_n].append(obj);

    mGraphLayer[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    /* add here other roles */
    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithPopStatsRole, nd);
    mNodeObjects[model_n].append(obj);
    mStatsLayerPop[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithCumFTimeRole, nd);
    mNodeObjects[model_n].append(obj);
    mStatsLayerCumftime[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithPopImpact, nd);
    mNodeObjects[model_n].append(obj);
    mStatsLayerImpact[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    obj = new NodeMapObject(this, model_n,NodeMapObject::GraphNodeWithBiomass, nd);
    mNodeObjects[model_n].append(obj);
    mStatsLayerBiomass[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);

    for (int i = 0; i < nd->getAdiacencyCount(); ++i) {
        EdgeMapObject *edge = new EdgeMapObject(this, i, nd.get());

        connect (edge, SIGNAL(edgeSelectionHasChanged(EdgeMapObject*)), this, SLOT(edgeSelectionHasChanged(EdgeMapObject*)));

        mEdgeObjects[model_n].append(edge);
        mEdgesLayer[model_n]->addEdge(edge, disable_redraw);
    }
}

void MapObjectsController::addHarbour(int model_n, std::shared_ptr<HarbourData> h, bool disable_redraw)
{
    HarbourMapObject *obj = new HarbourMapObject(this, mModels[model_n].get(), h.get());
    mHarbourObjects[model_n].append(obj);

    mEntityLayer[model_n]->addGeometry(obj->getGeometryEntity(), disable_redraw);
}

void MapObjectsController::clearEditorLayer()
{
    mEditorLayer->clearGeometries();
}

void MapObjectsController::addEditorLayerGeometry(std::shared_ptr<Geometry> geometry)
{
    mEditorLayer->addGeometry(geometry);
}

void MapObjectsController::delSelectedEdges(int model)
{
    foreach (EdgeMapObject *edge, mEdgeSelection[model]) {
        NodeData* nd = edge->node();
        NodeData* tg = edge->target();

//        int nodeid1 = nd->get_idx_node();

        // TODO: automatically remove other side edges
//        int nodeid2 = nd->getAdiacencyByIdx(eid1);

//        NodeData *nd2 = mModels[model]->getNodesList()[nodeid2];
//        nd2->removeAdiacencyByTarget(nodeid1);
//        nd->removeAdiacencyByIdx(eid1);
        nd->removeAdiacencyByTarget(tg->get_idx_node());

        mEdgesLayer[model]->removeEdge(edge);
    }

    mEdgeSelection[model].clear();
    emit edgeSelectionChanged(0);
}

void MapObjectsController::geometryClicked(const Geometry *geometry)
{
    WidgetAncillaryData *objPtr = reinterpret_cast<WidgetAncillaryData *>(geometry->ancillaryData());

    if (objPtr && objPtr->object()) {
        switch(mEditorMode) {
        case NoEditorMode:
            objPtr->object()->showProperties();
            break;
        case EdgeEditorMode:
            objPtr->object()->toggleSelection();
            break;
        case NodeEditorMode:
            objPtr->object()->toggleSelection();
            break;
        }
    }
}

void MapObjectsController::widgetClosed(QObject *widget)
{
    if (mClosing)
        return;
    WidgetUserData *obj = reinterpret_cast<WidgetUserData*>(widget->userData(0));
    if (obj) {
        mWidgetLayer->removeGeometry(obj->widget(), mClosing);
    }
}

void MapObjectsController::signalAppIsClosing()
{
    mClosing = true;
}

void MapObjectsController::removeAllWidgets()
{
    foreach (const auto &wid, mWidgetLayer->getGeometryWidgets()) {
        wid->getWidget()->close();
        mWidgetLayer->removeGeometry(wid, true);
    }
}

void MapObjectsController::edgeSelectionHasChanged(EdgeMapObject *object)
{
    int modelIndex = object->node()->getModel()->index();

    if (object->selected())
        mEdgeSelection[modelIndex].insert(object);
    else
        mEdgeSelection[modelIndex].remove(object);

    emit edgeSelectionChanged(mEdgeSelection[modelIndex].size());
}

void MapObjectsController::nodeSelectionHasChanged(NodeMapObject *node)
{
    int modelIndex = node->node()->getModel()->index();

    if (node->selected())
        mNodeSelection[modelIndex].insert(node);
    else
        mNodeSelection[modelIndex].remove(node);

    emit nodeSelectionChanged(mNodeSelection[modelIndex].size());
}

void MapObjectsController::redraw()
{
    mMap->requestRedraw();
}
