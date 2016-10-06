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
#include <GeometryPointShape.h>

#include <QSortFilterProxyModel>
#include <QMessageBox>

#include <QItemDelegate>
#include <QValidator>
#include <QLineEdit>

namespace {
class CoordDelegate : public QItemDelegate {
public:
    explicit CoordDelegate(QObject *parent = nullptr)
        : QItemDelegate(parent) {}

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        Q_UNUSED(option);
        Q_UNUSED(index);

        QLineEdit *editor = new QLineEdit(parent);
        editor->setValidator(new QDoubleValidator);
        return editor;
    }
    void setEditorData(QWidget *editor, const QModelIndex &index) const override
    {
        QString value =index.model()->data(index, Qt::EditRole).toString();
        QLineEdit *line = static_cast<QLineEdit*>(editor);
        line->setText(value);
    }
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override
    {
        QLineEdit *line = static_cast<QLineEdit*>(editor);
        QString value = line->text();
        model->setData(index, value);
    }
};
}

CsvSpecsPage::CsvSpecsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CsvSpecsPage),
    mFilename(), mSeparator(';')
{
    ui->setupUi(this);

    mModel = new CsvTableModel(std::make_shared<QList<QStringList>> ());
    mModel->setFirstLineHeaders(true);
    connect (mModel, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)), this, SLOT(dataChanged(QModelIndex,QModelIndex,QVector<int>)));

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);

#if QT_VERSION > 0x050500
    mVesselsSpecProxyModel = new QSortFilterProxyModel(this);

    mVesselsSpecProxyModel->setSourceModel(mModel);
    ui->tableView->setModel(mVesselsSpecProxyModel);
#else
    ui->tableView->setModel(mModel);
#endif

    connect (ui->tableView->selectionModel(), &QItemSelectionModel::currentRowChanged, [this](QModelIndex from,QModelIndex to) {
        Q_UNUSED(to);
        emit currentRowChanged(from.row());
        size_t row = from.row();     // Ids are one-based, rows are zero based.
        if (row < mData.size() && ui->map) {
            Data &dt = mData[row];
//            qDebug()  << "Move: " << row << dt.lat << dt.lon;
            ui->map->setMapFocusPoint(qmapcontrol::PointWorldCoord(dt.lon, dt.lat));
        }
    });

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
        auto datalist = std::make_shared<QList<QStringList>>(i.import(mFilename));
        mModel->setSource(datalist);

        if (mIdIndex != -1 && mLatIndex != -1 && mLonIndex != -1) {
            std::map<int, int> indexlist;


            for (int n = 0; n < datalist->size(); ++n) {
                if (n == 0) { // Skip headers
                    continue;
                }
                QStringList fields = datalist->at(n);
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
                float lon = fields[mLonIndex].toFloat(&ok);
                if (!ok) {
                    qWarning() << "Bad Lon: " << fields[mLonIndex];
                    throw CsvImporter::Exception(tr("Bad Lon field"));
                }

                int cnt = 0;
                auto indxlistit = indexlist.find(id);
                if (indxlistit != indexlist.end()) {
                    cnt = std::get<1>(*indxlistit)+1;
                } else {
                    indxlistit = std::get<0>(indexlist.insert(std::make_pair(id,0)));
                }

                size_t row = n-1;

                Data d;
                d.id = id;
                d.lat = lat;
                d.lon = lon;
                d.index = cnt;

                while (mData.size() <= row) {
                    mData.emplace_back(Data());
                }
                mData[row] = d;

                std::get<1>(*indxlistit) = cnt;

                if (mMapGraphicsModel != nullptr) {
                    mMapGraphicsModel->addGraphicsData (row, lat, lon);
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
    try {
        CsvExporter ex;
        ex.setSeparator(mSeparator);
        ex.exportFile(mFilename, mModel->rawData());
    } catch (CsvImporter::Exception &x) {
        QMessageBox::warning(this, tr("Save failed"),
                             tr("Cannot save %1: %2").arg(mFilename).arg(x.what()));
        return;
    }

    QMessageBox::information(this, tr("Save Csv"),
                         tr("File successfully saved to %1").arg(mFilename));

}

void CsvSpecsPage::setSaveEnabled(bool enabled)
{
    ui->saveCsv->setEnabled(enabled);
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

    ui->tableView->setItemDelegateForColumn(lat, new CoordDelegate);
    ui->tableView->setItemDelegateForColumn(lon, new CoordDelegate);
}

qmapcontrol::QMapControl *CsvSpecsPage::getMapControlWidget() const
{
    return ui->map;
}

void CsvSpecsPage::dataChanged(QModelIndex from, QModelIndex to, QVector<int> roles)
{
    if (!roles.empty() &&!roles.contains(Qt::DisplayRole))
        return;

    if (mIdIndex == -1 || mLatIndex == -1 || mLonIndex == -1)
        return;

    // Check if lat and lon are contained.
    for (int row = from.row(); row <= to.row(); ++row) {
        if ( (from.column() <= mLatIndex && mLatIndex <= to.column()) ||
             (from.column() <= mLonIndex && mLonIndex <= to.column()) ) {

            auto lat = mModel->data(mModel->index(row, mLatIndex), Qt::DisplayRole).toDouble();
            auto lon = mModel->data(mModel->index(row, mLonIndex), Qt::DisplayRole).toDouble();
            mData[row].lat = lat;
            mData[row].lon = lon;
//            qDebug()  << "Changed: " << row << lat << lon;
            mMapGraphicsModel->updateGraphicsData(row, lat, lon);
            ui->map->setMapFocusPoint(qmapcontrol::PointWorldCoord(lon, lat));
        }
    }
}

