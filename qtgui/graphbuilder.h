#ifndef GRAPHBUILDER_H
#define GRAPHBUILDER_H

#include <QList>
#include <QPointF>

#include <memory>
#include <gdal/ogrsf_frmts.h>

class GraphBuilder
{
public:
    enum Type { Hex };

    GraphBuilder();

    void setType (Type type) {
        mType = type;
    }

    void setDistance (double distance) {
        mStep = distance;
    }

    void setLimits (double lonMin, double lonMax, double latMin, double latMax) ;
    void setShapefile (std::shared_ptr<OGRDataSource> src);

    QList<QPointF> buildGraph();

    static void pointSumWithBearing (const QPointF &p1, double dist, double bearing, QPointF &p2);
    static const double earthRadius;

private:
    Type mType;
    double mStep;
    double mLatMin, mLatMax, mLonMin, mLonMax;

    std::shared_ptr<OGRDataSource> mShapefile;
};

#endif // GRAPHBUILDER_H
