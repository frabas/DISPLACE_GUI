// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2019 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "displacemodel.h"
#include <exceptions.h>
#include <dbhelper.h>
#include <calendar.h>
#include <modelobjects/metierdata.h>
#include "storage/modelmetadataaccessor.h"
#include "mapsdataprovider.h"

#include <mapobjects/harbourmapobject.h>
#include <profiler.h>

#include <mapobjectscontroller.h>
#include <GeographicLib/Geodesic.hpp>

#include <readdata.h>
#include <qdebug.h>
#include <QtAlgorithms>
#include <QtDebug>

#include "storage/sqliteoutputstorage.h"
#include "msqlitecpp/v1/sqlitestorage.h"

const char *FLD_TYPE = "type";
const char *FLD_NODEID = "nodeid";
const char *FLD_EDGEID = "edgeid";

DisplaceModel::DisplaceModel()
        : mModelType(EmptyModelType),
          mDb(0),
          mCalendar(),
#if defined (_WIN32)
        mOutDir("C:"),
#endif
          mInputName(), mBasePath(), mOutputName(),
          mSimuName("simu1"),
          mLinkedDbName(),
          mIndex(-1),
          mSimulSteps(8762),
          mUseStaticPaths(0),
          mCurrentStep(0), mLastStep(0),
          mLastStats(-1),
          mNodesStatsDirty(false),
          mVesselsStatsDirty(false),
          mFirmsStatsDirty(false),
          mShipsStatsDirty(false),
          mScenario(),
          mConfig(),
          mInterestingPop(),
          mInterestingPop2(),
          mInterestingSizeTotal(false),
          mInterestingSizeAvg(true),
          mInterestingSizeMin(false),
          mInterestingSizeMax(false),
          mInterestingSizes(),
          mFuncGroups(std::make_shared<InterestingListWithSpecialValues<int>>()),
          mFishfarmsTypes(std::make_shared<InterestingListWithSpecialValues<int>>()),
          mWindfarmsTypes(std::make_shared<InterestingListWithSpecialValues<int>>()),
          mShipsTypes(std::make_shared<InterestingListWithSpecialValues<int>>()),
          mOutputFileParser(new OutputFileParser(this)),
          mParserThread(new QThread(this)),
          mShortestPathFolder()
{
    OGRRegisterAll();

    const char *pszDriverName = "Memory";
    GDALDriver *poDriver;
    poDriver = (GDALDriver *) GDALGetDriverByName(pszDriverName);

    mMapsDataProvider = std::make_shared<MapsDataProvider>();
    mDataSource = poDriver->Create("memory", 0, 0, 0, GDT_Unknown, nullptr);
    Q_ASSERT(mDataSource);

    mSpatialRef = new OGRSpatialReference();
    mSpatialRef->SetWellKnownGeogCS("WGS84");

    createFeaturesLayer();

    mOutputFileParser->moveToThread(mParserThread);
    mParserThread->start();

    connect(this, SIGNAL(parseOutput(QString, int)), mOutputFileParser, SLOT(parse(QString, int)));
    connect(mOutputFileParser, SIGNAL(error(QString)), SIGNAL(errorParsingStatsFile(QString)));
    //connect (mOutputFileParser, SIGNAL(parseCompleted()), SIGNAL(outputParsed()));
    connect(mOutputFileParser, &OutputFileParser::parseCompleted, [this]() {
        emit outputParsed();
    });

    mFuncGroups->setValuesFormatString(tr("Functional Group #%1"));
    mFuncGroups->addSpecialValue(tr("Total"));
    mFuncGroups->addSpecialValue(tr("Average"), true);
    mFuncGroups->addSpecialValue(tr("Min"));
    mFuncGroups->addSpecialValue(tr("Max"));

    mFishfarmsTypes->setValuesFormatString(tr("Fishfarm Type #%1"));
    mFishfarmsTypes->addSpecialValue(tr("Total"));
    mFishfarmsTypes->addSpecialValue(tr("Average"), true);
    mFishfarmsTypes->addSpecialValue(tr("Min"));
    mFishfarmsTypes->addSpecialValue(tr("Max"));

    mWindfarmsTypes->setValuesFormatString(tr("Windfarm Type #%1"));
    mWindfarmsTypes->addSpecialValue(tr("Total"));
    mWindfarmsTypes->addSpecialValue(tr("Average"), true);
    mWindfarmsTypes->addSpecialValue(tr("Min"));
    mWindfarmsTypes->addSpecialValue(tr("Max"));

    mShipsTypes->setValuesFormatString(tr("Ship Type #%1"));
    mShipsTypes->addSpecialValue(tr("Total"));
    mShipsTypes->addSpecialValue(tr("Average"), true);
    mShipsTypes->addSpecialValue(tr("Min"));
    mShipsTypes->addSpecialValue(tr("Max"));
}

DisplaceModel::~DisplaceModel()
{
    if (mParserThread->isRunning()) {
        mParserThread->quit();
        mParserThread->wait(2000);
    }
}

MapsDataProvider &DisplaceModel::getMapDataProvider()
{
    return *mMapsDataProvider;
}

void DisplaceModel::createFeaturesLayer()
{
    mNodesLayerIndex = mDataSource->GetLayerCount();
    mNodesLayer = mDataSource->CreateLayer("nodes", mSpatialRef, wkbPoint);
    Q_ASSERT(mNodesLayer);

    // Create any field
    OGRFieldDefn fld2(FLD_TYPE, OFTInteger);
    mNodesLayer->CreateField(&fld2);
    OGRFieldDefn fld(FLD_NODEID, OFTInteger);
    mNodesLayer->CreateField(&fld);
    OGRFieldDefn fld3(FLD_EDGEID, OFTInteger);
    mNodesLayer->CreateField(&fld3);

}

bool DisplaceModel::edit(QString modelname)
{
    if (mModelType != EmptyModelType) {
        return false;
    }

    // ...

    mInputName = modelname;
    mModelType = EditorModelType;

    return true;
}

bool DisplaceModel::load(QString path, ModelType type)
{
    if (mModelType != EmptyModelType) {
        return false;
    }

    qDebug() << "Loading model from folder" << path;

    if (!parse(path, &mBasePath, &mInputName, &mOutputName)) {
        mLastError = tr("Cannot parse path for base name and simulation name.");
        return false;
    }
    mFullPath = path;

    qDebug() << "Loading: " << mBasePath << mInputName << mOutputName;

    /* Load files ... */

    try {
        mScenario = Scenario::readFromFile(mInputName, mBasePath, mOutputName);
        mConfig = Config::readFromFile(mInputName, mBasePath, mOutputName);
        mCalendar = std::shared_ptr<Calendar>(Calendar::load(mBasePath, mInputName));

        QString shortestPath = QString("%1/shortPaths_%2_a_graph%3")
                .arg(mBasePath).arg(mInputName).arg(mScenario.getGraph());

        if (QFile(shortestPath).exists()) {
            qDebug() << "linking shortest path folder: " << shortestPath;
            linkShortestPathFolder(shortestPath);
        } else {
            qDebug() << shortestPath << "doesn't exist";
        }
        mInterestingHarb = mConfig.m_interesting_harbours;


        loadNodes();
        if (!loadVessels()) {
            throw DisplaceException("Cannot read Vessels Features");
        }
        loadGraphs();
        if (!initShips()) {
            throw DisplaceException("Cannot read Ships Features");
        }

        initFirm();

        initFishfarm();
        initWindmill();
        initBenthos();
        initPopulations();
        initNations();
    } catch (DisplaceException &ex) {
        mLastError = ex.what();
        qDebug() << "ERROR: " << mLastError;
        return false;
    }

    cout << "load...ok" << endl;
    mModelType = type;
    return true;
}

bool DisplaceModel::parse(const QString &path, QString *basepath, QString *inputname, QString *outputname)
{
    // parse this form:  inputfolder + "/simusspe_" + folder_name_parameterization + "/" + namefolderoutput+".dat";

    QRegExp regexp("(.*)/simusspe_([^/]+)/([^/]+).dat");

    if (regexp.indexIn(path) == -1) {
        return false;
    }

    *basepath = regexp.cap(1);
    *inputname = regexp.cap(2);
    *outputname = regexp.cap(3);

    return true;
}

bool DisplaceModel::loadDatabase(QString path)
{
    if (mModelType != EmptyModelType) {
        return false;
    }
    try {
        setSimulationSqlStorage(path);
        if (mOutSqlite->isOutdated()) {
            return false;
        }

        ModelMetadataAccessor accessor(mOutSqlite->metadata());
        mConfig.setNbpops(accessor.nbPops());
        mConfig.setSzGroups(accessor.nbSize());
        mConfig.setNbbenthospops(accessor.nbBenthos());
        mCalendar = std::shared_ptr<Calendar>(Calendar::build(mOutSqlite));

        mLastStep = accessor.lastTStep();
        auto nl = mOutSqlite->getNationsList();
        mNations.clear();
        for (auto n : nl)
            mNations.push_back(std::make_shared<NationData>(QString::fromStdString(n)));

        mModelType = ModelType::OfflineModelType;

        loadNodesFromDb();
        loadVesselsFromDb();
        loadFishfarmsFromDb();
        initBenthos();

        setCurrentStep(mLastStep);
    } catch (sqlite::SQLiteException &x) {
        qWarning() << "Error loading db: " << x.what();
        return false;
    }
    return true;
}

bool DisplaceModel::clearStats()
{
    m_vessel_last_step = -1;
    mVesselsStatsDirty = false;

    mStatsBenthos.clear();
    mStatsBenthosCollected.clear();

    mStatsFishfarms.clear();
    mStatsFishfarmsCollected.clear();

    mStatsShips.clear();
    mStatsShipsCollected.clear();

    return true;
}

bool DisplaceModel::saveScenarioAs(const QString &path)
{
    if (!parse(path, &mBasePath, &mInputName, &mOutputName)) {
        return false;
    }

    mFullPath = path;
    return saveScenario();
}

bool DisplaceModel::saveScenario()
{
    QString error;
    if (!mScenario.save(mBasePath, mInputName, mOutputName, &error)) {
        mLastError = error;
        return false;
    }

    return true;
}

bool DisplaceModel::saveConfig()
{
    QString error;
    if (!mConfig.save(mBasePath, mInputName, mOutputName, &error)) {
        mLastError = error;
        return false;
    }

    return true;
}

void DisplaceModel::simulationEnded()
{
}

void DisplaceModel::setSimulationSqlStorage(const QString &path)
{
    mOutSqlite = std::make_shared<SQLiteOutputStorage>(path.toStdString());
    mOutSqlite->open();
    if (mDb) { delete mDb; }
    mDb = new DbHelper;
    mDb->attachDb(mOutSqlite);
    mMapsDataProvider->attach(mOutSqlite);
}

int DisplaceModel::getBenthosIdx(int benthosId) const
{
    auto p = mBenthosInfo.find(benthosId);
    if (p == mBenthosInfo.end()) { // not found throw exception
        throw std::runtime_error("Benthos not found");
    }

    return (*p)->getIdx();
}


int DisplaceModel::getHarboursCount() const
{
    return mHarbours.size();
}

QString DisplaceModel::getHarbourId(int idx) const
{
    return QString::fromStdString(mHarbours.at(idx)->mHarbour->get_name());
}

int DisplaceModel::getNodesCount() const
{
    return mNodes.size();
}

QString DisplaceModel::getNodeId(int idx) const
{
    // return QString::fromStdString(mNodes.at(idx)->get_name());
    return QString::number(mNodes.at(idx)->get_idx_node().toIndex());
}

QList<std::shared_ptr<NodeData> > DisplaceModel::getAllNodesWithin(const QPointF &centerpoint, double dist_km) const
{
    QList<std::shared_ptr<NodeData> > nodes;

#if GEOGRAPHICLIB_VERSION_MINOR > 25
    const GeographicLib::Geodesic &geod = GeographicLib::Geodesic::WGS84();
#else
    const GeographicLib::Geodesic& geod = GeographicLib::Geodesic::WGS84;
#endif

    double mx, my, Mx, My, d;
    geod.Direct(centerpoint.y(), centerpoint.x(), 0, dist_km * 1000, My, d);
    geod.Direct(centerpoint.y(), centerpoint.x(), 90, dist_km * 1000, d, Mx);
    geod.Direct(centerpoint.y(), centerpoint.x(), 180, dist_km * 1000, my, d);
    geod.Direct(centerpoint.y(), centerpoint.x(), 270, dist_km * 1000, d, mx);

    mMutex.lock();
    mNodesLayer->ResetReading();
    mNodesLayer->SetSpatialFilterRect(mx, my, Mx, My);
    OGRFeature *f;
    while ((f = mNodesLayer->GetNextFeature()) != 0) {
        if (f->GetFieldAsInteger(FLD_TYPE) == (int) OgrTypeNode) {
            int nodeid = f->GetFieldAsInteger(FLD_NODEID);
            nodes.push_back(mNodes[nodeid]);
        }
    }
    mMutex.unlock();

    return nodes;
}

void DisplaceModel::checkStatsCollection(int tstep)
{
    if (mLastStats != tstep && mNodesStatsDirty) {
        commitNodesStatsFromSimu(tstep);
    }

    mLastStats = tstep;
}

int DisplaceModel::getSimulationSteps() const
{
    return mSimulSteps;
}

QString DisplaceModel::getOutDir() const
{
    return mOutDir;
}


void DisplaceModel::setSimulationSteps(int value)
{
    mSimulSteps = value;
}

int DisplaceModel::getUseStaticPaths() const
{
    return mUseStaticPaths;
}

void DisplaceModel::setUseStaticPaths(int value)
{
    mUseStaticPaths = value;
}


void DisplaceModel::updateNodesStatFromSimu(QString data)
{
    QStringList fields = data.split(",");
    int tstep = fields[1].toInt();
    int start = fields[2].toInt();
    int num = fields[3].toInt();

    checkStatsCollection(tstep);

    if (fields[0] == "cumftime") {
        for (int i = 0; i < num; ++i) {
            mNodes.at(start + i)->set_cumftime(fields[4 + i].toDouble());
        }
        mNodesStatsDirty = true;
    }
    if (fields[0] == "cumsweptarea") {
        for (int i = 0; i < num; ++i) {
            mNodes.at(start + i)->set_cumsweptarea(fields[4 + i].toDouble());
        }
        mNodesStatsDirty = true;
    }
    if (fields[0] == "cumsubsurfacesweptarea") {
        for (int i = 0; i < num; ++i) {
            mNodes.at(start + i)->set_cumsubsurfacesweptarea(fields[4 + i].toDouble());
        }
        mNodesStatsDirty = true;
    }
    if (fields[0] == "cumcatches") {
        for (int i = 0; i < num; ++i) {
            mNodes.at(start + i)->set_cumcatches(fields[4 + i].toDouble());
        }
        mNodesStatsDirty = true;
    }
    if (fields[0] == "cumcatches_with_threshold") {
        for (int i = 0; i < num; ++i) {
            mNodes.at(start + i)->set_cumcatches_with_threshold(fields[4 + i].toDouble());
        }
        mNodesStatsDirty = true;
    }
    if (fields[0] == "cumdiscards") {
        for (int i = 0; i < num; ++i) {
            mNodes.at(start + i)->set_cumdiscards(fields[4 + i].toDouble());
        }
        mNodesStatsDirty = true;
    }

    if (fields[0] == "salinity") {
        for (int i = 0; i < num; ++i) {
            mNodes.at(start + i)->setSalinity(fields[4 + i].toDouble());
        }
        mNodesStatsDirty = true;
    }


}

void DisplaceModel::commitFishfarmsStats(int tstep)
{
    if (mStatsFishfarmsCollected.dirty()) {
        // Fishfarm stats are not saved on db, but loaded on the fly
        mStatsFishfarms.insertValue(tstep, mStatsFishfarmsCollected);
        mStatsFishfarmsCollected.clear();
    }
}

void DisplaceModel::commitShipsStats(int tstep)
{
    if (mStatsShipsCollected.dirty()) {
        // Fishfarm stats are not saved on db, but loaded on the fly
        mStatsShips.insertValue(tstep, mStatsShipsCollected);
        mStatsShipsCollected.clear();
    }
}

void DisplaceModel::commitNodesStatsFromSimu(int tstep, bool force)
{
    if (mNodesStatsDirty || force) {
        mNodesStatsDirty = false;
    }

    if (mShipsStatsDirty || force) {
        mShipsStatsDirty = false;
    }
}

