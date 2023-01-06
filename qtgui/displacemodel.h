// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2022 Francois Bastardie <fba@aqua.dtu.dk>

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

#ifndef DISPLACEMODEL_H
#define DISPLACEMODEL_H

#include <scenario.h>
#include <config.h>
#include <nodepenalty.h>

#include <modelobjects/nodedata.h>
#include <modelobjects/vesseldata.h>
#include <modelobjects/firmdata.h>
#include <modelobjects/shipdata.h>
#include <modelobjects/fishfarmdata.h>
#include <modelobjects/windmilldata.h>
#include <modelobjects/benthos.h>
#include <modelobjects/metierdata.h>
#include <modelobjects/populationdata.h>
#include <modelobjects/nationdata.h>
#include <modelobjects/harbourdata.h>
#include <historicaldatacollector.h>
#include <outputfileparser.h>
#include <graphbuilder_shp.h>
#include <objects/metiersentity.h>
#include <utils/interestinglist.h>
#include <utils/interestinglistwithspecialvalues.h>
#include <stats/benthosstats.h>
#include <stats/fishfarmsstats.h>
#include <stats/shipsstats.h>
#include <stats/windfarmsstats.h>

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>
#include <QThread>
#include <QMutex>

#include <memory>
#include <functional>

#include <ogrsf_frmts.h>

#include <QDebug>

class DbHelper;
class MapObjectsController;
class Calendar;
class MapsDataProvider;

class SQLiteOutputStorage;

class DisplaceModel : public QObject
{
    Q_OBJECT
public:
    typedef HistoricalDataCollector<BenthosStats> BenthosStatsContainer;
    typedef HistoricalDataCollector<FishfarmsStats> FishfarmsStatsContainer;
    typedef HistoricalDataCollector<ShipsStats> ShipsStatsContainer;

    enum ModelType {
        LiveModelType, EditorModelType, OfflineModelType,
        EmptyModelType
    };

    enum SpecialGroups { Total, Average, Min, Max,
                          LastSpecialGroup };

    DisplaceModel();
    ~DisplaceModel();

    ModelType modelType() const { return mModelType; }

    MapsDataProvider &getMapDataProvider();

    bool edit(QString modelname);
    void setIndex(int idx) { mIndex = idx; }
    int index() const { return mIndex; }

    bool load (QString path, ModelType type);
    bool loadDatabase (QString path);
    bool clearStats();
    bool saveScenarioAs(const QString &path);
    bool saveScenario();
    bool saveConfig();

    std::shared_ptr<Calendar> calendar() const { return mCalendar; }

    /** \brief Signals the simulation has ended. Flush all buffers */
    void Q_DECL_DEPRECATED simulationEnded();

    // Getter
    QString fullpath() const { return mFullPath; }
    QString outDir() const { return mOutDir; }
    QString inputName() const { return mInputName; }
    QString basepath() const { return mBasePath; }
    QString outputName() const { return mOutputName; }
    void setOutDir(const QString &name) { mOutDir = name; }
    void setOutputName(const QString &name) { mOutputName = name; }
    QString simulationName() const { return mSimuName; }
    void setSimulationName(const QString &name) { mSimuName = name; }
    void setSimulationSqlStorage(const QString &path);

    QString linkedDatabase() const { return mLinkedDbName; }
    bool isModelLoaded() const { return !mInputName.isEmpty(); }

    int getNBPops() const {
        return mConfig.getNbpops();
    }
    int getNBMets() const {
        return mConfig.getNbmets();
    }
    int getNBBenthosPops() const {
        return mConfig.getNbbenthospops();
    }
    int getSzGrupsCount() const {
        return mConfig.getSzGroups();
    }
    int getBenthosIdx(int benthosId) const;

    QString getOutDir() const;
    int getSimulationSteps() const;
    void setSimulationSteps(int value);
    int getUseStaticPaths() const;
    void setUseStaticPaths(int value);

    /* Graphs operation */

