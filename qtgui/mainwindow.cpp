#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "displacemodel.h"
#include <mapobjectscontroller.h>
#include <objecttreemodel.h>
#include <objects/harbourentity.h>
#include <objects/nodeentity.h>
#include <objects/vesselentity.h>
#include <simulator.h>
#include <editpalettedialog.h>

#include <inputfileexporter.h>
#include <inputfileparser.h>
#include <scenariodialog.h>
#include <configdialog.h>
#include <simulationsetupdialog.h>
#include <creategraphdialog.h>
#include <aboutdialog.h>
#include <createshortestpathdialog.h>
#include <csveditor.h>
#include <mergedatadialog.h>
#include <mergepopulationdatadialog.h>
#include <workers/populationdistributiondatamergerstrategy.h>
#include <workers/mergerstrategy.h>
#include <utils/imageformathelpers.h>
#include <utils/mrupathmanager.h>
#include <utils/displaceexception.h>

#include <workers/shortestpathbuilderworker.h>
#include <workers/datamerger.h>

#include <mousemode.h>
#include <mousemode/drawpenaltypolygon.h>
#include <mousemode/movefilteringmousemodedecorator.h>
#include <mousemode/singleclickmousemode.h>
#include <mousemode/edgeaddmousemode.h>

#include <graphinteractioncontroller.h>
#include <graphbuilder.h>

#include <waitdialog.h>

#include <backgroundworker.h>
#include <shortestpathbuilder.h>
#include <pathpenaltydialog.h>
#include <linkharboursdialog.h>
#include <shapefileoperationdialog.h>
#include <savegraphdialog.h>

#include <QMapControl/QMapControl.h>
#include <QMapControl/ImageManager.h>

#include <gdal/ogrsf_frmts.h>
#include <GeographicLib/Geodesic.hpp>
#include <version.h>    // Version.h should be included after GeographicLib because it undefines VERSION symbol

#include <QBoxLayout>
#include <QTextEdit>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QInputDialog>
#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>

#include <functional>

const int MainWindow::maxModels = MAX_MODELS;
const QString MainWindow::dbSuffix = ".db";
const QString MainWindow::dbFilter = QT_TR_NOOP("Displace Database files (*.db);;All files (*.*)") ;
const QString MainWindow::dbLastDirKey = "db_lastdir";
const int MainWindow::playTimerDefault = 20;

