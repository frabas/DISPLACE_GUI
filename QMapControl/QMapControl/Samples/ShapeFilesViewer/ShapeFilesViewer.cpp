//
// Created by fuga on 18/07/2020.
//

#include "ShapeFilesViewer.h"
#include "QMapControl/src/QMapControl/RasterUtils.h"

#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>

#include <QDebug>
#include <QSettings>

#include <stdexcept>
#include <sstream>

using namespace qmapcontrol;

ShapeFilesViewer::ShapeFilesViewer()
: QMainWindow(nullptr)
{
    statusBar()->show();

    map = new QMapControl(QSizeF(800, 600), this);
    map->enablePersistentCache(std::chrono::minutes{24 * 60});
    map->startPersistentCacheHousekeeping();

    baseAdapter = std::make_shared<MapAdapterGoogle>();
    baseLayer = std::make_shared<LayerMapAdapter>("Map Layer", baseAdapter);

    map->addLayer(baseLayer);

    map->setMapFocusPoint(PointWorldCoord(2.1521, 41.3298));
    map->setZoom(9);
    map->setBackgroundColour(Qt::white);

    setCentralWidget(map);

    buildMenu();

    connect(map, &QMapControl::mapFocusPointChanged, this, &ShapeFilesViewer::mapFocusPointChanged);
    connect(map, &QMapControl::mouseEventMoveCoordinate, this, &ShapeFilesViewer::mapMouseMove);
}

void ShapeFilesViewer::buildMenu()
{
    auto fileMenu = menuBar()->addMenu(tr("&File"));
    auto actionSelectShapeFile = new QAction("Load Shapefile...");
    fileMenu->addAction(actionSelectShapeFile);

    connect(actionSelectShapeFile, &QAction::triggered, this, &ShapeFilesViewer::onLoadShapeFile);

    auto actionSelectTiffFile = new QAction("Load Raster...");
    fileMenu->addAction(actionSelectTiffFile);

    connect(actionSelectTiffFile, &QAction::triggered, this, &ShapeFilesViewer::onLoadTiffFile);

    auto layersMenu = menuBar()->addMenu(tr("&Layers"));
    auto actionLayermap = new QAction("Map");
    actionLayermap->setCheckable(true);
    actionLayermap->setChecked(true);
    layersMenu->addAction(actionLayermap);
    connect(actionLayermap, &QAction::toggled, this, [this](bool checked) {
        baseLayer->setVisible(checked);
    });

    displayMenu = layersMenu->addMenu("&Display");
    layerDeleteMenu = layersMenu->addMenu("Delete");
}

void ShapeFilesViewer::onLoadShapeFile()
{
    QSettings settings;

    try {
        auto shp = std::make_shared<Shapefile>();

        auto basedir = settings.value("shapefiledir").toString();
        auto shapefile = QFileDialog::getOpenFileName(this, tr("Select Shapefile to load"), basedir,
                                                      tr("ShapeFiles (*.shp);;All files (*.*)"));

        if (!shapefile.isEmpty()) {
            QFileInfo info(shapefile);

            shp->dataset = (GDALDataset *) OGROpen(shapefile.toStdString().c_str(), 0, nullptr);
            if (!shp->dataset) {
                std::ostringstream ss;
                ss << "Can't load shapefile " << shapefile.toStdString() << ": ";
                throw std::runtime_error(ss.str());
            }

            auto spatialRef = shp->dataset->GetLayer(0)->GetSpatialRef();
            if (spatialRef)
                qDebug() << "First layer projection: " << spatialRef->GetName();


            auto stdShapeFileName = shapefile.toStdString();

            // NOTE: the second parameter *must* be either nullstring or the name
            // of a layer present in the shape file! Otherwise nothing will be displayed.
            shp->adapter = std::make_shared<ESRIShapefile>(shp->dataset, "");

            shp->adapter->setPen(QPen(Qt::red));
            QColor col(Qt::yellow);
            col.setAlpha(64);
            shp->adapter->setBrush(QBrush(col));

            shp->name = info.fileName().toStdString();
            shp->layer = std::make_shared<LayerESRIShapefile>(shp->name);
            connect(shp->layer.get(), &LayerESRIShapefile::featuresClicked, this,
                    &ShapeFilesViewer::onShapefileFeaturesClicked);
            shp->layer->addESRIShapefile(shp->adapter);

            map->addLayer(shp->layer);
            shp->layer->setVisible(true);

            settings.setValue("shapefiledir", info.absolutePath());

            shapefiles.push_back(shp);
            updateLayersMenu();
        }
    }
    catch (std::exception &x) {
        QMessageBox::warning(this, "Error loading shapefile", x.what());
    }
}