void DisplaceModel::startCollectingStats()
{
}

void DisplaceModel::endCollectingStats()
{
}

void
DisplaceModel::collectNodePopStats(int tstep, int node_idx, const QList<double> &stats, const QList<double> &stats_w,
                                   double tot, double wtot)
{
    checkStatsCollection(tstep);
    mNodes.at(node_idx)->setPop(stats, tot);
    mNodes.at(node_idx)->setPopW(stats_w, wtot);

    mNodesStatsDirty = true;
}

void DisplaceModel::collectPopCumftime(int step, int node_idx, double cumftime)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->set_cumftime(cumftime);
    mNodesStatsDirty = true;
}

void DisplaceModel::collectPopCumsweptarea(int step, int node_idx, double cumsweptarea)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->set_cumsweptarea(cumsweptarea);
    mNodesStatsDirty = true;
}

void DisplaceModel::collectPopCumsubsurfacesweptarea(int step, int node_idx, double cumsubsurfacesweptarea)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->set_cumsubsurfacesweptarea(cumsubsurfacesweptarea);
    mNodesStatsDirty = true;
}

void DisplaceModel::collectPopCumcatches(int step, int node_idx, double cumcatches)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->set_cumcatches(cumcatches);
    mNodesStatsDirty = true;
}

void DisplaceModel::collectPopCumcatchesWithThreshold(int step, int node_idx, double cumcatches_with_threshold)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->set_cumcatches_with_threshold(cumcatches_with_threshold);
    mNodesStatsDirty = true;
}

void DisplaceModel::collectPopCumdiscards(int step, int node_idx, double cumdiscards)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->set_cumdiscards(cumdiscards);
    mNodesStatsDirty = true;
}

void DisplaceModel::collectPopCumdiscardsratio(int step, int node_idx, double cumdiscardsratio)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->set_cumdiscardsratio(cumdiscardsratio);
    mNodesStatsDirty = true;
}

void DisplaceModel::collectPopNbchoked(int step, int node_idx, double nbchoked)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->set_nbchoked(nbchoked);
    mNodesStatsDirty = true;
}

void DisplaceModel::collectPopTariffs(int step, int node_idx, vector<double> tariffs)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->set_tariffs(tariffs);
    mNodesStatsDirty = true;
}


void DisplaceModel::collectPopImpact(int step, int node_idx, int popid, double impact)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->setImpact(popid, impact);
    mNodesStatsDirty = true;
}

void DisplaceModel::collectPopCumcatchesPerPop(int step, int node_idx, int popid, double cumcatchesperpop)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->setCumcatchesPerPop(popid, cumcatchesperpop);
    mNodesStatsDirty = true;
}

void DisplaceModel::collectBathymetry(int step, int node_idx, double bathy)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->setBathymetry(bathy);
    mNodesStatsDirty = true;
}

void DisplaceModel::collectShippingdensity(int step, int node_idx, double shippingdensity)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->setShippingdensity(shippingdensity);
    mNodesStatsDirty = true;
}

void DisplaceModel::collectSiltfraction(int step, int node_idx, double siltfraction)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->setSiltfraction(siltfraction);
    mNodesStatsDirty = true;
}


void DisplaceModel::collectWind(int step, int node_idx, double wind)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->setWind(wind);
    mNodesStatsDirty = true;
}

void DisplaceModel::collectSalinity(int step, int node_idx, double salinity)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->setSalinity(salinity);
    mNodesStatsDirty = true;
}

void DisplaceModel::collectSST(int step, int node_idx, double sst)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->setSST(sst);
    mNodesStatsDirty = true;
}

void DisplaceModel::collectNitrogen(int step, int node_idx, double nitrogen)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->setNitrogen(nitrogen);
    mNodesStatsDirty = true;
}

void DisplaceModel::collectPhosphorus(int step, int node_idx, double phosphorus)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->setPhosphorus(phosphorus);
    mNodesStatsDirty = true;
}

void DisplaceModel::collectOxygen(int step, int node_idx, double oxygen)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->setOxygen(oxygen);
    mNodesStatsDirty = true;
}

void DisplaceModel::collectDissolvedCarbon(int step, int node_idx, double dissolvedcarbon)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->setDissolvedCarbon(dissolvedcarbon);
    mNodesStatsDirty = true;
}

void DisplaceModel::collectPopBenthosBiomass(int step, int node_idx, int funcid, double benthosbiomass)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->setBenthosBiomass(funcid, benthosbiomass);
    mNodesStatsDirty = true;

    mStatsBenthosCollected.collectBiomass(step, funcid,
                                          getBenthosIdx(mNodes.at(node_idx)->get_marine_landscape()),
                                          benthosbiomass);
}

void DisplaceModel::collectPopBenthosNumber(int step, int node_idx, int funcid, double benthosnumber)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->setBenthosNumber(funcid, benthosnumber);
    mNodesStatsDirty = true;

    mStatsBenthosCollected.collectNumber(step, funcid,
                                         getBenthosIdx(mNodes.at(node_idx)->get_marine_landscape()),
                                         benthosnumber);
}

void DisplaceModel::collectPopBenthosMeanWeight(int step, int node_idx, int funcid, double meanweight)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->setBenthosMeanweight(funcid, meanweight);
    mNodesStatsDirty = true;

    mStatsBenthosCollected.collectMeanWeight(step, funcid,
                                             getBenthosIdx(mNodes.at(node_idx)->get_marine_landscape()),
                                             meanweight);
}


void DisplaceModel::collectPopBenthosBiomassOverK(int step, int node_idx, int funcid, double benthosbiomassoverK)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->setBenthosBiomassOverK(funcid, benthosbiomassoverK);
    mNodesStatsDirty = true;

    mStatsBenthosCollected.collectBiomassOverK(step, funcid,
                                               getBenthosIdx(mNodes.at(node_idx)->get_marine_landscape()),
                                               benthosbiomassoverK);
}

void DisplaceModel::collectPopBenthosNumberOverK(int step, int node_idx, int funcid, double benthosnumberoverK)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->setBenthosNumberOverK(funcid, benthosnumberoverK);
    mNodesStatsDirty = true;

    mStatsBenthosCollected.collectNumberOverK(step, funcid,
                                              getBenthosIdx(mNodes.at(node_idx)->get_marine_landscape()),
                                              benthosnumberoverK);
}

void DisplaceModel::commitPopBenthosStats(int tstep)
{
    if (mStatsBenthosCollected.dirty()) {
        mStatsBenthos.insertValue(tstep, mStatsBenthosCollected);
        mStatsBenthosCollected.setDirty(false);
    }

    // Clear the statistics at end.
#if 0
    if (mCalendar && mCalendar->isYear(tstep)) {
        mStatsBenthosCollected.clear();
    }
#endif
}

void DisplaceModel::collectFishfarmFishMeanWeight(int step, int node_idx, int farmid, int farmtype, double meanw_kg)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->setFishfarmFishMeanWeight(farmtype, meanw_kg);
    mNodesStatsDirty = true;


    mStatsFishfarmsCollected.collectMeanWeightKg(step,
                                                 farmid, // mNodes.at(node_idx)->get_ff_names_on_node().at(0) does not work
                                                 farmtype,
                                                 meanw_kg);
}

void DisplaceModel::collectFishfarmFishHarvestedKg(int step, int node_idx, int farmid, int farmtype,
                                                   double fish_harvested_kg)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->setFishfarmFishHarvestedKg(farmtype, fish_harvested_kg);
    mNodesStatsDirty = true;

    mStatsFishfarmsCollected.collectFishHarvestedKg(step,
                                                    farmid,
                                                    farmtype,
                                                    fish_harvested_kg);
}

void DisplaceModel::collectFishfarmEggsHarvestedKg(int step, int node_idx, int farmid, int farmtype,
                                                   double eggs_harvested_kg)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->setFishfarmEggsHarvestedKg(farmtype, eggs_harvested_kg);
    mNodesStatsDirty = true;

    mStatsFishfarmsCollected.collectEggsHarvestedKg(step,
                                                    farmid,
                                                    farmtype,
                                                    eggs_harvested_kg);
}


void DisplaceModel::collectFishfarmAnnualProfit(int step, int node_idx, int farmid, int farmtype,
                                                double fishfarm_annualprofit)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->setFishfarmAnnualProfit(farmtype, fishfarm_annualprofit);
    mNodesStatsDirty = true;

    mStatsFishfarmsCollected.collectFishfarmAnnualProfit(step,
                                                         farmid,
                                                         farmtype,
                                                         fishfarm_annualprofit);
}

void DisplaceModel::collectFishfarmNetDischargeN(int step, int node_idx, int farmid, int farmtype,
                                                 double fishfarm_netdischargeN)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->setFishfarmNetDischargeN(farmtype, fishfarm_netdischargeN);
    mNodesStatsDirty = true;

    mStatsFishfarmsCollected.collectNetDischargeN(step,
                                                  farmid,
                                                  farmtype,
                                                  fishfarm_netdischargeN);
}


void DisplaceModel::collectFishfarmNetDischargeP(int step, int node_idx, int farmid, int farmtype,
                                                 double fishfarm_netdischargeP)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->setFishfarmNetDischargeP(farmtype, fishfarm_netdischargeP);
    mNodesStatsDirty = true;

    mStatsFishfarmsCollected.collectNetDischargeP(step,
                                                  farmid,
                                                  farmtype,
                                                  fishfarm_netdischargeP);
}

void DisplaceModel::collectFishfarmCumulNetDischargeN(int step, int node_idx, int farmid, int farmtype,
                                                      double fishfarm_cumulnetdischargeN)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->setFishfarmCumulNetDischargeN(farmtype, fishfarm_cumulnetdischargeN);
    mNodesStatsDirty = true;

    mStatsFishfarmsCollected.collectCumulNetDischargeN(step,
                                                       farmid,
                                                       farmtype,
                                                       fishfarm_cumulnetdischargeN);
}


void DisplaceModel::collectFishfarmCumulNetDischargeP(int step, int node_idx, int farmid, int farmtype,
                                                      double fishfarm_cumulnetdischargeP)
{
    checkStatsCollection(step);
    mNodes.at(node_idx)->setFishfarmCumulNetDischargeP(farmtype, fishfarm_cumulnetdischargeP);
    mNodesStatsDirty = true;

    mStatsFishfarmsCollected.collectCumulNetDischargeP(step,
                                                       farmid,
                                                       farmtype,
                                                       fishfarm_cumulnetdischargeP);
}


void DisplaceModel::collectShipNbTransportedUnits(int step, int node_idx, int shipid, int shiptype,
                                                  double nb_transported_units)
{
    mStatsShipsCollected.collectNbTransportedUnits(step,
                                                   shipid,
                                                   shiptype,
                                                   nb_transported_units);
}


void DisplaceModel::collectShipFuelUseHour(int step, int node_idx, int shipid, int shiptype, double fuel_use_h)
{
    mStatsShipsCollected.collectFuelUseHour(step,
                                            shipid,
                                            shiptype,
                                            fuel_use_h);
}

void DisplaceModel::collectShipNOxEmissiongPerkW(int step, int node_idx, int shipid, int shiptype,
                                                 double NOx_emission_gperkW)
{
    mStatsShipsCollected.collectNOxEmissiongPerkW(step,
                                                  shipid,
                                                  shiptype,
                                                  NOx_emission_gperkW);
}

void DisplaceModel::collectShipSOxEmissionPercentPerTotalFuelmass(int step, int node_idx, int shipid, int shiptype,
                                                                  double SOx_emission_percentpertotalfuelmass)
{
    mStatsShipsCollected.collectSOxEmissionPercentPerTotalFuelmass(step,
                                                                   shipid,
                                                                   shiptype,
                                                                   SOx_emission_percentpertotalfuelmass);
}


void DisplaceModel::collectShipGHGemissiongPerkW(int step, int node_idx, int shipid, int shiptype,
                                                 double GHG_emission_gperkW)
{
    mStatsShipsCollected.collectGHGemissiongPerkW(step,
                                                  shipid,
                                                  shiptype,
                                                  GHG_emission_gperkW);
}

void DisplaceModel::collectShipPMEemissiongPerkW(int step, int node_idx, int shipid, int shiptype,
                                                 double PME_emission_gperkW)
{
    mStatsShipsCollected.collectPMEemissiongPerkW(step,
                                                  shipid,
                                                  shiptype,
                                                  PME_emission_gperkW);
}


void DisplaceModel::collectShipfuelUseLitre(int step, int node_idx, int shipid, int shiptype, double fuel_use_litre)
{
    mStatsShipsCollected.collectfuelUseLitre(step,
                                             shipid,
                                             shiptype,
                                             fuel_use_litre);
}

void DisplaceModel::collectShipNOxEmission(int step, int node_idx, int shipid, int shiptype, double NOx_emission)
{
    mStatsShipsCollected.collectNOxEmission(step,
                                            shipid,
                                            shiptype,
                                            NOx_emission);
}

void DisplaceModel::collectShipSOxEmission(int step, int node_idx, int shipid, int shiptype, double SOx_emission)
{
    mStatsShipsCollected.collectSOxEmission(step,
                                            shipid,
                                            shiptype,
                                            SOx_emission);
}

void DisplaceModel::collectShipGHGemission(int step, int node_idx, int shipid, int shiptype, double GHG_emission)
{
    mStatsShipsCollected.collectGHGemission(step,
                                            shipid,
                                            shiptype,
                                            GHG_emission);
}

void DisplaceModel::collectShipPMEemission(int step, int node_idx, int shipid, int shiptype, double PME_emission)
{
    mStatsShipsCollected.collectPMEemission(step,
                                            shipid,
                                            shiptype,
                                            PME_emission);
}

void DisplaceModel::collectVesselStats(int tstep, const VesselStats &stats)
{
    if (m_vessel_last_step != -1 && tstep != m_vessel_last_step) {
        commitVesselsStats(m_vessel_last_step);
    }

    std::shared_ptr<VesselData> vessel;
    if (stats.vesselId != -1 && stats.vesselId < mVessels.size()) {
        vessel = mVessels.at(stats.vesselId);
    }

    int nat = -1;
    if (vessel) {
        vessel->setLastHarbour(stats.lastHarbour);
        vessel->setRevenueAV(stats.revenueAV);
        vessel->setRevenueExAV(stats.revenueExAV);
        vessel->mVessel->set_reason_to_go_back(stats.reasonToGoBack);
        vessel->mVessel->set_timeatsea(stats.timeAtSea);
        vessel->addCumFuelCons(stats.cumFuelCons);
        vessel->addSweptArea(stats.sweptArea);
        vessel->addRevenuePerSweptArea(stats.revenuePerSweptArea);
        vessel->addVpuf(stats.vpuf);
        vessel->setGVA(stats.GVA);
        vessel->setGVAPerRevenue(stats.GVAPerRevenue);
        vessel->setLabourSurplus(stats.LabourSurplus);
        vessel->setGrossProfit(stats.GrossProfit);
        vessel->setNetProfit(stats.NetProfit);
        vessel->setNetProfitMargin(stats.NetProfitMargin);
        vessel->setGVAPerFTE(stats.GVAPerFTE);
        vessel->setRoFTA(stats.RoFTA);
        vessel->setBER(stats.BER);
        vessel->setCRBER(stats.CRBER);
        vessel->setNetPresentValue(stats.NetPresentValue);
        vessel->setNumTrips(stats.numTrips);


        nat = vessel->getNationality();
    } else {
        // retrieve nationality from vessel name
        QString natname = QString::fromStdString(Vessel::nationalityFromName(stats.name));
        // TODO this is not optimal.

        int i;
        for (i = 0; i < mNations.size(); ++i) {
            auto n = mNations[i];
            if (n->getName() == natname) {
                break;
            }
        }
        if (i == mNations.size()) { // nation not found, add it
            auto nn = std::make_shared<NationData>();
            nn->setName(natname);
            mNations.push_back(nn);
            nat = i;
        }
    }

    int n = stats.mCatches.size();
    for (int i = 0; i < n; ++i) {
        if (vessel) {
            vessel->addCatch(i, stats.mCatches[i]);
        }
    }

    int n2 = stats.mDiscards.size();
    for (int i = 0; i < n2; ++i) {
        if (vessel) {
            vessel->addDiscard(i, stats.mDiscards[i]);
        }
    }

    mVesselsStatsDirty = true;
    m_vessel_last_step = tstep;
}