const int MainWindow::playTimerRates[] = {
    50, 40, 25, 20, 15, 10, 5, 2, 1
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    models(),
    currentModel(0),
    currentModelIdx(-1),
    mSimulation(0),
    mMapController(0),
    map(0),
    treemodel(0),
    mPlayTimerInterval(playTimerDefault),
    mMouseMode(0),
    mWaitDialog(0)
{
    ui->setupUi(this);

    QActionGroup *grp = new QActionGroup(this);
    grp->addAction(ui->actionNode_Editor);
    grp->addAction(ui->actionEdge_Edit);

    mMouseModeInfoLabel = new QLabel(this);
    statusBar()->addPermanentWidget(mMouseModeInfoLabel);
    mMouseModeInfoLabel->hide();

    mMemInfoLabel = new QLabel(this);
    statusBar()->addPermanentWidget(mMemInfoLabel);

    mCoordinatesInfoLabel = new QLabel(this);
    statusBar()->addWidget(mCoordinatesInfoLabel, 1);

    mStatusInfoLabel = new QLabel(this);
    statusBar()->addWidget(mStatusInfoLabel, 3);

    ui->menuWindows->addAction(ui->statsView->toggleViewAction());
    ui->menuWindows->addAction(ui->objectMapView->toggleViewAction());
    ui->menuWindows->addAction(ui->dockWidget->toggleViewAction());
    ui->menuWindows->addAction(ui->consoleView->toggleViewAction());

    QSettings set;
    restoreGeometry(set.value("mainGeometry").toByteArray());
    restoreState(set.value("mainState").toByteArray());

    for (int i = 0; i < maxModels; ++i) {
        models[i] = 0;
    }

    connect (this, SIGNAL(modelStateChanged()), this, SLOT(updateModelState()));
    connect (&mPlayTimer, SIGNAL(timeout()), this, SLOT(playTimerTimeout()));
    connect (&mMemoryWatchTimer, SIGNAL(timeout()), this, SLOT(memoryTimerTimeout()));

    mMemoryWatchTimer.start(2500);

    mSimulation = new Simulator();
    mSimulation->setVerbosityLevel(set.value(Simulator::SET_VERBOSITY, 0).toInt());
    connect (mSimulation, SIGNAL(log(QString)), this, SLOT(simulatorLogging(QString)));
    connect (mSimulation, SIGNAL(processStateChanged(QProcess::ProcessState,QProcess::ProcessState)), this, SLOT(simulatorProcessStateChanged(QProcess::ProcessState,QProcess::ProcessState)));
    connect (mSimulation, SIGNAL(simulationStepChanged(int)), this, SLOT(simulatorProcessStepChanged(int)));

    connect (mSimulation, SIGNAL(vesselMoved(int,int,float,float,float,float,int)),
             this, SLOT(vesselMoved(int,int,float,float,float,float,int)));
    connect (mSimulation, SIGNAL(nodesStatsUpdate(QString)), this, SLOT(simulatorNodeStatsUpdate(QString)));
    connect (mSimulation, SIGNAL(outputFileUpdated(QString,int)), this, SLOT(updateOutputFile(QString,int)));
    connect (mSimulation, SIGNAL(debugMemoryStats(long,long)), this, SLOT(simulatorDebugMemoryStats(long,long)));
    connect (mSimulation, SIGNAL(debugCapture(QString)), this, SLOT(simulatorCaptureLine(QString)));

    ui->cmdProfileEnable->setChecked(false);
    ui->profilingOutput->setVisible(false);

    /* Setup graph controller */
    new GraphInteractionController(ui->plotHarbours, this);
    new GraphInteractionController(ui->plotPopulations, this);
    new GraphInteractionController(ui->plotNations, this);

    simulatorProcessStateChanged(QProcess::NotRunning, QProcess::NotRunning);

    map = new qmapcontrol::QMapControl(ui->mapWidget);
    mMapController = new MapObjectsController(map);
    connect (mMapController, SIGNAL(edgeSelectionChanged(int)), this, SLOT(edgeSelectionsChanged(int)));
    connect (mMapController, SIGNAL(nodeSelectionChanged(int)), this, SLOT(edgeSelectionsChanged(int)));

    connect (map, SIGNAL(mapFocusPointChanged(PointWorldCoord)), this, SLOT(mapFocusPointChanged(PointWorldCoord)));
    connect (map, SIGNAL(mouseEventPressCoordinate(QMouseEvent*,PointWorldCoord)), this, SLOT(mapMousePress(QMouseEvent*,PointWorldCoord)));
    connect (map, SIGNAL(mouseEventReleaseCoordinate(QMouseEvent*,PointWorldCoord,PointWorldCoord)), this, SLOT(mapMouseRelease(QMouseEvent*,PointWorldCoord,PointWorldCoord)));
    connect (map, SIGNAL(mouseEventMoveCoordinate(QMouseEvent*,PointWorldCoord,PointWorldCoord)), this, SLOT(mapMouseMove(QMouseEvent*,PointWorldCoord,PointWorldCoord)));

    map->setBackgroundColour(Qt::white);

    QPixmap pixmap;
    pixmap.fill( Qt::white );
    qmapcontrol::ImageManager::get().setLoadingPixmap(pixmap);

    QHBoxLayout *layout = new QHBoxLayout;
    ui->mapWidget->setLayout(layout);
    layout->addWidget(map);

    ui->mapWidget->setWidget(map);

    /* Stats windows setup */

    mStatsController = new StatsController(this);
    mStatsController->setPopulationPlot(ui->plotPopulations);
    mStatsController->setHarboursPlot(ui->plotHarbours);
    mStatsController->setNationsPlot(ui->plotNations);

    /* Tree model setup */
    treemodel = new ObjectTreeModel(mMapController, mStatsController);
    ui->treeView->setModel(treemodel);
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect (ui->treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(treeViewContextMenuRequested(QPoint)));

    ui->actionGraph->setChecked(false);
    on_actionGraph_toggled(false);  /* Force action function execution */

    int idx = newEditorModel("new model");
    ui->modelSelector->setCurrentIndex(idx);
    updateModelList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

class Loader : public BackgroundWorker {
    QString mDir;
public:
    Loader(MainWindow *main, QString dir)
        : BackgroundWorker(main), mDir(dir) {
    }

    virtual void execute() override {
        qDebug() << "Loader started";
        QString error;
        if (!mMain->loadLiveModel(mDir, &error)) {
            setFail(QString(tr("Error loading model %1: %2")).arg(mDir).arg(error));
            emit warning(tr("Load failed."), getError());
            return;
        }

    }
};


void MainWindow::on_action_Load_triggered()
{
    QSettings sets;
    QString lastpath;

    lastpath = sets.value("lastpath", QDir::homePath()).toString();

    QString dir  = QFileDialog::getOpenFileName(this,
                                                tr("Select a scenario file - look at simusspe_* directory"),
                                                lastpath, QString("*.dat"));
    if (dir.isEmpty())
        return;

    QDir d (dir);
    sets.setValue("lastpath", d.absolutePath());

    Loader *loader = new Loader(this,dir);

    startBackgroundOperation(loader, 0);
}

void MainWindow::on_modelSelector_currentIndexChanged(int index)
{
    if (currentModelIdx != -1) {
        mMapController->setModelVisibility(currentModelIdx, MapObjectsController::Invisible);
    }

    int currentStep = currentModel ? currentModel->getCurrentStep() : 0;
    currentModelIdx = ui->modelSelector->itemData(index).toInt();
    if (currentModelIdx >= 0)
        currentModel = models[currentModelIdx];
    else
        currentModel = 0;

    treemodel->setCurrentModel(currentModelIdx, currentModel.get());

    mMapController->setModelVisibility(currentModelIdx, MapObjectsController::Visible);
    mStatsController->updateStats(currentModel.get());

    DisplaceModel::ModelType type = currentModel == 0 ? DisplaceModel::EmptyModelType : currentModel->modelType() ;

    bool e = (type == DisplaceModel::OfflineModelType);
    ui->play_bk->setEnabled(e);
    ui->play_fbk->setEnabled(e);
    ui->play_ffwd->setEnabled(e);
    ui->play_first->setEnabled(e);
    ui->play_fwd->setEnabled(e);
    ui->play_last->setEnabled(e);
    ui->play_step->setEnabled(e);
    ui->play_auto->setEnabled(e);
    if (!e || currentModel == 0) {
        ui->play_step->setValue(0);
    } else {
        if (type == DisplaceModel::OfflineModelType) {
            int last = currentModel->getLastStep();
            ui->play_step->setValue(currentModel->getCurrentStep());
            ui->play_step->setMinimum(0);
            ui->play_step->setMaximum(last);

            ui->play_step->setSuffix(QString(tr("/%1")).arg(last));
        } else if (type == DisplaceModel::EditorModelType) {
            ui->play_step->setValue(0);
            ui->play_step->setMinimum(0);
            ui->play_step->setMaximum(0);
        }
    }

    ui->play_step->setValue(currentStep);

    /* Editor specific tools */
    e = type == DisplaceModel::EditorModelType;
    ui->actionLoad_Graph->setEnabled(e);
    ui->actionSave_Graph->setEnabled(e);
    ui->actionExport_Graph->setEnabled(e);

    ui->actionAdd->setEnabled(e);
    ui->actionClear_Graph->setEnabled(e);
    ui->actionDelete->setEnabled(e);
    ui->actionEdge_Edit->setEnabled(e);
    ui->actionGraph->setEnabled(e);
    ui->actionNode_Editor->setEnabled(e);
    ui->actionProperties->setEnabled(e);
    ui->actionAbort_Operation->setEnabled(false);
}

void MainWindow::simulatorLogging(QString msg)
{
    ui->console->appendPlainText(msg);
}

void MainWindow::simulatorProcessStateChanged(QProcess::ProcessState oldstate, QProcess::ProcessState newstate)
{
    if (models[0] != 0) {
        ui->cmdStart->setEnabled(newstate == QProcess::NotRunning);
        ui->cmdStop->setEnabled(newstate == QProcess::Running);
        ui->cmdSetup->setEnabled(newstate == QProcess::NotRunning);

        if (newstate != QProcess::Running)
            simulatorProcessStepChanged(-1);

        if (oldstate == QProcess::Running && newstate == QProcess::NotRunning) { // simulation has completed
            models[0]->simulationEnded();
        }
    } else {
        ui->cmdStart->setEnabled(false);
        ui->cmdStop->setEnabled(false);
        ui->cmdSetup->setEnabled(false);
        simulatorProcessStepChanged(-1);
    }
}

void MainWindow::simulatorProcessStepChanged(int step)
{
    if (step != -1) {
        ui->info_simstep->setText(QString(tr("Simulation step: %1")).arg(step));

        models[0]->commitNodesStatsFromSimu(step);
    } else {
        ui->info_simstep->setText(QString(tr("Simulation step:")));
    }
}

void MainWindow::simulatorNodeStatsUpdate(QString data)
{
    models[0]->updateNodesStatFromSimu(data);
}

void MainWindow::simulatorDebugMemoryStats(long rss, long peak)
{
    mStatusInfoLabel->setText(QString("Simulator Memory RSS: %1Mb peak %2Mb").arg(rss/1024).arg(peak/1024));
}

void MainWindow::simulatorCaptureLine(QString line)
{
    ui->profilingOutput->appendPlainText(line);
}

void MainWindow::vesselMoved(int step, int idx, float x, float y, float course, float fuel, int state)
{
    models[0]->updateVessel (step, idx, x, y, course, fuel, state);
    mMapController->updateVesselPosition(0, idx);
}

void MainWindow::updateModelState()
{
    simulatorProcessStateChanged(mSimulation->processState(),mSimulation->processState());
    updateModelList();
}

void MainWindow::updateOutputFile(QString path, int n)
{
    models[0]->parseOutputStatsFile(path.trimmed(), n);
}

void MainWindow::outputUpdated()
{
    mMapController->updateNodes(0);
    mStatsController->updateStats(models[0].get());
}

void MainWindow::mapFocusPointChanged(qmapcontrol::PointWorldCoord pos)
{
    mCoordinatesInfoLabel->setText(QString("Pos: %1 %2").arg(pos.latitude(),5).arg(pos.longitude(),5));
}

void MainWindow::mapMousePress(QMouseEvent *event, PointWorldCoord point)
{
    Q_UNUSED(event);
    if (!mMouseMode)    // no mouse mode active
        return;

    if (!mMouseMode->pressEvent(point.rawPoint()))
        abortMouseMode();
}

void MainWindow::mapMouseRelease(QMouseEvent *, PointWorldCoord, PointWorldCoord point)
{
    if (!mMouseMode)    // no mouse mode active
        return;

    if (!mMouseMode->releaseEvent(point.rawPoint()))
        abortMouseMode();
}

void MainWindow::mapMouseMove(QMouseEvent *, PointWorldCoord, PointWorldCoord point)
{
    if (!mMouseMode)    // no mouse mode active
        return;

    if (!mMouseMode->moveEvent(point.rawPoint()))
        abortMouseMode();
}

void MainWindow::showMessage(const QString &message)
{
    mStatusInfoLabel->setText(message);
}

void MainWindow::edgeSelectionsChanged(int num)
{
    ui->actionDelete->setEnabled(num > 0);
    ui->actionProperties->setEnabled(num > 0);
}

void MainWindow::treeViewContextMenuRequested(QPoint point)
{
    QModelIndex index = ui->treeView->indexAt(point);
    if (index.isValid()) {
        objecttree::ObjectTreeEntity *entity = treemodel->entity(index);
        QMenu *menu = entity->contextMenu();
        if (menu)
            menu->exec(ui->treeView->mapToGlobal(point));
    }
}

void MainWindow::errorImportingStatsFile(QString msg)
{
    QMessageBox::warning(this, tr("Error importing stats file"),
                         msg);
}

void MainWindow::playTimerTimeout()
{
    if (currentModel->getCurrentStep() < currentModel->getLastStep())
        on_play_fwd_clicked();
    else
        on_play_auto_clicked();
}

void MainWindow::memoryTimerTimeout()
{
    mMemInfo.update();
    mMemInfoLabel->setText(QString(tr("Used memory: %1Mb Peak: %2Mb")).arg(mMemInfo.rss()/1024).arg(mMemInfo.peakRss()/1024));
}

void MainWindow::waitStart()
{
    if (!mWaitDialog) {
        mWaitDialog = new WaitDialog(this);
    }

    mWaitDialog->show();
}

void MainWindow::waitEnd()
{
    qDebug()  << "Wait End";

    if (mWaitDialog) {
        mWaitDialog->close();
        delete mWaitDialog;
        mWaitDialog = 0;
    }
}

void MainWindow::editorAddNode(QPointF point)
{
    if (!isEditorModel()) {
        abortMouseMode();
        return;
    }

    GraphBuilder::Node newnode;
    newnode.point = point;
    newnode.good = true;
    newnode.harbour = 0;
    QList<GraphBuilder::Node> nl;
    nl << newnode;
    currentModel->addGraph(nl, mMapController);

    completeMouseMode();
}

void MainWindow::editorAddEdge(int from, int to)
{
    if (!isEditorModel()) {
        abortMouseMode();
        return;
    }

    double d;

#if GEOGRAPHICLIB_VERSION_MINOR > 25
    const GeographicLib::Geodesic& geod = GeographicLib::Geodesic::WGS84();
#else
    const GeographicLib::Geodesic& geod = GeographicLib::Geodesic::WGS84;
#endif

    std::shared_ptr<NodeData> fn = currentModel->getNodesList()[from];
    std::shared_ptr<NodeData> tn = currentModel->getNodesList()[to];

    geod.Inverse(fn->get_y(), fn->get_x(), tn->get_y(), tn->get_x(), d);

    qDebug() << "EDGE" << from << to << d;

    int id1 = currentModel->addEdge(from, to, d / 1000.0);
    int id2 = currentModel->addEdge(to, from, d / 1000.0);

    mMapController->addEdge(currentModelIdx, currentModel->getNodesList()[from]->getAdiacencyByIdx(id1), true);
    mMapController->addEdge(currentModelIdx, currentModel->getNodesList()[to]->getAdiacencyByIdx(id2), true);

    completeMouseMode();
}

void MainWindow::updateModelList()
{
    int n = ui->modelSelector->currentData().toInt();
    ui->modelSelector->clear();

    int sel = -1;
    for (int i = 0; i < MAX_MODELS; ++i) {
        if (models[i] != 0) {
            ui->modelSelector->addItem(
                        QString(tr("[%1] %2 %3 %4")).arg(i).arg(models[i]->inputName())
                        .arg(models[i]->outputName()).arg(models[i]->simulationName()),
                        i);
            if (i == n)
                sel = i;
        } else {
            ui->modelSelector->addItem(
                        QString(tr("[%1]")).arg(i),
                        i);
        }
    }

    ui->modelSelector->setCurrentIndex(sel);
}

void MainWindow::updateAllDisplayObjects()
{
    mMapController->updateMapObjectsFromModel(currentModelIdx, currentModel.get());
    mStatsController->updateStats(currentModel.get());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (mSimulation->isRunning()) {
        int res = QMessageBox::question(this, tr("Simulation running"),
                              tr("A simulation process is running. Closing the app will force close it. Do you want to proceed?"),
                              QMessageBox::No, QMessageBox::Yes);

        if (res == QMessageBox::Yes) {
            mSimulation->forceStop();
            event->accept();
        } else {
            event->ignore();
        }
    } else {
        event->accept();
    }

    if (event->isAccepted()) {
        QSettings sets;
        sets.setValue("mainGeometry", saveGeometry());
        sets.setValue("mainState", saveState());

        mMapController->signalAppIsClosing();
        mMapController->removeAllWidgets();
        qApp->closeAllWindows();
    }
}

void MainWindow::centerMap(const qmapcontrol::PointWorldCoord &pt)
{
    map->setMapFocusPointAnimated(pt, 5, std::chrono::milliseconds(100));
}

void MainWindow::centerMapOnHarbourId(int id)
{
    std::shared_ptr<HarbourData> h (currentModel->getHarboursList()[id]);
    centerMap(qmapcontrol::PointWorldCoord(h->mHarbour->get_x(), h->mHarbour->get_y()));
}

void MainWindow::centerMapOnNodeId(int id)
{
    std::shared_ptr<NodeData> h (currentModel->getNodesList()[id]);
    centerMap(qmapcontrol::PointWorldCoord(h->get_x(), h->get_y()));
}

void MainWindow::centerMapOnVesselId(int id)
{
    std::shared_ptr<VesselData> h(currentModel->getVesselList()[id]);
    centerMap(qmapcontrol::PointWorldCoord(h->mVessel->get_x(), h->mVessel->get_y()));
}

void MainWindow::on_cmdStart_clicked()
{
    if (!mSimulation->isRunning() && models[0] != 0) {
        if ((mLastRunDatabase == models[0]->linkedDatabase() || mLastRunSimulationName == models[0]->simulationName()) && mSimulation->wasSimulationStarted()) {
            int res = QMessageBox::information(this, tr("Restart simulation"),
                                               tr("Restarting simulation will eventually overwrite the results data, either in a linked db or in the output files. Are you sure to continue?"),
                                               QMessageBox::Yes, QMessageBox::No);
            if (res == QMessageBox::No)
                return;
        }

        ui->profilingOutput->clear();
        mLastRunSimulationName = models[0]->simulationName();
        mLastRunDatabase = models[0]->linkedDatabase();
        models[0]->prepareDatabaseForSimulation();
        models[0]->clearStats();
        mSimulation->setSimSteps(models[0]->getSimulationSteps());
        mSimulation->setUseStaticPaths(models[0]->getUseStaticPaths());
        mSimulation->setPreexistingPathsShop(models[0]->getPreexistingPathsShop());
        mSimulation->setOutputName(models[0]->outputName());
        mSimulation->start(models[0]->inputName(), models[0]->basepath(), models[0]->simulationName());
    }
}

void MainWindow::on_cmdStop_clicked()
{
    if (mSimulation && mSimulation->isRunning()) {
        mSimulation->forceStop();
    }
}

void MainWindow::on_actionScenario_triggered()
{
    if (!currentModel || currentModel->modelType() == DisplaceModel::OfflineModelType)
        return;

    bool askForReload = (currentModel->modelType() == DisplaceModel::LiveModelType);
    openScenarioDialog(currentModel->fullpath(), askForReload, false);
}

void MainWindow::openScenarioDialog(QString suggestedPath, bool askForReload, bool forceRename)
{
    if (currentModel) {
        Scenario d = currentModel->scenario();
        ScenarioDialog dlg (d, this);
        dlg.setScenarioPath(suggestedPath);
        if (forceRename)
            dlg.setForceRename();
        if (dlg.exec() == QDialog::Accepted) {
            int r = QMessageBox::question(this, tr("Saving scenario"),
                                          QString(tr("The scenario file must be saved%1. Proceed?"))
                                            .arg(askForReload ? " and the model reloaded" : ""),
                                          QMessageBox::No, QMessageBox::Yes);
            if (r == QMessageBox::Yes) {
                currentModel->setScenario(dlg.getScenario());
                bool ok;
                if (dlg.isRenamed()) {
                    ok = currentModel->saveScenarioAs(dlg.getScenarioPath());
                } else {
                    ok = currentModel->saveScenario();
                }

                if (ok) {
                    QMessageBox::information(this, tr("Model saved"),
                                             QString(tr("The model %1 has been saved successfully.")).arg(currentModel->inputName()));
                } else {
                    QMessageBox::warning(this, tr("Save failed"),
                                         QString(tr("There was an error saving the model %1: %2"))
                                         .arg(dlg.getScenarioPath())
                                         .arg(currentModel->getLastError()));
                    return;
                }

                if (askForReload) {
                    Loader *loader = new Loader(this,currentModel->fullpath());
                    startBackgroundOperation(loader,0);
                }
            }
        }
    }
}

void MainWindow::on_actionConfiguration_triggered()
{
    if (!currentModel || currentModel->modelType() != DisplaceModel::LiveModelType)
        return;

    if (currentModel) {
        Config c = currentModel->config();
        ConfigDialog dlg (currentModel.get(), this);
        dlg.set(c);
        if (dlg.exec() == QDialog::Accepted) {
            if (!dlg.get(c)) {
                QMessageBox::warning(this, tr("Configuration failed"),
                                     tr("An error occurred while parsing configuration. Please check carefully the entered data."));
                return;
            }

            currentModel->setConfig(c);
            if (!currentModel->saveConfig()) {
                QMessageBox::warning(this, tr("Save failed"),
                                     QString(tr("There was an error saving the Config file for model %1: %2"))
                                             .arg(currentModel->fullpath())
                                             .arg(currentModel->getLastError())
                                     );
                return;
            }
        }
    }
}

#if 0
void MainWindow::on_actionSave_triggered()
{
    if (models[0] && models[0]->save()) {
        QMessageBox::information(this, tr("Model saved"),
                                 QString(tr("The model %1 has been saved successfully.")).arg(models[0]->inputName()));
        return;
    } else {
        QMessageBox::warning(this, tr("Load failed"),
                             tr("There was an error saving the model."));
        return;
    }
}
#endif

void MainWindow::on_treeView_doubleClicked(const QModelIndex &index)
{
    if (!treemodel->isObject(index))
        return;

    ObjectTreeModel::Category cat = treemodel->getCategory(index);
    switch (cat) {

    case ObjectTreeModel::Harbours:
        centerMapOnHarbourId((reinterpret_cast<objecttree::HarbourEntity *>(treemodel->entity(index)))->getHarbourId());
        break;

    case ObjectTreeModel::Nodes:
        centerMapOnNodeId((reinterpret_cast<objecttree::NodeEntity *>(treemodel->entity(index)))->getNodeId());
        break;

    case ObjectTreeModel::Vessels:
        centerMapOnVesselId((reinterpret_cast<objecttree::VesselEntity *>(treemodel->entity(index)))->getVesselId());
        break;

    default:    // nothing to do
        return;
    }
}

void MainWindow::on_saveConsoleButton_clicked()
{
    QString path = QFileDialog::getSaveFileName(this,
                                                tr("Save Console output"),
                                                QString(), QString(tr("Text files (*.txt);;All files (*.*)")));

    if (!path.isEmpty()) {
        QFile file (path);
        if (!file.open(QFile::WriteOnly)) {
            QMessageBox::warning(this, tr("Save failed"),
                                 QString(tr("Cannot save file: %1")).arg(file.errorString()));
            return;
        }

        QTextStream strm(&file);
        strm << ui->console->toPlainText();
        file.close();
    }
}

void MainWindow::on_cmdSetup_clicked()
{
    QSettings set;

    SimulationSetupDialog dlg(this);

    dlg.setSimulationSteps(models[0]->getSimulationSteps());
    dlg.setUseStaticPaths(models[0]->getUseStaticPaths());
    dlg.setPreexistingPathsShop(models[0]->getPreexistingPathsShop());
    dlg.setSimulationName(models[0]->simulationName());
    dlg.setSimulationOutputName(models[0]->outputName());
    dlg.setMoveVesselsOption(mSimulation->getMoveVesselOption());
    dlg.setNumThreads(set.value(Simulator::SET_NUMTHREADS, 4).toInt());
    dlg.setVerbosityLevel(set.value(Simulator::SET_VERBOSITY, 0).toInt());

    if (dlg.exec() == QDialog::Accepted) {
        models[0]->setSimulationSteps(dlg.getSimulationSteps());
        models[0]->setUseStaticPaths(dlg.getUseStaticPaths());
        models[0]->setPreexistingPathsShop(dlg.getPreexistingPathsShop());
        models[0]->setSimulationName(dlg.getSimulationName());
//        models[0]->setOutputName(dlg.getSimulationOutputName());
        mSimulation->setMoveVesselOption(dlg.getMoveVesselsOption());

        set.setValue(Simulator::SET_NUMTHREADS, dlg.getNumThreads());
        set.setValue(Simulator::SET_VERBOSITY, dlg.getVerbosityLevel());
        if (mSimulation)
            mSimulation->setVerbosityLevel(dlg.getVerbosityLevel());

        updateModelList();
    }
}

void MainWindow::on_action_Link_database_triggered()
{
    if (models[0] == 0) {
        QMessageBox::warning(this, tr("Link database"), tr("Please load a simulation before linking a database."));
        return;
    }

    if (mSimulation->isRunning()) {
        QMessageBox::warning(this, tr("Link database"), tr("Cannot link database while a simulation is running."));
        return;
    }

    QSettings sets;
    QString dbname =  QFileDialog::getSaveFileName(this, tr("Link database"),
                                         sets.value(dbLastDirKey).toString(), dbFilter,0);

    if (!dbname.isEmpty()) {
        QFileInfo info (dbname);
        if (info.suffix().isEmpty()) {
            dbname += dbSuffix;
            info = QFileInfo(dbname);
        }

        if (info.exists()) {
            QFile f(dbname);
            f.remove();
        }

        if (!models[0]->linkDatabase(dbname)) {
            QMessageBox::warning(this, tr("Link database failed"),
                                 QString(tr("Cannot link database file %1: %2"))
                                 .arg(dbname).arg(models[0]->getLastError()));
            return;
        }

        sets.setValue(dbLastDirKey, info.absolutePath());
    }
}

void MainWindow::on_actionImport_results_triggered()
{
    if (currentModelIdx == 0 || currentModel == 0)
        return;

    QSettings sets;
    QString name =  QFileDialog::getOpenFileName(this, tr("Import data result file"),
                                         sets.value("import_last").toString());

    if (!name.isEmpty()) {
        QFileInfo info (name);

        currentModel->parseOutputStatsFile(name, -1);

        sets.setValue("import_last", info.absolutePath());
    }
}

void MainWindow::on_actionLoad_results_triggered()
{
    QSettings sets;
    QString dbname =  QFileDialog::getOpenFileName(this, tr("Load Results file"),
                                         sets.value(dbLastDirKey).toString(), dbFilter);

    if (!dbname.isEmpty()) {
        int i;
        for (i = 1; i < MAX_MODELS; ++i) {
            if (models[i] == 0)
                break;
        }

        if (i == MAX_MODELS) {
            QMessageBox::warning(this, tr("Load database failed"),
                                 tr("We cannot load more database."));
            return;
        }
        QFileInfo info (dbname);
        if (info.suffix().isEmpty()) {
            dbname += dbSuffix;
            info = QFileInfo(dbname);
        }

        std::shared_ptr<DisplaceModel> newmodel(new DisplaceModel());

        if (!newmodel->loadDatabase(dbname)) {
            QMessageBox::warning(this, tr("Database Load failed"),
                                 QString(tr("Cannot load this database.")));
            return;
        }

        sets.setValue(dbLastDirKey, info.absolutePath());

        newmodel->setIndex(i);
        models[i] = newmodel;

        mMapController->setModel(i, newmodel);
        mMapController->createMapObjectsFromModel(i,models[i].get());
        ui->modelSelector->setCurrentIndex(i);

        emit modelStateChanged();
    }

}

int MainWindow::newEditorModel(QString name)
{
    int i = MAX_MODELS-1;
    std::shared_ptr<DisplaceModel> edmodel = std::shared_ptr<DisplaceModel>(new DisplaceModel());
    edmodel->edit(name);
    edmodel->setIndex(i);
    models[i] = edmodel;

    mMapController->setModel(i, edmodel);
    mMapController->createMapObjectsFromModel(i,models[i].get());
    ui->modelSelector->setCurrentIndex(i);

    emit modelStateChanged();

    return i;
}

void MainWindow::startBackgroundOperation(BackgroundWorker *work, WaitDialog *waitdialog, QObject *receiver, const char *onEndSlot)
{
    QThread *thread = new QThread(this);

    if (mWaitDialog) {
        mWaitDialog->close();
        delete mWaitDialog;
    }

    if (waitdialog == 0) {
        mWaitDialog = new WaitDialog(this);
    } else {
        mWaitDialog = waitdialog;
    }

    work->moveToThread(thread);
    connect (thread, SIGNAL(started()), work, SLOT(process()));
    connect (work, SIGNAL(workStarted()), this, SLOT(waitStart()));
    connect (work, SIGNAL(workEnded()), this, SLOT(waitEnd()));
    connect (work, SIGNAL(progress(int)), mWaitDialog, SLOT(setProgression(int)));
    connect (work, SIGNAL(warning(QString,QString)), this, SLOT(showWarningMessageBox(QString,QString)));
    if (onEndSlot)
        connect (work, SIGNAL(completed(bool)), receiver, onEndSlot);

    thread->start();
}

void MainWindow::startBackgroundOperation(BackgroundWorkerWithWaitDialog *work)
{
    startBackgroundOperation(work, work->getWaitDialog());
}

void MainWindow::startMouseMode(MouseMode * newmode)
{
    abortMouseMode();
    mMouseMode = newmode;

    if (mMouseMode) {
        mMouseMode->setMouseModeInterface(this);
        mMouseModeInfoLabel->show();
        mMouseModeInfoLabel->setText(mMouseMode->getModeDescription());

        mMouseMode->beginMode();

        ui->actionAbort_Operation->setEnabled(true);
    }
}

void MainWindow::endMouseMode(bool success)
{
    mMouseModeInfoLabel->hide();
    mStatusInfoLabel->setText("");
    if (!mMouseMode)
        return;

    if (success) {
        mMouseMode->endMode(success);
        treemodel->refresh();
    }

    ui->actionAbort_Operation->setEnabled(false);

    delete mMouseMode;
    mMouseMode = 0;
}

bool MainWindow::isEditorModel()
{
    return (currentModel && currentModel->modelType() == DisplaceModel::EditorModelType);
}

void MainWindow::abortMouseMode()
{
    endMouseMode(false);
}

void MainWindow::completeMouseMode()
{
    endMouseMode(true);
}

void MainWindow::showWarningMessageBox(QString title, QString message)
{
    QMessageBox::warning(this, title, message);
}

void MainWindow::on_play_step_valueChanged(int step)
{
    if (currentModel && currentModel->modelType() == DisplaceModel::OfflineModelType) {
        currentModel->setCurrentStep(step);
        mStatsController->setCurrentTimeStep(step);
        updateAllDisplayObjects();
    }
}

void MainWindow::on_play_fwd_clicked()
{
    ui->play_step->setValue(currentModel->getCurrentStep() + 1);
}

void MainWindow::on_play_bk_clicked()
{
    ui->play_step->setValue(currentModel->getCurrentStep() - 1);
}

void MainWindow::on_play_fbk_clicked()
{
     ui->play_step->setValue(currentModel->getCurrentStep() - 50);
}

void MainWindow::on_play_ffwd_clicked()
{
     ui->play_step->setValue(currentModel->getCurrentStep() + 50);
}

void MainWindow::on_play_first_clicked()
{
     ui->play_step->setValue(0);
}

void MainWindow::on_play_last_clicked()
{
     ui->play_step->setValue(currentModel->getLastStep());
}

void MainWindow::on_play_auto_clicked()
{
    bool en = mPlayTimer.isActive();
    if (en) {
        mPlayTimer.stop();
        ui->play_auto->setIcon(QIcon(":/icons/start.png"));
    } else {
        mPlayTimer.setInterval(mPlayTimerInterval);
        mPlayTimer.setSingleShot(false);
        mPlayTimer.start();
        ui->play_auto->setIcon(QIcon(":/icons/pause.png"));
    }

    //ui->play_auto->setIcon();
    ui->play_bk->setEnabled(en);
    ui->play_fbk->setEnabled(en);
    ui->play_ffwd->setEnabled(en);
    ui->play_first->setEnabled(en);
    ui->play_fwd->setEnabled(en);
    ui->play_last->setEnabled(en);
    ui->play_step->setEnabled(en);
}

void MainWindow::on_actionPalettes_triggered()
{
    showPaletteDialog(ValueRole);
}

void MainWindow::on_actionPopulations_triggered()
{
    showPaletteDialog(PopulationRole);
}

void MainWindow::showPaletteDialog (PaletteRole role)
{
    EditPaletteDialog dlg(this);
    Palette pal = mMapController->getPalette(currentModelIdx, role);
    dlg.linkPalette(&pal);
    dlg.showSpecials(false);
    if (dlg.exec() == QDialog::Accepted) {
        mMapController->setPalette(currentModelIdx, role, pal);

        mMapController->forceRedraw();
    }
}

void MainWindow::on_popStatSelector_currentIndexChanged(int index)
{
    mStatsController->setPopulationStat((StatsController::PopulationStat)index);
}

void MainWindow::on_nationsStatsSelector_currentIndexChanged(int index)
{
    mStatsController->setNationsStat((StatsController::NationsStat)index);
}

void MainWindow::on_harbStatSelector_currentIndexChanged(int index)
{
    mStatsController->setHarbourStat((StatsController::HarboursStat)index);
}

void MainWindow::on_play_params_clicked()
{
    bool ok;
    QStringList rates;
    for (size_t i = 0; i < sizeof(playTimerRates) / sizeof(playTimerRates[0]); ++i) {
        rates << QString::number(playTimerRates[i]);
    }
    int current = rates.size();
    rates << QString::number(1000 / mPlayTimerInterval);

    QString value = QInputDialog::getItem(this, tr("Autoplay frame rate"), tr("Frame rate, in fps"), rates, current, true, &ok);
    if (ok) {
        int n = value.toInt(&ok);
        if (ok) {
            mPlayTimerInterval = 1000 / n;
            mPlayTimer.setInterval(mPlayTimerInterval);
        } else {
            QMessageBox::warning(this, tr("Invalid value"), tr("The value selected is not valid"));
            return;
        }
    }
}

void MainWindow::on_actionQuit_triggered()
{
    close();
}

void MainWindow::on_actionImport_Shapefile_triggered()
{
    if (currentModel == 0) {
        QMessageBox::information(this, tr("Importing shape file"), tr("Please load a simulation or database before importing a shapefile"));
        return;
    }


    QSettings sets;
    QString name =  QFileDialog::getOpenFileName(this, tr("Import shapefile"),
                                         sets.value("import_shape").toString());

    if (!name.isEmpty()) {
        QFileInfo info (name);

        OGRDataSource *ds = OGRSFDriverRegistrar::Open(name.toStdString().c_str(), FALSE);

        QString layer;

        if (ds->GetLayerCount() > 1) {
            QStringList items;
            for(int i = 0; i < ds->GetLayerCount(); ++i)
                items << ds->GetLayer(i)->GetName();

            layer = QInputDialog::getItem(this, tr("Shapefile open"), tr("Please select the a layer, or cancel for all layers"), items,0, false);
        }

        mMapController->importShapefile(currentModelIdx, name, layer);
        sets.setValue("import_shape", info.absolutePath());

        treemodel->refresh();
    }

}

void MainWindow::on_actionGraph_toggled(bool en)
{
    /* Enable/Disable editor commands */

    ui->actionClear_Graph->setEnabled(en);
    ui->actionEdge_Edit->setEnabled(en);
    ui->actionNode_Editor->setEnabled(en);

    /* -- */
    ui->actionAdd->setEnabled(en);
    ui->actionDelete->setEnabled(en);
    ui->actionProperties->setEnabled(en);

    if (!en) {
        mMapController->setEditorMode(MapObjectsController::NoEditorMode);
    }
}

void MainWindow::on_actionEdge_Edit_toggled(bool en)
{
    if (en) {
        map->setMouseButtonRight(QMapControl::MouseButtonMode::SelectLine, false);
        mMapController->setEditorMode(MapObjectsController::EdgeEditorMode);
    }
}

void MainWindow::on_actionNode_Editor_toggled(bool en)
{
    if (en) {
        map->setMouseButtonRight(QMapControl::MouseButtonMode::SelectBox, false);
        mMapController->setEditorMode(MapObjectsController::NodeEditorMode);
    }
}

void MainWindow::on_actionDelete_triggered()
{
    mMapController->delSelected(currentModelIdx);
}

void MainWindow::on_actionClear_Graph_triggered()
{
    if (!currentModel || currentModel->modelType() != DisplaceModel::EditorModelType)
        return;

    if (QMessageBox::warning(this, tr("Clear graph"), tr("This operation will permanently remove the all the nodes from graph. Proceed?"),
                             QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes) {
        currentModel->clearAllNodes();
        mMapController->clearAllNodes(currentModelIdx);
        mMapController->redraw();
    }
}

class GraphBuilderWorker : public BackgroundWorker, public GraphBuilder::Feedback {
    GraphBuilder *builder;
    WaitDialog *waitDialog;
    QList<GraphBuilder::Node> result;
public:
    GraphBuilderWorker (MainWindow *win, GraphBuilder *b, WaitDialog *dlg)
        : BackgroundWorker(win),
          builder(b),
          waitDialog(dlg) {
    }

    virtual void execute() override {
        builder->setFeedback(this);
        result = builder->buildGraph();
        mMain->graphCreated(result);
    }

    void setMax (int m) {
        waitDialog->setProgress(true, m);
    }

    void setStep(int step) {
        emit progress(step);
    }
};

void MainWindow::on_actionCreate_Graph_triggered()
{
    if (!currentModel || currentModel->modelType() != DisplaceModel::EditorModelType)
        return;

    CreateGraphDialog dlg(this);
    QStringList list;
    list << QString();
    list << mMapController->getShapefilesList(currentModelIdx);
    dlg.setShapefileList(list);

    if (dlg.exec() == QDialog::Accepted) {
        GraphBuilder *gb = new GraphBuilder();
        gb->setType(dlg.getType());
        gb->setDistance(dlg.step() * 1000);
        gb->setLimits(dlg.minLon(), dlg.maxLon(), dlg.minLat(), dlg.maxLat());

        QString s = dlg.getSelectedShapefile();
        if (!s.isEmpty())
            gb->setShapefile(mMapController->cloneShapefileDatasource(currentModelIdx, s));

        WaitDialog *wdlg = new WaitDialog(this);
        wdlg->setText(tr("Wait while graph is created..."));
        wdlg->setProgress(false, 100);

        GraphBuilderWorker *wrkr = new GraphBuilderWorker(this, gb, wdlg);
        startBackgroundOperation(wrkr, wdlg);
    }
}

void MainWindow::graphCreated(const QList<GraphBuilder::Node> &nodes)
{
    currentModel->addGraph (nodes, mMapController);
}

void MainWindow::addPenaltyPolygon(const QList<QPointF> &points)
{
    PathPenaltyDialog dlg(this);
    dlg.showShapefileOptions(false);

    if (dlg.exec() == QDialog::Accepted) {
        currentModel->addPenaltyToNodesByAddWeight(points, dlg.weight(), dlg.isClosedForFishing(),
                                        dlg.isPenaltyQ1(), dlg.isPenaltyQ2(), dlg.isPenaltyQ3(), dlg.isPenaltyQ4());
        mMapController->redraw();
        QMessageBox::warning(this, tr("Penalties applied"),
                             tr("Graph weights are changed, you'll need to recreate the shortest path."));
    }
}

bool MainWindow::loadLiveModel(QString path, QString *error)
{
    std::shared_ptr<DisplaceModel> m(new DisplaceModel());

    if (!m->load(path)) {
        if (error)
            *error = m->getLastError();
        return false;
    }

    /* Connect model */
    connect (m.get(), SIGNAL(errorParsingStatsFile(QString)), this, SLOT(errorImportingStatsFile(QString)));
    connect (m.get(), SIGNAL(outputParsed()), this, SLOT(outputUpdated()));

    mMapController->removeModel(0);

    m->setIndex(0);
    mMapController->setModel(0, m);
    mMapController->createMapObjectsFromModel(0, m.get());
    ui->modelSelector->setCurrentIndex(0);
    models[0] = m;

    mSimulation->linkModel(models[0]);

    emit modelStateChanged();

    return true;
}

void MainWindow::on_actionExport_Graph_triggered()
{
    if (!currentModel || currentModel->modelType() != DisplaceModel::EditorModelType)
        return;

    QSettings sets;
    QString lastpath;

    lastpath = sets.value("last_export", QDir::homePath()).toString();

    QString fn = QFileDialog::getSaveFileName(this, tr("Export Graph"), lastpath, tr("Graph Files (*.dat)"));
    if (!fn.isEmpty()) {
        if (currentModel->exportGraph(fn)) {
            QFileInfo info(fn);
            sets.setValue("last_export", info.absolutePath());
        } else {
            QMessageBox::warning(this, tr("Export failed"), QString(tr("Graph export has failed: %1")).arg(currentModel->getLastError()));
        }
    }
}

void MainWindow::on_actionLoad_Harbours_triggered()
{
    if (!currentModel || currentModel->modelType() != DisplaceModel::EditorModelType)
        return;

    QSettings sets;
    QString lastpath;

    lastpath = sets.value("last_harb", QDir::homePath()).toString();

    QString fn = QFileDialog::getOpenFileName(this, tr("Import Harbours file"), lastpath, tr("Harbour Files (*.dat)"));
    if (!fn.isEmpty()) {
        QList<std::shared_ptr<HarbourData> > list;
        QString error;
        InputFileParser parser;
        if (parser.parseHarbourFile(fn, list, &error)) {
            currentModel->importHarbours(list);
            foreach (std::shared_ptr<HarbourData> h, list)
                mMapController->addHarbour(currentModelIdx, h, true);

            mMapController->redraw();
            QFileInfo info(fn);
            sets.setValue("last_harb", info.absolutePath());
        } else {
            QMessageBox::warning(this, tr("Export failed"), QString(tr("Graph export has failed: %1")).arg(error));
        }
    }

}

void MainWindow::on_actionLink_Shortest_Path_Folder_triggered()
{
    if (!currentModel || currentModel->modelType() != DisplaceModel::EditorModelType)
        return;

    QSettings sets;
    QString lastpath;

    lastpath = sets.value("last_spath", QDir::homePath()).toString();

    QString fn = QFileDialog::getExistingDirectory(this, tr("Link shortest path folder"), lastpath);
    if (!fn.isEmpty()) {
        currentModel->linkShortestPathFolder(fn);
        sets.setValue("last_spath", fn);
    }

}

void MainWindow::on_actionCreate_Shortest_Path_triggered()
{
    if (!currentModel || currentModel->modelType() != DisplaceModel::EditorModelType)
        return;

    CreateShortestPathDialog dlg(this);
    dlg.setShortestPathFolder(currentModel->linkedShortestPathFolder());
    dlg.setOutputFolder(currentModel->linkedGraphFolder());
    dlg.setGraphName(QString::number(currentModel->scenario().getGraph()));
    if (dlg.exec() != QDialog::Accepted)
        return;

    currentModel->linkShortestPathFolder(dlg.getShortestPathFolder());
    currentModel->linkGraphFolder(dlg.getOutputFolder());
    Scenario sce = currentModel->scenario();
    sce.setGraph(dlg.getGraphName().toInt());
    currentModel->setScenario(sce);

    WaitDialog *dialog = new WaitDialog(this);
    displace::workers::ShortestPathBuilderWorker *builder = new displace::workers::ShortestPathBuilderWorker(this, dialog, currentModel.get());

    SaveGraphDialog savedlg(this);
    savedlg.setName(QString::number(sce.getGraph()));
    savedlg.setOutputFolder(dlg.getOutputFolder());

    if (savedlg.exec() == QDialog::Accepted) {
        QString graphpath = savedlg.getGraphFilename();
        QString coordspath = savedlg.getCoordsFilename();
        QString landpath = savedlg.getLandscapeFilename();
        QString acpath = savedlg.getAreacodesFilename();
        QString polypath = savedlg.getClosedPolygonFilename();

        QString error;
        InputFileExporter exporter;
        if (exporter.exportGraph(graphpath, coordspath, landpath, acpath, polypath, currentModel.get(), &error)) {
        } else {
            QMessageBox::warning(this, tr("Error Saving greph/coords file"), error);
            return;
        }
    }

    if (dlg.isAllNodesAreRelevantChecked()) {
        builder->setRelevantNodes(currentModel->getNodesList());
    } else {
        InputFileParser parser;
        QString p1, p2;
        if (!parser.pathParseRelevantNodes(dlg.getRelevantNodesFolder(), p1, p2)) {
            QMessageBox::warning(this, tr("Cannot parse selected file name."),
                                 tr("Cannot parse the selected file name into relevant nodes pattern. it must be: /.../vesselsspe_xxx_quartery.dat"));
            return;
        }

        int i = 1;
        bool ok;
        QSet<int> nodes;
        do {
            QString in= p1.arg(i++);
            qDebug() << "Parsing file: " << in;
            ok = parser.parseRelevantNodes(in, nodes);
        } while (ok);
        i = 1;
        do {
            QString in = p2.arg(i++);
            qDebug() << "Parsing file: " << in;
            ok = parser.parseRelevantNodes(in, nodes);
        } while (ok);

        qDebug() << "nodes :" << nodes.size();

        QList<std::shared_ptr<NodeData> >l;
        foreach (int i, nodes) {
            l.push_back(currentModel->getNodesList()[i]);
        }
        builder->setRelevantNodes(l);
    }

    builder->run(this,SLOT(end_ShortestPathCreated(bool)) );
}

void MainWindow::end_ShortestPathCreated(bool completed)
{
    if (completed) {
        QString outname = currentModel->outputName();
        QString path = currentModel->fullpath();
        path = path.replace(outname + ".dat", outname + "_XX.dat");
        currentModel->setOutputName(outname + "_XX");
        openScenarioDialog(path, false, true);
    } else {
        QMessageBox::warning(this, tr("Shortest path creation failed."), tr("Process was not completed."));
    }
}


void MainWindow::on_actionAdd_Penalty_on_Polygon_triggered()
{
    startMouseMode(new DrawPenaltyPolygon(this, mMapController));
}

void MainWindow::on_actionAdd_Penalty_from_File_triggered()
{
    if (!currentModel || currentModel->modelType() != DisplaceModel::EditorModelType)
        return;

    PathPenaltyDialog dlg(this);
    dlg.setShapefileList(mMapController->getShapefilesList(currentModelIdx));
    dlg.showShapefileOptions(true);

    if (dlg.exec() == QDialog::Accepted) {
        double weight = dlg.weight();
        QString shp = dlg.selectedShapefile();
        std::shared_ptr<OGRDataSource> ds = mMapController->cloneShapefileDatasource(currentModelIdx, shp);

        int n = ds->GetLayerCount();
        for (int i = 0; i < n ;  ++i) {
            OGRLayer *lr = ds->GetLayer(i);
            lr->SetSpatialFilter(0);
            lr->ResetReading();

            OGRFeature *feature;
            while ((feature = lr->GetNextFeature())) {
                currentModel->addPenaltyToNodesByAddWeight(feature->GetGeometryRef(), weight, dlg.isClosedForFishing(),
                                                            dlg.isPenaltyQ1(), dlg.isPenaltyQ2(), dlg.isPenaltyQ3(), dlg.isPenaltyQ4());
            }
        }

        mMapController->redraw();

        QMessageBox::warning(this, tr("Penalties applied"), tr("Graph weights are changed, you'll need to recreate the shortest path."));
    }
}

void MainWindow::assignCodesFromShapefileGen (QString title, QString shp, const char *const fieldname, std::function<void(OGRGeometry*,int)> func)
{
    std::shared_ptr<OGRDataSource> ds = mMapController->cloneShapefileDatasource(currentModelIdx, shp);
    if (ds.get() == nullptr) {
        // not opened. get a new

        ds = std::shared_ptr<OGRDataSource>(OGRSFDriverRegistrar::Open(shp.toStdString().c_str(), FALSE));
    }

    if (ds.get() == nullptr) {
        QMessageBox::warning(this, tr("Failed opening file"),
                             tr("Cannot open/get the selected shapefile. The file may be not readable."));
        return;
    }

    int nftr = 0;
    int n_nofield = 0;
    int n = ds->GetLayerCount();
    for (int i = 0; i < n ;  ++i) {
        OGRLayer *lr = ds->GetLayer(i);
        lr->SetSpatialFilter(0);
        lr->ResetReading();

        OGRFeature *feature;
        while ((feature = lr->GetNextFeature())) {
            int fld = feature->GetFieldIndex(fieldname);

            if (fld != -1) {
                int code = feature->GetFieldAsInteger(fld);
                func(feature->GetGeometryRef(), code);
            } else {
                ++n_nofield;
            }

            ++nftr;
        }
    }

    mMapController->redraw();

    if (n_nofield > 0) {
        QMessageBox::warning(this, title,
                             QString("%1 features in the shapefile didn't contain the proper field named '%2'.")
                             .arg(n_nofield).arg(fieldname));
    } else {
        QMessageBox::information(this, title,
                                 QString("%1 features were correctly processed.")
                                 .arg(nftr));
    }

}

void MainWindow::on_actionAssign_Landscape_codes_triggered()
{
    QString title = tr("Set Landscape codes ('hab_code' field required)");

    if (!currentModel || currentModel->modelType() != DisplaceModel::EditorModelType)
        return;

    ShapefileOperationDialog dlg(this);
    dlg.setWindowTitle(title);
    dlg.setShapefileList(mMapController->getShapefilesList(currentModelIdx));

    if (dlg.exec() == QDialog::Accepted) {
        const char * fieldname = "hab_code";
        QString shp = dlg.selectedShapefile();

        assignCodesFromShapefileGen(title, shp, fieldname, [&](OGRGeometry *geom, int code) {
            currentModel->setLandscapeCodesFromFeature(geom, code); } );
    }
}

void MainWindow::on_actionAssign_Area_codes_triggered()
{
    QString title = tr("Set Area codes ('area_code' field required)");

    if (!currentModel || currentModel->modelType() != DisplaceModel::EditorModelType)
        return;

    ShapefileOperationDialog dlg(this);
    dlg.setWindowTitle(title);
    dlg.setShapefileList(mMapController->getShapefilesList(currentModelIdx));

    if (dlg.exec() == QDialog::Accepted) {
        const char * fieldname = "area_code";
        QString shp = dlg.selectedShapefile();

        assignCodesFromShapefileGen(title,shp, fieldname, [&](OGRGeometry *geom, int code) {
            currentModel->setAreaCodesFromFeature(geom, code); } );
    }
}

void MainWindow::on_actionLoad_Graph_triggered()
{
    if (!currentModel || currentModel->modelType() != DisplaceModel::EditorModelType)
        return;

    QSettings sets;
    QString lastpath = sets.value("last_graphpath", QDir::homePath()).toString();

    QString fn = QFileDialog::getOpenFileName(this, tr("Import Graph file"), lastpath);
    if (!fn.isEmpty()) {
        QString graphpath, coordspath;

        QFileInfo info(fn);
        QString fnn = info.fileName();

        if (fnn.startsWith("graph")) {
            graphpath = fn;
            coordspath = info.absolutePath() + "/coord" + fnn.mid(5);

            int res = QMessageBox::question(this, tr("Coordinates file"),
                                      QString(tr("Do you want also to load %1 as a coordinates file?")).arg(coordspath),
                                      QMessageBox::Yes, QMessageBox::No, QMessageBox::Open);
            if (res == QMessageBox::Open) {
                coordspath = QFileDialog::getOpenFileName(this, tr("Import Coords file"), coordspath);
            } else if (res == QMessageBox::No) {
                coordspath = QString();
            }
        } else if (fnn.startsWith("coord")) {
            coordspath = fn;
            graphpath = info.absolutePath() + "/graph" + fnn.mid(5);

            int res = QMessageBox::question(this, tr("Graph file"),
                                      QString(tr("Do you want also to load %1 as a graph file?")).arg(graphpath),
                                      QMessageBox::Yes, QMessageBox::No, QMessageBox::Open);
            if (res == QMessageBox::Open) {
                graphpath = QFileDialog::getOpenFileName(this, tr("Import Graph file"), coordspath);
            } else if (res == QMessageBox::No) {
                graphpath = QString();
            }
        } else {
            QMessageBox::warning(this, tr("Cannot load file"), tr("Selected file must start either with graph or with coords."));
            return;
        }

        InputFileParser parser;
        QList<GraphBuilder::Node> nodes;
        QString error;
        if (parser.parseGraph(graphpath, coordspath, nodes, &error)) {
            qDebug()  << nodes.size() << "Nodes loaded.";

            currentModel->addGraph(nodes, mMapController);
//            currentModel->importGraph (graphpath, coordspath);
        } else {
            QMessageBox::warning(this, tr("Error loading greph/coords file"), error);
            return;
        }

        sets.setValue("last_graphpath", fn);
        treemodel->refresh();
    }

}

void MainWindow::on_actionSave_Graph_triggered()
{
    if (!currentModel || currentModel->modelType() != DisplaceModel::EditorModelType)
        return;

    SaveGraphDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {

        QString graphpath = dlg.getGraphFilename();
        QString coordspath = dlg.getCoordsFilename();
        QString landpath = dlg.getLandscapeFilename();
        QString acpath = dlg.getAreacodesFilename();
        QString polypath = dlg.getClosedPolygonFilename();

        QString error;
        InputFileExporter exporter;
        if (exporter.exportGraph(graphpath, coordspath, landpath, acpath, polypath, currentModel.get(), &error)) {
        } else {
            QMessageBox::warning(this, tr("Error Saving greph/coords file"), error);
            return;
        }
    }

}

struct sorter {
    double weight;
    std::shared_ptr<NodeData> node;

    sorter(std::shared_ptr<NodeData> _node, double _weight) {
        weight = _weight;
        node = _node;
    }

    friend bool operator < (const sorter &s1, const sorter &s2) {
        return s1.weight < s2.weight;
    }
};

void MainWindow::on_actionLink_Harbours_to_Graph_triggered()
{
    if (!currentModel || currentModel->modelType() != DisplaceModel::EditorModelType)
        return;

    if (currentModel->getHarboursCount() == currentModel->getNodesCount()) {
        QMessageBox::warning(this, tr("Cannot link harbours and ndoes"),
                             tr("All nodes are habours in this model. Cannot proceed."));
        return;
    }

    LinkHarboursDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        if (dlg.isRemoveLinksSet()) {
            foreach (std::shared_ptr<HarbourData> harbour, currentModel->getHarbourList()) {
                currentModel->getNodesList()[harbour->mHarbour->get_idx_node()]->removeAllAdiacencies();
            }
        }

#if GEOGRAPHICLIB_VERSION_MINOR > 25
        const GeographicLib::Geodesic& geod = GeographicLib::Geodesic::WGS84();
#else
        const GeographicLib::Geodesic& geod = GeographicLib::Geodesic::WGS84;
#endif

        foreach (std::shared_ptr<HarbourData> harbour, currentModel->getHarbourList()) {
            int harbid = harbour->mHarbour->get_idx_node();
            QPointF pos(harbour->mHarbour->get_x(), harbour->mHarbour->get_y());

            double distance = dlg.getMaxDinstance();

            QList<std::shared_ptr<NodeData> > nodes;
            QList<sorter> snodes;
            do {
                nodes = currentModel->getAllNodesWithin(pos, distance);
                distance *= 2.0;

                if (nodes.size() > 1) {
                    double dist;
                    foreach (std::shared_ptr<NodeData> node, nodes) {
                        if (node->get_idx_node() != harbid) {
                            if (dlg.isAvoidHHLinks() && node->mNode->get_is_harbour())
                                continue;
                            geod.Inverse(harbour->mHarbour->get_y(), harbour->mHarbour->get_x(), node->get_y(), node->get_x(), dist);
                            snodes.push_back(sorter(node, dist));
                        }
                    }

                    qSort(snodes);

                    int n = dlg.getMaxLinks();
                    if (n == -1)
                        n = snodes.count();
                    else
                        n = min(snodes.count(), dlg.getMaxLinks());
                    for (int i = 0; i < n; ++i) {
                        int nodeid = snodes[i].node->get_idx_node();
                        int he_id = currentModel->addEdge(harbid, nodeid, snodes[i].weight / 1000.0);
                        int te_id = currentModel->addEdge(nodeid, harbid, snodes[i].weight / 1000.0);
                        mMapController->addEdge(currentModelIdx, currentModel->getNodesList()[harbid]->getAdiacencyByIdx(he_id), true);
                        mMapController->addEdge(currentModelIdx, currentModel->getNodesList()[nodeid]->getAdiacencyByIdx(te_id), true);
                    }
                }
            } while (snodes.size() == 0 && dlg.isAvoidLonelyHarboursSet());
        }
    }
}

void MainWindow::on_actionAdd_triggered()
{
    if (!isEditorModel())
        return;

    switch (mMapController->getEditorMode()) {
    case MapObjectsController::NoEditorMode:
        break;
    case MapObjectsController::NodeEditorMode:
        if (true) {
            SingleClickMouseMode *mode = new SingleClickMouseMode(tr("Add Graph Node Mode"));
            connect (mode, SIGNAL(modeCompleted(QPointF)), this, SLOT(editorAddNode(QPointF)));
            startMouseMode(new MoveFilteringMouseModeDecorator(mode));
        }
        break;
    case MapObjectsController::EdgeEditorMode:
        if (true) {
            EdgeAddMouseMode *mode = new EdgeAddMouseMode(currentModel.get());
            connect (mode, SIGNAL(edgeAdded(int,int)), this, SLOT(editorAddEdge(int,int)));
            startMouseMode(new MoveFilteringMouseModeDecorator(mode));
        }
        break;
    }
}

void MainWindow::on_actionAbort_Operation_triggered()
{
    abortMouseMode();
}

void MainWindow::on_actionAbout_displace_triggered()
{
    AboutDialog dlg(this);
    dlg.exec();
}

void MainWindow::on_cmdProfileEnable_toggled(bool checked)
{
    ui->profilingOutput->setVisible(checked);
}

void MainWindow::on_cmdProfileSave_clicked()
{
    QSettings set;
    QString defpos = set.value("report_path", QDir::homePath()).toString();
    QString path = QFileDialog::getSaveFileName(this, tr("Append report to file"), defpos, tr("Text files (*.txt);;All files (*.*)"));
    if (!path.isEmpty()) {
        QFile f(path);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Append)) {
            QMessageBox::warning(this, tr("Save failed"), QString("Cannot save to %1: %2").arg(path).arg(f.errorString()));
            return;
        }

        QTextStream strm(&f);
        strm << endl << endl << "--------" << endl;
        strm << QDateTime::currentDateTime().toLocalTime().toString() << " Version " << VERSION << endl;
        strm << models[0]->inputName() << " " << models[0]->outputName() << " " << models[0]->simulationName() << endl;
        strm << models[0]->getSimulationSteps() << " total steps" << endl;
        strm << "Linked database: " << models[0]->linkedDatabase() << endl;
#ifdef DEBUG
        strm << "Debug version" << endl;
#else
        strm << "Release version" << endl;
#endif
        strm << endl;
        strm << ui->profilingOutput->toPlainText();
        strm << endl;

        f.close();

        QFileInfo info(path);
        set.setValue("report_path", info.absolutePath());
    }
}

