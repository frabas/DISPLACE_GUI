#ifndef CSVSPECSPAGE_H
#define CSVSPECSPAGE_H

#include <QWidget>
#include <memory>

namespace Ui {
class CsvSpecsPage;
}

class CsvTableModel;
class MapControlGraphicsModel;

QT_BEGIN_NAMESPACE
class QSortFilterProxyModel;
class QProcess;
QT_END_NAMESPACE

namespace qmapcontrol { class QMapControl; }

class CsvSpecsPage : public QWidget
{
    Q_OBJECT

public:
    explicit CsvSpecsPage(QWidget *parent = 0);
    ~CsvSpecsPage();

    void load();
    void save();

    void setFilename(QString n) {
        mFilename = n;
    }
    void setSeparator(QChar sep) {
        mSeparator = sep;
    }

    void enableMap();
    void setMapControlGraphicsModel(std::shared_ptr<MapControlGraphicsModel> model);
    void setupMapInitialDisplayConditions(const QPointF &center, int zoom);
    void setupIdLatLonCsvIndex(int id, int lat, int lon);

   qmapcontrol::QMapControl *getMapControlWidget() const;
private:
    Ui::CsvSpecsPage *ui;

    QString mFilename;
    QChar mSeparator;

    std::shared_ptr<MapControlGraphicsModel> mMapGraphicsModel;
    std::shared_ptr<QList<QStringList>> mData;
    CsvTableModel *mModel;
    QSortFilterProxyModel *mVesselsSpecProxyModel;

    int mIdIndex = -1;
    int mLatIndex = -1, mLonIndex = -1;
};

#endif // CSVSPECSPAGE_H
