#ifndef GRAPHBUILDER_H
#define GRAPHBUILDER_H

#include <QList>
#include <QPointF>

#include <memory>
#include <gdal/ogrsf_frmts.h>

class GraphBuilder
{
public:
    class Feedback {
    public:
        virtual void setMax(int m) = 0;
        virtual void setStep(int step) = 0;
    };

    enum Type { Hex, Quad };

    class Node {
    public:
        QPointF point;
        QList<int> adiacencies;
        QList<double> weight;
        bool good;
    };

    GraphBuilder();

    void setType (Type type) {
        mType = type;
    }

    void setDistance (double distance) {
        mStep = distance;
    }

    void setFeedback (Feedback *feedback) {
        mFeedback = feedback;
    }

    void setLimits (double lonMin, double lonMax, double latMin, double latMax) ;
    void setShapefile (std::shared_ptr<OGRDataSource> src);

    QList<Node> buildGraph();

    static void pointSumWithBearing (const QPointF &p1, double dist, double bearing, QPointF &p2);
    static const double earthRadius;

private:
    void createAdiacencies (QList<Node> &nodes, const QList<int> &pidx, const QList<int> &idx, const QList<int> &nidx, int row_index);
    void pushAd(QList<Node> &node, int source, int target);

    Type mType;
    double mStep;
    double mLatMin, mLatMax, mLonMin, mLonMax;

    std::shared_ptr<OGRDataSource> mShapefile;

    Feedback *mFeedback;
};

#endif // GRAPHBUILDER_H
