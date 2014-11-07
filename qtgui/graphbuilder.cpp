#include "graphbuilder.h"

#include <cmath>
#include <m_constants.h>

#include <QDebug>

const double GraphBuilder::earthRadius = 6371000;   // ...


GraphBuilder::GraphBuilder()
{
}

void GraphBuilder::setLimits(double lonMin, double lonMax, double latMin, double latMax)
{
    mLatMin = std::min(latMin, latMax) * M_PI / 180.0;
    mLatMax = std::max(latMin, latMax) * M_PI / 180.0;
    mLonMin = std::min(lonMin, lonMax) * M_PI / 180.0;
    mLonMax = std::max(lonMin, lonMax) * M_PI / 180.0;
}

void GraphBuilder::setShapefile(std::shared_ptr<OGRDataSource> src)
{
    mShapefile = src;
}

QList<GraphBuilder::Node> GraphBuilder::buildGraph()
{
    QList<Node> res;

    double lat = mLatMin;
    double latinc = std::sqrt(3) / 2.0 * mStep;

    QPointF p1(mLonMin, mLatMin), p2;

    QList<int> idx0, idx1, idx2;

    int nr = 0, nc = 0;
    while (lat <= mLatMax) {
        if ((nr % 2) == 1) {
            pointSumWithBearing(QPointF(mLonMin, lat), mStep/2, M_PI_2, p1);
        } else {
            p1.setX(mLonMin);
            p1.setY(lat);
        }

        nc = 0;
        while (p1.x() <= mLonMax) {
            Node n;
            n.point = QPointF(p1.x() * 180.0 / M_PI, p1.y() * 180.0 / M_PI);
            n.good = true;

            if (mShapefile.get()) {
                OGRPoint point (n.point.x(), n.point.y());
                for (int lr = 0; lr < mShapefile->GetLayerCount(); ++lr) {
                    OGRLayer *layer = mShapefile->GetLayer(lr);
                    layer->SetSpatialFilter(&point); //getting only the feature intercepting the point

                    layer->ResetReading();
                    if (layer->GetNextFeature() != 0) {
                        n.good = false;
                        break;
                    }
                }
            }

            res.append(n);
            idx0.push_back(res.size()-1);

            pointSumWithBearing(p1, mStep, M_PI_2, p2);
            p1 = p2;

            ++nc;
        }

        createAdiacencies(res, idx2, idx1, idx0, nr-1);

        pointSumWithBearing(p1, latinc, 0, p2);
        lat = p2.y();
        ++nr;

        idx2 = idx1;
        idx1 = idx0;
        idx0.clear();
    }

    createAdiacencies(res, idx2, idx1, idx0, nr-1);

    return res;
}

void GraphBuilder::pointSumWithBearing(const QPointF &p1, double dist, double bearing, QPointF &p2)
{
    // φ Latitude λ Longitude d distance R earth radius [6371km], brng bearing (rad, north, clockwise)
    // var φ2 = Math.asin( Math.sin(φ1)*Math.cos(d/R) +
    //      Math.cos(φ1)*Math.sin(d/R)*Math.cos(brng) );
    // var λ2 = λ1 + Math.atan2(Math.sin(brng)*Math.sin(d/R)*Math.cos(φ1),
    //     Math.cos(d/R)-Math.sin(φ1)*Math.sin(φ2));

    p2.setY(std::asin (
                std::sin(p1.y()) * std::cos(dist / earthRadius) +
                std::cos(p1.y()) * std::sin(dist / earthRadius) * std::cos(bearing)));
    p2.setX(p1.x() +
            std::atan2( std::sin(bearing) * std::sin(dist/earthRadius) * std::cos(p1.y()),
                        std::cos(dist/earthRadius) - std::sin(p1.y()) * std::sin(p2.y()))
            );
}

void GraphBuilder::createAdiacencies(QList<GraphBuilder::Node> &nodes, const QList<int> &pidx, const QList<int> &idx, const QList<int> &nidx, int row_index)
{
//    qDebug() << row_index<< idx.size();
    for (int i = 0; i < idx.size(); ++i) {
        // current node is nodes[i]

//        if (!nodes[idx[i]].good)
//            continue;

        if (i > 0 /*&& nodes[idx[i-1]].good*/)
            nodes[idx[i]].adiancies.push_back(idx[i-1]);     // left node
        if (i < idx.size()-1 /*&& nodes[idx[i+1]].good*/)
            nodes[idx[i]].adiancies.push_back(idx[i+1]);     // right node

        if ((row_index % 2) == 0) {     // even
            if (i > 0 && i-1 < pidx.size() /*&& nodes[pidx[i-1]].good*/)
                nodes[idx[i]].adiancies.push_back(pidx[i-1]);
            if (i < pidx.size() /*&& nodes[pidx[i]].good*/)
                nodes[idx[i]].adiancies.push_back(pidx[i]);

            if (i > 0 && i-1 < nidx.size() /*&& nodes[nidx[i-1]].good*/)
                nodes[idx[i]].adiancies.push_back(nidx[i-1]);
            if (i < nidx.size() /*&& nodes[nidx[i]].good*/)
                nodes[idx[i]].adiancies.push_back(nidx[i]);
        } else {    // odd
            if (i < pidx.size() /*&& nodes[pidx[i]].good*/)
                nodes[idx[i]].adiancies.push_back(pidx[i]);
            if (i+1 < pidx.size() /*&& nodes[pidx[i+1]].good*/)
                nodes[idx[i]].adiancies.push_back(pidx[i+1]);

            if (i < nidx.size() /*&& nodes[nidx[i]].good*/)
                nodes[idx[i]].adiancies.push_back(nidx[i]);
            if (i+1 < nidx.size() /*&& nodes[nidx[i+1]].good*/)
                nodes[idx[i]].adiancies.push_back(nidx[i+1]);
        }

//        qDebug() << "edges" << idx[i] << nodes[idx[i]].adiancies;
    }
}