void DisplaceModel::commitVesselsStats(int tstep)
{
    if (mVesselsStatsDirty) {
        mVesselsStatsDirty = false;
    }
}

void DisplaceModel::clearAllNodes()
{
    mNodes.clear();
    mHarbours.clear();
    mPenaltyNodes.clear();
    mDataSource->DeleteLayer(mNodesLayerIndex);
    createFeaturesLayer();
}

bool DisplaceModel::addGraph(const QList<GraphBuilder::Node> &nodes, MapObjectsController *controller)
{
    if (mModelType != EditorModelType) {
        return false;
    }

    QList<std::shared_ptr<NodeData> > newnodes;
    QList<std::shared_ptr<HarbourData> > newharbours;
    QList<types::NodeId> translated_nodes;
    int nodeidx = mNodes.count();
    int cntr = 0;
            foreach(GraphBuilder::Node node, nodes) {
            if (node.good) {
//            int nodeid = mNodes.size();

                OGRFeature *feature = OGRFeature::CreateFeature(mNodesLayer->GetLayerDefn());
                feature->SetField(FLD_TYPE, (int) OgrTypeNode);
                feature->SetField(FLD_NODEID, nodeidx + cntr);     // was nodeid

                OGRPoint pt;
                pt.setX(node.point.x());
                pt.setY(node.point.y());

                feature->SetGeometry(&pt);

                mNodesLayer->CreateFeature(feature);

                std::shared_ptr<Node> nd;

                translated_nodes.push_back(types::NodeId(nodeidx + cntr));
                if (node.harbour) {
                    std::shared_ptr<Harbour> h(
                            new Harbour(types::NodeId(nodeidx + cntr), node.point.x(), node.point.y(), node.harbour));
                    nd = h;
                    std::shared_ptr<HarbourData> hd(new HarbourData(h));
                    mHarbours.push_back(hd);
                    newharbours.push_back(hd);
                } else {
                    nd = std::shared_ptr<Node>(
                            new Node(types::NodeId(nodeidx + cntr), node.point.x(), node.point.y(), 0, 0, 0, 0, 0, 0, 0,
                                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                     0, 0));
                }

                std::shared_ptr<NodeData> nodedata(new NodeData(nd, this));

                mNodes.push_back(nodedata);

                if (!node.good) {
                    nodedata->setDeleted(true);
                }

                newnodes.push_back(nodedata);
                ++cntr;
            } else {
                translated_nodes.push_back(types::special::InvalidNodeId);
            }
        }

    mScenario.setNrow_coord(cntr);

    cntr = 0;
    int numedges = 0;
            foreach(GraphBuilder::Node node, nodes) {
            if (node.good) {
                std::shared_ptr<NodeData> nodedata = mNodes[nodeidx + cntr];
                for (int i = 0; i < node.adiacencies.size(); ++i) {
                    int adidx = node.adiacencies[i];
                    if (nodes[adidx].good) {
                        addEdge(nodedata, translated_nodes[adidx], node.weight.size() > i ? node.weight[i] : 0.0);
                    }
                    ++numedges;
                }
                ++cntr;
            }
        }

    mScenario.setNrow_graph(numedges);

            foreach(std::shared_ptr<NodeData> node, newnodes) {
            if (!node->mNode->get_harbour()) {
                controller->addNode(mIndex, node);
            }
        }
            foreach (std::shared_ptr<HarbourData> h, newharbours) {
            controller->addHarbour(mIndex, h, true);
        }

    controller->redraw();

    return true;
}

bool DisplaceModel::removeNode(std::shared_ptr<NodeData> node)
{
    // remove from: mNodesLayer, mNodes,
    // update scenario nrow_coord, nrow_graph
    qDebug() << "1) Node " << node->get_idx_node().toIndex() << " has " << node.use_count() << " instances";

    //mNodes[].reset();       // removed

    auto idx = node->get_idx_node().toIndex();


    // fix the node indexes
    for (auto i = idx + 1; i < mNodes.count(); ++i) {
        mNodes.at(i)->mNode->setNodeIdx(types::NodeId{static_cast<uint16_t>(i - 1)});
    }
    mNodes.removeAt(idx);

    mNodesLayer->SetAttributeFilter(
            QString("%1 = %2").arg(FLD_NODEID).arg(node->get_idx_node().toIndex()).toStdString().c_str());

    OGRFeature *ftr;
    while ((ftr = mNodesLayer->GetNextFeature())) {
        mNodesLayer->DeleteFeature(ftr->GetFID());
    }
    mNodesLayer->ResetReading();

    // TODO: Remove mPenaltyNodesQ1..4


    qDebug() << "2) Node " << node->get_idx_node().toIndex() << " has " << node.use_count() << " instances";
    return true;
}

/**
 * @brief DisplaceModel::addEdge
 * @param nodedata
 * @param targetidx
 * @param weight
 * @return Adiacency id for that node
 */
int DisplaceModel::addEdge(std::shared_ptr<NodeData> nodedata, types::NodeId targetidx, double weight)
{
    std::shared_ptr<NodeData> tg = mNodes.at(targetidx.toIndex());
    int i = nodedata->appendAdiancency(tg, weight);

    OGRFeature *e = OGRFeature::CreateFeature(mNodesLayer->GetLayerDefn());
    e->SetField(FLD_TYPE, (int) OgrTypeEdge);
    e->SetField(FLD_NODEID, nodedata->get_idx_node().toIndex());
    e->SetField(FLD_EDGEID, i);

    OGRLineString edge;
    edge.addPoint(nodedata->get_x(), nodedata->get_y());
    edge.addPoint(mNodes[targetidx.toIndex()]->get_x(), mNodes[targetidx.toIndex()]->get_y());
    e->SetGeometry(&edge);

    mNodesLayer->CreateFeature(e);

    mScenario.setNrow_graph(mScenario.getNrow_graph() + 1);
    return i;
}

/**
 * @brief DisplaceModel::addEdge
 * @param srcidx
 * @param targetidx
 * @param weight
 * @return Adiacency id
 */
int DisplaceModel::addEdge(types::NodeId srcidx, types::NodeId targetidx, double weight)
{
    return addEdge(mNodes[srcidx.toIndex()], targetidx, weight);
}

bool DisplaceModel::exportGraph(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        mLastError = file.errorString();
        return false;
    }

    QTextStream strm(&file);
            foreach (std::shared_ptr<NodeData> nd, mNodes) {
            if (!nd->isDeleted()) {
                strm << nd->get_x() << " " << nd->get_y() << " " << nd->get_harbour() << endl;
            }
        }

    file.close();
    return true;
}

bool DisplaceModel::importHarbours(QList<std::shared_ptr<HarbourData> > &list)
{
            foreach (std::shared_ptr<HarbourData> h, list) {
            int hid = mHarbours.size();
            int nid = mNodes.size();
            h->mHarbour->set_idx_node(types::NodeId(nid));
//        h->mHarbour->set_is_harbour(hid);
            mHarbours.push_back(h);

            std::shared_ptr<NodeData> n(new NodeData(h->mHarbour, this));
            n->setHarbourId(hid);
            mNodes.push_back(n);
        }

    return true;
}

void
DisplaceModel::addPenaltyToNodesByAddWeight(const QList<QPointF> &poly, double weight, double nbOfDaysClosedPerMonth,
                                            bool onQ1, bool onQ2, bool onQ3, bool onQ4, vector<bool> checkedMonths,
                                            const vector<int> &checkedVesSizes,
                                            vector<int> bannedMetiers)
{
    OGRLinearRing *gring = (OGRLinearRing *) OGRGeometryFactory::createGeometry(wkbLinearRing);

            foreach (const QPointF &pt, poly) {
            gring->addPoint(pt.x(), pt.y());
        }
    gring->closeRings();

    OGRPolygon *gpoly = (OGRPolygon *) OGRGeometryFactory::createGeometry(wkbPolygon);
    gpoly->addRing(gring);

    addPenaltyToNodesByAddWeight(gpoly, weight, nbOfDaysClosedPerMonth,
                                 onQ1, onQ2, onQ3, onQ4, checkedMonths, checkedVesSizes, bannedMetiers);

    delete gpoly;
}

void DisplaceModel::setLandscapeCodesFromFeature(OGRGeometry *geometry, int code)
{
    setCodeFromFeature(geometry, code, [&](std::shared_ptr<NodeData> nd, int c) {
        nd->setMarineLandscape(c);
    });
}

void DisplaceModel::setWindFromFeature(OGRGeometry *geometry, double wind)
{
    setWdFromFeature(geometry, wind, [&](std::shared_ptr<NodeData> nd, double w) {
        nd->setWind(w);
    });
}

void DisplaceModel::setSSTFromFeature(OGRGeometry *geometry, double sst)
{
    setTFromFeature(geometry, sst, [&](std::shared_ptr<NodeData> nd, double t) {
        nd->setSST(t);
    });
}

void DisplaceModel::setSalinityFromFeature(OGRGeometry *geometry, double psu)
{
    setSalFromFeature(geometry, psu, [&](std::shared_ptr<NodeData> nd, double s) {
        nd->setSalinity(s);
    });
}

void DisplaceModel::setNitrogenFromFeature(OGRGeometry *geometry, double nitro)
{
    setNitroFromFeature(geometry, nitro, [&](std::shared_ptr<NodeData> nd, double ni) {
        nd->setNitrogen(ni);
    });
}

void DisplaceModel::setPhosphorusFromFeature(OGRGeometry *geometry, double phos)
{
    setPhosFromFeature(geometry, phos, [&](std::shared_ptr<NodeData> nd, double ph) {
        nd->setPhosphorus(ph);
    });
}

void DisplaceModel::setOxygenFromFeature(OGRGeometry *geometry, double oxygen)
{
    setOxyFromFeature(geometry, oxygen, [&](std::shared_ptr<NodeData> nd, double ox) {
        nd->setOxygen(ox);
    });
}

void DisplaceModel::setDissolvedCarbonFromFeature(OGRGeometry *geometry, double dissc)
{
    setDissoFromFeature(geometry, dissc, [&](std::shared_ptr<NodeData> nd, double dc) {
        nd->setDissolvedCarbon(dc);
    });
}


void DisplaceModel::setBenthosBiomassFromFeature(OGRGeometry *geometry, double bio)
{
    setBenthosBioFromFeature(geometry, bio, [&](std::shared_ptr<NodeData> nd, double b) {
        nd->setBenthosBiomass(b);
    });
}

void DisplaceModel::setBenthosNumberFromFeature(OGRGeometry *geometry, double nb)
{
    setBenthosNbFromFeature(geometry, nb, [&](std::shared_ptr<NodeData> nd, double n) {
        nd->setBenthosNumber(n);
    });
}

void DisplaceModel::setAreaCodesFromFeature(OGRGeometry *geometry, int code)
{
    setCodeFromFeature(geometry, code, [&](std::shared_ptr<NodeData> nd, int c) {
        nd->setCodeArea(c);
    });
}

void DisplaceModel::setCodeFromFeature(OGRGeometry *geometry, int code,
                                       std::function<void(std::shared_ptr<NodeData>, int)> func)
{
    mNodesLayer->ResetReading();
    mNodesLayer->SetSpatialFilter(geometry);
    OGRFeature *ftr;
    while ((ftr = mNodesLayer->GetNextFeature())) {
        switch (ftr->GetFieldAsInteger(FLD_TYPE)) {
            case OgrTypeNode:
                int id = ftr->GetFieldAsInteger(FLD_NODEID);
                std::shared_ptr<NodeData> nd = mNodes[id];
                func(nd, code);
                break;
        }
    }
}

void DisplaceModel::setWdFromFeature(OGRGeometry *geometry, double wd,
                                     std::function<void(std::shared_ptr<NodeData>, int)> func)
{
    mNodesLayer->ResetReading();
    mNodesLayer->SetSpatialFilter(geometry);
    OGRFeature *ftr;
    while ((ftr = mNodesLayer->GetNextFeature())) {
        switch (ftr->GetFieldAsInteger(FLD_TYPE)) {
            case OgrTypeNode:
                int id = ftr->GetFieldAsInteger(FLD_NODEID);
                std::shared_ptr<NodeData> nd = mNodes[id];
                func(nd, wd);
                break;
        }
    }
}

void DisplaceModel::setTFromFeature(OGRGeometry *geometry, double t,
                                    std::function<void(std::shared_ptr<NodeData>, int)> func)
{
    mNodesLayer->ResetReading();
    mNodesLayer->SetSpatialFilter(geometry);
    OGRFeature *ftr;
    while ((ftr = mNodesLayer->GetNextFeature())) {
        switch (ftr->GetFieldAsInteger(FLD_TYPE)) {
            case OgrTypeNode:
                int id = ftr->GetFieldAsInteger(FLD_NODEID);
                std::shared_ptr<NodeData> nd = mNodes[id];
                func(nd, t);
                break;
        }
    }
}

void DisplaceModel::setSalFromFeature(OGRGeometry *geometry, double sal,
                                      std::function<void(std::shared_ptr<NodeData>, int)> func)
{
    mNodesLayer->ResetReading();
    mNodesLayer->SetSpatialFilter(geometry);
    OGRFeature *ftr;
    while ((ftr = mNodesLayer->GetNextFeature())) {
        switch (ftr->GetFieldAsInteger(FLD_TYPE)) {
            case OgrTypeNode:
                int id = ftr->GetFieldAsInteger(FLD_NODEID);
                std::shared_ptr<NodeData> nd = mNodes[id];
                func(nd, sal);
                break;
        }
    }
}

void DisplaceModel::setNitroFromFeature(OGRGeometry *geometry, double nitro,
                                        std::function<void(std::shared_ptr<NodeData>, int)> func)
{
    mNodesLayer->ResetReading();
    mNodesLayer->SetSpatialFilter(geometry);
    OGRFeature *ftr;
    while ((ftr = mNodesLayer->GetNextFeature())) {
        switch (ftr->GetFieldAsInteger(FLD_TYPE)) {
            case OgrTypeNode:
                int id = ftr->GetFieldAsInteger(FLD_NODEID);
                std::shared_ptr<NodeData> nd = mNodes[id];
                func(nd, nitro);
                break;
        }
    }
}

void DisplaceModel::setPhosFromFeature(OGRGeometry *geometry, double phos,
                                       std::function<void(std::shared_ptr<NodeData>, int)> func)
{
    mNodesLayer->ResetReading();
    mNodesLayer->SetSpatialFilter(geometry);
    OGRFeature *ftr;
    while ((ftr = mNodesLayer->GetNextFeature())) {
        switch (ftr->GetFieldAsInteger(FLD_TYPE)) {
            case OgrTypeNode:
                int id = ftr->GetFieldAsInteger(FLD_NODEID);
                std::shared_ptr<NodeData> nd = mNodes[id];
                func(nd, phos);
                break;
        }
    }
}

void DisplaceModel::setOxyFromFeature(OGRGeometry *geometry, double oxy,
                                      std::function<void(std::shared_ptr<NodeData>, int)> func)
{
    mNodesLayer->ResetReading();
    mNodesLayer->SetSpatialFilter(geometry);
    OGRFeature *ftr;
    while ((ftr = mNodesLayer->GetNextFeature())) {
        switch (ftr->GetFieldAsInteger(FLD_TYPE)) {
            case OgrTypeNode:
                int id = ftr->GetFieldAsInteger(FLD_NODEID);
                std::shared_ptr<NodeData> nd = mNodes[id];
                func(nd, oxy);
                break;
        }
    }
}

void DisplaceModel::setDissoFromFeature(OGRGeometry *geometry, double disso,
                                        std::function<void(std::shared_ptr<NodeData>, int)> func)
{
    mNodesLayer->ResetReading();
    mNodesLayer->SetSpatialFilter(geometry);
    OGRFeature *ftr;
    while ((ftr = mNodesLayer->GetNextFeature())) {
        switch (ftr->GetFieldAsInteger(FLD_TYPE)) {
            case OgrTypeNode:
                int id = ftr->GetFieldAsInteger(FLD_NODEID);
                std::shared_ptr<NodeData> nd = mNodes[id];
                func(nd, disso);
                break;
        }
    }
}

