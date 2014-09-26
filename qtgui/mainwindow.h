#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

#include <memory>

namespace Ui {
class MainWindow;
}

namespace qmapcontrol {
class QMapControl;
class PointWorldCoord;
}

class ObjectTreeModel;
class DisplaceModel;
class Simulator;
class MapObjectsController;

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

#define MAX_MODELS 2

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void simulatorLogging(QString);
    void simulatorProcessStateChanged (QProcess::ProcessState);
    void simulatorProcessStepChanged (int step);
    void vesselMoved (int idx, float x, float y, float course, float fuel, int state);
    void updateModelState();

    void on_action_Load_triggered();
    void on_modelSelector_currentIndexChanged(int index);
    void on_cmdStart_clicked();
    void on_cmdStop_clicked();
    void on_actionScenario_triggered();
    void on_actionSave_triggered();
    void on_treeView_doubleClicked(const QModelIndex &index);

    void on_saveConsoleButton_clicked();

signals:
    void modelStateChanged();

protected:
    void updateModelList();
    void closeEvent ( QCloseEvent * event );

    void centerMap(const qmapcontrol::PointWorldCoord &pt);
    void centerMapOnHarbourId (int id);
    void centerMapOnNodeId (int id);
    void centerMapOnVesselId (int id);

private:
    Ui::MainWindow *ui;

    // Data model
    DisplaceModel *models[MAX_MODELS];
    DisplaceModel *currentModel;
    int currentModelIdx;
    Simulator *mSimulation;
    MapObjectsController *mMapController;

    // Geospatial objects
    //std::shared_ptr<qmapcontrol::QMapControl> map;
    qmapcontrol::QMapControl *map;

    // tree model adapter
    ObjectTreeModel *treemodel;

    static const int maxModels;
};

#endif // MAINWINDOW_H
