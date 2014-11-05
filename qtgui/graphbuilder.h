#ifndef GRAPHBUILDER_H
#define GRAPHBUILDER_H

#include <QList>
#include <QPointF>

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

    void setLimits (double lonMin, double lonMax, double latMin, double latMax) {
        mLatMin = std::min(latMin, latMax);
        mLatMax = std::max(latMin, latMax);
        mLonMin = std::min(lonMin, lonMax);
        mLonMax = std::max(lonMin, lonMax);
    }

    QList<QPointF> buildGraph();

private:
    Type mType;
    double mStep;
    double mLatMin, mLatMax, mLonMin, mLonMax;
};

#endif // GRAPHBUILDER_H
