#include "mergerstrategy.h"

#include <modelobjects/nodedata.h>
#include <GeographicLib/Geodesic.hpp>

using namespace displace::workers;

const char *const MergerStrategy::MergedField = "PT_GRAPH";
const char *const MergerStrategy::LatField = "SI_LATI";
const char *const MergerStrategy::LongField = "SI_LONG";


MergerStrategy::MergerStrategy(MergeType type)
    : Strategy(),
      mOwner(nullptr),
      mType(type)
{
}

void MergerStrategy::attach(DataMerger *merger)
{
    mOwner = merger;
}

bool MergerStrategy::processHeaderField(QString field, int i)
{
    if (field == MergedField) {
        col_pt_graph = i;
    } else if(field == LatField) {
        col_lat = i;
    } else if (field == LongField) {
        col_lon = i;
    }

    mFields.push_back(field);
    return true;
}

bool MergerStrategy::postHeaderProcessed()
{
    if (col_lat == -1 || col_lon == -1) {
        (new displace::DisplaceException(QString(QObject::tr("%1 or %2 fields are missing"))
                       .arg(LatField).arg(LongField)))->raise();
    }

    if (col_pt_graph == -1) {
        colpresent = false;
        mFields.push_back(MergedField);
        col_pt_graph = mFields.size()-1;
    }

    return true;
}

bool MergerStrategy::saveOutput(QString out)
{
    QFile outfile(out);

    if (!outfile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        (new displace::DisplaceException(outfile.errorString()))->raise();

    QTextStream outstream(&outfile);

    outstream << mFields.join(mOwner->separator()) << endl;

    int row = 0;
    foreach (QString line, mResults) {
        outstream << line << endl;
        ++row;

        if (mOwner->mustExit()) {
            outfile.close();
            return false;
        }
    }

    outfile.close();

    return true;
}

void MergerStrategy::processLine (int linenum, QString line)
{
    bool ok;

    QStringList entry = line.split(mOwner->separator(), QString::SkipEmptyParts);
    if (entry.size() < col_lat || entry.size() < col_lon)  // Skip empty / incorrect lines
        return;

    double lat = entry.at(col_lat).toDouble(&ok);
    if (!ok)
        (new displace::DisplaceException(QString(QObject::tr("Error parsing line %1 field %2")).arg(linenum).arg(col_lat)))->raise();

    double lon = entry.at(col_lon).toDouble(&ok);
    if (!ok)
        (new displace::DisplaceException(QString(QObject::tr("Error parsing line %1 field %2")).arg(linenum).arg(col_lat)))->raise();

    int idx = -1;

#if GEOGRAPHICLIB_VERSION_MINOR > 25
        const GeographicLib::Geodesic& geod = GeographicLib::Geodesic::WGS84();
#else
        const GeographicLib::Geodesic& geod = GeographicLib::Geodesic::WGS84;
#endif

    QList<std::shared_ptr<NodeData>> nodes = mOwner->getAllNodesWithin(QPointF(lon,lat), mOwner->distance());

    double mindist = 1e90;
    double dist;
    std::shared_ptr<NodeData> nearestNode;
    foreach (std::shared_ptr<NodeData> node, nodes) {
        if (mType == Weights && node->get_is_harbour())
            continue;
        if (mType == Ping && !node->get_is_harbour())
            continue;

        geod.Inverse(node->get_y(), node->get_x(), lat, lon, dist);
        if (dist < mindist) {
            nearestNode = node;
            mindist = dist;
        }
    }

    if (nearestNode)
        idx = nearestNode->get_idx_node();

    // update the field.
    while (entry.size() < col_pt_graph)
        entry.push_back(".");

    if (!colpresent)
        entry.insert(col_pt_graph, ".");
    entry[col_pt_graph] = QString::number(idx);

    mutex.lock();
    mResults.push_back(entry.join(mOwner->separator()));
    mutex.unlock();

}
