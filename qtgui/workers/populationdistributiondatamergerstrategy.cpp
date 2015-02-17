#include "populationdistributiondatamergerstrategy.h"

#include <GeographicLib/Geodesic.hpp>

#include <QDebug>

using namespace displace::workers;

const char *const PopulationDistributionDataMergerStrategy::YearField = "Year";
const char *const PopulationDistributionDataMergerStrategy::SemesterField = "Semester";
const char *const PopulationDistributionDataMergerStrategy::LatField = "ShootLat";
const char *const PopulationDistributionDataMergerStrategy::LongField = "ShootLon";
const char *const PopulationDistributionDataMergerStrategy::StockField = "Stock";
const char *const PopulationDistributionDataMergerStrategy::IndivFieldPattern = "nb_indiv.";

PopulationDistributionDataMergerStrategy::PopulationDistributionDataMergerStrategy (DataMerger *owner)
    : DataMerger::Strategy(),
      mOwner(owner)
{
}

bool PopulationDistributionDataMergerStrategy::processHeaderField(QString field, int i)
{
    if (field == YearField)
        col_yr = i;
    else if (field == SemesterField)
        col_sem = i;
    else if (field == LatField)
        col_lat = i;
    else if (field == LongField)
        col_lon = i;
    else if (field == StockField)
        col_stock = i;
    else {
        if (field.startsWith(IndivFieldPattern)) {
            if (col_indiv == -1)
                col_indiv = i;
            ++num_col_indiv;
        }
    }
    return true;
}

bool PopulationDistributionDataMergerStrategy::postHeaderProcessed()
{
    if (col_yr == -1 ||
            col_sem == -1 ||
            col_lat == -1 ||
            col_lon == -1 ||
            col_stock == -1 ||
            col_indiv == -1) {
        qDebug() << " ERRORRRRRR" << col_yr << col_sem << col_lat << col_lon << col_stock << col_indiv;
        (new DataMerger::Exception(QString(QObject::tr("Some required field is missing. Please check the input file."))))->raise();
    }
    return true;
}

void PopulationDistributionDataMergerStrategy::processLine (QString line)
{
//    qDebug() << "Processing: " << line;

    bool ok;

    QStringList entry = line.split(mOwner->separator(), QString::SkipEmptyParts);

    double lat = entry.at(col_lat).toDouble(&ok);
    if (!ok)
        (new DataMerger::Exception(QString(QObject::tr("Error parsing field %1, not a double")).arg(col_lat)))->raise();

    double lon = entry.at(col_lon).toDouble(&ok);
    if (!ok)
        (new DataMerger::Exception(QString(QObject::tr("Error parsing field %1, not a double")).arg(col_lon)))->raise();

#if GEOGRAPHICLIB_VERSION_MINOR > 25
    const GeographicLib::Geodesic& geod = GeographicLib::Geodesic::WGS84();
#else
    const GeographicLib::Geodesic& geod = GeographicLib::Geodesic::WGS84;
#endif

    QList<std::shared_ptr<NodeData>> nodes = mOwner->getAllNodesWithin(QPointF(lon,lat), mOwner->distance());

    // COLLECT NODE + Y/SEM/STOCK => u(x) w(x) per pop
    // See: http://en.wikipedia.org/wiki/Inverse_distance_weighting

    Result res(num_col_indiv);
    res.year = entry.at(col_yr).toInt(&ok);
    if (!ok)
        (new DataMerger::Exception(QString(QObject::tr("Error parsing field %1, not a double")).arg(col_yr)))->raise();

    res.semester = entry.at(col_sem).toInt(&ok);
    if (!ok)
        (new DataMerger::Exception(QString(QObject::tr("Error parsing field %1, not a double")).arg(col_sem)))->raise();

    res.stock = getStockName(entry.at(col_stock));

    if (nodes.size() > 0)
        qDebug() << res.year << res.semester << res.stock << lat << lon << "Nodes: " << nodes.size();

    foreach (std::shared_ptr<NodeData> node, nodes) {
        QMutexLocker lock(&mutex);

        res.node = node->get_idx_node();
        ResultKey key = genResultKey(res);

        double dist;
        geod.Inverse(node->get_y(), node->get_x(), lat, lon, dist);

        if (dist < 1e-5) {
            // Change or insert the values. QMap::insert will replace the value if already present.

            res.centered = true;
            for (int i = 0; i < num_col_indiv; ++i) {
                res.population[i] = entry.at(col_indiv + i).toInt(&ok);
                if (!ok)
                    (new DataMerger::Exception(QString(QObject::tr("Error parsing field %1, not a double")).arg(col_indiv + i)))->raise();
                res.weights[i] = -1;    // not valid, not needed - see flag
            }

            mResults.insert(key, res);
        } else {
            auto vals = mResults.find(key);

            if (vals == mResults.end()) {
                vals = mResults.insert(key,res);       // weights and populations are initialized to 0, so it's ok to sum
            }
            for (int i = 0; i < num_col_indiv; ++i) {
                res.population[i] = entry.at(col_indiv + i).toInt(&ok) + vals.value().population[i];
                if (!ok)
                    (new DataMerger::Exception(QString(QObject::tr("Error parsing field %1, not a double")).arg(col_indiv + i)))->raise();
                res.weights[i] = 1.0/dist + vals.value().weights[i];
            }

            mResults.insert(key,res);
        }
    }
}

