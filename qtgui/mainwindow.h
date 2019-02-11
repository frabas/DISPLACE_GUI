// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2018 Francois Bastardie <fba@aqua.dtu.dk>

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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <outputfileparser.h>
#include <statscontroller.h>

#include <memoryinfo.h>

#include <QMainWindow>
#include <QProcess>
#include <QTimer>

#include <memory>
#include <functional>

#include <QMapControl/Point.h>

#include <graphbuilder_shp.h>
#include <backgroundworker.h>

#include <mousemode.h>

using namespace qmapcontrol;

namespace Ui {
class MainWindow;
}

namespace qmapcontrol {
class QMapControl;
class PointWorldCoord;
}

namespace displace {
namespace workers {
class DataMerger;
}
}

using namespace displace::workers;

class ObjectTreeModel;
class DisplaceModel;
class Simulator;
class MapObjectsController;
class WaitDialog;
class MouseMode;
class OGRGeometry;
struct GraphProperties;

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

class MainWindow : public QMainWindow, public MouseModeInterface
{
    Q_OBJECT

public:
    static constexpr int MAX_MODELS = 5;

private:
    static constexpr int ModelIdxEditor = MAX_MODELS-1;
    static constexpr int ModelIdxSimulator = 0;


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void simulatorLogging(QString);
    void simulationEnded(int exitcode);
    void simulatorProcessStateChanged (QProcess::ProcessState oldstate, QProcess::ProcessState newstate);
    void simulatorProcessStepChanged (int step);
    void simulatorNodeStatsUpdate(QString);
    void simulatorDebugMemoryStats(long,long);
    void simulatorCaptureLine(QString);

    void vesselMoved (int step, int idx, float x, float y, float course, float fuel, int state);
    void shipMoved (int step, int idx, float x, float y, float course);
    void updateModelState();
    void updateOutputFile (QString,int);
    void outputUpdated();
    void mapFocusPointChanged(PointWorldCoord);
    void mapMousePress(QMouseEvent*,PointWorldCoord);
    void mapMouseRelease(QMouseEvent*,PointWorldCoord,PointWorldCoord);
    void mapMouseMove(QMouseEvent*,PointWorldCoord,PointWorldCoord);
    void showMessage(const QString &);
    void abortMouseMode ();
    void completeMouseMode();
    void simulatorSqlStorageChanged(QString path);

    void showWarningMessageBox(QString,QString);
    void edgeSelectionsChanged(int);
    void treeViewContextMenuRequested(QPoint);

    void errorImportingStatsFile(QString);

    void playTimerTimeout();
    void memoryTimerTimeout();

    void waitStart();
    void waitEnd();

    void editorAddNode(QPointF);
    void editorAddEdge(int from, int to);

    void mergeCompleted(DataMerger *merger);

    void on_action_Load_triggered();
    void on_modelSelector_currentIndexChanged(int index);
    void on_cmdStart_clicked();
    void on_cmdStop_clicked();
    void on_actionScenario_triggered();
    void on_treeView_doubleClicked(const QModelIndex &index);
    void on_saveConsoleButton_clicked();
    void on_cmdSetup_clicked();
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
    void on_actionPalettes0to1_triggered();
    void on_popStatSelector_currentIndexChanged(int index);
    void on_nationsStatsSelector_currentIndexChanged(int index);
    void on_vesselsStatsSelector_currentIndexChanged(int index);
    void on_harbStatSelector_currentIndexChanged(int index);
    void on_actionPopulations_triggered();
    void on_actionTariffs_triggered();
    void on_actionBathymetry_triggered();
    void on_actionSalinity_triggered();
    void on_actionSST_triggered();
    void on_actionWind_triggered();
    void on_actionNitrogen_triggered();
    void on_actionPhosphorus_triggered();
    void on_actionOxygen_triggered();
    void on_actionDissolvedCarbon_triggered();
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
    void on_actionExport_Graph_triggered();
    void on_actionLoad_Harbours_triggered();
    void on_actionLink_Shortest_Path_Folder_triggered();
    void on_actionCreate_Shortest_Path_triggered();
    void on_actionAdd_Penalty_on_Polygon_triggered();
    void on_actionAdd_Penalty_from_File_triggered();
    void on_actionLoad_Graph_triggered();
    void on_actionSave_Graph_triggered();
    void on_actionLink_Harbours_to_Graph_triggered();
    void on_actionAdd_triggered();
    void on_actionAbort_Operation_triggered();
    void on_actionAbout_displace_triggered();
    void on_cmdProfileEnable_toggled(bool checked);
    void on_cmdProfileSave_clicked();
    void on_actionCSV_Editor_triggered();
    void on_actionMergeWeights_triggered();
    void on_actionMergePings_triggered();
    void on_actionExport_Map_triggered();
    void on_actionExport_Harbours_triggered();
    void on_actionExport_Populations_triggered();
    void on_actionExport_Nations_triggered();
    void on_actionAssign_Landscape_codes_triggered();
    void on_actionAssign_Wind_triggered();
    void on_actionAssign_SST_triggered();
    void on_actionAssign_Salinity_triggered();
    void on_actionAssign_Nitrogen_triggered();
    void on_actionAssign_Phosphorus_triggered();
    void on_actionAssign_Oxygen_triggered();
    void on_actionAssign_DissolvedCarbon_triggered();
    void on_actionAssign_Total_benthos_biomass_triggered();
    void on_actionAssign_Total_benthos_number_triggered();
    void on_actionAssign_Area_codes_triggered();
    void on_actionCalcPopDistribution_triggered();
    void on_actionLoadStockNames_triggered();
    void on_actionDecision_Trees_Editor_triggered();
    void on_actionCheck_for_isolated_subgraphs_triggered();
    void on_actionRemove_isolated_subgraphs_triggered();

