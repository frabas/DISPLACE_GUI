#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <outputfileparser.h>
#include <statscontroller.h>

#include <utils/memoryinfo.h>

#include <QMainWindow>
#include <QProcess>
#include <QTimer>

#include <memory>
#include <QMapControl/Point.h>

using namespace qmapcontrol;

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

#define MAX_MODELS 5

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void simulatorLogging(QString);
    void simulatorProcessStateChanged (QProcess::ProcessState oldstate, QProcess::ProcessState newstate);
    void simulatorProcessStepChanged (int step);
    void simulatorNodeStatsUpdate(QString);

    void vesselMoved (int step, int idx, float x, float y, float course, float fuel, int state);
    void updateModelState();
    void updateOutputFile (QString,int);
    void outputUpdated();
    void mapFocusPointChanged(PointWorldCoord);

    void edgeSelectionsChanged(int);

    void errorImportingStatsFile(QString);

    void playTimerTimeout();
    void memoryTimerTimeout();

    void on_action_Load_triggered();
    void on_modelSelector_currentIndexChanged(int index);
    void on_cmdStart_clicked();
    void on_cmdStop_clicked();
    void on_actionScenario_triggered();
    void on_actionSave_triggered();
    void on_treeView_doubleClicked(const QModelIndex &index);
    void on_saveConsoleButton_clicked();
    void on_cmdSetup_clicked();
    void on_action_Link_database_triggered();
    void on_actionImport_results_triggered();
    void on_actionLoad_results_triggered();
    void on_play_step_valueChanged(int step);
    void on_play_fwd_clicked();
    void on_play_bk_clicked();
    void on_play_fbk_clicked();
    void on_play_ffwd_clicked();
    void on_play_first_clicked();
    void on_play_last_clicked();
    void on_play_auto_clicked();
    void on_actionPalettes_triggered();
    void on_popStatSelector_currentIndexChanged(int index);
    void on_nationsStatsSelector_currentIndexChanged(int index);
    void on_harbStatSelector_currentIndexChanged(int index);
    void on_actionPopulations_triggered();
    void on_actionConfiguration_triggered();
    void on_play_params_clicked();
    void on_actionQuit_triggered();
    void on_actionImport_Shapefile_triggered();
    void on_actionGraph_toggled(bool arg1);
    void on_actionEdge_Edit_toggled(bool arg1);
    void on_actionDelete_triggered();
    void on_actionNode_Editor_toggled(bool arg1);
    void on_actionClear_Graph_triggered();
    void on_actionCreate_Graph_triggered();

signals:
    void modelStateChanged();

protected:
    void updateModelList();
    void updateAllDisplayObjects();
    void closeEvent ( QCloseEvent * event );

    void centerMap(const qmapcontrol::PointWorldCoord &pt);
    void centerMapOnHarbourId (int id);
    void centerMapOnNodeId (int id);
    void centerMapOnVesselId (int id);

    void showPaletteDialog(PaletteRole role);

    int newEditorModel(QString name);

private:
    Ui::MainWindow *ui;

    // Data model
    std::shared_ptr<DisplaceModel> models[MAX_MODELS];
    std::shared_ptr<DisplaceModel> currentModel;
    int currentModelIdx;
    Simulator *mSimulation;
    MapObjectsController *mMapController;
    StatsController *mStatsController;

    // Geospatial objects
    qmapcontrol::QMapControl *map;

    // tree model adapter
    ObjectTreeModel *treemodel;

    QTimer mPlayTimer;
    int mPlayTimerInterval;
    QTimer mMemoryWatchTimer;
    MemoryInfo mMemInfo;
    QLabel *mMemInfoLabel;

    QString mLastRunSimulationName;
    QString mLastRunDatabase;

    static const QString dbSuffix;
    static const QString dbFilter;
    static const QString dbLastDirKey;
    static const int maxModels;
    static const int playTimerDefault;
    static const int playTimerRates[];
};

#endif // MAINWINDOW_H
