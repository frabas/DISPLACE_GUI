#ifndef SIMPLEPLANARGRAPHBUILDER_H
#define SIMPLEPLANARGRAPHBUILDER_H

#include <algo/geographicgridbuilder.h>

#include <QPointF>

namespace displace {
namespace graphbuilders {

class SimplePlanarGraphBuilder : public GeographicGridBuilder
{
public:
    enum Type { Quad, Hex };

    SimplePlanarGraphBuilder(Type type, double lat_min_deg, double lon_min_deg, double lat_max_deg, double lon_max_deg, double step_x);
    ~SimplePlanarGraphBuilder();

    // GeographicGridBuilder interface
public:
    bool beginCreateGrid();
    QPointF getNext();
    bool atEnd();
    bool isAtLineStart();
    bool endCreateGrid();

protected:
    void pointSumWithBearing(const QPointF &p1, double dist, double bearing, QPointF &p2);

private:
    double mLonMin, mLatMin, mLonMax, mLatMax;
    double stepx, fal;

    QPointF p1, p2;
    double clat, flon;
    bool linestart;
    int nr;
};

}
}

#endif // SIMPLEPLANARGRAPHBUILDER_H
