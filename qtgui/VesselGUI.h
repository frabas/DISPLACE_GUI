#ifndef VESSELGUI_H
#define VESSELGUI_H


#include <QGraphicsItem>
#include "Vessel.h"

#define MULTI 100 // multiplier of coord x,y for VesselGUI on the scene


//! [0]
class VesselGUI :  public Vessel, public QGraphicsItem
{
public:
    VesselGUI(Node* a_location, int idx_vessel, string name, int nbpops, int nbszgroups, vector<int> harbours, vector<int> fgrounds,
               vector<double> freq_harbours, vector<double> freq_fgrounds, vector<double> vessel_betas_per_pop,
               vector<double> percent_tac_per_pop,
               multimap <int, int> possible_metiers, multimap <int, double> freq_possible_metiers,
               double speed, double fuelcons, double length, double KW,
               double  carrycapacity, double tankcapacity, double nbfpingspertrip,
               double resttime_par1, double resttime_par2, double av_trip_duration);


    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

protected:
    void advance(int step);

private:
    qreal angle;
    qreal speed;
    qreal mouseEyeDirection;
    QColor color;
};
//! [0]



#endif // VESSELGUI_H
