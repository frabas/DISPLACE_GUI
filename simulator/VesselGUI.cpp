
#include "VesselGUI.h"

#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>

#include <QDebug>


#include <math.h>

#define MULTI 100 // multiplier of coord x,y for VesselGUI on the scene



static const double Pi = 3.14159265358979323846264338327950288419717;
static double TwoPi = 2.0 * Pi;

static qreal normalizeAngle(qreal angle)
{
    while (angle < 0)
        angle += TwoPi;
    while (angle > TwoPi)
        angle -= TwoPi;
    return angle;
}



//! [0]
VesselGUI::VesselGUI(Node* a_location, int idx_vessel, string name, int nbpops, int nbszgroups, vector<int> harbours, vector<int> fgrounds,
                     vector<double> freq_harbours, vector<double> freq_fgrounds, vector<double> vessel_betas_per_pop,
                     vector<double> percent_tac_per_pop,
                     multimap <int, int> possible_metiers, multimap <int, double> freq_possible_metiers,
                     double speed, double fuelcons, double length, double KW,
                     double  carrycapacity, double tankcapacity, double nbfpingspertrip,
                     double resttime_par1, double resttime_par2, double av_trip_duration) :
      Vessel(a_location, idx_vessel, name, nbpops, nbszgroups, harbours, fgrounds,
               freq_harbours, freq_fgrounds, vessel_betas_per_pop, percent_tac_per_pop,
                possible_metiers, freq_possible_metiers, speed, fuelcons, length, KW,
                  carrycapacity, tankcapacity, nbfpingspertrip,
                         resttime_par1, resttime_par2, av_trip_duration)
{

    color=  QColor(qrand() % 256, qrand() % 256, qrand() % 256, 100);
   // color=Qt::black;
    angle=0;
    speed= speed;
    mouseEyeDirection=0;
    setRotation(qrand() % (360 * 16));
}
//! [0]

//! [1]
QRectF VesselGUI::boundingRect() const
{
    qreal adjust = 0.5;
    return QRectF(-18 - adjust, -22 - adjust,
                  36 + adjust, 60 + adjust);
}
//! [1]

//! [2]
QPainterPath VesselGUI::shape() const
{
    QPainterPath path;
    path.addRect(-10, -20, 20, 40);
    return path;
}
//! [2]

//! [3]
void VesselGUI::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // Body
    painter->setBrush(color);
    painter->drawEllipse(-10, -20, 20, 40);

    // Eyes
    //painter->setBrush(Qt::white);
    //painter->drawEllipse(-10, -17, 8, 8);
    //painter->drawEllipse(2, -17, 8, 8);

    // Nose
    //painter->setBrush(Qt::black);
    //painter->drawEllipse(QRectF(-2, -22, 4, 4));

    // Pupils
    //painter->drawEllipse(QRectF(-8.0 + mouseEyeDirection, -17, 4, 4));
    //painter->drawEllipse(QRectF(4.0 + mouseEyeDirection, -17, 4, 4));

    // Ears
    painter->setBrush(scene()->collidingItems(this).isEmpty() ? Qt::darkYellow : Qt::red);
    painter->drawEllipse(-6, -12, 16, 16);
    //painter->drawEllipse(-17, -12, 16, 16);
    //painter->drawEllipse(1, -12, 16, 16);

    // Tail
    //QPainterPath path(QPointF(0, 20));
    //path.cubicTo(-5, 22, -5, 22, 0, 25);
    //path.cubicTo(5, 27, 5, 32, 0, 30);
    //path.cubicTo(-5, 32, -5, 42, 0, 35);
    //painter->setBrush(Qt::NoBrush);
    //painter->drawPath(path);
}
//! [3]

//! [4]
void VesselGUI::advance(int step)
{
    if (!step)
        return;
//! [4]
    // Don't move too far away

    // Try not to crash with any other mice
//! [7]
    QList<QGraphicsItem *> dangerMice = scene()->items(QPolygonF()
                                                       << mapToScene(0, 0)
                                                       << mapToScene(-30, -50)
                                                       << mapToScene(30, -50));
    foreach (QGraphicsItem *item, dangerMice) {
        if (item == this)
            continue;

        QLineF lineToMouse(QPointF(0, 0), mapFromItem(item, 0, 0));
        qreal angleToMouse = ::acos(lineToMouse.dx() / lineToMouse.length());
        if (lineToMouse.dy() < 0)
            angleToMouse = TwoPi - angleToMouse;
        angleToMouse = normalizeAngle((Pi - angleToMouse) + Pi / 2);

        if (angleToMouse >= 0 && angleToMouse < Pi / 2) {
            // Rotate right
            angle += 0.5;
        } else if (angleToMouse <= TwoPi && angleToMouse > (TwoPi - Pi / 2)) {
            // Rotate left
            angle -= 0.5;
//! [7] //! [8]
        }
//! [8] //! [9]
    }
//! [9]

    // Add some random movement
//! [10]
    if (dangerMice.size() > 1 && (qrand() % 10) == 0) {
      //  if (qrand() % 1)
         //   angle += (qrand() % 100) / 500.0;
        //else
       //     angle -= (qrand() % 100) / 500.0;
    }
//! [10]

//! [11]
    speed=get_speed();
    //set_Rotation() is 0 to 360, while get_course() return angle in range left[-180;0] right[0;+180] so:
    if(get_course()>0) setRotation(this->get_course()); // 0 to 180...ok
    unsigned int a_int= static_cast<unsigned int>(get_course() + 0.5);
    if(get_course()<0) setRotation( (a_int % 180) +181); //but convert 0;-180 into 360;180


    setPos(get_x()*MULTI,get_y()*MULTI);


   // qDebug()  << QString("x ") << QString::number( this->get_x()) <<
   //               QString(" y ") << QString::number( this->get_y()) <<
   //              QString(" idx ") << QString::number( this->get_idx()) <<
   //              QString(" speed ") << QString::number( this->get_speed()) <<
   //               QString(" course ") << QString::number( this->get_course())<< endl;
   // QPointF a_pos=pos();
   // qDebug() << "and on scene: " << QString::number(qreal(a_pos.x())) <<"   " <<
   //                                  QString::number(qreal(a_pos.y())) << endl;
}
//! [11]

