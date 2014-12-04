#ifndef DISPLACEMODEL_H
#define DISPLACEMODEL_H

#include <scenario.h>
#include <config.h>

#include <modelobjects/nodedata.h>
#include <modelobjects/vesseldata.h>
#include <modelobjects/benthos.h>
#include <modelobjects/populationdata.h>
#include <modelobjects/nationdata.h>
#include <modelobjects/harbourdata.h>
#include <historicaldatacollector.h>
#include <outputfileparser.h>
#include <graphbuilder.h>

#include <QObject>
#include <QString>
#include <QList>
#include <QMap>
#include <QThread>

#include <memory>

#include <gdal/ogrsf_frmts.h>

#include <QDebug>

class DbHelper;
class MapObjectsController;
class Calendar;

class DisplaceModel : public QObject
{
    Q_OBJECT
public:
    typedef QVector<PopulationData> PopulationStat;
    typedef HistoricalDataCollector<PopulationStat> PopulationStatContainer;
    typedef QVector<NationStats> NationsStats;
    typedef HistoricalDataCollector<NationsStats> NationsStatsContainer;
    typedef QVector<HarbourStats> HarboursStats;
    typedef HistoricalDataCollector<HarboursStats> HarboursStatsContainer;

    enum ModelType {
        LiveModelType, EditorModelType, OfflineModelType,
        EmptyModelType
    };

    DisplaceModel();

    ModelType modelType() const { return mModelType; }

    bool edit(QString modelname);
    void setIndex(int idx) { mIndex = idx; }
    int index() const { return mIndex; }

    bool load (QString path);
    bool loadDatabase (QString path);
    bool linkDatabase (QString path);
    bool prepareDatabaseForSimulation ();
    bool clearStats();
    bool saveScenarioAs(const QString &path);
    bool saveScenario();
    bool saveConfig();

    std::shared_ptr<Calendar> calendar() const { return mCalendar; }

    /** \brief Signals the simulation has ended. Flush all buffers */
    void simulationEnded();

    // Getter
    QString fullpath() const { return mFullPath; }
    QString inputName() const { return mInputName; }
    QString basepath() const { return mBasePath; }
    QString outputName() const { return mOutputName; }
    void setOutputName(const QString &name) { mOutputName = name; }
    QString simulationName() const { return mSimuName; }
    void setSimulationName(const QString &name) { mSimuName = name; }

    QString linkedDatabase() const { return mLinkedDbName; }
    bool isModelLoaded() const { return !mInputName.isEmpty(); }

    int getNBPops() const {
        return mConfig.getNbpops();
    }
    int getSzGrupsCount() const {
        return mConfig.getSzGroups();
    }

    int getSimulationSteps() const;
    void setSimulationSteps(int value);

    /* Graphs operation */

    const QList<std::shared_ptr<HarbourData> > &getHarboursList() const { return mHarbours; }
    int getHarboursCount() const;
    QString getHarbourId(int idx) const;

    const QList<std::shared_ptr<NodeData> > &getNodesList() const { return mNodes; }
    int getNodesCount() const;
    QString getNodeId(int idx) const;
    QList<std::shared_ptr<NodeData> > getAllNodesWithin (const QPointF &centerpoint, double dist) const;

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

    const QList<std::shared_ptr<Benthos> > &getBenthosList() const { return mBenthos; }
    int getBenthosCount() const;

    /* Access to Population statistics */
    int getPopulationsCount() const;
    const PopulationData &getPopulationsAtStep (int step, int idx) const {
        if (idx >= mStatsPopulations.getValue(step).size()) {
            qDebug() << step << idx << mStatsPopulations.getValue(step).size();
            Q_ASSERT(false);
        }
        return mStatsPopulations.getValue(step).at(idx);
    }
    int getPopulationsValuesCount() const {
        return mStatsPopulations.getUniqueValuesCount();
    }
    PopulationStatContainer::Container::const_iterator getPopulationsFirstValue() const {
        return mStatsPopulations.getFirst();
    }