    const QList<std::shared_ptr<HarbourData> > &getHarboursList() const { return mHarbours; }
    int getHarboursCount() const;
    QString getHarbourId(int idx) const;

    const QList<std::shared_ptr<NodeData> > &getNodesList() const { return mNodes; }
    int getNodesCount() const;
    QString getNodeId(int idx) const;
    QList<std::shared_ptr<NodeData> > getAllNodesWithin (const QPointF &centerpoint, double dist_km) const;

    /** \brief receive a Stats update for nodes from the Simulator
     *
     * The string passed has the following format:
     *    stat,tstep,first,number,data
     *
     * */
    void updateNodesStatFromSimu(QString);

    const QList<std::shared_ptr<VesselData> > &getVesselList() const { return mVessels; }
    int getVesselCount() const;
    QString getVesselId(int idx) const;
    void updateVessel (int tstep, int idx, float x, float y, float course, float fuel, int state );

    const QList<std::shared_ptr<FishfarmData> > &getFishfarmList() const { return mFishfarms; }
    int getFishfarmsCount() const;
    QString getFishfarmId(int idx) const;
    void updateFishfarm (int idx, float x, float y);


    const QList<std::shared_ptr<FirmData> > &getFirmList() const { return mFirms; }
    int getFirmCount() const;
    QString getFirmId(int idx) const;
    void updateFirm (int idx, float x, float y);


    const QList<std::shared_ptr<WindmillData> > &getWindmillList() const { return mWindmills; }
    int getWindmillCount() const;
    QString getWindmillId(int idx) const;
    void updateWindmill (int idx, float x, float y);

    const QList<std::shared_ptr<ShipData> > &getShipList() const { return mShips; }
    int getShipsCount() const;
    QString getShipId(int idx) const;
    void updateShip (int tstep, int idx, float x, float y, float course);

    const QList<std::shared_ptr<Benthos> > &getBenthosList() const { return mBenthos; }
    int getBenthosCount() const;

    const QList<std::shared_ptr<objecttree::MetiersInterest> > &getMetiersList() const { return mMetiers; }
    //int getMetiersCount() const {
    //    return mMetiers.size();
    //}
    void addMetier(int id);

    /* Access to Population statistics */
    int getPopulationsCount() const;
    int getMetiersCount() const;
    int getBenthosPopulationsCount() const;
    const QList<std::shared_ptr<PopulationData> > &getPopulationsList() const { return mPops; }
    const PopulationData &getPop(int idx) const { return *mPops.at(idx); }

    /* Access to Nations statistics */

    const QList<std::shared_ptr<NationData> > &getNationsList() const { return mNations; }
    const NationData &getNation(int idx) const { return *mNations.at(idx); }

    /* Access to Vessels statistics */

    const QList<std::shared_ptr<VesselData> > &getVesselsList() const { return mVessels; }
    const VesselData &getVessel(int idx) const { return *mVessels.at(idx); }

    /* Access to Harbour statistics */

    const QList<std::shared_ptr<HarbourData> > &getHarbourList() const { return mHarbours; }
    const HarbourData &getHarbourData(int idx) const { return *mHarbours.at(idx); }

    /** Retrieve the statistics for a specific Harbour from the DB, or the latest available if it's a live simulation */
    HarbourStats retrieveHarbourIdxStatAtStep (int idx, int step);

    /* Benthos Statistics */
    const BenthosStatsContainer &getBenthosStatistics() { return mStatsBenthos; }

    /* Fishfarms Statistics */
    const FishfarmsStatsContainer &getFishfarmsStatistics() { return mStatsFishfarms; }

    /* Ships Statistics */
    const ShipsStatsContainer &getShipsStatistics() { return mStatsShips; }

    /* Scenario and configuration */

    Scenario scenario() const;
    void setScenario(const Scenario &scenario);

    Config config() const;
    void setConfig(const Config &config);

    /* Stock names */

