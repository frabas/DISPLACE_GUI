#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

namespace Ui {
class MainWindow;
}

namespace qmapcontrol {
class MapControl;
class MapAdapter;
class MapLayer;
class GeometryLayer;
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

    qmapcontrol::MapControl *map;
    qmapcontrol::MapAdapter *mapadapter;
    qmapcontrol::MapAdapter *seamarkadapter;
    qmapcontrol::MapLayer *mainlayer;
    qmapcontrol::MapLayer *seamarklayer;
    qmapcontrol::GeometryLayer *entitylayer;

    // tree model adapter
    ObjectTreeModel *treemodel;

    static const int maxModels;
};

#endif // MAINWINDOW_H
