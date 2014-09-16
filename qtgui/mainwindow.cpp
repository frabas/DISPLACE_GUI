#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <mapcontrol.h>
#include <emptymapadapter.h>
#include <maplayer.h>
#include <osmmapadapter.h>
#include <wmsmapadapter.h>
#include <objecttreemodel.h>

#include <QBoxLayout>
#include <QTextEdit>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    map(0),
    mapadapter(0),
    mainlayer(0),
    treemodel(new ObjectTreeModel())
{
    ui->setupUi(this);

    map = new qmapcontrol::MapControl(ui->mapWidget);

    QHBoxLayout *layout = new QHBoxLayout;
    ui->mapWidget->setLayout(layout);
    layout->addWidget(map);

    ui->mapWidget->setWidget(map);

    map->showScale(false);

    // create mapadapter, for mainlayer and overlay
  // mapadapter = new qmapcontrol::EmptyMapAdapter();
   // mapadapter = new qmapcontrol::WMSMapAdapter("www2.demis.nl",
     //                                           "/wms/wms.asp?wms=WorldMap&LAYERS=Countries,Borders,Cities,Rivers,Settlements,Hillshading,Waterbodies,Railroads,Highways,Roads&FORMAT=image/png&VERSION=1.1.1&SERVICE=WMS&REQUEST=GetMap&STYLES=&EXCEPTIONS=application/vnd.ogc.se_inimage&SRS=EPSG:4326&TRANSPARENT=FALSE", 256);

    // create mapadapter, for mainlayer and overlay
    mapadapter = new qmapcontrol::OSMMapAdapter();

    // create a layer with the mapadapter and type MapLayer
    mainlayer = new qmapcontrol::MapLayer("OpenStreetMap-Layer", mapadapter);

    // add Layer to the MapControl
    map->addLayer(mainlayer);


    /* Tree model setup */
    ui->treeView->setModel(treemodel);
}

MainWindow::~MainWindow()
{
    delete ui;
}