    const QMap<QString, int> getStockNames() const { return mStockNames; }
    void setStockNames (const QMap<QString,int> &names) { mStockNames = names; }

    /* Live Simulation data */

    void setCurrentStep(int step);
    int getCurrentStep() const { return mCurrentStep; }
    int getLastStep() const { return mLastStep; }


    /* Interesting pop access functions */
    const QList<int> &getInterestingPops() const { return mInterestingPop; }
    const QList<int> &getInterestingPops2() const { return mInterestingPop2; }

    /** \brief insert the pop into the list of interest for pops */
    void setInterestingPop(int n);
    void setInterestingPop2(int n);

    /** \brief remove the pop from the list of interest for pops */
    void remInterestingPop(int n);
    void remInterestingPop2(int n);
    bool isInterestingPop(int n);
    bool isInterestingPop2(int n);
    void clearInterestingPop();
    void clearInterestingPop2();

    QList<int> getInterestingBenthos() const { return mInterestingBenthos.list(); }
    void setInterestingBenthos(int n) { mInterestingBenthos.set(n); }
    void remInterestingBenthos(int n) { mInterestingBenthos.rem(n); }
    bool isInterestingBenthos(int n) const { return mInterestingBenthos.has(n); }
    void clrInterestingBenthos() { mInterestingBenthos.clear(); }

    QList<int> getInterestingFishfarms() const { return mInterestingFishfarms.list(); }
    void setInterestingFishfarms(int n) { mInterestingFishfarms.set(n); }
    void remInterestingFishfarms(int n) { mInterestingFishfarms.rem(n); }
    bool isInterestingFishfarms(int n) const { return mInterestingFishfarms.has(n); }
    void clrInterestingFishfarms() { mInterestingFishfarms.clear(); }

    QList<int> getInterestingWindfarms() const { return mInterestingWindfarms.list(); }
    void setInterestingWindfarms(int n) { mInterestingWindfarms.set(n); }
    void remInterestingWindfarms(int n) { mInterestingWindfarms.rem(n); }
    bool isInterestingWindfarms(int n) const { return mInterestingWindfarms.has(n); }
    void clrInterestingWindfarms() { mInterestingWindfarms.clear(); }

    QList<int> getInterestingShips() const { return mInterestingShips.list(); }
    void setInterestingShips(int n) { mInterestingShips.set(n); }
    void remInterestingShips(int n) { mInterestingShips.rem(n); }
    bool isInterestingShips(int n) const { return mInterestingShips.has(n); }
    void clrInterestingShips() { mInterestingShips.clear(); }

    /* Interesting pop access functions */
    bool isInterestingSizeTotal() const { return mInterestingSizeTotal; }
    void setInterestingSizeTotal(bool b) { mInterestingSizeTotal = b; }
    bool isInterestingSizeAvg() const { return mInterestingSizeAvg; }
    void setInterestingSizeAvg(bool b) { mInterestingSizeAvg = b; }
    bool isInterestingSizeMin() const { return mInterestingSizeMin; }
    void setInterestingSizeMin(bool b) { mInterestingSizeMin = b; }
    bool isInterestingSizeMax() const { return mInterestingSizeMax; }
    void setInterestingSizeMax(bool b) { mInterestingSizeMax = b; }

    const QList<int> &getInterestingSizes() const { return mInterestingSizes; }

    int getNumFuncGroups() const;
    std::shared_ptr<InterestingListWithSpecialValues<int>>  getFunctionalGroupsList() const { return mFuncGroups; }

    int getNumFishfarmsTypes() const;
    std::shared_ptr<InterestingListWithSpecialValues<int>>  getFishfarmsTypesList() const { return mFishfarmsTypes; }

    int getNumWindfarmsTypes() const;
    std::shared_ptr<InterestingListWithSpecialValues<int>>  getWindfarmsTypesList() const { return mWindfarmsTypes; }

    int getNumShipsTypes() const;
    std::shared_ptr<InterestingListWithSpecialValues<int>>  getShipsTypesList() const { return mShipsTypes; }

