/* --------------------------------------------------------------------------
 * DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
 * AND EFFORT DISPLACEMENT
 * Copyright (c) 2012, 2013, 2014 Francois Bastardie <fba@aqua.dtu.dk>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <outputfileparser.h>
#include <statscontroller.h>

#include <memoryinfo.h>

#include <QMainWindow>
#include <QProcess>
#include <QTimer>

#include <memory>
#include <QMapControl/Point.h>

#include <graphbuilder.h>
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

class ObjectTreeModel;
class DisplaceModel;
class Simulator;
class MapObjectsController;
class WaitDialog;
class MouseMode;

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

#define MAX_MODELS 5

class MainWindow : public QMainWindow, public MouseModeInterface
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
    void mapMousePress(QMouseEvent*,PointWorldCoord);
    void mapMouseRelease(QMouseEvent*,PointWorldCoord,PointWorldCoord);
    void mapMouseMove(QMouseEvent*,PointWorldCoord,PointWorldCoord);
    void showMessage(const QString &);
    void abortMouseMode ();
    void completeMouseMode();

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

    void on_action_Load_triggered();
    void on_modelSelector_currentIndexChanged(int index);
    void on_cmdStart_clicked();
    void on_cmdStop_clicked();
    void on_actionScenario_triggered();
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

signals:
    void modelStateChanged();

public:
    bool loadLiveModel(QString path, QString *error);

    void graphCreated(const QList<GraphBuilder::Node> &nodes);
    void addPenaltyPolygon (const QList<QPointF> &points);

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

    void startBackgroundOperation (BackgroundWorker *work, WaitDialog *waitdialog);
    void startBackgroundOperation (BackgroundWorkerWithWaitDialog *work);
    void startMouseMode (MouseMode *);
    void endMouseMode (bool success = true);

    bool isEditorModel();

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