void DisplaceModel::setBenthosBioFromFeature(OGRGeometry *geometry, double bio,
                                             std::function<void(std::shared_ptr<NodeData>, double)> func)
{
    mNodesLayer->ResetReading();
    mNodesLayer->SetSpatialFilter(geometry);
    OGRFeature *ftr;
    while ((ftr = mNodesLayer->GetNextFeature())) {
        switch (ftr->GetFieldAsInteger(FLD_TYPE)) {
            case OgrTypeNode:
                int id = ftr->GetFieldAsInteger(FLD_NODEID);
                std::shared_ptr<NodeData> nd = mNodes[id];
                func(nd, bio);
                break;
        }
    }
}

void DisplaceModel::setBenthosNbFromFeature(OGRGeometry *geometry, double nb,
                                            std::function<void(std::shared_ptr<NodeData>, double)> func)
{
    mNodesLayer->ResetReading();
    mNodesLayer->SetSpatialFilter(geometry);
    OGRFeature *ftr;
    while ((ftr = mNodesLayer->GetNextFeature())) {
        switch (ftr->GetFieldAsInteger(FLD_TYPE)) {
            case OgrTypeNode:
                int id = ftr->GetFieldAsInteger(FLD_NODEID);
                std::shared_ptr<NodeData> nd = mNodes[id];
                func(nd, nb);
                break;
        }
    }
}

void DisplaceModel::addPenaltyToNodesByAddWeight(OGRGeometry *geometry, double weight, double nbOfDaysClosedPerMonth,
                                                 bool onQ1, bool onQ2, bool onQ3, bool onQ4, vector<bool> checkedMonths,
                                                 const vector<int> &checkedVesSizes,
                                                 vector<int> bannedMetiers)
{
    QList<int> penaltyNodes;

    mNodesLayer->ResetReading();
    mNodesLayer->SetSpatialFilter(geometry);
    OGRFeature *ftr;
    while ((ftr = mNodesLayer->GetNextFeature())) {
        switch (ftr->GetFieldAsInteger(FLD_TYPE)) {
            case OgrTypeNode:
                if (true) {
                    int id = ftr->GetFieldAsInteger(FLD_NODEID);
                    penaltyNodes.push_back(id);

#if 0
                    std::shared_ptr<NodeData> nd = mNodes[id];
                    for (int i = 0; i < nd->getAdiacencyCount(); ++i) {
                        nd->setAdiacencyWeight(i, nd->getAdiacencyWeight(i) + weight);
                    }
#endif
                }
                break;
            case OgrTypeEdge:
                if (true) {
                    int nodeid = ftr->GetFieldAsInteger(FLD_NODEID);
                    int edgeid = ftr->GetFieldAsInteger(FLD_EDGEID);

                    if (mNodes[nodeid]->getAdiacencyCount() > edgeid) {
                        mNodes[nodeid]->setAdiacencyWeight(edgeid, mNodes[nodeid]->getAdiacencyWeight(edgeid) + weight);
                    } else {
                        qWarning() << "NodeID" << nodeid << " has only " << mNodes[nodeid]->getAdiacencyCount()
                                   << " but edge" << edgeid << " requested." << __FILE__ << __LINE__;
                    }
                }
                break;
        }
    }

    if (nbOfDaysClosedPerMonth > 0.0 && penaltyNodes.size() > 0) {
        displace::NodePenalty pen;

        pen.q[0] = onQ1;
        pen.q[1] = onQ2;
        pen.q[2] = onQ3;
        pen.q[3] = onQ4;
        pen.months = checkedMonths;
        pen.nbOfDaysClosed = nbOfDaysClosedPerMonth;
        pen.vesSizes = checkedVesSizes;
        pen.polyId = 0;
        pen.metiers = bannedMetiers;

        for (auto n : penaltyNodes) {
            pen.nodeId = n;
            mPenaltyNodes.push_back(pen);
        }
    }
}

#if 0 // TODO REMOVE ME
int DisplaceModel::countPenaltyPolygons(int quarter) const
{
    switch(quarter) {
    case 0:
        return mPenaltyNodesQ1.size();
    case 1:
        return mPenaltyNodesQ2.size();
    case 2:
        return mPenaltyNodesQ3.size();
    case 3:
        return mPenaltyNodesQ4.size();
    default:
        throw std::runtime_error("Unhandled case in countPenaltyPolygons");
    }
}

const QList<int> DisplaceModel::getPenaltyPolygonsAt(int quarter, int ndx) const
{
    switch(quarter) {
    case 0:
        return mPenaltyNodesQ1.at(ndx);
    case 1:
        return mPenaltyNodesQ2.at(ndx);
    case 2:
        return mPenaltyNodesQ3.at(ndx);
    case 3:
        return mPenaltyNodesQ4.at(ndx);
    default:
        throw std::runtime_error("Unhandled case in getPenaltyPolygonsAt");
    }
}
#endif


int DisplaceModel::getVesselCount() const
{
    return mVessels.size();
}

QString DisplaceModel::getVesselId(int idx) const
{
    return QString::fromStdString(mVessels.at(idx)->mVessel->get_name());
}

void DisplaceModel::updateVessel(int tstep, int idx, float x, float y, float course, float fuel, int state)
{
    std::shared_ptr<VesselData> v(mVessels.at(idx));
    v->mVessel->set_xy(x, y);
    v->mVessel->set_course(course);
    v->mVessel->set_cumfuelcons(fuel);
    v->mVessel->set_state(state);
}


int DisplaceModel::getShipsCount() const
{
    return mShips.size();
}

QString DisplaceModel::getShipId(int idx) const
{
    return QString::fromStdString(mShips.at(idx)->mShip->get_name());
}

void DisplaceModel::updateShip(int tstep, int idx, float x, float y, float course)
{
    (void) tstep;
    std::shared_ptr<ShipData> sh(mShips.at(idx));
    sh->mShip->set_xy(x, y);
    sh->mShip->set_course(course);
}


int DisplaceModel::getFishfarmsCount() const
{
    return mFishfarms.size();
}


int DisplaceModel::getFirmCount() const
{
    return mFirms.size();
}

int DisplaceModel::getWindmillCount() const
{
    return mWindmills.size();
}


QString DisplaceModel::getFishfarmId(int idx) const
{
    return QString::number(mFishfarms.at(idx)->mFishfarm->get_name());
}

QString DisplaceModel::getFirmId(int idx) const
{
    return QString::number(mFirms.at(idx)->mFirm->get_idx());
}

QString DisplaceModel::getWindmillId(int idx) const
{
    return QString::number(mWindmills.at(idx)->mWindmill->get_idx());
}

void DisplaceModel::updateFishfarm(int idx, float x, float y)
{
    std::shared_ptr<FishfarmData> ff(mFishfarms.at(idx));
    ff->mFishfarm->set_x(x);
    ff->mFishfarm->set_y(y);

    if (mDb) {
        //  mDb->addFishfarmPosition(idx, ff);
    }
}

void DisplaceModel::updateFirm(int idx, float x, float y)
{
    std::shared_ptr<FirmData> fi(mFirms.at(idx));
    fi->mFirm->set_x(x);
    fi->mFirm->set_y(y);

    if (mDb) {
        //  mDb->addFirmPosition(idx, ff);
    }
}

void DisplaceModel::updateWindmill(int idx, float x, float y)
{
    std::shared_ptr<WindmillData> ff(mWindmills.at(idx));
    ff->mWindmill->set_x(x);
    ff->mWindmill->set_y(y);

    if (mDb) {
        //  mDb->addWindmillPosition(idx, ff);
    }
}


int DisplaceModel::getBenthosCount() const
{
    return mBenthos.size();
}


void DisplaceModel::addMetier(int id)
{
    auto m = std::make_shared<objecttree::MetiersInterest>();
    m->metierId = id;
    mMetiers.push_back(m);
}

int DisplaceModel::getPopulationsCount() const
{
    return mConfig.getNbpops();
}

int DisplaceModel::getBenthosPopulationsCount() const
{
    return mConfig.getNbbenthospops();
}

HarbourStats DisplaceModel::retrieveHarbourIdxStatAtStep(int idx, int step)
{
    if (mModelType != OfflineModelType || !mDb) {
        return HarbourStats(); //getHarboursStatAtStep(step, idx);
    }

    return mDb->getHarbourStatsAtStep(idx, step);
}

Scenario DisplaceModel::scenario() const
{
    return mScenario;
}

void DisplaceModel::setScenario(const Scenario &scenario)
{
    mScenario = scenario;
}

Config DisplaceModel::config() const
{
    return mConfig;
}

void DisplaceModel::setConfig(const Config &config)
{
    mConfig = config;
    mInterestingHarb = mConfig.m_interesting_harbours;
}

void DisplaceModel::setCurrentStep(int step)
{
    mCurrentStep = step;
    if (mDb) {
        if (modelType() != ModelType::LiveModelType) {
            // SHOULD NOT BE CALLED IN LIVE MODE
            mDb->updateVesselsToStep(mCurrentStep, mVessels);
            mDb->updateStatsForNodesToStep(mCurrentStep, mNodes);
            mDb->updatePopValuesForNodesToStep(mCurrentStep, mNodes);
        }
    }
}

void DisplaceModel::setInterestingPop(int n)
{
    if (!mInterestingPop.contains(n)) {
        mInterestingPop.append(n);
    }
    qSort(mInterestingPop);
}

void DisplaceModel::setInterestingPop2(int n)
{
    if (!mInterestingPop2.contains(n)) {
        mInterestingPop2.append(n);
    }
    qSort(mInterestingPop2);
}

void DisplaceModel::remInterestingPop(int n)
{
    mInterestingPop.removeAll(n);
}

void DisplaceModel::remInterestingPop2(int n)
{
    mInterestingPop2.removeAll(n);
}

bool DisplaceModel::isInterestingPop(int n)
{
    return mInterestingPop.contains(n);
}

bool DisplaceModel::isInterestingPop2(int n)
{
    return mInterestingPop2.contains(n);
}

void DisplaceModel::clearInterestingPop()
{
    mInterestingPop.clear();
}

void DisplaceModel::clearInterestingPop2()
{
    mInterestingPop2.clear();
}

int DisplaceModel::getNumFuncGroups() const
{
    return config().getNbbenthospops();
}

int DisplaceModel::getNumFishfarmsTypes() const
{
    // return config().getNumFishfarmsTypes();
    return 3; // TO DO: detect nb of farms type
}

int DisplaceModel::getNumWindfarmsTypes() const
{
    // return config().getNumWindfarmsTypes();
    return 3; // TO DO: detect nb of farms type
}

int DisplaceModel::getNumShipsTypes() const
{
    // return config().getNumShipsTypes();
    return 5; // TO DO: detect nb of ship type
}

void DisplaceModel::setInterestingSize(int n)
{
    if (!mInterestingSizes.contains(n)) {
        mInterestingSizes.append(n);
    }
    qSort(mInterestingSizes);
}

void DisplaceModel::remInterestingSize(int n)
{
    mInterestingSizes.removeAll(n);
}

bool DisplaceModel::isInterestingSize(int n)
{
    return mInterestingSizes.contains(n);
}

void DisplaceModel::setInterestingHarb(types::NodeId n)
{
    if (!mInterestingHarb.contains(n)) {
        mInterestingHarb.append(n);
    }
    qSort(mInterestingHarb);
}

void DisplaceModel::remInterestingHarb(types::NodeId n)
{
    mInterestingHarb.removeAll(n);
}

bool DisplaceModel::isInterestingHarb(types::NodeId n)
{
    return mInterestingHarb.contains(n);
}

void DisplaceModel::parseOutputStatsFile(QString file, int tstep)
{
    emit parseOutput(file, tstep);
}

/* Warn: copy and pasted from simulator's main.cpp */
#define NBSZGROUP 14

