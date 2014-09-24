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
class MapAdapter;
class LayerMapAdapter;
class LayerGeometry;
}

class ObjectTreeModel;
class DisplaceModel;
class Simulator;

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
    void updateModelState();

    void on_action_Load_triggered();
    void on_modelSelector_currentIndexChanged(int index);
    void on_cmdStart_clicked();
    void on_cmdStop_clicked();

    void on_actionScenario_triggered();

    void on_actionSave_triggered();

signals:
    void modelStateChanged();

protected:
    void updateModelList();
    void closeEvent ( QCloseEvent * event );

private:
    Ui::MainWindow *ui;

    // Data model
    DisplaceModel *models[MAX_MODELS];
    DisplaceModel *currentModel;
    int currentModelIdx;
    Simulator *mSimulation;

    // Geospatial objects
    //std::shared_ptr<qmapcontrol::QMapControl> map;
    qmapcontrol::QMapControl *map;
    std::shared_ptr<qmapcontrol::MapAdapter> mapadapter;
    std::shared_ptr<qmapcontrol::MapAdapter> seamarkadapter;
    std::shared_ptr<qmapcontrol::LayerMapAdapter> mainlayer;
    std::shared_ptr<qmapcontrol::LayerMapAdapter> seamarklayer;
    std::shared_ptr<qmapcontrol::LayerGeometry> entitylayer;

    // tree model adapter
    ObjectTreeModel *treemodel;

    static const int maxModels;
};

#endif // MAINWINDOW_H
