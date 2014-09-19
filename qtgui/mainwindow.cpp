#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <mapcontrol.h>
#include <maplayer.h>
#include <osmmapadapter.h>
#include <objecttreemodel.h>
#include <openseamapadapter.h>

#include <QBoxLayout>
#include <QTextEdit>

const int MainWindow::maxModels = MAX_MODELS;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    models(),
    map(0),
    mapadapter(0),
    mainlayer(0),
    treemodel(0)
{
    ui->setupUi(this);

    for (int i = 0; i < maxModels; ++i)
        models[i] = 0;

    map = new qmapcontrol::MapControl(ui->mapWidget);

    QPixmap pixmap;
    pixmap.fill( Qt::white );
    map->setLoadingPixmap(pixmap);

    QHBoxLayout *layout = new QHBoxLayout;
    ui->mapWidget->setLayout(layout);
    layout->addWidget(map);

    ui->mapWidget->setWidget(map);

    map->showScale(false);

    seamarkadapter = new qmapcontrol::OpenSeaMapAdapter();

    // create mapadapter, for mainlayer and overlay
    mapadapter = new qmapcontrol::OSMMapAdapter();

    // create a layer with the mapadapter and type MapLayer
    mainlayer = new qmapcontrol::MapLayer("OpenStreetMap", mapadapter);

    seamarklayer = new qmapcontrol::MapLayer("Seamark", seamarkadapter);


    // add Layer to the MapControl
    map->addLayer(mainlayer);
    map->addLayer(seamarklayer);

    map->setView(QPointF(11.54105,54.49299));
    map->setZoom(10);

    /* Tree model setup */
    treemodel = new ObjectTreeModel(map);
    ui->treeView->setModel(treemodel);
}

MainWindow::~MainWindow()
{
    delete ui;
}