    const PopulationData &getPopulations(int idx) const { return getPopulationsAtStep(mCurrentStep,idx); }

    /* Access to Nations statistics */

    const QList<std::shared_ptr<NationData> > &getNationsList() const { return mNations; }
    const NationData &getNation(int idx) const { return *mNations.at(idx); }

    int getNationsStatsCount() const {
        return mStatsNations.getUniqueValuesCount();
    }
    NationsStatsContainer::Container::const_iterator getNationsStatsFirstValue() const {
        return mStatsNations.getFirst();
    }
    const NationsStats &getNationsStatAtStep(int step) const {
        return mStatsNations.getValue(step);
    }
    const NationStats &getNationStatAtStep(int step, int idx) const {
        return mStatsNations.getValue(step).at(idx);
    }

    /* Access to Harbour statistics */

    const QList<std::shared_ptr<HarbourData> > &getHarbourList() const { return mHarbours; }
    const HarbourData &getHarbourData(int idx) const { return *mHarbours.at(idx); }

    int getHarboursStatsCount() const {
        return mStatsHarbours.getUniqueValuesCount();
    }
    HarboursStatsContainer::Container::const_iterator getHarboursStatsFirstValue() const {
        return mStatsHarbours.getFirst();
    }
    const HarboursStats &getHarboursStatAtStep(int step) const {
        return mStatsHarbours.getValue(step);
    }
    const HarbourStats &getHarboursStatAtStep(int step, int idx) const {
        return mStatsHarbours.getValue(step).at(idx);
    }
    /** Retrieve the statistics for a specific Harbour from the DB, or the latest available if it's a live simulation */
    HarbourStats retrieveHarbourIdxStatAtStep (int idx, int step);


    /* Scenario and configuration */

    Scenario scenario() const;
    void setScenario(const Scenario &scenario);

    Config config() const;
    void setConfig(const Config &config);

    /* Live Simulation data */

    void setCurrentStep(int step);
    int getCurrentStep() const { return mCurrentStep; }
    int getLastStep() const { return mLastStep; }


    /* Interesting pop access functions */
    const QList<int> &getInterestingPops() const { return mInterestingPop; }

    /** \brief insert the pop into the list of interest for pops */
    void setInterestingPop(int n);

    /** \brief remove the pop from the list of interest for pops */
    void remInterestingPop(int n);
    bool isInterestingPop(int n);
    void clearInterestingPop();

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

    /** \brief insert the pop into the list of interest for pops */
    void setInterestingSize(int n);

    /** \brief remove the pop from the list of interest for pops */
    void remInterestingSize(int n);
    bool isInterestingSize(int n);

    /* Interesting harbours - see pop */
    const QList<int> &getInterestingHarbours() const { return mInterestingHarb; }

    /** \brief insert the pop into the list of interest for pops */
    void setInterestingHarb(int n);

    /** \brief remove the pop from the list of interest for pops */
    void remInterestingHarb(int n);
    bool isInterestingHarb(int n);

    /* Interesting Nations */
    const QList<int> &getInterestingNations() const { return mInterestingNations; }

    /** \brief insert the pop into the list of interest for pops */
    void setInterestingNations(int n) {
        if (!mInterestingNations.contains(n))
            mInterestingNations.append(n);
            qSort(mInterestingNations);
    }

    /** \brief remove the pop from the list of interest for pops */
    void remInterestingNations(int n) {
        mInterestingNations.removeAll(n);
    }
    bool isInterestingNations(int n) {
        return mInterestingNations.contains(n);
    }

    //

    QString getLastError() const { return mLastError; }

    void parseOutputStatsFile (QString file, int tstep);

    void commitNodesStatsFromSimu(int tstep);