void MainWindow::on_actionCSV_Editor_triggered()
{
    CsvEditor *editor = new CsvEditor();
    connect (editor, SIGNAL(destroyed()), editor, SLOT(deleteLater()));

    editor->show();
}

void MainWindow::on_actionMergeWeights_triggered()
{
    if (!currentModel || currentModel->modelType() != DisplaceModel::EditorModelType)
        return;

    MergeDataDialog dlg(this);
    dlg.setWindowTitle(tr("Merge Weights file"));
    if (dlg.exec()) {
        displace::workers::MergerStrategy *strategy = new displace::workers::MergerStrategy(displace::workers::MergerStrategy::Weights);
        displace::workers::DataMerger *merger = new displace::workers::DataMerger(strategy, currentModel.get());
        connect (merger, SIGNAL(completed(DataMerger*)), this, SLOT(mergeCompleted(DataMerger*)));

        if (mWaitDialog != 0) delete mWaitDialog;
        mWaitDialog = new WaitDialog(this);
        merger->setWaitDialog(mWaitDialog);
        merger->setDistance(dlg.getDistance());
        merger->setSeparator(dlg.separator());
        merger->start(dlg.getInputFile(), dlg.getOutputFile());

    }
}

/// \todo: This is duplicated code - see MainWindow::on_actionMergeWeights_triggered(). Must be simplified and unified.
void MainWindow::on_actionMergePings_triggered()
{
    if (!currentModel || currentModel->modelType() != DisplaceModel::EditorModelType)
        return;

    MergeDataDialog dlg(this);
    dlg.setWindowTitle(tr("Merge Ping file"));
    if (dlg.exec()) {
        displace::workers::MergerStrategy *strategy = new displace::workers::MergerStrategy(displace::workers::MergerStrategy::Ping);
        displace::workers::DataMerger *merger = new displace::workers::DataMerger(strategy, currentModel.get());
        connect (merger, SIGNAL(completed(DataMerger*)), this, SLOT(mergeCompleted(DataMerger*)));

        if (mWaitDialog != 0) delete mWaitDialog;
        mWaitDialog = new WaitDialog(this);
        merger->setWaitDialog(mWaitDialog);
        merger->setDistance(dlg.getDistance());
        merger->setSeparator(dlg.separator());
        merger->start(dlg.getInputFile(), dlg.getOutputFile());
    }
}