bool DisplaceModel::loadNodes()
{
    int nrow_coord = mScenario.getNrow_coord();
    auto a_port = mScenario.getA_port();
    vector<string> dyn_alloc_sce = mScenario.getDyn_alloc_sce_asVector();
    int nbpops = mConfig.getNbpops();
    int nbbenthospops = mConfig.getNbbenthospops();
    string a_quarter = "quarter1";// start quarter

    // input data, coord nodes of the graph
    ifstream coord_graph;

    // casting a_pop into a string
    stringstream out;
    out << mScenario.getGraph();
    string a_graph_s = out.str();
    string filename_graph = mBasePath.toStdString() +
                            "/graphsspe/coord" + a_graph_s + ".dat";

    string filename_code_area_graph = mBasePath.toStdString() +
                                      "/graphsspe/code_area_for_graph" + a_graph_s + "_points.dat";

    string filename_code_marine_landscape_graph = mBasePath.toStdString() +
                                                  "/graphsspe/coord" + a_graph_s + "_with_landscape.dat";

    string filename_wind_graph = mBasePath.toStdString() +
                                 "/graphsspe/coord" + a_graph_s + "_with_wind.dat";

    string filename_sst_graph = mBasePath.toStdString() +
                                "/graphsspe/coord" + a_graph_s + "_with_sst.dat";

    string filename_salinity_graph = mBasePath.toStdString() +
                                     "/graphsspe/coord" + a_graph_s + "_with_salinity.dat";

    string filename_Nitrogen_graph = mBasePath.toStdString() +
                                     "/graphsspe/coord" + a_graph_s + "_with_nitrogen.dat";

    string filename_Phosphorus_graph = mBasePath.toStdString() +
                                       "/graphsspe/coord" + a_graph_s + "_with_phosphorus.dat";

    string filename_Oxygen_graph = mBasePath.toStdString() +
                                   "/graphsspe/coord" + a_graph_s + "_with_oxygen.dat";

    string filename_DissolvedCarbon_graph = mBasePath.toStdString() +
                                            "/graphsspe/coord" + a_graph_s + "_with_dissolvedcarbon.dat";

    string filename_bathymetry_graph = mBasePath.toStdString() +
                                       "/graphsspe/coord" + a_graph_s + "_with_bathymetry.dat";

    string filename_shippingdensity_graph = mBasePath.toStdString() +
                                            "/graphsspe/coord" + a_graph_s + "_with_shippingdensity.dat";

    string filename_siltfraction_graph = mBasePath.toStdString() +
                                         "/graphsspe/coord" + a_graph_s + "_with_siltfraction.dat";

    string filename_code_benthos_biomass_graph = mBasePath.toStdString() +
                                                 "/graphsspe/coord" + a_graph_s + "_with_benthos_total_biomass.dat";

    string filename_code_benthos_number_graph = mBasePath.toStdString() +
                                                "/graphsspe/coord" + a_graph_s + "_with_benthos_total_number.dat";

    coord_graph.open(filename_graph.c_str());
    if (coord_graph.fail()) {
        throw DisplaceException(QString(QObject::tr("Cannot load %1: %2"))
                                        .arg(filename_graph.c_str())
                                        .arg(strerror(errno)));
    }
    vector<double> graph_coord_x;
    vector<double> graph_coord_y;
    vector<int> graph_coord_harbour;
    if (!fill_from_coord(coord_graph, graph_coord_x, graph_coord_y, graph_coord_harbour, nrow_coord)) {
        throw DisplaceException(QString(QObject::tr("Cannot parse %1: %2"))
                                        .arg(filename_graph.c_str()));
    }
    coord_graph.close();

    // input data, code area for each point of the graph (e.g. 1: NS, 2: BW, 3: BE, 10: open sea)
    ifstream code_area_graph;
    code_area_graph.open(filename_code_area_graph.c_str());
    if (code_area_graph.fail()) {
        throw DisplaceException(QString(QObject::tr("Cannot load %1: %2"))
                                        .arg(filename_code_area_graph.c_str())
                                        .arg(strerror(errno)));
    }
    vector<int> graph_point_code_area;
    if (!fill_from_code_area(code_area_graph, graph_point_code_area, nrow_coord)) {
        throw DisplaceException(QString(QObject::tr("Cannot parse %1: %2"))
                                        .arg(filename_code_area_graph.c_str()));
    }

    // input data, for the marine landscape for each point of the graph (e.g. 111, 112, etc. e.g. see the BALANCE map coding)
    ifstream code_landscape_graph;
    code_landscape_graph.open(filename_code_marine_landscape_graph.c_str());
    if (code_landscape_graph.fail()) {
        throw DisplaceException(QString(QObject::tr("Cannot load %1: %2"))
                                        .arg(filename_code_marine_landscape_graph.c_str())
                                        .arg(strerror(errno)));
    }
    vector<int> graph_point_code_landscape;
    if (!fill_from_code_marine_landscape(code_landscape_graph, graph_point_code_landscape, nrow_coord)) {
        throw DisplaceException(QString(QObject::tr("Cannot parse %1: %2"))
                                        .arg(filename_code_marine_landscape_graph.c_str()));
    }

    // input data, for the wind for each point of the graph
    ifstream wind_graph;
    wind_graph.open(filename_wind_graph.c_str());
    if (wind_graph.fail()) {
        throw DisplaceException(QString(QObject::tr("Cannot load %1: %2"))
                                        .arg(filename_wind_graph.c_str())
                                        .arg(strerror(errno)));
    }
    vector<double> graph_point_wind;
    if (!fill_from_wind(wind_graph, graph_point_wind, nrow_coord)) {
        throw DisplaceException(QString(QObject::tr("Cannot parse %1: %2"))
                                        .arg(filename_wind_graph.c_str()));
    }

    // input data, for the sst for each point of the graph
    ifstream sst_graph;
    sst_graph.open(filename_sst_graph.c_str());
    if (sst_graph.fail()) {
        throw DisplaceException(QString(QObject::tr("Cannot load %1: %2"))
                                        .arg(filename_sst_graph.c_str())
                                        .arg(strerror(errno)));
    }
    vector<double> graph_point_sst;
    if (!fill_from_sst(sst_graph, graph_point_sst, nrow_coord)) {
        throw DisplaceException(QString(QObject::tr("Cannot parse %1: %2"))
                                        .arg(filename_sst_graph.c_str()));
    }

    // input data, for the salinity for each point of the graph
    ifstream salinity_graph;
    salinity_graph.open(filename_salinity_graph.c_str());
    if (salinity_graph.fail()) {
        throw DisplaceException(QString(QObject::tr("Cannot load %1: %2"))
                                        .arg(filename_salinity_graph.c_str())
                                        .arg(strerror(errno)));
    }
    vector<double> graph_point_salinity;
    if (!fill_from_salinity(salinity_graph, graph_point_salinity, nrow_coord)) {
        throw DisplaceException(QString(QObject::tr("Cannot parse %1: %2"))
                                        .arg(filename_salinity_graph.c_str()));
    }


    // input data, for the Nitrogen for each point of the graph
    ifstream Nitrogen_graph;
    Nitrogen_graph.open(filename_Nitrogen_graph.c_str());
    if (Nitrogen_graph.fail()) {
        throw DisplaceException(QString(QObject::tr("Cannot load %1: %2"))
                                        .arg(filename_Nitrogen_graph.c_str())
                                        .arg(strerror(errno)));
    }
    vector<double> graph_point_Nitrogen;
    if (!fill_from_Nitrogen(Nitrogen_graph, graph_point_Nitrogen, nrow_coord)) {
        throw DisplaceException(QString(QObject::tr("Cannot parse %1: %2"))
                                        .arg(filename_Nitrogen_graph.c_str()));
    }

    // input data, for the Phosporus for each point of the graph
    ifstream Phosphorus_graph;
    Phosphorus_graph.open(filename_Phosphorus_graph.c_str());
    if (Phosphorus_graph.fail()) {
        throw DisplaceException(QString(QObject::tr("Cannot load %1: %2"))
                                        .arg(filename_Phosphorus_graph.c_str())
                                        .arg(strerror(errno)));
    }
    vector<double> graph_point_Phosphorus;
    if (!fill_from_Phosphorus(Phosphorus_graph, graph_point_Phosphorus, nrow_coord)) {
        throw DisplaceException(QString(QObject::tr("Cannot parse %1: %2"))
                                        .arg(filename_Phosphorus_graph.c_str()));
    }

    // input data, for the Oxygen for each point of the graph
    ifstream Oxygen_graph;
    Oxygen_graph.open(filename_Oxygen_graph.c_str());
    if (Oxygen_graph.fail()) {
        throw DisplaceException(QString(QObject::tr("Cannot load %1: %2"))
                                        .arg(filename_Oxygen_graph.c_str())
                                        .arg(strerror(errno)));
    }
    vector<double> graph_point_Oxygen;
    if (!fill_from_Oxygen(Oxygen_graph, graph_point_Oxygen, nrow_coord)) {
        throw DisplaceException(QString(QObject::tr("Cannot parse %1: %2"))
                                        .arg(filename_Oxygen_graph.c_str()));
    }

    // input data, for the DissolvedCarbon for each point of the graph
    ifstream DissolvedCarbon_graph;
    DissolvedCarbon_graph.open(filename_DissolvedCarbon_graph.c_str());
    if (DissolvedCarbon_graph.fail()) {
        throw DisplaceException(QString(QObject::tr("Cannot load %1: %2"))
                                        .arg(filename_DissolvedCarbon_graph.c_str())
                                        .arg(strerror(errno)));
    }
    vector<double> graph_point_DissolvedCarbon;
    if (!fill_from_DissolvedCarbon(DissolvedCarbon_graph, graph_point_DissolvedCarbon, nrow_coord)) {
        throw DisplaceException(QString(QObject::tr("Cannot parse %1: %2"))
                                        .arg(filename_DissolvedCarbon_graph.c_str()));
    }

    // input data, for the bathymetry for each point of the graph
    ifstream bathymetry_graph;
    bathymetry_graph.open(filename_bathymetry_graph.c_str());
    if (bathymetry_graph.fail()) {
        throw DisplaceException(QString(QObject::tr("Cannot load %1: %2"))
                                        .arg(filename_bathymetry_graph.c_str())
                                        .arg(strerror(errno)));
    }
    vector<double> graph_point_bathymetry;
    if (!fill_from_bathymetry(bathymetry_graph, graph_point_bathymetry, nrow_coord)) {
        throw DisplaceException(QString(QObject::tr("Cannot parse %1: %2"))
                                        .arg(filename_bathymetry_graph.c_str()));
    }

    // input data, for the shippingdensity for each point of the graph
    ifstream shippingdensity_graph;
    shippingdensity_graph.open(filename_shippingdensity_graph.c_str());
    if (shippingdensity_graph.fail()) {
        throw DisplaceException(QString(QObject::tr("Cannot load %1: %2"))
                                        .arg(filename_shippingdensity_graph.c_str())
                                        .arg(strerror(errno)));
    }
    vector<double> graph_point_shippingdensity;
    if (!fill_from_shippingdensity(shippingdensity_graph, graph_point_shippingdensity, nrow_coord)) {
        throw DisplaceException(QString(QObject::tr("Cannot parse %1: %2"))
                                        .arg(filename_shippingdensity_graph.c_str()));
    }

    ifstream siltfraction_graph;
    siltfraction_graph.open(filename_siltfraction_graph.c_str());
    if (siltfraction_graph.fail()) {
        throw DisplaceException(QString(QObject::tr("Cannot load %1: %2"))
                                        .arg(filename_siltfraction_graph.c_str())
                                        .arg(strerror(errno)));
    }
    vector<double> graph_point_siltfraction;
    if (!fill_from_siltfraction(siltfraction_graph, graph_point_siltfraction, nrow_coord)) {
        throw DisplaceException(QString(QObject::tr("Cannot parse %1: %2"))
                                        .arg(filename_siltfraction_graph.c_str()));
    }

    vector<double> graph_point_landscape_norm(nrow_coord, 0);
    vector<double> graph_point_landscape_alpha(nrow_coord, 0);
    vector<double> graph_point_wind_norm(nrow_coord, 0);
    vector<double> graph_point_wind_alpha(nrow_coord, 0);
    vector<double> graph_point_sst_norm(nrow_coord, 0);
    vector<double> graph_point_sst_alpha(nrow_coord, 0);
    vector<double> graph_point_salinity_norm(nrow_coord, 0);
    vector<double> graph_point_salinity_alpha(nrow_coord, 0);
    vector<double> graph_point_Nitrogen_norm(nrow_coord, 0);
    vector<double> graph_point_Nitrogen_alpha(nrow_coord, 0);
    vector<double> graph_point_Phosphorus_norm(nrow_coord, 0);
    vector<double> graph_point_Phosphorus_alpha(nrow_coord, 0);
    vector<double> graph_point_Oxygen_norm(nrow_coord, 0);
    vector<double> graph_point_Oxygen_alpha(nrow_coord, 0);
    vector<double> graph_point_DissolvedCarbon_norm(nrow_coord, 0);
    vector<double> graph_point_DissolvedCarbon_alpha(nrow_coord, 0);



    // input data, for the benthos biomass for each point of the graph
    ifstream code_benthos_graph;
    code_benthos_graph.open(filename_code_benthos_biomass_graph.c_str());
    if (code_benthos_graph.fail()) {
        throw DisplaceException(QString(QObject::tr("Cannot load %1: %2"))
                                        .arg(filename_code_benthos_biomass_graph.c_str())
                                        .arg(strerror(errno)));
    }
    vector<double> graph_point_benthos_biomass;
    if (!fill_from_benthos_biomass(code_benthos_graph, graph_point_benthos_biomass, nrow_coord)) {
        throw DisplaceException(QString(QObject::tr("Cannot parse %1: %2"))
                                        .arg(filename_code_benthos_biomass_graph.c_str()));
    }

    // input data, for the benthos number for each point of the graph
    ifstream code_benthos_number_graph;
    code_benthos_number_graph.open(filename_code_benthos_number_graph.c_str());
    if (code_benthos_number_graph.fail()) {
        throw DisplaceException(QString(QObject::tr("Cannot load %1: %2"))
                                        .arg(filename_code_benthos_number_graph.c_str())
                                        .arg(strerror(errno)));
    }
    vector<double> graph_point_benthos_number;
    if (!fill_from_benthos_number(code_benthos_number_graph, graph_point_benthos_number, nrow_coord)) {
        throw DisplaceException(QString(QObject::tr("Cannot parse %1: %2"))
                                        .arg(filename_code_benthos_number_graph.c_str()));
    }


    // read harbour specific files
    auto harbour_names = read_harbour_names(mInputName.toStdString(), mBasePath.toStdString());

    // creation of a vector of nodes from coord
    // and check with the coord in input.
    // use inheritance i.e. a Harbour is child of a Node
    // use polymorphism i.e. store either Harbour or Node in the vector of pointers 'nodes'

//    vector <Node* > nodes(graph_coord_x.size());
    // REPLACE std::shared_ptr<Node> by Node* in every files if you want to remove boost

    for (unsigned int i = 0; i < graph_coord_x.size(); i++) {
        if (graph_coord_harbour[i]) {
            types::NodeId inode(i);

            string a_name = "none";
            types::NodeId a_point(0);
            // get the name of this harbour
            auto lower_g = harbour_names.lower_bound(inode);
            auto upper_g = harbour_names.upper_bound(inode);
            for (auto pos = lower_g; pos != upper_g; pos++) {
                a_point = pos->first;
                a_name = pos->second;
            }

            map<int, double> init_fuelprices;
            multimap<int, double> fishprices_each_species_per_cat;
            if (a_name != "none" && a_point == inode) {

                cout << "load prices for port " << a_name << " which is point " << a_point << endl;
                //int er = read_prices_per_harbour(a_point, a_quarter, prices, mName.toStdString());
                read_prices_per_harbour_each_pop_per_cat(a_point, a_quarter, fishprices_each_species_per_cat,
                                                         mInputName.toStdString(), mBasePath.toStdString());
                // if not OK then deadly bug: possible NA or Inf in harbour files need to be checked (step 7)
                cout << "....OK" << endl;
            } else {

                cout << a_point
                     << " : harbour not found in the harbour names (probably because no declared landings from studied vessels in those ports)"
                     << endl;
                //int er = read_prices_per_harbour(a_port, "quarter1", prices, mName.toStdString()); // delete later on when final parameterisation
                cout << "then go for the port: " << a_port << " instead" << endl;
                read_prices_per_harbour_each_pop_per_cat(a_port, "quarter1", fishprices_each_species_per_cat,
                                                         mInputName.toStdString(), mBasePath.toStdString());
                cout << "....OK" << endl;

            }

            // read fuel price (vessel size dependent for the time being)
            if (!binary_search(dyn_alloc_sce.begin(), dyn_alloc_sce.end(),
                               "fuelprice_plus20percent")) {
                cout << "read fuel price..." << endl;
                read_fuel_prices_per_vsize(a_port, "quarter1", init_fuelprices, mInputName.toStdString(),
                                           mBasePath.toStdString());
                cout << "...OK" << endl;
            } else {
                cout << "read fuel price..." << endl;
                read_fuel_prices_per_vsize(a_port, "quarter1", init_fuelprices, mInputName.toStdString(),
                                           mBasePath.toStdString());

                map<int, double>::iterator pos;
                for (pos = init_fuelprices.begin(); pos != init_fuelprices.end(); pos++) {
                    pos->second = (pos->second) * 1.2;
                }

                for (pos = init_fuelprices.begin(); pos != init_fuelprices.end(); pos++) {
                    cout << pos->first << " " << pos->second;
                }
                cout << "...OK" << endl;

            }

            // TODO check this, node 0 is a valid node.
            vector<types::NodeId> usual_fgrounds;
            usual_fgrounds.push_back(types::NodeId(0));

            vector<double> freq_usual_fgrounds;
            freq_usual_fgrounds.push_back(1.0);

            cout << "create an harbour..." << endl;
            std::shared_ptr<Harbour> h(new Harbour(types::NodeId(i),
                                                   graph_coord_x[i],
                                                   graph_coord_y[i],
                                                   graph_coord_harbour[i],
                                                   graph_point_code_area[i],
                                                   graph_point_code_landscape[i],
                                                   graph_point_landscape_norm[i],
                                                   graph_point_landscape_alpha[i],
                                                   graph_point_wind[i],
                                                   graph_point_wind_norm[i],
                                                   graph_point_wind_alpha[i],
                                                   graph_point_sst[i],
                                                   graph_point_sst_norm[i],
                                                   graph_point_sst_alpha[i],
                                                   graph_point_salinity[i],
                                                   graph_point_salinity_norm[i],
                                                   graph_point_salinity_alpha[i],
                                                   graph_point_Nitrogen[i],
                                                   graph_point_Nitrogen_norm[i],
                                                   graph_point_Nitrogen_alpha[i],
                                                   graph_point_Phosphorus[i],
                                                   graph_point_Phosphorus_norm[i],
                                                   graph_point_Phosphorus_alpha[i],
                                                   graph_point_Oxygen[i],
                                                   graph_point_Oxygen_norm[i],
                                                   graph_point_Oxygen_alpha[i],
                                                   graph_point_DissolvedCarbon[i],
                                                   graph_point_DissolvedCarbon_norm[i],
                                                   graph_point_DissolvedCarbon_alpha[i],
                                                   graph_point_bathymetry[i],
                                                   graph_point_shippingdensity[i],
                                                   graph_point_siltfraction[i],
                                                   graph_point_benthos_biomass[i],
                                                   graph_point_benthos_number[i],
                                                   0, // because benthos mean weight is not informed by GIS layer
                                                   0, // because  is not informed by GIS layer
                                                   0, // because  is not informed by GIS layer
                                                   nbpops,
                                                   nbbenthospops,
                                                   NBSZGROUP,
                                                   a_name,
                    //prices,
                                                   fishprices_each_species_per_cat,
                                                   init_fuelprices,
                                                   usual_fgrounds,
                                                   freq_usual_fgrounds
            ));
            std::shared_ptr<HarbourData> hd(new HarbourData(h));
            mHarbours.push_back(hd);

            std::shared_ptr<NodeData> n(new NodeData(h, this));
            n->setHarbourId(mHarbours.size() - 1);
            mNodes.push_back(n);
        } else {
            //cout << "create a node..." << endl;
            std::shared_ptr<Node> nd(new Node(types::NodeId(i),
                                              graph_coord_x[i],
                                              graph_coord_y[i],
                                              graph_coord_harbour[i],
                                              graph_point_code_area[i],
                                              graph_point_code_landscape[i],
                                              graph_point_landscape_norm[i],
                                              graph_point_landscape_alpha[i],
                                              graph_point_wind[i],
                                              graph_point_wind_norm[i],
                                              graph_point_wind_alpha[i],
                                              graph_point_sst[i],
                                              graph_point_sst_norm[i],
                                              graph_point_sst_alpha[i],
                                              graph_point_salinity[i],
                                              graph_point_salinity_norm[i],
                                              graph_point_salinity_alpha[i],
                                              graph_point_Nitrogen[i],
                                              graph_point_Nitrogen_norm[i],
                                              graph_point_Nitrogen_alpha[i],
                                              graph_point_Phosphorus[i],
                                              graph_point_Phosphorus_norm[i],
                                              graph_point_Phosphorus_alpha[i],
                                              graph_point_Oxygen[i],
                                              graph_point_Oxygen_norm[i],
                                              graph_point_Oxygen_alpha[i],
                                              graph_point_DissolvedCarbon[i],
                                              graph_point_DissolvedCarbon_norm[i],
                                              graph_point_DissolvedCarbon_alpha[i],
                                              graph_point_bathymetry[i],
                                              graph_point_shippingdensity[i],
                                              graph_point_siltfraction[i],
                                              graph_point_benthos_biomass[i],
                                              graph_point_benthos_number[i],
                                              0,// because benthos mean weight is not informed by GIS layer
                                              0,// because is not informed by GIS layer
                                              0,// because is not informed by GIS layer
                                              nbpops,
                                              nbbenthospops,
                                              NBSZGROUP));
            std::shared_ptr<NodeData> n(new NodeData(nd, this));

            mNodes.push_back(n);

        }
    }


    // read nodes in polygons for area-based management
#if 0
    cout << "read node in polygons..." << endl;
    string a_graph_name="a_graph";
    a_graph_name=a_graph_name+a_graph_s;
    multimap<int, int> nodes_in_polygons= read_nodes_in_polygons(a_quarter, a_graph_name, mInputName.toStdString(), mBasePath.toStdString());
    // check for area_closure
    vector<int> polygons;
    vector<int> polygon_nodes;
    for (multimap<int, int>::const_iterator pos=nodes_in_polygons.begin(); pos != nodes_in_polygons.end(); pos++)
        {
            // get all values across the keys
            polygons.push_back(pos->first);
            polygon_nodes.push_back(pos->second);
        }
        sort (polygon_nodes.begin(), polygon_nodes.end());

#endif
    // TODO this could be removed.
#if 0
    for(unsigned int a_idx=0; a_idx<mNodes.size(); a_idx++)
        {
        if(binary_search (polygon_nodes.begin(), polygon_nodes.end(), mNodes.at(a_idx)->get_idx_node()))
           {
            mNodes.at(a_idx)->setAreaType(1);
           } else{
            mNodes.at(a_idx)->setAreaType(0);
           }
        }
    cout << "OK for in polygons..." << endl;
#endif



    // read tariff map
    string a_graph_name = "a_graph";
    a_graph_name = a_graph_name + a_graph_s;
    if (binary_search(dyn_alloc_sce.begin(), dyn_alloc_sce.end(), "fishing_credits")) {
        auto initial_tariffs_on_nodes = read_initial_tariffs_on_nodes(mInputName.toStdString(), mBasePath.toStdString(),
                                                                      a_graph_name);


        // init
        for (size_t a_idx = 0; a_idx < mNodes.size(); a_idx++) {
            auto idx_node = mNodes.at(a_idx)->get_idx_node();

            // initial tariff for this particular node
            auto lower_init_cr = initial_tariffs_on_nodes.lower_bound(idx_node);
            auto upper_init_cr = initial_tariffs_on_nodes.upper_bound(idx_node);
            vector<double> init_tariffs;
            for (auto pos = lower_init_cr; pos != upper_init_cr; pos++)
                init_tariffs.push_back(pos->second);

            if (initial_tariffs_on_nodes.count(idx_node) == 0) {
                init_tariffs.push_back(0);
            } // put 0 if this node is not informed

            mNodes.at(a_idx)->set_tariffs(init_tariffs); // type 0
        }

    } else {
        // need to inform with a vector of three zeros at least
        vector<double> init_tariffs(3, 0);
        for (size_t a_idx = 0; a_idx < mNodes.size(); a_idx++) {
            mNodes.at(a_idx)->set_tariffs(init_tariffs); // type 0
        }
    }


    return true;
}

