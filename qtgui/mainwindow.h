#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

namespace qmapcontrol {
class MapControl;
class MapAdapter;
class MapLayer;
}

class ObjectTreeModel;
class DisplaceModel;

#define MAX_MODELS 2

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    // Data model
    DisplaceModel *models[MAX_MODELS];

    // Geospatial objects

    qmapcontrol::MapControl *map;
    qmapcontrol::MapAdapter *mapadapter;
    qmapcontrol::MapAdapter *seamarkadapter;
    qmapcontrol::MapLayer *mainlayer;
    qmapcontrol::MapLayer *seamarklayer;

    // tree model adapter
    ObjectTreeModel *treemodel;

    static const int maxModels;
};

#endif // MAINWINDOW_H