    /** \brief insert the pop into the list of interest for pops */
    void setInterestingSize(int n);

    /** \brief remove the pop from the list of interest for pops */
    void remInterestingSize(int n);
    bool isInterestingSize(int n);

    /* Interesting harbours - see pop */
    const QList<types::NodeId> &getInterestingHarbours() const { return mInterestingHarb; }

    /** \brief insert the pop into the list of interest for pops */
    void setInterestingHarb(types::NodeId n);

    /** \brief remove the pop from the list of interest for pops */
    void remInterestingHarb(types::NodeId n);
    bool isInterestingHarb(types::NodeId n);

    /* Interesting Nations */
    const QList<int> &getInterestingNations() const { return mInterestingNations; }


    /** \brief insert the pop into the list of interest for pops */
    void setInterestingNations(int n) {
        if (!mInterestingNations.contains(n))
            mInterestingNations.append(n);
            std::sort(mInterestingNations.begin(),mInterestingNations.end());
    }

    /** \brief remove the pop from the list of interest for pops */
    void remInterestingNations(int n) {
        mInterestingNations.removeAll(n);
    }
    bool isInterestingNations(int n) {
        return mInterestingNations.contains(n);
    }


    /* Interesting Vessels */
    const QList<int> &getInterestingVessels() const { return mInterestingVessels; }

    void setInterestingVessels(int n) {
        if (!mInterestingVessels.contains(n))
            mInterestingVessels.append(n);
            std::sort(mInterestingVessels.begin(),mInterestingVessels.end());
    }

    /** \brief remove the pop from the list of interest for pops */
    void remInterestingVessels(int n) {
        mInterestingVessels.removeAll(n);
    }
    bool isInterestingVessels(int n) {
        return mInterestingVessels.contains(n);
    }



    //

    QString getLastError() const { return mLastError; }

    void parseOutputStatsFile (QString file, int tstep);

    void commitNodesStatsFromSimu(int tstep, bool force = false);

    // Stats collecting. Must store data to db
    void startCollectingStats();
    void endCollectingStats();
    void collectNodePopStats (int tstep, int node_idx, const QList<double> &stats, const QList<double> &stats_w, double tot, double wtot);
    void collectPopCumftime(int step, int node_idx, double cumftime);
    void collectPopCumsweptarea(int step, int node_idx, double cumsweptarea);
    void collectPopCumsubsurfacesweptarea(int step, int node_idx, double cumsubsurfacesweptarea);
    void collectPopCumcatches(int step, int node_idx, double cumcatches);
    void collectPopCumcatchesWithThreshold(int step, int node_idx, double cumcatches_with_threshold);
    void collectPopCumdiscards(int step, int node_idx, double cumdiscards);
    void collectPopCumdiscardsratio(int step, int node_idx, double cumdiscardsratio);
    void collectPopNbchoked(int step, int node_idx, double nbchoked);
    void collectPopTariffs(int step, int node_idx, vector<double> tariffs);
    void collectPopImpact(int step, int node_idx, int popid, double impact);
    void collectPopCumcatchesPerPop(int step, int node_idx, int popid, double cumcatchesperpop);

    void collectSalinity(int step, int node_idx, double salinity);
    void collectWind(int step, int node_idx, double wind);
    void collectSST(int step, int node_idx, double sst);
    void collectNitrogen(int step, int node_idx, double nitrogen);
    void collectPhosphorus(int step, int node_idx, double phosphorus);
    void collectOxygen(int step, int node_idx, double oxygen);
    void collectDissolvedCarbon(int step, int node_idx, double dissolvedcarbon);
    void collectBathymetry(int step, int node_idx, double bathymetry);
    void collectShippingdensity(int step, int node_idx, double shippingdensity);
    void collectSiltfraction(int step, int node_idx, double siltfraction);
    void collectIcesrectanglecode(int step, int node_idx, double icesrectanglecode);