    // Stats collecting. Must store data to db
    void startCollectingStats();
    void endCollectingStats();
    void collectNodePopStats (int tstep, int node_idx, const QList<double> &stats, const QList<double> &stats_w, double tot, double wtot);
    void collectPopCumftime(int step, int node_idx, double cumftime);
    void collectPopImpact(int step, int node_idx, int popid, double impact);

    void collectPopdynN(int step, int popid, const QVector<double> &pops, double value);
    void collectPopdynF(int step, int popid, const QVector<double> &pops, double value);

    void collectVesselStats (int step, const VesselStats &stats);

    /* Editor stuff */

    void clearAllNodes();
    bool addGraph(const QList<GraphBuilder::Node> &points, MapObjectsController *controller);
    int addEdge(std::shared_ptr<NodeData> nodedata, int targetidx, double weight);
    int addEdge(int srcidx, int targetidx, double weight);
    bool exportGraph(const QString &path);
    bool importHarbours (QList<std::shared_ptr<HarbourData> > &list);
    void addPenaltyToNodesByAddWeight(const QList<QPointF> &poly, double weight);
    void addPenaltyToNodesByAddWeight(OGRGeometry *geometry, double weight);

    bool isShortestPathFolderLinked() const { return !mShortestPathFolder.isEmpty(); }
    void linkShortestPathFolder(QString path) { mShortestPathFolder = path; }
    QString linkedShortestPathFolder() const { return mShortestPathFolder; }

protected:
    bool loadNodes();
    bool loadVessels();
    bool loadGraphs();
    bool initBenthos();
    bool initPopulations();
    bool initNations();
    void createFeaturesLayer();

    bool loadNodesFromDb();
    bool loadVesselsFromDb();
    bool loadHistoricalStatsFromDb();

    void checkStatsCollection(int tstep);

    bool parse(const QString &path, QString *basepath, QString *inputname, QString *outputname);

signals:
    void parseOutput(QString, int);
    void outputParsed();
    void errorParsingStatsFile(QString);

private:
    ModelType mModelType;
    DbHelper *mDb;
    std::shared_ptr<Calendar> mCalendar;
    QString mFullPath;
    QString mInputName;
    QString mBasePath;
    QString mOutputName;
    QString mSimuName;
    QString mLinkedDbName;
    int mIndex;

    int mSimulSteps;
    int mCurrentStep, mLastStep;
    int mLastStats;
    bool mNodesStatsDirty;
    bool mPopStatsDirty;
    bool mVesselsStatsDirty;

    Scenario mScenario;
    Config mConfig;

    QList<int> mInterestingPop;
    bool mInterestingSizeTotal, mInterestingSizeAvg, mInterestingSizeMin, mInterestingSizeMax;
    QList<int> mInterestingSizes;
    QList<int> mInterestingHarb;
    QList<int> mInterestingNations;

    QList<std::shared_ptr<HarbourData>> mHarbours;
    QList<std::shared_ptr<NodeData> > mNodes;
    QList<std::shared_ptr<VesselData> > mVessels;
    QList<std::shared_ptr<Benthos> > mBenthos;
    QList<std::shared_ptr<NationData> > mNations;

    PopulationStatContainer mStatsPopulations;
    PopulationStat mStatsPopulationsCollected;
    NationsStatsContainer mStatsNations;
    NationsStats mStatsNationsCollected;
    HarboursStatsContainer mStatsHarbours;
    HarboursStats mStatsHarboursCollected;

    QMap<int, std::shared_ptr<Benthos> > mBenthosInfo;

    // --- Working objects

    OutputFileParser *mOutputFileParser;
    QThread *mParserThread;

    QString mLastError;

    /* Editor stuff */
    enum OgrType { OgrTypeNode = 0, OgrTypeEdge = 1 };
    OGRDataSource *mDataSource;
    OGRLayer *mNodesLayer;
    int mNodesLayerIndex;

    OGRSpatialReference *mSpatialRef;

    QString mShortestPathFolder;
};

#endif // DISPLACEMODEL_H