void MainWindow::on_actionCalcPopDistribution_triggered()
{
    if (!currentModel || currentModel->modelType() != DisplaceModel::EditorModelType)
        return;

    if (currentModel->getStockNames().size() == 0) {
        int r = QMessageBox::question(this, tr("Calculate Population distribution"),
                                      tr("No Stock names were loaded. Names will be automatically assigned. Do you want to proceed?"),
                                      QMessageBox::No, QMessageBox::Yes);
        if (r == QMessageBox::No)
            return;
    }

    MergePopulationDataDialog dlg(this);
    dlg.setOutputRequiresTemplate(2);
    dlg.setDefaultOutputToInput(false);
    dlg.setWindowTitle(tr("Calculate Population distribution"));
    if (dlg.exec()) {
        displace::workers::PopulationDistributionDataMergerStrategy *strategy = new displace::workers::PopulationDistributionDataMergerStrategy(currentModel.get());
        displace::workers::DataMerger *merger = new displace::workers::DataMerger(strategy, currentModel.get());
        connect (merger, SIGNAL(completed(DataMerger*)), this, SLOT(mergeCompleted(DataMerger*)));

        if (mWaitDialog != 0) delete mWaitDialog;
        mWaitDialog = new WaitDialog(this);
        merger->setWaitDialog(mWaitDialog);
        merger->setDistance(dlg.getDistance());
        merger->setSeparator(dlg.separator());
        merger->start(dlg.getInputFile(), dlg.getOutputFile());
    }
}

