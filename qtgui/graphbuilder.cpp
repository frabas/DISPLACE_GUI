#include "graphbuilder.h"

#include <cmath>

#include <QDebug>

GraphBuilder::GraphBuilder()
{
}

QList<QPointF> GraphBuilder::buildGraph()
{
    QList<QPointF> res;

    double ys = std::sqrt(3) / 2.0 * mStep;
    int nr = std::floor((mLatMax - mLatMin + ys) / ys);
    int nce = std::floor((mLonMax - mLonMin + mStep) / mStep);
    int nco = std::floor((mLonMax - mLonMin - mStep / 2.0 + mStep) / mStep);

    qDebug() << "Graph: " << nr << nce << nco;
    qDebug() << mStep << ys;

    for (int y = 0; y < nr; ++y) {
        int nx = (y % 2) == 0 ? nce : nco;

        for (int x = 0; x < nx; ++x) {
            double py = mLatMin + y * ys;
            double px = mLonMin + x * mStep;

            if ((y % 2) == 1)
                px += mStep / 2.0;

            res.append(QPointF(px,py));
            qDebug() << x << y << px << py;
        }
    }

    return res;
}
