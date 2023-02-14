// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2023 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------------------------------------------------------------

/* --------------------------------------------------------------------------
 * DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
 * AND EFFORT DISPLACEMENT
 * Copyright (c) 2012, 2013, 2014 Francois Bastardie <fba@aqua.dtu.dk>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */
#ifndef GRAPHBUILDER_H
#define GRAPHBUILDER_H

#include <QList>
#include <QPointF>

#include <memory>
#include <ogrsf_frmts.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wshift-negative-value"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#pragma GCC diagnostic pop

#include <utility>

namespace displace {
namespace graphbuilders {
class GeographicGridBuilder;
}
}

namespace deprecated {

class [[deprecated]] GraphBuilder
{
public:
    class Feedback {
    public:
        virtual void setMax(int m) = 0;
        virtual void setStep(int step) = 0;
    };

    enum Type { Hex, Quad, HexTrivial, QuadTrivial };

    class Node {
    public:
        QPointF point;
        int harbour;
        QList<int> adiacencies;
        QList<double> weight;
        bool good;

        Node()
            : point(), harbour(0), adiacencies(), weight(), good(false) {
        }
    };

    GraphBuilder();

    void setType (Type type) {
        mType = type;
    }

    void setDefaultDistance (double distance) {
        mStep = distance;
    }

    void setDistance1 (double distance) {
        mStep1 = distance;
    }

    void setDistance2 (double distance) {
        mStep2 = distance;
    }

    void setFeedback (Feedback *feedback) {
        mFeedback = feedback;
    }

    void setLimits (double lonMin, double lonMax, double latMin, double latMax) ;
    void setIncludingShapefile1 (std::shared_ptr<OGRDataSource> src);
    void setIncludingShapefile2 (std::shared_ptr<OGRDataSource> src);
    void setExcludingShapefile (std::shared_ptr<OGRDataSource> src);
    void setExcludeZoneEdgeRemovalEnabled (bool en) {
        mRemoveEdgesInExcludeZone = en;
    }
    void setMaxLinks(int num = -1) {
        mMaxLinks = num;
    }
    void setMinLinks(int num = -1) {
        mMinLinks = num;
    }

    QList<Node> buildGraph();

    static void pointSumWithBearing (const QPointF &p1, double dist, double bearing, QPointF &p2);
    static const double earthRadius;

    bool outsideEnabled() const;
    void setOutsideEnabled(bool outsideEnabled);
    void setLinkLimits(double limit_km);

private:
    typedef CGAL::Exact_predicates_inexact_constructions_kernel         K;
    typedef CGAL::Triangulation_vertex_base_with_info_2<unsigned, K>    Vb;

    /*
    typedef CGAL::Triangulation_data_structure_2<Vb>                    Tds;
    typedef CGAL::Delaunay_triangulation_2<K, Tds>                      Delaunay;*/

    typedef CGAL::Constrained_triangulation_face_base_2<K>           Fb;
    typedef CGAL::Triangulation_data_structure_2<Vb,Fb>              TDS;
    typedef CGAL::Exact_predicates_tag                               Itag;
    typedef CGAL::Constrained_Delaunay_triangulation_2<K, TDS, Itag> CDT;

    typedef CDT::Point                                             Point;

    void fillWithNodes(displace::graphbuilders::GeographicGridBuilder *builder, QList<Node> &res, CDT &tri, std::vector<std::shared_ptr<OGRDataSource> > including, std::vector<std::shared_ptr<OGRDataSource> > excluding, bool outside, int &progress);
    void pushAd(QList<Node> &node, int source, int target);

    Type mType;
    bool mOutsideEnabled, mRemoveEdgesInExcludeZone;
    double mStep, mStep1, mStep2;
    double mLatMin, mLatMax, mLonMin, mLonMax;
    double mLinkLimits;
    int mMaxLinks, mMinLinks;

    std::shared_ptr<OGRDataSource> mShapefileInc1;
    std::shared_ptr<OGRDataSource> mShapefileInc2;
    std::shared_ptr<OGRDataSource> mShapefileExc;

    Feedback *mFeedback;
};
}

#endif // GRAPHBUILDER_H