bool DisplaceModel::loadVessels()
{
    //int nrow_coord = mScenario.getNrow_coord();
    //auto a_port = mScenario.getA_port();
    vector<string> dyn_alloc_sce = mScenario.getDyn_alloc_sce_asVector();
    int nbpops = mConfig.getNbpops();
    string a_quarter = "quarter1";// start quarter
    string a_semester = "semester1";

    int selected_vessels_only = 0;
    string a_graph_name = "a_graph";

    stringstream graphnum;
    graphnum << mScenario.getGraph();
    a_graph_name = a_graph_name + graphnum.str();

    // read general vessel features
    // (quarter specific, mainly because of the gamma parameters)
    vector<string> vesselids;
    vector<int> vid_is_actives;
    vector<int> vid_is_part_of_ref_fleets;
    vector<double> speeds;
    vector<double> fuelcons;
    vector<double> lengths;
    vector<double> vKWs;
    vector<double> carrycapacities;
    vector<double> tankcapacities;
    vector<double> nbfpingspertrips;
    vector<double> resttime_par1s;
    vector<double> resttime_par2s;
    vector<double> av_trip_duration;
    vector<double> mult_fuelcons_when_steaming;
    vector<double> mult_fuelcons_when_fishing;
    vector<double> mult_fuelcons_when_returning;
    vector<double> mult_fuelcons_when_inactive;
    vector<int> firm_ids;
    vector<VesselCalendar> calendar;

    cout << "read_vessels_features() in loadVessels()" << endl;

    if (!read_vessels_features(a_quarter, vesselids, vid_is_actives, vid_is_part_of_ref_fleets,
                               speeds, fuelcons, lengths, vKWs,
                               carrycapacities, tankcapacities, nbfpingspertrips,
                               resttime_par1s, resttime_par2s, av_trip_duration,
                               mult_fuelcons_when_steaming, mult_fuelcons_when_fishing,
                               mult_fuelcons_when_returning, mult_fuelcons_when_inactive, firm_ids,
                               mInputName.toStdString(), mBasePath.toStdString(), selected_vessels_only, calendar)) {
        return false;
    }

    // read general vessel features
    // (quarter specific, mainly because of the gamma parameters)

    vector<double> this_vessel_nb_crews;
    vector<double> annual_other_incomes;
    vector<double> landing_costs_percents;
    vector<double> crewshare_and_unpaid_labour_costs_percents;
    vector<double> other_variable_costs_per_unit_efforts;
    vector<double> annual_insurance_costs_per_crews;
    vector<double> standard_labour_hour_opportunity_costss;
    vector<double> standard_annual_full_time_employement_hourss;
    vector<double> other_annual_fixed_costss;
    vector<double> vessel_values;
    vector<double> annual_depreciation_rates;
    vector<double> opportunity_interest_rates;
    vector<double> annual_discount_rates;

    cout << "read_vessels_economic_features() in loadVessels()" << endl;
    if (!read_vessels_economics_features(
            vesselids,
            this_vessel_nb_crews,
            annual_other_incomes,
            landing_costs_percents,
            crewshare_and_unpaid_labour_costs_percents,
            other_variable_costs_per_unit_efforts,
            annual_insurance_costs_per_crews,
            standard_labour_hour_opportunity_costss,
            standard_annual_full_time_employement_hourss,
            other_annual_fixed_costss,
            vessel_values,
            annual_depreciation_rates,
            opportunity_interest_rates,
            annual_discount_rates,
            mInputName.toStdString(), mBasePath.toStdString()
    )) {
        return false;
    }


    cout << "fill in multimaps in loadVessels()" << endl;

    // read the more complex objects (i.e. when several info for a same vessel)...
    // also quarter specific but semester specific for the betas because of the survey design they are comning from...
    cout << "read_fgrounds in loadVessels()" << endl;
    auto fgrounds = read_fgrounds(a_quarter, mInputName.toStdString(), mBasePath.toStdString());
    cout << "read_fgrounds_init in loadVessels()" << endl;
    auto fgrounds_init = read_fgrounds_init(a_quarter, mInputName.toStdString(), mBasePath.toStdString());
    cout << "read_harbours in loadVessels()" << endl;
    auto harbours = read_harbours(a_quarter, mInputName.toStdString(), mBasePath.toStdString());

    cout << "read_freq_fgrounds in loadVessels()" << endl;
    multimap<string, double> freq_fgrounds = read_freq_fgrounds(a_quarter, mInputName.toStdString(),
                                                                mBasePath.toStdString());
    cout << "read_freq_fgrounds_init in loadVessels()" << endl;
    multimap<string, double> freq_fgrounds_init = read_freq_fgrounds_init(a_quarter, mInputName.toStdString(),
                                                                          mBasePath.toStdString());
    cout << "read_freq_harbours in loadVessels()" << endl;
    multimap<string, double> freq_harbours = read_freq_harbours(a_quarter, mInputName.toStdString(),
                                                                mBasePath.toStdString());
    cout << "read_vessels_betas in loadVessels()" << endl;
    multimap<string, double> vessels_betas = read_vessels_betas(a_semester, mInputName.toStdString(),
                                                                mBasePath.toStdString());
    cout << "read_vessels_tacs in loadVessels()" << endl;
    multimap<string, double> vessels_tacs = read_vessels_tacs(a_semester, mInputName.toStdString(),
                                                              mBasePath.toStdString());

    // debug
    if (fgrounds.size() != freq_fgrounds.size()) {
        cout << "please correct .dat files so that fgrounds and freq_fgrounds have same size!!!" << endl;
        // int tmp;
        // cin >> tmp;				 // pause
    }
    if (harbours.size() != freq_harbours.size()) {
        cout << "please correct .dat files so that harbours and freq_harbours have same size!!!" << endl;
        // int tmp;
        // cin >> tmp;				 // pause
    }

    // read nodes in polygons for area-based management
#if 0
    cout << "nodes_in_polygons() in loadVessels()" << endl;
    multimap<int, int> nodes_in_polygons= read_nodes_in_polygons(a_quarter, a_graph_name, mInputName.toStdString(), mBasePath.toStdString());


    // check
    //for (multimap<int, int>::iterator pos=nodes_in_polygons.begin(); pos != nodes_in_polygons.end(); pos++)
    //{
    //    cout << " a polygon node is " << pos->second << endl;
    //}
    //cout << " for " << a_graph_name << "in quarter " << a_quarter << endl;
# endif


    multimap<string, double> fishing_credits;
    if (binary_search(dyn_alloc_sce.begin(), dyn_alloc_sce.end(), "fishing_credits")) {
        fishing_credits = read_initial_fishing_credits(mInputName.toStdString(), mBasePath.toStdString());
    }


    //creation of a vector of vessels from vesselids, graph, harbours and fgrounds
    // and check the start coord
    multimap<types::NodeId, int> possible_metiers;
    multimap<types::NodeId, double> freq_possible_metiers;
    multimap<types::NodeId, double> gshape_cpue_per_stk_on_nodes;
    multimap<types::NodeId, double> gscale_cpue_per_stk_on_nodes;
    vector<types::NodeId> spe_fgrounds;
    vector<types::NodeId> spe_fgrounds_init;
    vector<types::NodeId> spe_harbours;
    vector<double> spe_freq_fgrounds;
    vector<double> spe_freq_fgrounds_init;
    vector<double> spe_freq_harbours;
    vector<double> spe_vessel_betas_per_pop;
    vector<double> spe_percent_tac_per_pop;
    vector<double> spe_fishing_credits;

    //here
    for (size_t i = 0; i < vesselids.size(); i++) {
        cout << "create vessel " << i << endl;
        // read vessel and quarter specific multimap
        // quarter specific to capture a piece of seasonality in the fishnig activity
        possible_metiers = read_possible_metiers(a_quarter, vesselids[i], mInputName.toStdString(),
                                                 mBasePath.toStdString());
        freq_possible_metiers = read_freq_possible_metiers(a_quarter, vesselids[i], mInputName.toStdString(),
                                                           mBasePath.toStdString());

        //cpue_per_stk_on_nodes = read_cpue_per_stk_on_nodes(a_quarter, vesselids[i], mName.toStdString());
        gshape_cpue_per_stk_on_nodes = read_gshape_cpue_per_stk_on_nodes(a_quarter, vesselids[i],
                                                                         mInputName.toStdString(),
                                                                         mBasePath.toStdString());
        gscale_cpue_per_stk_on_nodes = read_gscale_cpue_per_stk_on_nodes(a_quarter, vesselids[i],
                                                                         mInputName.toStdString(),
                                                                         mBasePath.toStdString());

        // debug
        if (possible_metiers.size() != freq_possible_metiers.size()) {
            cout << "please correct .dat files so that possible_metiers and freq_possible_metiers have same size!!!"
                 << "for the vessel " << vesselids[i] << endl;
            // int tmp;
            // cin >> tmp;			 // pause
        }

        // read the even more complex objects (i.e. when several info for a same vessel and a same ground)...
        // for creating the vessel object, search into the multimaps
        spe_fgrounds = find_entries(fgrounds, vesselids[i]);
        cout << " nb of grounds for this vessel is " << spe_fgrounds.size() << endl;
        spe_fgrounds_init = find_entries(fgrounds_init, vesselids[i]);
        spe_freq_fgrounds = find_entries(freq_fgrounds, vesselids[i]);
        spe_freq_fgrounds_init = find_entries(freq_fgrounds_init, vesselids[i]);
        spe_harbours = find_entries(harbours, vesselids[i]);
        spe_freq_harbours = find_entries_s_d(freq_harbours, vesselids[i]);
        spe_vessel_betas_per_pop = find_entries_s_d(vessels_betas, vesselids[i]);
        spe_percent_tac_per_pop = find_entries_s_d(vessels_tacs, vesselids[i]);

        if (binary_search(dyn_alloc_sce.begin(), dyn_alloc_sce.end(), "fishing_credits")) {
            spe_fishing_credits = find_entries_s_d(fishing_credits, vesselids[i]);
            for (size_t icr = 0; icr < spe_fishing_credits.size(); ++icr) {
                spe_fishing_credits.at(icr) = spe_fishing_credits.at(icr) * mScenario.getTotalAmountCredited();
            }

        }

        // choose a departure (node) harbour for this vessel according to the observed frequency in data
        types::NodeId start_harbour;

        if (!spe_harbours.empty()) {
            // need to convert in array, see myRutils.cpp
            auto one_harbour = do_sample(1, spe_harbours.size(), spe_harbours, spe_freq_harbours);
            start_harbour = one_harbour[0];
        } else {
            // if missing info for a given vessel for this quarter
            cout << "no specified harbour in this quarter for this vessel..." << endl;
            // CAUTION: LIKE A MAGIC NUMBER HERE!!!
            start_harbour = find_entries(harbours, vesselids[0])[0];
            spe_harbours.push_back(start_harbour);
            spe_freq_harbours.push_back(1);
            cout << "then take node: " << start_harbour << endl;
        }

        std::shared_ptr<Vessel> v(new Vessel(mNodes.at(start_harbour.toIndex())->mNode.get(),
                                             i,
                                             vesselids[i],
                                             nbpops,
                                             NBSZGROUP,
                                             spe_harbours,
                                             spe_fgrounds,
                                             spe_fgrounds_init,
                                             spe_freq_harbours,
                                             spe_freq_fgrounds,
                                             spe_freq_fgrounds_init,
                                             spe_vessel_betas_per_pop,
                                             spe_percent_tac_per_pop,
                                             possible_metiers,
                                             freq_possible_metiers,
                                             gshape_cpue_per_stk_on_nodes,
                                             gscale_cpue_per_stk_on_nodes,
                                             vid_is_actives[i],
                                             vid_is_part_of_ref_fleets[i],
                                             speeds[i],
                                             fuelcons[i],
                                             lengths[i],
                                             vKWs[i],
                                             carrycapacities[i],
                                             tankcapacities[i],
                                             nbfpingspertrips[i],
                                             resttime_par1s[i],
                                             resttime_par2s[i],
                                             av_trip_duration[i],
                                             mult_fuelcons_when_steaming[i],
                                             mult_fuelcons_when_fishing[i],
                                             mult_fuelcons_when_returning[i],
                                             mult_fuelcons_when_inactive[i],
                                             firm_ids[i],
                                             calendar[i],
                                             i < this_vessel_nb_crews.size() ? this_vessel_nb_crews[i] : 0,
                                             i < annual_other_incomes.size() ? annual_other_incomes[i] : 0,
                                             i < landing_costs_percents.size() ? landing_costs_percents[i] : 0,
                                             i < crewshare_and_unpaid_labour_costs_percents.size()
                                             ? crewshare_and_unpaid_labour_costs_percents[i] : 0,
                                             i < other_variable_costs_per_unit_efforts.size()
                                             ? other_variable_costs_per_unit_efforts[i] : 0,
                                             i < annual_insurance_costs_per_crews.size()
                                             ? annual_insurance_costs_per_crews[i] : 0,
                                             i < standard_labour_hour_opportunity_costss.size()
                                             ? standard_labour_hour_opportunity_costss[i] : 0,
                                             i < standard_annual_full_time_employement_hourss.size()
                                             ? standard_annual_full_time_employement_hourss[i] : 0,
                                             i < other_annual_fixed_costss.size() ? other_annual_fixed_costss[i] : 0,
                                             i < vessel_values.size() ? vessel_values[i] : 0,
                                             i < annual_depreciation_rates.size() ? annual_depreciation_rates[i] : 0,
                                             i < opportunity_interest_rates.size() ? opportunity_interest_rates[i] : 0,
                                             i < annual_discount_rates.size() ? annual_discount_rates[i] : 0
        ));

        std::shared_ptr<VesselData> vd(new VesselData(v));
        mVessels.push_back(vd);

        // some useful setters...
        // will also be useful when change of YEAR-QUARTER
        v->set_spe_fgrounds(spe_fgrounds);
        v->set_spe_fgrounds_init(spe_fgrounds_init);
        v->set_spe_harbours(spe_harbours);
        v->set_spe_freq_fgrounds(spe_freq_fgrounds);
        v->set_spe_freq_fgrounds_init(spe_freq_fgrounds_init);
        v->set_spe_freq_harbours(spe_freq_harbours);
        v->set_spe_betas_per_pop(spe_vessel_betas_per_pop);
        v->set_spe_percent_tac_per_pop(spe_percent_tac_per_pop);
        v->set_spe_possible_metiers(possible_metiers);
        v->set_spe_freq_possible_metiers(freq_possible_metiers);

        if (binary_search(dyn_alloc_sce.begin(), dyn_alloc_sce.end(), "fishing_credits")) {
            v->set_fishing_credits(spe_fishing_credits);
        }

        // inform grounds in closed areas
# if 0
        vector <int> grds = v->get_fgrounds();
        vector <int> fgrounds_in_closed_areas;
        vector<int> polygons;
        vector<int> polygon_nodes;
        for (multimap<int, int>::const_iterator pos=nodes_in_polygons.begin(); pos != nodes_in_polygons.end(); pos++)
            {
                // get all values across the keys
                polygons.push_back(pos->first);
                polygon_nodes.push_back(pos->second);
            }
        sort (polygon_nodes.begin(), polygon_nodes.end());
        for(unsigned int i=0; i<grds.size();++i){
            int ground=grds.at(i);
            std::vector<int>::iterator it=find(polygon_nodes.begin(), polygon_nodes.end(), grds.at(i));
            if(it!=polygon_nodes.end()) fgrounds_in_closed_areas.push_back(ground); // found
        }
        v->set_fgrounds_in_closed_areas(fgrounds_in_closed_areas);

        // check
        cout << "for this vessel, check included fgrounds in polygon_nodes" << endl;
        for (unsigned int i=0;i<fgrounds_in_closed_areas.size();++i)
          {
          cout << fgrounds_in_closed_areas.at(i) << " ";
          }
        cout << endl;
# endif

        // for dyn sce. CAUTION: MAGIC NUMBERS HERE FOR SOME SCENARIOS....
        // dyn sce.
        if (binary_search(dyn_alloc_sce.begin(), dyn_alloc_sce.end(), "reduced_speed_10percent")) {
            // a decrease by 10%...
            v->set_speed(v->get_speed() * 0.9);
            // corresponds to a decrease by 30% in fuelcons
            v->set_fuelcons(v->get_fuelcons() * 0.7);
            // cubic law  c=v^3, see Ronen 1982
            // e.g. assuming a v at 10, the fuel conso is lowered by (in %) =>  (1- (((seq(0.1,1,by=0.1)*10)^3 ) / (1*10^3)) )*100
        }
        // dyn sce.
        if (binary_search(dyn_alloc_sce.begin(), dyn_alloc_sce.end(), "reduced_speed_20percent")) {
            // a decrease by 20%...
            v->set_speed(v->get_speed() * 0.8);
            // corresponds to a decrease by 48.8% in fuelcons
            v->set_fuelcons(v->get_fuelcons() * 0.512);
            // cubic law  c=v^3, see Ronen 1982
            // e.g. assuming a v at 10, the fuel conso is lowered by (in %) =>  (1- (((seq(0.1,1,by=0.1)*10)^3 ) / (1*10^3)) )*100
        }
        // dyn sce.
        if (binary_search(dyn_alloc_sce.begin(), dyn_alloc_sce.end(), "reduced_speed_30percent")) {
            // a decrease by 30%...
            v->set_speed(v->get_speed() * 0.7);
            // corresponds to a decrease by 65.7% in fuelcons
            v->set_fuelcons(v->get_fuelcons() * 0.343);
            // cubic law  c=v^3, see Ronen 1982
            // e.g. assuming a v at 10, the fuel conso is lowered by (in %) =>  (1- (((seq(0.1,1,by=0.1)*10)^3 ) / (1*10^3)) )*100
        }

        // a particular setters for the CPUE STUFF...
        // for implicit pops or "out of range" fishing: create cpue_nodes_species
        // a vector of vector (with dims [relative index of fishing ground nodes;  pops])
        // we use a vector of vector instead of a multimap in order to speed up the simulation
        // by avoiding a (costly) call to find_entries_i_d() in the do_catch() method
        vector<types::NodeId> gshape_name_nodes_with_cpue;
        for (auto iter = gshape_cpue_per_stk_on_nodes.begin(); iter != gshape_cpue_per_stk_on_nodes.end();
             iter = gshape_cpue_per_stk_on_nodes.upper_bound(iter->first)) {
            gshape_name_nodes_with_cpue.push_back(iter->first);
        }

        // sort and unique
        sort(gshape_name_nodes_with_cpue.begin(), gshape_name_nodes_with_cpue.end());
        auto it = std::unique(gshape_name_nodes_with_cpue.begin(), gshape_name_nodes_with_cpue.end());
        gshape_name_nodes_with_cpue.resize(std::distance(gshape_name_nodes_with_cpue.begin(), it));


        // init cpue_nodes_species for this vessel
        int nbnodes = gshape_name_nodes_with_cpue.size();
        // init the vector of vector with Os
        mVessels.at(i)->mVessel->init_gshape_cpue_nodes_species(nbnodes, nbpops);
        // init the vector of vector with Os
        mVessels.at(i)->mVessel->init_gscale_cpue_nodes_species(nbnodes, nbpops);
        for (size_t n = 0; n < gshape_name_nodes_with_cpue.size(); n++) {
            // look into the multimap...
            auto gshape_cpue_species = find_entries(gshape_cpue_per_stk_on_nodes, gshape_name_nodes_with_cpue[n]);
            // look into the multimap...
            auto gscale_cpue_species = find_entries(gscale_cpue_per_stk_on_nodes, gshape_name_nodes_with_cpue[n]);
            if (!gshape_cpue_species.empty()) {
                // caution here: the n is the relative index of the node for this vessel i.e. this is not the graph index of the node (because it would have been useless to create a huge matrix filled in by 0 just to preserve the graph idex in this case!)
                mVessels.at(i)->mVessel->set_gshape_cpue_nodes_species(n, gshape_cpue_species);
                // caution here: the n is the relative index of the node for this vessel i.e. this is not the graph index of the node (because it would have been useless to create a huge matrix filled in by 0 just to preserve the graph idex in this case!)
                mVessels.at(i)->mVessel->set_gscale_cpue_nodes_species(n, gscale_cpue_species);
            }
        }

        cout
                << "is everything OK in the gshape_cpue_nodes_species and gscale_cpue_nodes_species file? if not check pop dim"
                << endl;
        // need to compute expected cpue (averaged over node but cumulated over species)
        // for this particular vessel, in order to scale the prior guess (see below)
        double expected_cpue = 0;
        vector<vector<double> > gshape_cpue_nodes_species = mVessels.at(i)->mVessel->get_gshape_cpue_nodes_species();
        vector<vector<double> > gscale_cpue_nodes_species = mVessels.at(i)->mVessel->get_gscale_cpue_nodes_species();
        auto fgrounds = mVessels.at(i)->mVessel->get_fgrounds();
        vector<double> expected_cpue_this_pop(nbpops);
        for (int pop = 0; pop < nbpops; pop++) {
            cout << "...for pop" << pop << endl;
            cout << "... on " << fgrounds.size() << " grounds" << endl;
            cout << "... given " << gshape_cpue_nodes_species.size() << " size gshape" << endl;
            cout << "... given " << gscale_cpue_nodes_species.size() << " size gscale" << endl;

            vector<double> cpue_per_fground(fgrounds.size());
            // init
            expected_cpue_this_pop.at(pop) = 0;

            // compute cpue on nodes
            for (size_t f = 0; f < fgrounds.size(); ++f) {
                // look into the vector of vector....
                double a_shape = gshape_cpue_nodes_species.at(f).at(pop);
                // look into the vector of vector....
                double a_scale = gscale_cpue_nodes_species.at(f).at(pop);

                // a dangerous fix:
                if (a_shape < 0 || a_scale < 0) {

                    cout << "Something weird with the Gamma parameters: some negative values loaded...." << endl;
                    for (size_t f = 0; f < fgrounds.size(); ++f) {
                        cout << " this vessel is is: " << mVessels.at(i)->mVessel->get_name() << endl;
                        cout << " this gr  gscale is: " << gscale_cpue_nodes_species.at(f).at(pop) << endl;
                        cout << " this gr  of gshape is: " << gshape_cpue_nodes_species.at(f).at(pop) << endl;
                    }
                    a_shape = 1;
                    a_scale = 0;
                }


                cpue_per_fground.at(f) = rgamma(a_shape, a_scale);
                //if( v->get_idx() ==2) dout << "cpue_per_fground.at(f)" <<cpue_per_fground.at(f) << endl;
                // unfortunately this might be 0 for the target species if the used metiers has small mismatch in pop.
            }

            //cout << "compute the average cpue for this pop across all nodes" << endl;
            // compute the average cpue for this pop across all nodes
            for (size_t f = 0; f < fgrounds.size(); ++f) {
                expected_cpue_this_pop.at(pop) += cpue_per_fground.at(f);

            }
            // do the mean
            if (expected_cpue_this_pop.at(pop) != 0) {
                expected_cpue_this_pop.at(pop) = expected_cpue_this_pop.at(pop) / fgrounds.size();
            }

            // sum over pop
            expected_cpue += expected_cpue_this_pop.at(pop);
        }
        cout << "expected_cpue_this_pop computation was successful...." << endl;


        // init at 0 cumcatch and cumeffort per trip,
        // init at best guest the experiencedcpue_fgrounds
        vector<double> freq_fgrounds = mVessels.at(i)->mVessel->get_freq_fgrounds();
        vector<double> init_for_fgrounds(fgrounds.size());
        vector<double> cumeffort_fgrounds = init_for_fgrounds;
        vector<double> cumcatch_fgrounds = init_for_fgrounds;
        vector<double> cumdiscard_fgrounds = init_for_fgrounds;
        vector<double> experienced_bycatch_prop_on_fgrounds = init_for_fgrounds;
        vector<double> experienced_avoided_stks_bycatch_prop_on_fgrounds = init_for_fgrounds;
        vector<double> experiencedcpue_fgrounds = init_for_fgrounds;
        vector<double> freq_experiencedcpue_fgrounds = init_for_fgrounds;
        vector<vector<double> > cumcatch_fgrounds_per_pop(fgrounds.size(), vector<double>(nbpops));
        vector<vector<double> > cumdiscard_fgrounds_per_pop(fgrounds.size(), vector<double>(nbpops));
        vector<vector<double> > experiencedcpue_fgrounds_per_pop(fgrounds.size(), vector<double>(nbpops));
        vector<vector<double> > freq_experiencedcpue_fgrounds_per_pop(fgrounds.size(), vector<double>(nbpops));
        for (size_t f = 0; f < fgrounds.size(); f++) {
            cumcatch_fgrounds[f] = 0;
            cumdiscard_fgrounds[f] = 0;
            experienced_bycatch_prop_on_fgrounds[f] = 0;
            experienced_avoided_stks_bycatch_prop_on_fgrounds[f] = 0;
            cumeffort_fgrounds[f] = 0;
            experiencedcpue_fgrounds[f] = freq_fgrounds[f] * expected_cpue;
            // this should be init so that it constitutes a good qualified guess to be a prior in the bayesian formula...
            // first condition: init different to 0 to allow the ground to be chosen even if it has not been visited yet...
            // second condition: to avoid starting from 0 cpue, init accounting for prior from frequency of visit from the data
            // third condition: to scale the start cpue, multiply by the expectancy of the cpue for this particular vessel

            // init the ones per pop
            for (int pop = 0; pop < nbpops; pop++) {
                // init
                cumcatch_fgrounds_per_pop[f][pop] = 0;
                cumdiscard_fgrounds_per_pop[f][pop] = 0;
                experiencedcpue_fgrounds_per_pop[f][pop] = freq_fgrounds[f] * expected_cpue_this_pop.at(pop);
            }
        }

        // per total...
        mVessels.at(i)->mVessel->set_cumcatch_fgrounds(cumcatch_fgrounds);
        mVessels.at(i)->mVessel->set_cumdiscard_fgrounds(cumdiscard_fgrounds);
        mVessels.at(i)->mVessel->set_experienced_bycatch_prop_on_fgrounds(experienced_bycatch_prop_on_fgrounds);
        mVessels.at(i)->mVessel->set_experienced_bycatch_prop_on_fgrounds(
                experienced_avoided_stks_bycatch_prop_on_fgrounds);
        mVessels.at(i)->mVessel->set_cumeffort_fgrounds(cumeffort_fgrounds);
        mVessels.at(i)->mVessel->set_experiencedcpue_fgrounds(experiencedcpue_fgrounds);
        mVessels.at(i)->mVessel->set_freq_experiencedcpue_fgrounds(freq_experiencedcpue_fgrounds);
        // compute for the first time, to get freq_experiencedcpue_fgrounds...
        mVessels.at(i)->mVessel->compute_experiencedcpue_fgrounds();
        // ...or per pop
        mVessels.at(i)->mVessel->set_cumcatch_fgrounds_per_pop(cumcatch_fgrounds_per_pop);
        mVessels.at(i)->mVessel->set_cumdiscard_fgrounds_per_pop(cumdiscard_fgrounds_per_pop);
        mVessels.at(i)->mVessel->set_experiencedcpue_fgrounds_per_pop(experiencedcpue_fgrounds_per_pop);
        mVessels.at(i)->mVessel->set_freq_experiencedcpue_fgrounds_per_pop(freq_experiencedcpue_fgrounds_per_pop);
        // compute for the first time, to get freq_experiencedcpue_fgrounds_per_pop...
        mVessels.at(i)->mVessel->compute_experiencedcpue_fgrounds_per_pop();

        // note that, at the start of the simu, freq of visit will be equivalent to freq_fgrounds
        // and then freq of visit will be updated (via the bayes rule) trip after trip from this initial freqency...
        // the expected_cpue is to scale to the encountered cpue i.e. freq of visit will decrease if experienced cpue < expected cpue
        // and vice versa...

        // initialise the individual quota from global_TAC*percent_in_simu*percent_this_vessel


//TODO: check this!
#if 0
        for (unsigned int sp=0; sp<populations.size(); sp++)
        {
            mVessels.at(i)->set_individual_tac_this_pop(export_individual_tacs, 0, populations, implicit_pops, sp, 1, 0.0);
        }
#endif

        // check
        cout << "create vessel " << v->get_idx() << " " << v->get_name() << " " << v->get_nationality() << " on "
             << v->get_loc()->get_idx_node().toIndex() << " with coordinates "
             << v->get_loc()->get_x() << " " << v->get_loc()->get_y() << endl;
        //   << " and metier " << v->get_metier()->get_name() <<  endl;
        //vector<double> a_ogive = v->get_metier()->get_selectivity_ogive() ;
        //cout << "with selectivity ogive " << endl;
        //for (int i=0; i<a_ogive.size(); i++)
        //{
        //    cout  << " " << a_ogive[i] << " " ;
        //}
        //cout << endl; // well...nothing there because a metier is still not assigned at this stage...



    }

    return true;
}