void MainWindow::mergeCompleted(DataMerger *merger)
{
    mWaitDialog->close();
    delete mWaitDialog;
    mWaitDialog = 0;

    try {
        if (merger->checkResult()) {
            QMessageBox::information(this, tr("Data merged"),
                                     tr("Data has been processed correctly."));
        } else {
            QMessageBox::warning(this, tr("Data merge aborted"),
                                 tr("Data merging was stopped by user request"));
        }
    } catch (displace::DisplaceException &x) {
        QMessageBox::warning(this, tr("Error merging files"),
                             QString(tr("An error occurred while merging files %1: %2"))
                             .arg(x.file())
                             .arg(x.message()));
    }

    delete merger;
}

void MainWindow::exportGraphics(QString label, QWidget *widget)
{
    QSettings set;
    QString defpos = set.value("ImageExport", QDir::homePath()).toString();
    QString lastform = set.value("ImageExport.format", "png").toString();
    QStringList filter = displace::helpers::images::supportedFormatsOnWriteAsFilter();
    QString deffilter;
    int idx = displace::helpers::images::supportedFormatsOnWrite().indexOf(lastform);
    if (idx != -1)
        deffilter = filter[idx];

    QString path = QFileDialog::getSaveFileName(this, QString(tr("Export %1 Image")).arg(label),
                                                defpos, filter.join(";;"), &deffilter);
    if (!path.isEmpty()) {
        int idx = path.lastIndexOf(QString("."));
        QString extension = (idx != -1 ? path.mid(idx+1) : "");
        if (extension.isEmpty()) {
            idx = filter.indexOf(deffilter);
            if (idx != -1) {
                extension = displace::helpers::images::supportedFormatsOnWrite().at(idx);
                if (!path.endsWith('.'))
                    path.append(".");
                path.append(extension);
            }
        }

        QFileInfo info(path);
        set.setValue("ImageExport", info.path());

        if(widget->grab().save(path)) {
            set.setValue("ImageExport.format", extension.toLower());

            QMessageBox::information(this, tr("Image saved"),
                                     QString(tr("Image saved: %1")).arg(path));
        } else {
            QMessageBox::warning(this, tr("Image save failed."),
                                 QString(tr("Cannot save image: %1")).arg(path));
        }
    }

}

