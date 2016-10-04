#include "csvspecspage.h"
#include "ui_csvspecspage.h"

#include <mapcontrolgraphicsmodel.h>

#include <csv/csvtablemodel.h>
#include <csv/csvimporter.h>
#include <csv/csvexporter.h>

#include <Layer.h>
#include <LayerMapAdapter.h>
#include <MapAdapterOSM.h>
#include <MapAdapterOpenSeaMap.h>

#include <QSortFilterProxyModel>
#include <QMessageBox>

CsvSpecsPage::CsvSpecsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CsvSpecsPage),
    mFilename(), mSeparator(';')
{
    ui->setupUi(this);

    mData = std::make_shared<QList<QStringList>> ();
    mModel = new CsvTableModel(mData);
    mModel->setFirstLineHeaders(true);

#if QT_VERSION > 0x050500
    mVesselsSpecProxyModel = new QSortFilterProxyModel(this);

    mVesselsSpecProxyModel->setSourceModel(mModel);
    ui->tableView->setModel(mVesselsSpecProxyModel);
#else
    ui->tableView->setModel(mModel);
#endif

    ui->map->setVisible(false);
}

CsvSpecsPage::~CsvSpecsPage()
{
    delete ui;
}

void CsvSpecsPage::load()
{
    try {
        CsvImporter i;
        i.setSeparator(mSeparator);
        mData = std::make_shared<QList<QStringList>>(i.import(mFilename));
        mModel->setSource(mData);

        if (mIdIndex != -1 && mLatIndex != -1 && mLonIndex != -1) {
            int n = 0;
            for (auto fields: *mData) {
                if (n++ == 0) { // Skip headers
                    continue;
                }
                bool ok;
                int id = fields[mIdIndex].toInt(&ok);
                if (!ok) {
                    qWarning() << "Bad ID: " << fields[mIdIndex];
                    throw CsvImporter::Exception(tr("Bad ID field"));
                }
                float lat = fields[mLatIndex].toFloat(&ok);
                if (!ok) {
                    qWarning() << "Bad Lat: " << fields[mLatIndex];
                    throw CsvImporter::Exception(tr("Bad Lat field"));
                }
                float lon = fields[mLatIndex].toFloat(&ok);
                if (!ok) {
                    qWarning() << "Bad Lon: " << fields[mLonIndex];
                    throw CsvImporter::Exception(tr("Bad Lon field"));
                }

                if (mMapGraphicsModel != nullptr) {
                    mMapGraphicsModel->addGraphicsData (id, lat, lon);
                }
            }
        }

        ui->filePath->setText(mFilename);
    } catch (CsvImporter::Exception &x) {
        QMessageBox::warning(this, tr("Load failed"),
                             tr("Cannot load %1: %2").arg(mFilename).arg(x.what()));
        return;
    }

}

void CsvSpecsPage::save()
{
    if (mData == nullptr)
        return;

    try {
        CsvExporter ex;
        ex.setSeparator(mSeparator);
        ex.exportFile(mFilename, *mData);
    } catch (CsvImporter::Exception &x) {
        QMessageBox::warning(this, tr("Save failed"),
                             tr("Cannot save %1: %2").arg(mFilename).arg(x.what()));
        return;
    }

    QMessageBox::information(this, tr("Save Csv"),
                         tr("File successfully saved to %1").arg(mFilename));

}

void CsvSpecsPage::enableMap()
{
    ui->map->setVisible(true);
}

void CsvSpecsPage::setMapControlGraphicsModel(std::shared_ptr<MapControlGraphicsModel> model)
{
    mMapGraphicsModel = model;
}

void CsvSpecsPage::setupMapInitialDisplayConditions(const QPointF &center, int zoom)
{
    ui->map->addLayer(std::make_shared<qmapcontrol::LayerMapAdapter>("OpenStreetMap", std::make_shared<qmapcontrol::MapAdapterOSM>()));
    ui->map->addLayer(std::make_shared<qmapcontrol::LayerMapAdapter>("Seamark", std::make_shared<qmapcontrol::MapAdapterOpenSeaMap>()));

    ui->map->setMapFocusPoint(qmapcontrol::PointWorldCoord(center.x(), center.y()));
    ui->map->setZoom(zoom);
}

void CsvSpecsPage::setupIdLatLonCsvIndex(int id, int lat, int lon)
{
    mIdIndex = id;
    mLatIndex = lat;
    mLonIndex = lon;
}

qmapcontrol::QMapControl *CsvSpecsPage::getMapControlWidget() const
{
    return ui->map;
}