bool DisplaceModel::loadGraphs()
{
    struct data {
        int from;
        int to;
        double weight;
    };

    QString filename_graph = mBasePath + "/graphsspe/graph" + QString::number(mScenario.getGraph()) + ".dat";
    qDebug() << "Loading: " << filename_graph << " With " << mScenario.getNrow_graph() << " lines";

    QFile f(filename_graph);
    if (!f.open(QIODevice::ReadOnly)) {
        throw DisplaceException(f.errorString());
        return false;
    }

    QTextStream strm(&f);
    QString line;
    int linenum = 0;
    QList<data> datas;

    while (!(line = strm.readLine()).isNull()) {
        bool ok;
        int stat = linenum / mScenario.getNrow_graph();
        int idx = linenum % mScenario.getNrow_graph();

        switch (stat) {
            case 0: // from
                datas.push_back(data());
                datas[idx].from = line.toInt(&ok);
                break;
            case 1: // to
                datas[idx].to = line.toInt(&ok);
                break;
            case 2: // weight
                datas[idx].weight = line.toDouble(&ok);
                break;
        }

        if (!ok) {
            throw DisplaceException(
                    QString("Graph %2 Parse error at line %1 (%3)").arg(linenum + 1).arg(filename_graph).arg(line));
            return false;
        }

        ++linenum;
    }

    /* file has been read. Now feed the data! */
            foreach (data d, datas) {
            mNodes[d.from]->appendAdiancency(mNodes.at(d.to), d.weight);
        }

    return true;
}


