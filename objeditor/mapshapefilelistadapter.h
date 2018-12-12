#ifndef MAPLISTADAPTER_H
#define MAPLISTADAPTER_H

#include <QAbstractListModel>
#include <vector>

#include <memory>

namespace qmapcontrol { class QMapControl; class LayerESRIShapefile; }
class GDALDataset;

class MapShapefileListAdapter : public QAbstractListModel
{
public:
    explicit MapShapefileListAdapter(qmapcontrol::QMapControl * control);

    void clearPaths();
    void addPath (const QString &path);
    void refresh();

    // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

private:
    qmapcontrol::QMapControl *mMapController;
    QStringList mPaths;     ///< The list of paths from which we must load the file names

    class MapEntry {
    private:
        QString mPath;
        QString mName;
        qmapcontrol::QMapControl *mController;
        GDALDataset *mDataSource;
        std::shared_ptr<qmapcontrol::LayerESRIShapefile> mLayer;

    public:
        explicit MapEntry(const QString &path, qmapcontrol::QMapControl *controller, QString name);
        ~MapEntry();

        MapEntry(const MapEntry &) = delete;
        MapEntry &operator =(const MapEntry &) = delete;
        MapEntry(MapEntry &&) = default;
        MapEntry &operator =(MapEntry &&) = default;

        QString getName() const;

        bool isVisible() const;
        void setVisible(bool visible);
    };

    std::vector<MapEntry> mEntries;
};

#endif // MAPLISTADAPTER_H