    void collectPopBenthosBiomass(int step, int node_idx, int funcid, double benthosbiomass);
    void collectPopBenthosNumber(int step, int node_idx, int funcid, double benthosnumber);
    void collectPopBenthosMeanWeight (int step, int node_idx, int funcid, double meanweight);
    void collectPopBenthosBiomassOverK(int step, int node_idx, int funcid, double benthosbiomassoverK);
    void collectPopBenthosNumberOverK(int step, int node_idx, int funcid, double benthosnumberoverK);
    void commitPopBenthosStats(int tstep);

    void collectFishfarmFishMeanWeight (int step, int node_idx, int farmid, int farmtype, double meanw_kg);
    void collectFishfarmFishHarvestedKg (int step, int node_idx,  int farmid, int farmtype, double fish_harvested_kg);
    void collectFishfarmEggsHarvestedKg (int step, int node_idx, int farmid, int farmtype, double eggs_harvested_kg);
    void collectFishfarmAnnualProfit (int step, int node_idx, int farmid, int farmtype, double fishfarm_annualprofit);
    void collectFishfarmNetDischargeN (int step, int node_idx, int farmid, int farmtype, double fishfarm_netdischargeN);
    void collectFishfarmNetDischargeP (int step, int node_idx, int farmid, int farmtype, double fishfarm_netdischargeP);
    void collectFishfarmCumulNetDischargeN (int step, int node_idx, int farmid, int farmtype, double fishfarm_cumulnetdischargeN);
    void collectFishfarmCumulNetDischargeP (int step, int node_idx, int farmid, int farmtype, double fishfarm_cumulnetdischargeP);
    void commitFishfarmsStats(int tstep);

    void collectShipNbTransportedUnits (int step, int node_idx, int shipid, int shiptype,  double nb_transported_units);
    void collectShipFuelUseHour (int step, int node_idx, int shipid,  int shiptype,  double fuel_use_h);
    void collectShipNOxEmissiongPerkW (int step, int node_idx, int shipid, int shiptype,  double NOx_emission_gperkW);
    void collectShipSOxEmissionPercentPerTotalFuelmass (int step, int node_idx, int shipid,  int shiptype,  double SOx_emission_percentpertotalfuelmass);
    void collectShipGHGemissiongPerkW (int step, int node_idx, int shipid,  int shiptype,  double GHG_emission_gperkW);
    void collectShipPMEemissiongPerkW (int step, int node_idx, int shipid,  int shiptype,  double PME_emission_gperkW);
    void collectShipfuelUseLitre(int step, int node_idx, int shipid,  int shiptype,  double fuel_use_litre);
    void collectShipNOxEmission (int step, int node_idx, int shipid,  int shiptype,  double NOx_emission);
    void collectShipSOxEmission (int step, int node_idx, int shipid,  int shiptype,  double SOx_emission);
    void collectShipGHGemission (int step, int node_idx, int shipid,  int shiptype,  double GHG_emission);
    void collectShipPMEemission (int step, int node_idx, int shipid,  int shiptype,  double PME_emission);
    void commitShipsStats(int tstep);

    void collectVesselStats (int step, const VesselStats &stats);
    void commitVesselsStats(int tstep);

    /* Editor stuff */

    void clearAllNodes();
    bool addGraph(const QList<GraphBuilder::Node> &points, MapObjectsController *controller);
    bool removeNode(std::shared_ptr<NodeData> node);
    int addEdge(std::shared_ptr<NodeData> nodedata, types::NodeId targetidx, double weight);
    int addEdge(types::NodeId srcidx, types::NodeId targetidx, double weight);
    bool exportGraph(const QString &path);
    bool importHarbours (QList<std::shared_ptr<HarbourData> > &list);
    void addPenaltyToNodesByAddWeight(const QList<QPointF> &poly, double weight, double nbOfDaysClosedPerMonth, 
                                       //bool onQ1, bool onQ2, bool onQ3, bool onQ4, 
                                       vector<bool> checkedMonths, const vector<int> &checkedVesSizes, const vector<int>& checkedNations, vector<int> bannedMetiers);
    void addPenaltyToNodesByAddWeight(OGRGeometry *geometry, double weight, double nbOfDaysClosedPerMonth,
                                         // bool onQ1, bool onQ2, bool onQ3, bool onQ4,
                                         vector<bool> checkedMonths, const vector<int> &checkedVesSizes, const vector<int>& checkedNations, vector<int> bannedMetiers);