void MainWindow::on_actionExport_Map_triggered()
{
    exportGraphics(tr("Map"), map);
}

void MainWindow::on_actionExport_Harbours_triggered()
{
    exportGraphics(tr("Harbours Plot"), ui->plotHarbours);
}

void MainWindow::on_actionExport_Populations_triggered()
{
    exportGraphics(tr("Populations Plot"), ui->plotPopulations);
}

void MainWindow::on_actionExport_Nations_triggered()
{
    exportGraphics(tr("Nations Plot"), ui->plotNations);
}

void MainWindow::on_actionLoadStockNames_triggered()
{
    MruPathManager mru;
    QString file = QFileDialog::getOpenFileName(this, tr("Load stock names"), mru.getMru(MruPathManager::StockNamesFile),
                                                tr("Dat,Txt files (*.dat *.txt);;All files (*.*)"));

    if (!file.isEmpty()) {
        InputFileParser parser;
        QMap<QString,int> stocks;
        try {
            parser.parseStockNamesFile(file, stocks);
        } catch (displace::DisplaceException &ex) {
            QMessageBox::warning(this, tr("Load stock names"), tr("An error occured while loading the stock names: %1 file %2 line %3")
                                 .arg(ex.message()).arg(ex.file()).arg(ex.line()));
            return;
        }
        currentModel->setStockNames(stocks);
        mru.setMru(MruPathManager::StockNamesFile, file);
        QMessageBox::information(this, tr("Load stock names"), tr("Stock names loaded correctly."));
    }
}

void MainWindow::on_actionDecision_Trees_Editor_triggered()
{
    QProcess *ed = new QProcess;
    QString app = qApp->applicationDirPath() + "/dtreeeditor";

#ifdef __WINNT
    app += ".exe";
#endif

    ed->start(app);
}