bool DisplaceModel::initShips()
{


    // read general ship features (incl. their specific lanes)
    vector<string> shipids;
    vector<double> imos;
    vector<double> yearbuilds;
    vector<string> flags;
    vector<string> types;
    vector<double> typecodes;
    vector<double> loas;
    vector<double> KWs;
    vector<double> breadths;
    vector<double> grosstonnages;
    vector<double> fueluses;
    vector<double> NOxEmission_gperKWhs;
    vector<double> SOxEmission_percentpertotalfuelmasss;
    vector<double> GHGEmissions;
    vector<double> PMEmissions;
    vector<double> nbunits;
    vector<double> vmaxs;
    vector<double> vcruises;
    vector<double> lane_ids;

    if (!read_ships_features(shipids, imos, yearbuilds, flags, types, typecodes,
                             loas, KWs, breadths, grosstonnages, nbunits, fueluses, NOxEmission_gperKWhs,
                             SOxEmission_percentpertotalfuelmasss, GHGEmissions, PMEmissions,
                             vmaxs, vcruises, lane_ids, mInputName.toStdString(), mBasePath.toStdString())) {
        return false;
    }

    // read shipping lanes
    multimap<int, double> shiplanes_lat = read_shiplanes_lat(mInputName.toStdString(), mBasePath.toStdString());
    multimap<int, double> shiplanes_lon = read_shiplanes_lon(mInputName.toStdString(), mBasePath.toStdString());


    vector<double> lats;
    vector<double> longs;


    for (unsigned int i = 0; i < shipids.size(); i++) {
        cout << "create ship " << shipids[i] << endl;

        lats = find_entries_i_d(shiplanes_lat, lane_ids[i]);
        longs = find_entries_i_d(shiplanes_lon, lane_ids[i]);

        std::shared_ptr<Ship> sh(new Ship(i, shipids[i], 1, imos[i], yearbuilds[i], flags[i],
                                          types[i], typecodes[i], loas[i], KWs[i], breadths[i],
                                          grosstonnages[i], nbunits[i],
                                          fueluses[i], NOxEmission_gperKWhs[i],
                                          SOxEmission_percentpertotalfuelmasss[i],
                                          GHGEmissions[i], PMEmissions[i],
                                          vmaxs[i], vcruises[i],
                                          longs, lats
        ));

        std::shared_ptr<ShipData> shd(new ShipData(sh));
        mShips.push_back(shd);

        cout << "....OK " << endl;

    }


    return true;
}


bool DisplaceModel::initFishfarm()
{

    // read general fishfarms features
    vector<int> all_fishfarms_ids;
    vector<string> fishfarms_names;
    vector<int> idx_nodes;
    vector<int> is_actives;
    vector<double> fishfarms_sizes;
    vector<double> fishfarms_longs;
    vector<double> fishfarms_lats;
    vector<double> mean_SSTs;
    vector<double> mean_salinities;
    vector<double> mean_windspeeds;
    vector<double> mean_currentspeeds;
    vector<double> max_depths;
    vector<double> diss_O2_mg_per_ls;
    vector<double> Linf_mms;
    vector<double> K_ys;
    vector<double> t0_ys;
    vector<double> fulton_condition_factors;
    vector<string> meanw_growth_model_types;
    vector<int> start_day_growings;
    vector<int> end_day_harvests;
    vector<int> nbyears_for_growths;
    vector<int> nb_days_fallowing_periods;
    vector<int> nb_fish_at_starts;
    vector<double> meanw_at_starts;
    vector<double> price_per_kg_at_starts;
    vector<double> target_meanw_at_harvests;
    vector<int> nb_fish_at_harvests;
    vector<double> meanw_at_harvests;
    vector<double> prop_harvest_kg_solds;
    vector<double> kg_eggs_per_kgs;
    vector<double> price_eggs_per_kgs;
    vector<double> N_in_fish_kg_3pers;
    vector<double> P_in_fish_kg_0_5pers;
    vector<string> feed_types;
    vector<double> feed_price_per_kgs;
    vector<double> total_feed_kgs;
    vector<double> prop_N_in_feeds;
    vector<double> prop_P_in_feeds;
    vector<double> total_feed_N_kgs;
    vector<double> total_feed_P_kgs;
    vector<string> feed_type_vets;
    vector<double> feed_vet_price_per_kgs;
    vector<double> total_feed_vet_kgs;
    vector<double> prop_N_in_feed_vets;
    vector<double> prop_P_in_feed_vets;
    vector<double> total_feed_vet_N_kgs;
    vector<double> total_feed_vet_P_kgs;
    vector<double> annual_discharge_N_kgs;
    vector<double> annual_discharge_P_kgs;
    vector<double> annual_discharge_C_kgs;
    vector<double> annual_discharge_heavymetals_kgs;
    vector<double> annual_discharge_medecine_kgs;
    vector<double> net_harvest_kg_per_sqkm_ys;
    vector<double> market_price_sold_fishs;
    vector<double> operating_cost_per_days;
    vector<double> annual_profits;


    if (!read_fishfarms_features(all_fishfarms_ids, fishfarms_names, idx_nodes, is_actives, fishfarms_sizes,
                                 fishfarms_longs, fishfarms_lats,
                                 mean_SSTs,
                                 mean_salinities,
                                 mean_windspeeds,
                                 mean_currentspeeds,
                                 max_depths,
                                 diss_O2_mg_per_ls,
                                 Linf_mms,
                                 K_ys,
                                 t0_ys,
                                 fulton_condition_factors,
                                 meanw_growth_model_types,
                                 start_day_growings,
                                 end_day_harvests,
                                 nbyears_for_growths,
                                 nb_days_fallowing_periods,
                                 nb_fish_at_starts,
                                 meanw_at_starts,
                                 price_per_kg_at_starts,
                                 target_meanw_at_harvests,
                                 nb_fish_at_harvests,
                                 meanw_at_harvests,
                                 prop_harvest_kg_solds,
                                 kg_eggs_per_kgs,
                                 price_eggs_per_kgs,
                                 N_in_fish_kg_3pers,
                                 P_in_fish_kg_0_5pers,
                                 feed_types,
                                 feed_price_per_kgs,
                                 total_feed_kgs,
                                 prop_N_in_feeds,
                                 prop_P_in_feeds,
                                 total_feed_N_kgs,
                                 total_feed_P_kgs,
                                 feed_type_vets,
                                 feed_vet_price_per_kgs,
                                 total_feed_vet_kgs,
                                 prop_N_in_feed_vets,
                                 prop_P_in_feed_vets,
                                 total_feed_vet_N_kgs,
                                 total_feed_vet_P_kgs,
                                 annual_discharge_N_kgs,
                                 annual_discharge_P_kgs,
                                 annual_discharge_C_kgs,
                                 annual_discharge_heavymetals_kgs,
                                 annual_discharge_medecine_kgs,
                                 net_harvest_kg_per_sqkm_ys,
                                 market_price_sold_fishs,
                                 operating_cost_per_days,
                                 annual_profits,
                                 mInputName.toStdString(), mBasePath.toStdString())) {
        return false;
    }


    for (size_t id = 0; id < all_fishfarms_ids.size(); ++id) {
        cout << "create fishfarms " << all_fishfarms_ids.at(id) << endl;

        auto node = mNodes.at(idx_nodes.at(id));
        auto fi = std::make_shared<Fishfarm>(all_fishfarms_ids.at(id), fishfarms_names.at(id), node->mNode.get(), 0,
                                             is_actives.at(id),
                                             fishfarms_sizes.at(id), fishfarms_longs.at(id), fishfarms_lats.at(id),
                                             mean_SSTs.at(id), mean_salinities.at(id), mean_windspeeds.at(id),
                                             mean_currentspeeds.at(id),
                                             max_depths.at(id), diss_O2_mg_per_ls.at(id),
                                             Linf_mms.at(id), K_ys.at(id), t0_ys.at(id),
                                             fulton_condition_factors.at(id), meanw_growth_model_types.at(id),
                                             start_day_growings.at(id), end_day_harvests.at(id),
                                             nbyears_for_growths.at(id), nb_days_fallowing_periods.at(id),
                                             nb_fish_at_starts.at(id), meanw_at_starts.at(id),
                                             price_per_kg_at_starts.at(id), target_meanw_at_harvests.at(id),
                                             nb_fish_at_harvests.at(id), meanw_at_harvests.at(id),
                                             prop_harvest_kg_solds.at(id), kg_eggs_per_kgs.at(id),
                                             price_eggs_per_kgs.at(id),
                                             N_in_fish_kg_3pers.at(id), P_in_fish_kg_0_5pers.at(id),
                                             feed_types.at(id), feed_price_per_kgs.at(id), total_feed_kgs.at(id),
                                             prop_N_in_feeds.at(id), prop_P_in_feeds.at(id),
                                             total_feed_N_kgs.at(id), total_feed_P_kgs.at(id),
                                             feed_type_vets.at(id), feed_vet_price_per_kgs.at(id),
                                             total_feed_vet_kgs.at(id), prop_N_in_feed_vets.at(id),
                                             prop_P_in_feed_vets.at(id),
                                             total_feed_vet_N_kgs.at(id), total_feed_vet_P_kgs.at(id),
                                             annual_discharge_N_kgs.at(id), annual_discharge_P_kgs.at(id),
                                             annual_discharge_C_kgs.at(id), annual_discharge_heavymetals_kgs.at(id),
                                             annual_discharge_medecine_kgs.at(id), net_harvest_kg_per_sqkm_ys.at(id),
                                             market_price_sold_fishs.at(id), operating_cost_per_days.at(id),
                                             annual_profits.at(id)
        );

        auto fid = std::make_shared<FishfarmData>(fi);
        mFishfarms.push_back(fid);
    }

    return true;


}


bool DisplaceModel::initWindmill()
{
    map<int, double> init_size_per_windmill = read_size_per_windmill(mInputName.toStdString(), mBasePath.toStdString());

    for (auto iter : init_size_per_windmill) {
        cout << "create windmill " << iter.first << endl;

        auto node = mNodes.at(iter.first);
        auto wm = std::make_shared<Windmill>(iter.first, "a_windfarm_name", node->mNode.get(), iter.second, 1, 500, 1);

        auto wmd = std::make_shared<WindmillData>(wm);
        mWindmills.push_back(wmd);
    }

    return true;
}


bool DisplaceModel::initFirm()
{

    // read general firm features
    vector<int> all_firm_ids;
    vector<string> firm_names;
    vector<int> nb_vessels_per_firm;
    vector<double> some_longs;
    vector<double> some_lats;

    if (!read_firms_features(all_firm_ids, firm_names, nb_vessels_per_firm, some_longs, some_lats,
                             mInputName.toStdString(), mBasePath.toStdString())) {
        return false;
    }


    for (size_t id = 0; id < all_firm_ids.size(); ++id) {
        cout << "create firm " << all_firm_ids.at(id) << endl;


        // TO DO: FIX THIS BELOW CODE
        // vector <Vessel*> vessels_of_the_firm(mVessels.size());


        // select from a vector of objects
        //  std::remove_copy_if(
        //             mVessels.begin(),
        //             mVessels.end(),
        //             std::back_inserter(vessels_of_the_firm),
        //             boost::bind(test_not_belong_to_firm, _1, all_firm_ids.at(id)) );


        //     auto fi = std::make_shared<Firm>(id, all_firm_ids.at(id), firm_names.at(id),  nb_vessels_per_firm.at(id),
        //                                     some_longs.at(id), some_lats.at(id), vessels_of_the_firm);

        //    auto fid = std::make_shared<FirmData>(fi);
        //    mFirms.push_back(fid);
    }

    return true;
}


bool DisplaceModel::initBenthos()
{
    QList<int> ids;

    cout << "create benthos" << endl;

            foreach (std::shared_ptr<NodeData> nd, mNodes) {
            int bm = nd->get_marine_landscape();
            std::shared_ptr<Benthos> benthos;

            QMap<int, std::shared_ptr<Benthos> >::iterator it = mBenthosInfo.find(bm);
            if (it == mBenthosInfo.end()) {
                benthos = std::shared_ptr<Benthos>(new Benthos(bm));
                ids.push_back(bm);
                mBenthosInfo.insert(bm, benthos);
            } else {
                benthos = it.value();
            }

            benthos->appendNode(nd.get());
        }

    qSort(ids);
    int idx = 0;
            foreach (int id, ids) {
            mBenthosInfo[id]->setIdx(idx++);
            mBenthos.push_back(mBenthosInfo[id]);
        }

    return true;
}


bool DisplaceModel::initPopulations()
{
    cout << "init pop" << endl;

    QList<int> imp = mConfig.implicit_pops();
    qSort(imp);

    QList<int> imp2 = mConfig.implicit_pops_level2();
    qSort(imp2);

    clearInterestingPop();


    for (int i = 0; i < mConfig.getNbpops(); ++i) {
        setInterestingPop(i);
    }
    for (int i = 0; i < imp.size(); ++i) {
        remInterestingPop(imp[i]);
    }

    /*
     * broken when not a single implicit pop is given in input
     *    int c = 0;
       for (int i = 0; i < imp.size(); ++i) {
           while (c < imp[i]) {
               setInterestingPop(c);
               ++c;
           }
           ++c;
       }
    */

    clearInterestingPop2();
    int c2 = 0;
    for (int i = 0; i < imp2.size(); ++i) {
        while (c2 < imp2[i]) {
            setInterestingPop2(c2);
            ++c2;
        }
        ++c2;
    }

    return true;
}

bool DisplaceModel::initNations()
{
    cout << "init nation" << endl;

    // nations are read from vessels.
    QMultiMap<QString, std::shared_ptr<VesselData> > nationSet;
            foreach (std::shared_ptr<VesselData> vessel, mVessels) {
            nationSet.insertMulti(QString::fromStdString(vessel->mVessel->get_nationality()), vessel);
        }

    mNations.clear();
    QList<QString> nationsName = nationSet.uniqueKeys();
    for (int i = 0; i < nationsName.size(); ++i) {
        std::shared_ptr<NationData> data(new NationData());
        data->setName(nationsName[i]);

        QList<std::shared_ptr<VesselData> > vessels = nationSet.values(nationsName[i]);
                foreach (std::shared_ptr<VesselData> vessel, vessels) {
                vessel->setNationality(i);
            }
        mNations.push_back(data);

        setInterestingNations(i);
    }

    return true;
}

bool DisplaceModel::loadNodesFromDb()
{
    mNodes.clear();
    if (!mDb->loadNodes(mNodes, mHarbours, this)) {
        return false;
    }
    return true;
}

bool DisplaceModel::loadVesselsFromDb()
{
    mVessels.clear();
    if (!mDb->loadVessels(mNodes, mVessels)) {
        return false;
    }
    initNations();

    return true;
}

bool DisplaceModel::loadFishfarmsFromDb()
{
    mFishfarms.clear();
    if (!mDb->loadFishFarms(mNodes, mFishfarms)) {
        return false;
    }
    return true;
}