    const QList<displace::NodePenalty> getPenaltyCollection() const { return mPenaltyNodes; }

    bool isShortestPathFolderLinked() const { return !mShortestPathFolder.isEmpty(); }
    void linkShortestPathFolder(QString path) { mShortestPathFolder = path; }
    QString linkedShortestPathFolder() const { return mShortestPathFolder; }

    bool isGraphFolderLinked() const { return !mGraphFolder.isEmpty(); }
    void linkGraphFolder(QString path) { mGraphFolder = path; }
    QString linkedGraphFolder() const { return mGraphFolder; }

    void setLandscapeCodesFromFeature(OGRGeometry *geometry, int code);
    void setWindFromFeature(OGRGeometry *geometry, double wind);
    void setSSTFromFeature(OGRGeometry *geometry, double sst);
    void setSalinityFromFeature(OGRGeometry *geometry, double psu);
    void setNitrogenFromFeature(OGRGeometry *geometry, double nit);
    void setPhosphorusFromFeature(OGRGeometry *geometry, double phos);
    void setOxygenFromFeature(OGRGeometry *geometry, double oxygen);
    void setDissolvedCarbonFromFeature(OGRGeometry *geometry, double dissc);
    void setBenthosBiomassFromFeature(OGRGeometry *geometry, double bio);
    void setBenthosNumberFromFeature(OGRGeometry *geometry, double nb);
    void setAreaCodesFromFeature(OGRGeometry *geometry, int code);

    std::shared_ptr<SQLiteOutputStorage> getOutputStorage() const { return mOutSqlite; }
protected:
    bool loadNodes();
    bool loadVessels();
    bool loadGraphs();
    bool initShips();
    bool initFirm();
    bool initFishfarm();
    bool initWindmill();
    bool initBenthos();
    bool initPopulations();
    bool initNations();
    void createFeaturesLayer();

    bool loadNodesFromDb();
    bool loadVesselsFromDb();
    bool loadFishfarmsFromDb();

    void checkStatsCollection(int tstep);
    bool parse(const QString &path, QString *basepath, QString *inputname, QString *outputname);
    void setCodeFromFeature(OGRGeometry *geometry, int code, std::function<void (std::shared_ptr<NodeData>, int)> func);
    void setWdFromFeature(OGRGeometry *geometry, double wd, std::function<void (std::shared_ptr<NodeData>, int)> func);
    void setTFromFeature(OGRGeometry *geometry, double t, std::function<void (std::shared_ptr<NodeData>, int)> func);
    void setSalFromFeature(OGRGeometry *geometry, double sal, std::function<void (std::shared_ptr<NodeData>, int)> func);
    void setNitroFromFeature(OGRGeometry *geometry, double sal, std::function<void (std::shared_ptr<NodeData>, int)> func);
    void setPhosFromFeature(OGRGeometry *geometry, double sal, std::function<void (std::shared_ptr<NodeData>, int)> func);
    void setOxyFromFeature(OGRGeometry *geometry, double sal, std::function<void (std::shared_ptr<NodeData>, int)> func);
    void setDissoFromFeature(OGRGeometry *geometry, double sal, std::function<void (std::shared_ptr<NodeData>, int)> func);
    void setBenthosBioFromFeature(OGRGeometry *geometry, double bio, std::function<void (std::shared_ptr<NodeData>, double)> func);
    void setBenthosNbFromFeature(OGRGeometry *geometry, double bio, std::function<void (std::shared_ptr<NodeData>, double)> func);

signals:
    void parseOutput(QString, int);
    void outputParsed();
    void errorParsingStatsFile(QString);

private:
    mutable QMutex mMutex;