void ShapeFilesViewer::onLoadTiffFile()
{
    QSettings settings;

    try {
        auto rst = std::make_shared<Rasterfile>();

        auto basedir = settings.value("tifffiledir").toString();
        auto tiffFileName = QFileDialog::getOpenFileName(this, tr("Select Raster file to load"), basedir,
                                                         tr("Raster Files (*.tif *.ecw);;All files (*.*)"));

        if (!tiffFileName.isEmpty()) {
            QFileInfo info(tiffFileName);

            rst->dataset = (GDALDataset *) GDALOpen(tiffFileName.toStdString().c_str(), GA_ReadOnly);

            double adfGeoTransform[6];
            qDebug() << "Driver: " << rst->dataset->GetDriver()->GetDescription() << " - "
                     << rst->dataset->GetDriver()->GetMetadataItem(GDAL_DMD_LONGNAME);
            qDebug() << "Size:" << rst->dataset->GetRasterXSize() << rst->dataset->GetRasterYSize()
                     << rst->dataset->GetRasterCount();
            if (rst->dataset->GetProjectionRef() != NULL)
                qDebug() << "Projection: " << rst->dataset->GetProjectionRef();
            if (rst->dataset->GetGeoTransform(adfGeoTransform) == CE_None) {
                qDebug() << "Origin = (" << adfGeoTransform[0] << "," << adfGeoTransform[3] << ")";
                qDebug() << "Pixel Size = (" << adfGeoTransform[1] << "," << adfGeoTransform[5] << ")";
            }

            OGRSpatialReference *oSRS = new OGRSpatialReference;

            // TODO ask the user to enter the correct WCG and Projection
            oSRS->importFromEPSG(32628);
            rst->dataset->SetSpatialRef(oSRS);

            rst->adapter = std::make_shared<AdapterRaster>(rst->dataset, oSRS, "");
            map->setMapFocusPoint(rst->adapter->getCenter());

            rst->name = info.fileName().toStdString();
            rst->layer = std::make_shared<LayerRaster>(rst->name);
            rst->layer->addRaster(rst->adapter);

            map->addLayer(rst->layer);
            rst->layer->setVisible(true);

            settings.setValue("tifffiledir", info.absolutePath());

            rasterfiles.push_back(rst);
            updateLayersMenu();
        }
    }
    catch (std::exception &x) {
        qWarning() << "Exception: " << x.what();
        QMessageBox::warning(this, "Import TIFF", QString("Import failed: %1").arg(x.what()));
    }
}

void ShapeFilesViewer::mapFocusPointChanged(qmapcontrol::PointWorldCoord focusPoint)
{
    statusBar()->showMessage(
            QString("Map Center Point: (lon %1, lat %2)").arg(focusPoint.longitude()).arg(focusPoint.latitude()));
}

void ShapeFilesViewer::mapMouseMove(QMouseEvent *mouseEvent, qmapcontrol::PointWorldCoord pressedPos,
                                    qmapcontrol::PointWorldCoord currentPos)
{
    auto focusPoint = map->mapFocusPointCoord();
    statusBar()->showMessage(
            QString("Map Center Point: (lon %1, lat %2) - Mouse Point: (lon %3, lat %4)")
                    .arg(focusPoint.longitude()).arg(focusPoint.latitude())
                    .arg(currentPos.longitude()).arg(currentPos.latitude()));
}

void ShapeFilesViewer::updateLayersMenu()
{
    layerDeleteMenu->clear();
    displayMenu->clear();

    for (auto shp : shapefiles) {
        auto displayAct = displayMenu->addAction(QString::fromStdString(shp->name));
        displayAct->setCheckable(true);
        displayAct->setChecked(true);
        connect(displayAct, qOverload<bool>(&QAction::triggered), this, [this, shp](bool visible) {
            map->getLayer(shp->name)->setVisible(visible);
        });

        auto deleteAct = layerDeleteMenu->addAction(QString::fromStdString(shp->name));
        connect(deleteAct, qOverload<bool>(&QAction::triggered), this, [this, shp](bool visible) {
            map->removeLayer(shp->name);

            shapefiles.erase(
                    std::remove_if(shapefiles.begin(), shapefiles.end(), [shp](std::shared_ptr<Shapefile> r) {
                        return r->name == shp->name;
                    }));
            updateLayersMenu();
        });
    }
    for (auto rasterfile : rasterfiles) {
        auto displayAct = displayMenu->addAction(QString::fromStdString(rasterfile->name));
        displayAct->setCheckable(true);
        displayAct->setChecked(true);
        connect(displayAct, qOverload<bool>(&QAction::triggered), this, [this, rasterfile](bool visible) {
            map->getLayer(rasterfile->name)->setVisible(visible);
        });

        auto deleteAct = layerDeleteMenu->addAction(QString::fromStdString(rasterfile->name));
        connect(deleteAct, qOverload<bool>(&QAction::triggered), this, [this, rasterfile](bool visible) {
            map->removeLayer(rasterfile->name);
            rasterfiles.erase(
                    std::remove_if(rasterfiles.begin(), rasterfiles.end(), [rasterfile](std::shared_ptr<Rasterfile> r) {
                        return r->name == rasterfile->name;
                    }));
            updateLayersMenu();
        });
    }
}

void ShapeFilesViewer::onShapefileFeaturesClicked(std::vector<OGRFeature *> features)
{
    QString msg = QString("%1 Features selected").arg(features.size());
    qDebug() << msg;
    statusBar()->showMessage(msg);
}

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("QMapControl");
    QCoreApplication::setOrganizationDomain("qmapcontrol.sample.local");
    QCoreApplication::setApplicationName("ShapeFileViewerSample");
    QApplication app(argc, argv);

    GDALAllRegister();

    ShapeFilesViewer mainWindow;
    mainWindow.resize(800, 600);
    mainWindow.show();

    return app.exec();
}
