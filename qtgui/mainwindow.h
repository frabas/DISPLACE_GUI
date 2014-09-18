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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    qmapcontrol::MapControl *map;
    qmapcontrol::MapAdapter *mapadapter;
    qmapcontrol::MapAdapter *seamarkadapter;
    qmapcontrol::MapLayer *mainlayer;
    qmapcontrol::MapLayer *seamarklayer;

    // tree model adapter
    ObjectTreeModel *treemodel;
};

#endif // MAINWINDOW_H