    ModelType mModelType;
    DbHelper *mDb;
    std::shared_ptr<MapsDataProvider> mMapsDataProvider;
    std::shared_ptr<SQLiteOutputStorage> mOutSqlite;
    std::shared_ptr<Calendar> mCalendar;
    QString mOutDir;
    QString mFullPath;
    QString mInputName;
    QString mBasePath;
    QString mOutputName;
    QString mSimuName;
    QString mLinkedDbName;
    int mIndex;

    int mSimulSteps;
    int mUseStaticPaths;
    int mCurrentStep, mLastStep;
    int mLastStats;
    bool mNodesStatsDirty;
    bool mVesselsStatsDirty;     // TODO: refactor this using an opaque class as for FishFarms
    int m_vessel_last_step;     // TODO: Same as above
    bool mFirmsStatsDirty;
    bool mShipsStatsDirty;

    Scenario mScenario;
    Config mConfig;

    QList<int> mInterestingPop;
    QList<int> mInterestingPop2;
    bool mInterestingSizeTotal, mInterestingSizeAvg, mInterestingSizeMin, mInterestingSizeMax;
    QList<int> mInterestingSizes;
    QList<types::NodeId> mInterestingHarb;
    QList<int> mInterestingNations;
    QList<int> mInterestingVessels;
    InterestingList<int> mInterestingBenthos;
    InterestingList<int> mInterestingFishfarms;
    InterestingList<int> mInterestingWindfarms;
    InterestingList<int> mInterestingShips;
    std::shared_ptr<InterestingListWithSpecialValues<int>> mFuncGroups;
    std::shared_ptr<InterestingListWithSpecialValues<int>> mFishfarmsTypes;
    std::shared_ptr<InterestingListWithSpecialValues<int>> mWindfarmsTypes;

    std::shared_ptr<InterestingListWithSpecialValues<int>> mShipsTypes;

    QList<displace::NodePenalty> mPenaltyNodes;
    QList<std::shared_ptr<HarbourData>> mHarbours;
    QList<std::shared_ptr<NodeData> > mNodes;
    QList<std::shared_ptr<VesselData> > mVessels;
    QList<std::shared_ptr<PopulationData> > mPops;
    QList<std::shared_ptr<FishfarmData> > mFishfarms;
    QList<std::shared_ptr<FirmData> > mFirms;
    QList<std::shared_ptr<WindmillData> > mWindmills;
    QList<std::shared_ptr<ShipData> > mShips;
    QList<std::shared_ptr<Benthos> > mBenthos;
    QList<std::shared_ptr<objecttree::MetiersInterest>> mMetiers;

    QList<std::shared_ptr<NationData> > mNations;
    BenthosStatsContainer mStatsBenthos;
    BenthosStats mStatsBenthosCollected;
    FishfarmsStatsContainer mStatsFishfarms;
    FishfarmsStats mStatsFishfarmsCollected;
    ShipsStatsContainer mStatsShips;

    ShipsStats mStatsShipsCollected;
    QMap<int, std::shared_ptr<Benthos> > mBenthosInfo;
    QMap<int, std::shared_ptr<Fishfarm> > mFishfarmInfo;

    QMap<QString, int> mStockNames;

    // --- Working objects
    OutputFileParser *mOutputFileParser;

    QThread *mParserThread;

    QString mLastError;
    /* Editor stuff */
    enum OgrType { OgrTypeNode = 0, OgrTypeEdge = 1 };
    GDALDataset *mDataSource;
    OGRLayer *mNodesLayer;

    int mNodesLayerIndex;

    OGRSpatialReference *mSpatialRef;
    QString mShortestPathFolder;

    QString mGraphFolder;
};

#endif // DISPLACEMODEL_H
