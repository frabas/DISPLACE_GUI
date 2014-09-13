#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "vesselGUI.h"
#include "MyGraphicsView.h"

#define MULTI 100 // multiplier of coord x,y for VesselGUI on the scene

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    cout << "coucou there!" << endl;

    ui->setupUi(this);


    //MapView *view = new MapView;
    MyGraphicsView *view = new MyGraphicsView;
    QStatusBar *statusbar = new QStatusBar;


    //Set-up the scene
    QGraphicsScene* scene = new QGraphicsScene;
    view->setScene(scene);

    //Set-up the view (pin up the scene at the bottom left corner and then invert by -1 with scale())
    // USE COORDINATES FROM CITIES AT THE CORNER OF THE GGMAP: Itzehoe GER, Koszalin Poland, Hirstals DK
    //scene->setSceneRect(6, 53, 10, 4);// point x, point y, size x and size y (corresponding to the ggmap)
    scene->setSceneRect(9.5190*MULTI,  53.9229*MULTI,  6.6532*MULTI, 3.668*MULTI);// point x, point y, x+size x, and y+size y
    ui->view->SetCenter(QPointF(12, 55)); //A modified version of centerOn(), handles special cases
    ui->view->setCursor(Qt::OpenHandCursor);

    // the chosen center
    scene->addLine ( 7*MULTI, 56*MULTI,9*MULTI, 56*MULTI, (QPen(Qt::black, 0)));
    scene->addLine ( 8*MULTI, 55*MULTI,8*MULTI, 57*MULTI, (QPen(Qt::black, 0)));

    // up corner left of the ggmap
    scene->addLine ( -4.646254*MULTI, 66.35229*MULTI,-6.646254*MULTI, 66.35229*MULTI, (QPen(Qt::black, 0)));
    scene->addLine ( -5.646254*MULTI, 65.35229*MULTI,-5.646254*MULTI, 67.35229*MULTI, (QPen(Qt::black, 0)));

    // bottom corner right of the ggmap
    scene->addLine ( 14.68133*MULTI, 50.88018*MULTI,16.68133*MULTI, 50.88018*MULTI, (QPen(Qt::black, 0)));
    scene->addLine ( 15.68133*MULTI, 49.88018*MULTI,15.68133*MULTI, 51.88018*MULTI, (QPen(Qt::black, 0)));

    scene->setItemIndexMethod(QGraphicsScene::NoIndex);

   //QPixmap pixmap(":/images/background_map_10_52_20_62.jpg");
    QPixmap pixmap(":/images/map_ggmap_port_balticonly.jpeg"); //do not forget to add this to VesselGUI.qrc....
    //QPixmap pixmap(":/images/cheese.jpg");
    QGraphicsPixmapItem* item = scene->addPixmap(pixmap);
    item->setPos(QPointF(9.5190*MULTI,57.588*MULTI)); // up left corner of the image find out with R locator()
    //item->setPos(9.5,55);

    //item->scale(1,-1); //invert the y-axis

    // resize the ggmap to the scene, but caution use MULTI to avoid altering the resolution.
    QSize a_size = scene->sceneRect().size().toSize();
    item->setPixmap(pixmap.scaled(a_size, Qt::IgnoreAspectRatio, Qt::FastTransformation));


    //Populate the scene
    // the chosen grid
    scene->addLine ( 6*MULTI, 52*MULTI,6*MULTI, 54*MULTI, (QPen(Qt::red, 0)));
    scene->addLine ( 5*MULTI, 53*MULTI,7*MULTI, 53*MULTI, (QPen(Qt::red, 0)));


    for(int x = (6*MULTI); x < (16*MULTI); x = x + (1*MULTI)) {
        for(int y = (53*MULTI); y < (57*MULTI); y = y + (1*MULTI)) {

            if(x % 2 == 0 && y % 2 == 0) {
                scene->addRect(x, y, (1*MULTI), (1*MULTI));

                QString pointString;
                QTextStream stream(&pointString);
                stream << "(" << x << "," << y << ")";
               // QGraphicsTextItem* item = scene->addText(pointString);
               //  item->setPos(x, y);
            } else {
                scene->addRect(x, y, (1*MULTI), (1*MULTI));
            }
        }
    }


    // initial zoom in
    ui->view->scale(qreal(2), qreal(-2)); // negative on y to invert the y axis so that the arrow points to the the ceiling.....


    //ui->view->fitInView(9.5190*MULTI,  53.9229*MULTI,  6.6532*MULTI, 3.668*MULTI, Qt::KeepAspectRatio);
    ui->view->setRenderHint(QPainter::Antialiasing);
    ui->view->setCacheMode(QGraphicsView::CacheBackground);
    ui->view->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    ui->view->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->view->setWindowTitle(QT_TRANSLATE_NOOP(QGraphicsView, "DISPLACE model"));

    // CAUTION: to make the link between the view of the ui an our own custumize MyGraphicsView class:
    // When in QtDesigner, right click your graphics view widget and choose "Promote to...",
    // then input the name of your custom graphics view based class.
    // Then Qt Creator should generate code where the standard graphics view object
    // is replaced by an instance of the class you have chosen.

    // make the link/export for the remaining c++ loop...
    m_scene=scene;
    ui->view->setScene(scene); // caution: the name of the view we named in Qt designer should correspond here?.....
    //this->setCentralWidget(view);

}

MainWindow::~MainWindow()
{
    delete ui;
}

QGraphicsScene *MainWindow::scene()
{
    return m_scene;
}


void MainWindow::on_quitButton_clicked()
{

}


void MainWindow::on_startButton_clicked()
{

}

void MainWindow::on_spinBox_valueChanged(int arg1)
{
   // this->spinBox->setValue(arg1);
}
