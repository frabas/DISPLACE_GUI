#include "populationdistributiondatamergerstrategy.h"

#include <GeographicLib/Geodesic.hpp>

#include <QDebug>

using namespace displace::workers;

const char *const PopulationDistributionDataMergerStrategy::YearField = "Year";
const char *const PopulationDistributionDataMergerStrategy::SemesterField = "Semester";
const char *const PopulationDistributionDataMergerStrategy::LatField = "ShootLat";
const char *const PopulationDistributionDataMergerStrategy::LongField = "ShootLon";
const char *const PopulationDistributionDataMergerStrategy::StockField = "StockId";
const char *const PopulationDistributionDataMergerStrategy::IndivFieldPattern = "nb_indiv.";

PopulationDistributionDataMergerStrategy::PopulationDistributionDataMergerStrategy (DisplaceModel *model)
    : DataMerger::Strategy(),
      mOwner(nullptr),
      mModel(model),
      mFilterStocks(true)
{
    mStockNames = mModel->getStockNames();
}

void PopulationDistributionDataMergerStrategy::attach(DataMerger *merger)
{
    mOwner = merger;
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
        (new displace::DisplaceException(QString(QObject::tr("Some required field is missing. Please check the input file."))))->raise();
    }
    return true;
}

void PopulationDistributionDataMergerStrategy::processLine (int linenum, QString line)
{
    bool ok;

    QStringList entry = line.split(mOwner->separator(), Qt::SplitBehaviorFlags::SkipEmptyParts);

    double lat = entry.at(col_lat).toDouble(&ok);
    if (!ok)
        (new displace::DisplaceException(QString(QObject::tr("Error parsing line %1 field %2")).arg(linenum).arg(col_lat)))->raise();

    double lon = entry.at(col_lon).toDouble(&ok);
    if (!ok)
        (new displace::DisplaceException(QString(QObject::tr("Error parsing line %1 field %2")).arg(linenum).arg(col_lat)))->raise();

    const GeographicLib::Geodesic& geod = GeographicLib::Geodesic::WGS84();

    // COLLECT NODE + Y/SEM/STOCK => u(x) w(x) per pop
    // See: http://en.wikipedia.org/wiki/Inverse_distance_weighting

    Result res(num_col_indiv);
    res.year = entry.at(col_yr).toInt(&ok);
    if (!ok)
        (new displace::DisplaceException(QString(QObject::tr("Error parsing line %1 field %2")).arg(linenum).arg(col_lat)))->raise();

    res.semester = entry.at(col_sem).toInt(&ok) -1;     // Semester is one-based on file.
    if (!ok)
        (new displace::DisplaceException(QString(QObject::tr("Error parsing line %1 field %2")).arg(linenum).arg(col_lat)))->raise();

    int stockid = getStockName(entry.at(col_stock));
    if (stockid == -1) // filter out unselected stocks
        return;

    res.stock = stockid;

//    if (nodes.size() > 0)
//        qDebug() << res.year << res.semester << res.stock << lat << lon << "Nodes: " << nodes.size();

    QList<std::shared_ptr<NodeData>> nodes = mOwner->getAllNodesWithin(QPointF(lon,lat), mOwner->distance());

    foreach (std::shared_ptr<NodeData> node, nodes) {
        QMutexLocker lock(&mutex);

        res.node = node->get_idx_node().toIndex();
        ResultKey key = genResultKey(res);

        double dist;
        geod.Inverse(node->get_y(), node->get_x(), lat, lon, dist);

        if (dist < 1e-5) {
            // Change or insert the values. QMap::insert will replace the value if already present.

            res.centered = true;
            for (int i = 0; i < num_col_indiv; ++i) {
                if (isGroupSelected(i)) {
                    res.population[i] = entry.at(col_indiv + i).toDouble(&ok);
                    if (!ok)
                        (new displace::DisplaceException(QString(QObject::tr("Error parsing line %1 field %2")).arg(linenum).arg(col_lat)))->raise();
                }
                res.weights[i] = -1;    // not valid, not needed - see flag
            }

            mResults.insert(key, res);
        } else {
            auto vals = mResults.find(key);

            if (vals == mResults.end()) {
                vals = mResults.insert(key,res);       // weights and populations are initialized to 0, so it's ok to sum
            }
            for (int i = 0; i < num_col_indiv; ++i) {
                if (isGroupSelected(i)) {
                    res.population[i] = entry.at(col_indiv + i).toDouble(&ok) + vals.value().population[i];
                    if (!ok)
                        (new displace::DisplaceException(QString(QObject::tr("Error parsing line %1 field %2")).arg(linenum).arg(col_lat)))->raise();
                    res.weights[i] = 1.0/dist + vals.value().weights[i];
                }
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
                QFile *f = new QFile(out.arg(stkit.key()).arg(s+1));

                qDebug() << "Save Output:"  << f->fileName();

                if (!f->open(QIODevice::WriteOnly | QIODevice::Truncate))
                    (new displace::DisplaceException(f->errorString()))->raise();
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
                if (isGroupSelected(i))
                    sumpop += resl.value().population[i] / resl.value().weights[i];
            }

            for (int i = 0; i < num_col_indiv; ++i) {
                if (isGroupSelected(i)) {
                    double v = (std::abs(sumpop) < 1e-5 ? 0 : resl.value().population[i] / sumpop);
                    *outstream[2 * resl.value().stock + resl.value().semester] << resl.value().node << " " << v << endl;
                }
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

    if (!mPopOutFileName.isEmpty()) {
        QFile f(mPopOutFileName);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            (new displace::DisplaceException(f.errorString()))->raise();
        }

        QTextStream ps (&f);

        QList<QString> keys = mStockNames.keys();
        for (int i = 0; i < keys.size(); ++i) {
            ps << i << " " << keys[i] << Qt::endl;
        }
        f.close();
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

void PopulationDistributionDataMergerStrategy::setStocks(QStringList stocks)
{
    foreach (QString stock, stocks) {
        int res = mStockNames.size();
        mStockNames.insert(stock, res);
    }
}

void PopulationDistributionDataMergerStrategy::setGroups(QList<int> groups)
{
    foreach (int i, groups) {
        while (mGroups.size() <= i)
            mGroups.push_back(false);
        mGroups[i] = true;
    }
}

void PopulationDistributionDataMergerStrategy::setPopulationOutputFileName(QString name)
{
    mPopOutFileName = name;
}

int PopulationDistributionDataMergerStrategy::getStockName(QString nm)
{
    QMutexLocker lock(&mutex);
    auto it = mStockNames.find(nm);

    if (it == mStockNames.end()) {
        if (!mFilterStocks) {
            // not found: put it
            int res = mStockNames.size();
            mStockNames.insert(nm, res);
            qDebug() << "New Stock " << nm << res;
            return res;
        } else {
            return -1;
        }
    } else {
        return it.value();
    }
}

bool PopulationDistributionDataMergerStrategy::isGroupSelected(int idx)
{
    return (idx < mGroups.size() ? mGroups[idx] : false);
}

PopulationDistributionDataMergerStrategy::ResultKey PopulationDistributionDataMergerStrategy::genResultKey(const PopulationDistributionDataMergerStrategy::Result &result)
{
    // Note: ignore the year.
    return result.node + (((result.semester) * 100 + result.stock) << 20);
}