bool PopulationDistributionDataMergerStrategy::saveOutput(QString out)
{
    QVector<QFile*> outfiles(2 * mStockNames.size());
    QVector<QTextStream *> outstream(2 * mStockNames.size());
    try {
        QMap<QString, int>::const_iterator stkit = mStockNames.begin();
        while (stkit != mStockNames.end()) {
            for (int s = 0; s < 2; ++s) {
                QFile *f = new QFile(out.arg(stkit.value()).arg(s));

                qDebug() << "Save Output:"  << f->fileName();

                if (!f->open(QIODevice::WriteOnly | QIODevice::Truncate))
                    (new DataMerger::Exception(f->errorString()))->raise();
                outfiles [2*stkit.value() + s] = f;
                outstream[2*stkit.value() + s] = new QTextStream(outfiles[2*stkit.value() + s]);

                *outstream[2*stkit.value() + s] << "idx_node avai" << endl;
            }

            ++stkit;
        }

        qDebug() << "Output of " << mResults.size() << " to " << outfiles.size() << "Files.";
        QMap<ResultKey, Result>::iterator resl = mResults.begin();
        while (resl != mResults.end()) {
            double sumpop = 0;
            for (int i = 0; i < num_col_indiv; ++i) {
                sumpop += resl.value().population[i] / resl.value().weights[i];
            }

            for (int i = 0; i < num_col_indiv; ++i) {
                *outstream[2 * resl.value().stock + resl.value().semester] << resl.value().node << " " << resl.value().population[i] / sumpop << endl;
            }

            ++resl;
        }

    } catch (QException x) {
        foreach (QFile *f, outfiles) {
            f->close();
            delete f;
        }
        foreach (QTextStream *t, outstream) {
            delete t;
        }

        qDebug() << "Exception catched: " << x.what();
        throw;
    }

    foreach (QFile *f, outfiles) {
        f->close();
        delete f;
    }
    foreach (QTextStream *t, outstream) {
        delete t;
    }

    return true;

}

int PopulationDistributionDataMergerStrategy::getStockName(QString nm)
{
    QMutexLocker lock(&mutex);
    auto it = mStockNames.find(nm);

    if (it == mStockNames.end()) {
        // not found: put it
        int res = mStockNames.size();
        mStockNames.insert(nm, res);
        qDebug() << "New Stock " << nm << res;
        return res;
    } else {
        return it.value();
    }
}

PopulationDistributionDataMergerStrategy::ResultKey PopulationDistributionDataMergerStrategy::genResultKey(const PopulationDistributionDataMergerStrategy::Result &result)
{
    // Note: ignore the year.
    return result.node + (((result.semester) * 100 + result.stock) << 20);
}

