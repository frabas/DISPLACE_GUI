#ifndef VESSELEDITORMAINWINDOW_H
#define VESSELEDITORMAINWINDOW_H

#include <QMainWindow>

#include <memory>

namespace Ui {
class VesselEditorMainWindow;
}

class CsvTableModel;

QT_BEGIN_NAMESPACE
class QSortFilterProxyModel;
class QPushButton;
QT_END_NAMESPACE

class MapShapefileListAdapter;

class VesselEditorMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit VesselEditorMainWindow(QWidget *parent = 0);
    ~VesselEditorMainWindow();

    static const QString VesselsSpecFilename;
    static const QString VesselsSpec2Filename;
    static const QString VesselsSpec3Filename;
    static const QString Pop1SpecFilename;
    static const QString Pop2SpecFilename;
    static const QString Pop3SpecFilename;
    static const QString ShippingFeaturesFilename;
    static const QString ShippingLanesCoordFilename;
    static const QString FishfarmsFeaturesFilename;
    static const QString WindmillsFeaturesFilename;
    static const QString FirmsFeaturesFilename;

private slots:
    void on_browseInputPath_clicked();
    void on_browseGISPath_clicked();
    void on_browseBasePath_clicked();
    void on_tabWidget_currentChanged(int index);

    void on_configRScript_clicked();

private:
    Ui::VesselEditorMainWindow *ui;
    MapShapefileListAdapter *mVesMapListAdapter;
    MapShapefileListAdapter *mPopMapListAdapter;
    MapShapefileListAdapter *mBenthosMapListAdapter;

    bool runScript(QString script);
    void fillRScriptsArgs(QStringList &args, const QString &script);

    void lightup (int id, bool on);
    void setNext (int id, int next);

    QVector<QPushButton *> mButtons;
};

#endif // VESSELEDITORMAINWINDOW_H