    void on_actionR_Console_triggered();

    void on_actionTime_Series_Editor_triggered();
    void on_actionVessel_Creation_Editor_triggered();

    void on_actionScheduler_Editor_triggered();

    void on_actionExportAllGraphics_triggered();

    void on_metierStatSelector_currentIndexChanged(int index);

    void on_actionShortest_Path_to_Binary_triggered();

    void on_action_Record_Current_Map_Position_triggered();

    void on_actionClear_configuration_triggered();

    void on_actionSet_Node_Symbol_Size_triggered();

    void on_benthosStatSelector_currentIndexChanged(int index);

    void on_fishfarmsStatSelector_currentIndexChanged(int index);

    void on_windfarmsStatSelector_currentIndexChanged(int index);

    void on_shipsStatSelector_currentIndexChanged(int index);

public slots:
    void end_ShortestPathCreated(bool);

signals:
    void modelStateChanged();

public:
    bool loadLiveModel(QString path, QString *error, int model_idx = 0);

    void graphCreated(const QList<GraphBuilder::Node> &nodes);
    void addPenaltyPolygon (const QList<QPointF> &points);

protected:
    void updateModelList();
    void updateAllDisplayObjects();
    void updateCalendarDisplay(int tstep);
    void closeEvent ( QCloseEvent * event );

    void centerMap(const qmapcontrol::PointWorldCoord &pt);
    void centerMapOnHarbourId (int id);
    void centerMapOnNodeId (int id);
    void centerMapOnVesselId (int id);
    void centerMapOnFirmId (int id);
    void centerMapOnShipId (int id);
    void centerMapOnFishfarmId(int id);
    void centerMapOnWindmillId(int id);

    void showPaletteDialog(PaletteRole role);

    int newEditorModel(QString name);

    void startBackgroundOperation (BackgroundWorker *work, WaitDialog *waitdialog);
    void startBackgroundOperation (BackgroundWorkerWithWaitDialog *work);
    QProcess *prepareAppExecutableStart(QString exename);
    void startMouseMode (MouseMode *);
    void endMouseMode (bool success = true);

    bool isEditorModel();

    void openScenarioDialog(QString suggestedPath, bool askForReload, bool forceRename);
    void exportGraphics(QString label, QWidget *widget);
    void exportPlot (QString outpath, StatsController::StatType type, int subtype, const GraphProperties &properties);

    void assignCodesFromShapefileGen(QString title, QString shp, const char * const fieldname, std::function<void(OGRGeometry *, int)> func);
    void assignWindFromShapefileGen(QString title, QString shp, const char * const fieldname, std::function<void(OGRGeometry *, int)> func);
    void assignSSTFromShapefileGen(QString title, QString shp, const char * const fieldname, std::function<void(OGRGeometry *, int)> func);
    void assignSalinityFromShapefileGen(QString title, QString shp, const char * const fieldname, std::function<void(OGRGeometry *, int)> func);
    void assignNitrogenFromShapefileGen(QString title, QString shp, const char * const fieldname, std::function<void(OGRGeometry *, int)> func);
    void assignPhosphorusFromShapefileGen(QString title, QString shp, const char * const fieldname, std::function<void(OGRGeometry *, int)> func);
    void assignOxygenFromShapefileGen(QString title, QString shp, const char * const fieldname, std::function<void(OGRGeometry *, int)> func);
    void assignDissolvedCarbonFromShapefileGen(QString title, QString shp, const char * const fieldname, std::function<void(OGRGeometry *, int)> func);
    void assignBenthosBiomassFromShapefileGen(QString title, QString shp, const char * const fieldname, std::function<void(OGRGeometry *, int)> func);
    void assignBenthosNumberFromShapefileGen(QString title, QString shp, const char * const fieldname, std::function<void(OGRGeometry *, int)> func);


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
    QLabel *mMouseModeInfoLabel;
    QLabel *mCoordinatesInfoLabel;
    QLabel *mStatusInfoLabel;

    QString mLastRunSimulationName;
    QString mLastRunDatabase;

    /* Other objects */

    MouseMode *mMouseMode;
    WaitDialog *mWaitDialog;

    static const QString dbSuffix;
    static const QString dbFilter;
    static const QString dbLastDirKey;
    static const int maxModels;
    static const int playTimerDefault;
    static const int playTimerRates[];
};

#endif // MAINWINDOW_H
